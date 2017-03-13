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
// Class scanline_p - a general purpose scanline container with packed spans.
//
//----------------------------------------------------------------------------
#ifndef AGG_SCANLINE_P32_INCLUDED
#define AGG_SCANLINE_P32_INCLUDED

#include "agg_basics.h"

namespace agg
{

    //========================================================================
    // 
    // This is a general purpose scaline container which supports the interface 
    // used in the rasterizer::render(). See description of agg_scanline_u8 
    // for details.
    // 
    //------------------------------------------------------------------------
    template<class T, unsigned AA_Shift=8> class scanline_p
    {
    public:
        enum { aa_shift = AA_Shift };
        typedef T cover_type;

        struct span
        {
            const T* pix_covers;
            T        span_cover;
            int      x;
            unsigned len;
        };

        typedef const span* const_iterator;

        ~scanline_p()
        {
            delete [] m_spans;
            delete [] m_covers;
        }

        scanline_p() :
            m_max_len(0),
            m_dx(0),
            m_dy(0),
            m_last_x(0x7FFF),
            m_last_y(0x7FFF),
            m_covers(0),
            m_cover_ptr(0),
            m_spans(0),
            m_span_ptr(0),
            m_num_spans(0)
        {
        }

        void reset(int min_x, int max_x, int dx, int dy);
        void reset_spans();
        void add_cell(int x, int y, unsigned cover);
        void add_span(int x, int y, unsigned len, unsigned cover);
        int is_ready(int y) const
        {
            return m_num_spans && (y ^ m_last_y);
        }
        void prepare() {}

        int            y()         const { return m_last_y + m_dy; }
        unsigned       num_spans() const { return m_num_spans; }
        const_iterator begin()     const { return m_spans + 1; }

    private:
        scanline_p(const scanline_p<T, AA_Shift>&);
        const scanline_p<T, AA_Shift>& operator = (const scanline_p<T, AA_Shift>&);

        unsigned m_max_len;
        int      m_dx;
        int      m_dy;
        int      m_last_x;
        int      m_last_y;
        T*       m_covers;
        T*       m_cover_ptr;
        span*    m_spans;
        span*    m_span_ptr;
        unsigned m_num_spans;
    };


    //------------------------------------------------------------------------
    template<class T, unsigned AA_Shift> 
    void scanline_p<T, AA_Shift>::reset(int min_x, int max_x, int dx, int dy)
    {
        unsigned max_len = max_x - min_x + 3;
        if(max_len > m_max_len)
        {
            delete [] m_spans;
            delete [] m_covers;
            m_covers  = new T [max_len];
            m_spans   = new span [max_len];
            m_max_len = max_len;
        }
        m_dx        = dx;
        m_dy        = dy;
        m_last_x    = 0x7FFF;
        m_last_y    = 0x7FFF;
        m_cover_ptr = m_covers;
        m_span_ptr  = m_spans;
        m_num_spans = 0;
        m_span_ptr->pix_covers = 0;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned AA_Shift> 
    void scanline_p<T, AA_Shift>::reset_spans()
    {
        m_last_x    = 0x7FFF;
        m_last_y    = 0x7FFF;
        m_cover_ptr = m_covers;
        m_span_ptr  = m_spans;
        m_num_spans = 0;
        m_span_ptr->pix_covers = 0;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned AA_Shift> 
    void scanline_p<T, AA_Shift>::add_cell(int x, int y, unsigned cover)
    {
        x += m_dx;
        *m_cover_ptr = (T)cover;
        if(x == m_last_x+1 && m_span_ptr->pix_covers)
        {
            m_span_ptr->len++;
        }
        else
        {
            ++m_span_ptr;
            ++m_num_spans;
            m_span_ptr->pix_covers = m_cover_ptr;
            m_span_ptr->span_cover = 0;
            m_span_ptr->x = x;
            m_span_ptr->len = 1;
        }
        m_last_x = x;
        m_last_y = y;
        ++m_cover_ptr;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned AA_Shift> 
    void scanline_p<T, AA_Shift>::add_span(int x, int y, unsigned len, unsigned cover)
    {
        x += m_dx;
        ++m_span_ptr;
        ++m_num_spans;
        m_span_ptr->pix_covers = 0;
        m_span_ptr->span_cover = (T)cover;
        m_span_ptr->x = x;
        m_span_ptr->len = len;
        m_last_x = x + len - 1;
        m_last_y = y;
    }


    //------------------------------------------------------------------------
    typedef scanline_p<int8u, 8> scanline_p8;

    template<unsigned AA_Shift> struct scanline_p32 : 
    public scanline_p<int32u, AA_Shift> {};
}


#endif
