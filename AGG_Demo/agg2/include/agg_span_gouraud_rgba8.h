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

#ifndef AGG_SPAN_GOURAUD_RGBA8_INCLUDED
#define AGG_SPAN_GOURAUD_RGBA8_INCLUDED

#include "agg_basics.h"
#include "agg_color_rgba8.h"
#include "agg_dda_line.h"
#include "agg_span_gouraud.h"

namespace agg
{

    //========================================================================
    template<class Allocator = span_allocator<rgba8> >
    class span_gouraud_rgba8 : public span_gouraud<rgba8, Allocator>
    {
    public:
        typedef Allocator alloc_type;
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;
        typedef span_gouraud<color_type, alloc_type> base_type;
        typedef typename base_type::coord_type coord_type;

    private:
        //--------------------------------------------------------------------
        struct rgba_calc
        {
            void init(const coord_type& c1, const coord_type& c2)
            {
                m_x1 = c1.x;
                m_y1 = c1.y;
                m_dx = c2.x - c1.x;
                m_dy = 1.0 / (c2.y - c1.y);
                m_r1 = c1.color.r;
                m_g1 = c1.color.g;
                m_b1 = c1.color.b;
                m_a1 = c1.color.a;
                m_dr = c2.color.r - m_r1;
                m_dg = c2.color.g - m_g1;
                m_db = c2.color.b - m_b1;
                m_da = c2.color.a - m_a1;
            }

            void calc(int y)
            {
                double k = 0.0;
                if(y > m_y1) k = (y - m_y1) * m_dy;
                rgba8 rgba;
                m_r = m_r1 + int(m_dr * k);
                m_g = m_g1 + int(m_dg * k);
                m_b = m_b1 + int(m_db * k);
                m_a = m_a1 + int(m_da * k);
                m_x = int(m_x1 + m_dx * k);
            }

            double m_x1;
            double m_y1;
            double m_dx;
            double m_dy;
            int    m_r1;
            int    m_g1;
            int    m_b1;
            int    m_a1;
            int    m_dr;
            int    m_dg;
            int    m_db;
            int    m_da;
            int    m_r;
            int    m_g;
            int    m_b;
            int    m_a;
            int    m_x;
        };

    public:

        //--------------------------------------------------------------------
        span_gouraud_rgba8(alloc_type& alloc) : base_type(alloc) {}

        //--------------------------------------------------------------------
        span_gouraud_rgba8(alloc_type& alloc, 
                           const color_type& c1, 
                           const color_type& c2, 
                           const color_type& c3,
                           double x1, double y1, 
                           double x2, double y2,
                           double x3, double y3, 
                           double d = 0) : 
            base_type(alloc, c1, c2, c3, x1, y1, x2, y2, x3, y3, d)
        {}

        //--------------------------------------------------------------------
        void prepare(unsigned max_span_len)
        {
            base_type::prepare(max_span_len);

            coord_type coord[3];
            arrange_vertices(coord);

            m_y2 = int(coord[1].y);

            m_swap = calc_point_location(coord[0].x, coord[0].y, 
                                         coord[2].x, coord[2].y,
                                         coord[1].x, coord[1].y) < 0.0;

            m_rgba1.init(coord[0], coord[2]);
            m_rgba2.init(coord[0], coord[1]);
            m_rgba3.init(coord[1], coord[2]);
        }

        //--------------------------------------------------------------------
        color_type* generate(int x, int y, unsigned len)
        {
            m_rgba1.calc(y);
            const rgba_calc* pc1 = &m_rgba1;
            const rgba_calc* pc2 = &m_rgba2;

            if(y < m_y2)
            {
                m_rgba2.calc(y+1);
            }
            else
            {
                m_rgba3.calc(y);
                pc2 = &m_rgba3;
            }

            if(m_swap)
            {
                const rgba_calc* t = pc2;
                pc2 = pc1;
                pc1 = t;
            }

            int nx = pc1->m_x;
            unsigned nlen = pc2->m_x - pc1->m_x + 1;

            if(nlen < len) nlen = len;

            dda_line_interpolator<16> r(pc1->m_r, pc2->m_r, nlen);
            dda_line_interpolator<16> g(pc1->m_g, pc2->m_g, nlen);
            dda_line_interpolator<16> b(pc1->m_b, pc2->m_b, nlen);
            dda_line_interpolator<16> a(pc1->m_a, pc2->m_a, nlen);

            if(nx < x)
            {
                unsigned d = unsigned(x - nx);
                r += d; 
                g += d; 
                b += d; 
                a += d;
            }

            color_type* span = allocator().span();
            do
            {
                span->r = r.y();
                span->g = g.y();
                span->b = b.y();
                span->a = a.y();
                ++r; 
                ++g; 
                ++b; 
                ++a;
                ++span;
            }
            while(--len);
            return allocator().span();
        }


    private:
        bool      m_swap;
        int       m_y2;
        rgba_calc m_rgba1;
        rgba_calc m_rgba2;
        rgba_calc m_rgba3;
    };



}

#endif
