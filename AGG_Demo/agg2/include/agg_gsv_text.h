//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.0 
// Copyright (C) 2002 Maxim Shemanarev (McSeem)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// Class gsv_text
//
//----------------------------------------------------------------------------

#ifndef AGG_GSV_TEXT_INCLUDED
#define AGG_GSV_TEXT_INCLUDED

#include "agg_basics.h"
#include "agg_conv_stroke.h"
#include "agg_conv_transform.h"

namespace agg
{


    //-------------------------------------------------------------------------
    class gsv_text
    {
        enum status
        {
            initial,
            next_char,
            start_glyph,
            glyph
        };

    public:
        ~gsv_text();
        gsv_text();

        void font(const void* font);
        void flip(bool flip_y) { m_flip = flip_y; }
        void load_font(const char* file);
        void size(double height, double width=0.0);
        void space(double space);
        void line_space(double line_space);
        void start_point(double x, double y);
        void text(const char* text);

        void rewind(unsigned id);
        unsigned vertex(double* x, double* y);

    private:
        // not supposed to be copied
        gsv_text(const gsv_text&);
        const gsv_text& operator = (const gsv_text&);

        int16u value(const int8u* p) const
        {
            int16u v;
            if(m_big_endian)
            {
                 *(int8u*)&v      = p[1];
                *((int8u*)&v + 1) = p[0];
            }
            else
            {
                 *(int8u*)&v      = p[0];
                *((int8u*)&v + 1) = p[1];
            }
            return v;
        }

    private:
        double      m_x;
        double      m_y;
        double      m_start_x;
        double      m_width;
        double      m_height;
        double      m_space;
        double      m_line_space;
        char        m_chr[2];
        char*       m_text;
        char*       m_text_buf;
        unsigned    m_buf_size;
        char*       m_cur_chr;
        const void* m_font;
        char*       m_loaded_font;
        status      m_status;
        bool        m_big_endian;
        bool        m_flip;

        int8u*      m_indices;
        int8*       m_glyphs;
        int8*       m_bglyph;
        int8*       m_eglyph;
        double      m_w;
        double      m_h;
    };




    //-------------------------------------------------------------------------
    template<class Transformer = affine_matrix> class gsv_text_outline
    {
    public:
        gsv_text_outline(gsv_text& text, const Transformer& trans) :
          m_polyline(text),
          m_trans(m_polyline, trans)
        {
        }

        void width(double w) 
        { 
            m_polyline.width(w); 
        }

        void transformer(const Transformer* trans) 
        {
            m_trans->transformer(trans);
        }

        void rewind(unsigned id) 
        { 
            m_trans.rewind(id); 
            m_polyline.line_join(gen_stroke::round_join);
            m_polyline.line_cap(gen_stroke::round_cap);
        }

        unsigned vertex(double* x, double* y)
        {
            return m_trans.vertex(x, y);
        }

    private:
        conv_stroke<gsv_text> m_polyline;
        conv_transform<conv_stroke<gsv_text>, Transformer> m_trans;
    };



}


#endif
