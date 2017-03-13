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

#ifndef AGG_RENDERER_SCANLINE_INCLUDED
#define AGG_RENDERER_SCANLINE_INCLUDED

#include "agg_basics.h"
#include "agg_renderer_base.h"

namespace agg
{

    //========================================================================
    template<class BaseRenderer, class SpanGenerator> class renderer_scanline_u
    {
    public:
        typedef BaseRenderer base_ren_type;

        //--------------------------------------------------------------------
        renderer_scanline_u(base_ren_type& ren, SpanGenerator& span_gen) :
            m_ren(&ren),
            m_span_gen(&span_gen)
        {
        }
        
        //--------------------------------------------------------------------
        void prepare(unsigned max_span_len) 
        { 
            m_span_gen->prepare(max_span_len); 
        }

        //--------------------------------------------------------------------
        template<class Scanline> void render(const Scanline& sl)
        {
            int y = sl.y();
            m_ren->first_clip_box();
            do
            {
                int xmin = m_ren->xmin();
                int xmax = m_ren->xmax();

                if(y >= m_ren->ymin() && y <= m_ren->ymax())
                {
                    unsigned num_spans = sl.num_spans();
                    typename Scanline::const_iterator span = sl.begin();
                    do
                    {
                        int x = span->x;
                        int len = span->len;
                        const typename Scanline::cover_type* covers = span->covers;

                        if(x < xmin)
                        {
                            len -= xmin - x;
                            covers += xmin - x;
                            x = xmin;
                        }
                        if(len > 0)
                        {
                            if(x + len > xmax)
                            {
                                len = xmax - x + 1;
                            }
                            if(len > 0)
                            {
                                m_ren->blend_color_hspan_no_clip(
                                    x, y, len, 
                                    m_span_gen->generate(x, y, len),
                                    covers);
                            }
                        }
                        ++span;
                    }
                    while(--num_spans);
                }
            }
            while(m_ren->next_clip_box());
        }
        
    private:
        base_ren_type* m_ren;
        SpanGenerator* m_span_gen;
    };



    //========================================================================
    template<class BaseRenderer> class renderer_scanline_p_solid
    {
    public:
        typedef BaseRenderer base_ren_type;
        typedef typename base_ren_type::color_type color_type;

        //--------------------------------------------------------------------
        renderer_scanline_p_solid(base_ren_type& ren) :
            m_ren(&ren)
        {
        }
        
        //--------------------------------------------------------------------
        void color(const color_type& c) { m_color = c; }
        const color_type& color() const { return m_color; }

        //--------------------------------------------------------------------
        void prepare(unsigned) {}

        //--------------------------------------------------------------------
        template<class Scanline> void render(const Scanline& sl)
        {
            int y = sl.y();
            unsigned num_spans = sl.num_spans();
            typename Scanline::const_iterator span = sl.begin();

            do
            {
                int x = span->x;
                if(span->pix_covers)
                {
                    m_ren->blend_solid_hspan(x, y, span->len, 
                                             m_color, 
                                             span->pix_covers);
                }
                else
                {
                    m_ren->blend_hline(x, y, x + span->len-1, 
                                       m_color, 
                                       span->span_cover);
                }
                ++span;
            }
            while(--num_spans);
        }
        
    private:
        base_ren_type* m_ren;
        color_type m_color;
    };




    //========================================================================
    template<class BaseRenderer> class renderer_scanline_u_solid
    {
    public:
        typedef BaseRenderer base_ren_type;
        typedef typename base_ren_type::color_type color_type;

        //--------------------------------------------------------------------
        renderer_scanline_u_solid(base_ren_type& ren) :
            m_ren(&ren)
        {
        }
        
        //--------------------------------------------------------------------
        void color(const color_type& c) { m_color = c; }
        const color_type& color() const { return m_color; }

        //--------------------------------------------------------------------
        void prepare(unsigned) {}

        //--------------------------------------------------------------------
        template<class Scanline> void render(const Scanline& sl)
        {
            int y = sl.y();
            unsigned num_spans = sl.num_spans();
            typename Scanline::const_iterator span = sl.begin();

            do
            {
                m_ren->blend_solid_hspan(span->x, y, span->len, 
                                         m_color, span->covers);
                ++span;
            }
            while(--num_spans);
        }
        
    private:
        base_ren_type* m_ren;
        color_type m_color;
    };




    //========================================================================
    template<class BaseRenderer> class renderer_scanline_bin_solid
    {
    public:
        typedef BaseRenderer base_ren_type;
        typedef typename base_ren_type::color_type color_type;

        //--------------------------------------------------------------------
        renderer_scanline_bin_solid(base_ren_type& ren) :
            m_ren(&ren)
        {
        }
        
        //--------------------------------------------------------------------
        void color(const color_type& c) { m_color = c; }
        const color_type& color() const { return m_color; }

        //--------------------------------------------------------------------
        void prepare(unsigned) {}

        //--------------------------------------------------------------------
        template<class Scanline> void render(const Scanline& sl)
        {
            unsigned num_spans = sl.num_spans();
            typename Scanline::const_iterator span = sl.begin();
            do
            {
                m_ren->blend_hline(span->x, 
                                   sl.y(), 
                                   span->x + span->len - 1, 
                                   m_color, 
                                   color_type::opaque());
                ++span;
            }
            while(--num_spans);
        }
        
    private:
        base_ren_type* m_ren;
        color_type m_color;
    };

}

#endif
