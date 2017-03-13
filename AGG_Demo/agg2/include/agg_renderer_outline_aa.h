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
#ifndef AGG_RENDERER_OUTLINE_AA_INCLUDED
#define AGG_RENDERER_OUTLINE_AA_INCLUDED

#include "agg_basics.h"
#include "agg_line_aa_basics.h"
#include "agg_dda_line.h"
#include "agg_ellipse_bresenham.h"
#include "agg_renderer_base.h"
#include "agg_gamma_functions.h"

namespace agg
{

    //=========================================================================
    class distance_interpolator0
    {
    public:
        //---------------------------------------------------------------------
        distance_interpolator0() {}
        distance_interpolator0(int x1, int y1, int x2, int y2, int x, int y) :
            m_dx(line_mr(x2) - line_mr(x1)),
            m_dy(line_mr(y2) - line_mr(y1)),
            m_dist((line_mr(x + line_subpixel_size/2) - line_mr(x2)) * m_dy - 
                   (line_mr(y + line_subpixel_size/2) - line_mr(y2)) * m_dx)
        {
            m_dx <<= line_mr_subpixel_shift;
            m_dy <<= line_mr_subpixel_shift;
        }

        //---------------------------------------------------------------------
        void inc_x() { m_dist += m_dy; }
        void dec_x() { m_dist -= m_dy; }
        void inc_y() { m_dist -= m_dx; }
        void dec_y() { m_dist += m_dx; }

        //---------------------------------------------------------------------
        void inc_x(int dy)
        {
            m_dist += m_dy; 
            if(dy > 0) m_dist -= m_dx; 
            if(dy < 0) m_dist += m_dx; 
        }

        //---------------------------------------------------------------------
        void dec_x(int dy)
        {
            m_dist -= m_dy; 
            if(dy > 0) m_dist -= m_dx; 
            if(dy < 0) m_dist += m_dx; 
        }

        //---------------------------------------------------------------------
        void inc_y(int dx)
        {
            m_dist -= m_dx; 
            if(dx > 0) m_dist += m_dy; 
            if(dx < 0) m_dist -= m_dy; 
        }

        void dec_y(int dx)
        //---------------------------------------------------------------------
        {
            m_dist += m_dx; 
            if(dx > 0) m_dist += m_dy; 
            if(dx < 0) m_dist -= m_dy; 
        }

        //---------------------------------------------------------------------
        int dist()     const { return m_dist; }
        int dx()       const { return m_dx;   }
        int dy()       const { return m_dy;   }

    private:
        //---------------------------------------------------------------------
        int m_dx;
        int m_dy;
        int m_dist;
    };



    //=========================================================================
    class distance_interpolator1
    {
    public:
        //---------------------------------------------------------------------
        distance_interpolator1() {}
        distance_interpolator1(int x1, int y1, int x2, int y2, int x, int y) :
            m_dx(x2 - x1),
            m_dy(y2 - y1),
            m_dist(int(double(x + line_subpixel_size/2 - x2) * double(m_dy) - 
                       double(y + line_subpixel_size/2 - y2) * double(m_dx)))
        {
            m_dx <<= line_subpixel_shift;
            m_dy <<= line_subpixel_shift;
        }

        //---------------------------------------------------------------------
        void inc_x() { m_dist += m_dy; }
        void dec_x() { m_dist -= m_dy; }
        void inc_y() { m_dist -= m_dx; }
        void dec_y() { m_dist += m_dx; }

        //---------------------------------------------------------------------
        void inc_x(int dy)
        {
            m_dist += m_dy; 
            if(dy > 0) m_dist -= m_dx; 
            if(dy < 0) m_dist += m_dx; 
        }

        //---------------------------------------------------------------------
        void dec_x(int dy)
        {
            m_dist -= m_dy; 
            if(dy > 0) m_dist -= m_dx; 
            if(dy < 0) m_dist += m_dx; 
        }

        //---------------------------------------------------------------------
        void inc_y(int dx)
        {
            m_dist -= m_dx; 
            if(dx > 0) m_dist += m_dy; 
            if(dx < 0) m_dist -= m_dy; 
        }

        void dec_y(int dx)
        //---------------------------------------------------------------------
        {
            m_dist += m_dx; 
            if(dx > 0) m_dist += m_dy; 
            if(dx < 0) m_dist -= m_dy; 
        }

        //---------------------------------------------------------------------
        int dist() const { return m_dist; }
        int dx()   const { return m_dx;   }
        int dy()   const { return m_dy;   }

    private:
        //---------------------------------------------------------------------
        int m_dx;
        int m_dy;
        int m_dist;
    };





    //=========================================================================
    class distance_interpolator2
    {
    public:
        //---------------------------------------------------------------------
        distance_interpolator2() {}
        distance_interpolator2(int x1, int y1, int x2, int y2,
                               int sx, int sy, int x,  int y) :
            m_dx(x2 - x1),
            m_dy(y2 - y1),
            m_dx_start(line_mr(sx) - line_mr(x1)),
            m_dy_start(line_mr(sy) - line_mr(y1)),

            m_dist(int(double(x + line_subpixel_size/2 - x2) * double(m_dy) - 
                       double(y + line_subpixel_size/2 - y2) * double(m_dx))),

            m_dist_start((line_mr(x + line_subpixel_size/2) - line_mr(sx)) * m_dy_start - 
                         (line_mr(y + line_subpixel_size/2) - line_mr(sy)) * m_dx_start)
        {
            m_dx       <<= line_subpixel_shift;
            m_dy       <<= line_subpixel_shift;
            m_dx_start <<= line_mr_subpixel_shift;
            m_dy_start <<= line_mr_subpixel_shift;
        }

        distance_interpolator2(int x1, int y1, int x2, int y2,
                               int ex, int ey, int x,  int y, int) :
            m_dx(x2 - x1),
            m_dy(y2 - y1),
            m_dx_start(line_mr(ex) - line_mr(x2)),
            m_dy_start(line_mr(ey) - line_mr(y2)),

            m_dist(int(double(x + line_subpixel_size/2 - x2) * double(m_dy) - 
                       double(y + line_subpixel_size/2 - y2) * double(m_dx))),

            m_dist_start((line_mr(x + line_subpixel_size/2) - line_mr(ex)) * m_dy_start - 
                         (line_mr(y + line_subpixel_size/2) - line_mr(ey)) * m_dx_start)
        {
            m_dx       <<= line_subpixel_shift;
            m_dy       <<= line_subpixel_shift;
            m_dx_start <<= line_mr_subpixel_shift;
            m_dy_start <<= line_mr_subpixel_shift;
        }


        //---------------------------------------------------------------------
        void inc_x() { m_dist += m_dy; m_dist_start += m_dy_start; }
        void dec_x() { m_dist -= m_dy; m_dist_start -= m_dy_start; }
        void inc_y() { m_dist -= m_dx; m_dist_start -= m_dx_start; }
        void dec_y() { m_dist += m_dx; m_dist_start += m_dx_start; }

        //---------------------------------------------------------------------
        void inc_x(int dy)
        {
            m_dist       += m_dy; 
            m_dist_start += m_dy_start; 
            if(dy > 0)
            {
                m_dist       -= m_dx; 
                m_dist_start -= m_dx_start; 
            }
            if(dy < 0)
            {
                m_dist       += m_dx; 
                m_dist_start += m_dx_start; 
            }
        }

        //---------------------------------------------------------------------
        void dec_x(int dy)
        {
            m_dist       -= m_dy; 
            m_dist_start -= m_dy_start; 
            if(dy > 0)
            {
                m_dist       -= m_dx; 
                m_dist_start -= m_dx_start; 
            }
            if(dy < 0)
            {
                m_dist       += m_dx; 
                m_dist_start += m_dx_start; 
            }
        }

        //---------------------------------------------------------------------
        void inc_y(int dx)
        {
            m_dist       -= m_dx; 
            m_dist_start -= m_dx_start; 
            if(dx > 0)
            {
                m_dist       += m_dy; 
                m_dist_start += m_dy_start; 
            }
            if(dx < 0)
            {
                m_dist       -= m_dy; 
                m_dist_start -= m_dy_start; 
            }
        }

        //---------------------------------------------------------------------
        void dec_y(int dx)
        {
            m_dist       += m_dx; 
            m_dist_start += m_dx_start; 
            if(dx > 0)
            {
                m_dist       += m_dy; 
                m_dist_start += m_dy_start; 
            }
            if(dx < 0)
            {
                m_dist       -= m_dy; 
                m_dist_start -= m_dy_start; 
            }
        }

        //---------------------------------------------------------------------
        int dist()       const { return m_dist;       }
        int dist_start() const { return m_dist_start; }
        int dist_end()   const { return m_dist_start; }

        //---------------------------------------------------------------------
        int dx()       const { return m_dx;       }
        int dy()       const { return m_dy;       }
        int dx_start() const { return m_dx_start; }
        int dy_start() const { return m_dy_start; }
        int dx_end()   const { return m_dx_start; }
        int dy_end()   const { return m_dy_start; }

    private:
        //---------------------------------------------------------------------
        int m_dx;
        int m_dy;
        int m_dx_start;
        int m_dy_start;

        int m_dist;
        int m_dist_start;
    };





    //=========================================================================
    class distance_interpolator3
    {
    public:
        //---------------------------------------------------------------------
        distance_interpolator3() {}
        distance_interpolator3(int x1, int y1, int x2, int y2,
                               int sx, int sy, int ex, int ey, 
                               int x,  int y) :
            m_dx(x2 - x1),
            m_dy(y2 - y1),
            m_dx_start(line_mr(sx) - line_mr(x1)),
            m_dy_start(line_mr(sy) - line_mr(y1)),
            m_dx_end(line_mr(ex) - line_mr(x2)),
            m_dy_end(line_mr(ey) - line_mr(y2)),

            m_dist(int(double(x + line_subpixel_size/2 - x2) * double(m_dy) - 
                       double(y + line_subpixel_size/2 - y2) * double(m_dx))),

            m_dist_start((line_mr(x + line_subpixel_size/2) - line_mr(sx)) * m_dy_start - 
                         (line_mr(y + line_subpixel_size/2) - line_mr(sy)) * m_dx_start),

            m_dist_end((line_mr(x + line_subpixel_size/2) - line_mr(ex)) * m_dy_end - 
                       (line_mr(y + line_subpixel_size/2) - line_mr(ey)) * m_dx_end)
        {
            m_dx       <<= line_subpixel_shift;
            m_dy       <<= line_subpixel_shift;
            m_dx_start <<= line_mr_subpixel_shift;
            m_dy_start <<= line_mr_subpixel_shift;
            m_dx_end   <<= line_mr_subpixel_shift;
            m_dy_end   <<= line_mr_subpixel_shift;
        }

        //---------------------------------------------------------------------
        void inc_x() { m_dist += m_dy; m_dist_start += m_dy_start; m_dist_end += m_dy_end; }
        void dec_x() { m_dist -= m_dy; m_dist_start -= m_dy_start; m_dist_end -= m_dy_end; }
        void inc_y() { m_dist -= m_dx; m_dist_start -= m_dx_start; m_dist_end -= m_dx_end; }
        void dec_y() { m_dist += m_dx; m_dist_start += m_dx_start; m_dist_end += m_dx_end; }

        //---------------------------------------------------------------------
        void inc_x(int dy)
        {
            m_dist       += m_dy; 
            m_dist_start += m_dy_start; 
            m_dist_end   += m_dy_end;
            if(dy > 0)
            {
                m_dist       -= m_dx; 
                m_dist_start -= m_dx_start; 
                m_dist_end   -= m_dx_end;
            }
            if(dy < 0)
            {
                m_dist       += m_dx; 
                m_dist_start += m_dx_start; 
                m_dist_end   += m_dx_end;
            }
        }

        //---------------------------------------------------------------------
        void dec_x(int dy)
        {
            m_dist       -= m_dy; 
            m_dist_start -= m_dy_start; 
            m_dist_end   -= m_dy_end;
            if(dy > 0)
            {
                m_dist       -= m_dx; 
                m_dist_start -= m_dx_start; 
                m_dist_end   -= m_dx_end;
            }
            if(dy < 0)
            {
                m_dist       += m_dx; 
                m_dist_start += m_dx_start; 
                m_dist_end   += m_dx_end;
            }
        }

        //---------------------------------------------------------------------
        void inc_y(int dx)
        {
            m_dist       -= m_dx; 
            m_dist_start -= m_dx_start; 
            m_dist_end   -= m_dx_end;
            if(dx > 0)
            {
                m_dist       += m_dy; 
                m_dist_start += m_dy_start; 
                m_dist_end   += m_dy_end;
            }
            if(dx < 0)
            {
                m_dist       -= m_dy; 
                m_dist_start -= m_dy_start; 
                m_dist_end   -= m_dy_end;
            }
        }

        //---------------------------------------------------------------------
        void dec_y(int dx)
        {
            m_dist       += m_dx; 
            m_dist_start += m_dx_start; 
            m_dist_end   += m_dx_end;
            if(dx > 0)
            {
                m_dist       += m_dy; 
                m_dist_start += m_dy_start; 
                m_dist_end   += m_dy_end;
            }
            if(dx < 0)
            {
                m_dist       -= m_dy; 
                m_dist_start -= m_dy_start; 
                m_dist_end   -= m_dy_end;
            }
        }

        //---------------------------------------------------------------------
        int dist()       const { return m_dist;       }
        int dist_start() const { return m_dist_start; }
        int dist_end()   const { return m_dist_end;   }

        //---------------------------------------------------------------------
        int dx()       const { return m_dx;       }
        int dy()       const { return m_dy;       }
        int dx_start() const { return m_dx_start; }
        int dy_start() const { return m_dy_start; }
        int dx_end()   const { return m_dx_end;   }
        int dy_end()   const { return m_dy_end;   }

    private:
        //---------------------------------------------------------------------
        int m_dx;
        int m_dy;
        int m_dx_start;
        int m_dy_start;
        int m_dx_end;
        int m_dy_end;

        int m_dist;
        int m_dist_start;
        int m_dist_end;
    };




    
    //=========================================================================
    template<class Renderer> class line_interpolator_aa_base
    {
    public:
        typedef Renderer renderer_type;
        typedef typename Renderer::color_type color_type;
        typedef typename Renderer::alpha_type alpha_type;

        //---------------------------------------------------------------------
        enum 
        { 
            max_half_width = 64
        };

        //---------------------------------------------------------------------
        line_interpolator_aa_base(renderer_type& ren, const line_parameters& lp) :
            m_lp(lp),
            m_li(lp.vertical ? line_dbl_hr(lp.x2 - lp.x1) :
                               line_dbl_hr(lp.y2 - lp.y1),
                 lp.vertical ? abs(lp.y2 - lp.y1) : 
                               abs(lp.x2 - lp.x1) + 1),
            m_ren(ren),
            m_len((lp.vertical == (lp.inc > 0)) ? -lp.len : lp.len),
            m_x(lp.x1 >> line_subpixel_shift),
            m_y(lp.y1 >> line_subpixel_shift),
            m_old_x(m_x),
            m_old_y(m_y),
            m_count((lp.vertical ? abs((lp.y2 >> line_subpixel_shift) - m_y) :
                                   abs((lp.x2 >> line_subpixel_shift) - m_x))),
            m_width(ren.subpixel_width()),
            m_max_extent(m_width >> (line_subpixel_shift - 2)),
            m_step(0)
        {
            agg::dda2_line_interpolator li(0, lp.vertical ? 
                                              (lp.dy << agg::line_subpixel_shift) :
                                              (lp.dx << agg::line_subpixel_shift),
                                           lp.len);

            unsigned i;
            int stop = m_width + line_subpixel_size * 2;
            for(i = 0; i < max_half_width; ++i)
            {
                m_dist[i] = li.y();
                if(m_dist[i] >= stop) break;
                ++li;
            }
            m_dist[i++] = 0x7FFF0000;
        }

        //---------------------------------------------------------------------
        template<class DI> int step_hor_base(DI& di)
        {
            ++m_li;
            m_x += m_lp.inc;
            m_y = (m_lp.y1 + m_li.y()) >> line_subpixel_shift;

            if(m_lp.inc > 0) di.inc_x(m_y - m_old_y);
            else             di.dec_x(m_y - m_old_y);

            m_old_y = m_y;

            return di.dist() / m_len;
        }

        //---------------------------------------------------------------------
        template<class DI> int step_ver_base(DI& di)
        {
            ++m_li;
            m_y += m_lp.inc;
            m_x = (m_lp.x1 + m_li.y()) >> line_subpixel_shift;

            if(m_lp.inc > 0) di.inc_y(m_x - m_old_x);
            else             di.dec_y(m_x - m_old_x);

            m_old_x = m_x;

            return di.dist() / m_len;
        }

        //---------------------------------------------------------------------
        bool vertical() const { return m_lp.vertical; }
        int  width() const { return m_width; }
        int  count() const { return m_count; }

    protected:
        const line_parameters& m_lp;
        dda2_line_interpolator m_li;
        renderer_type&         m_ren;
        int m_len;
        int m_x;
        int m_y;
        int m_old_x;
        int m_old_y;
        int m_count;
        int m_width;
        int m_max_extent;
        int m_step;
        int m_dist[max_half_width + 1];
        alpha_type m_alphas[max_half_width * 2 + 4];
    };







    //=========================================================================
    template<class Renderer> class line_interpolator_aa0 :
    public line_interpolator_aa_base<Renderer>
    {
    public:
        typedef Renderer renderer_type;
        typedef typename Renderer::color_type color_type;
        typedef typename Renderer::alpha_type alpha_type;

        //---------------------------------------------------------------------
        line_interpolator_aa0(renderer_type& ren, const line_parameters& lp) :
            line_interpolator_aa_base<Renderer>(ren, lp),
            m_di(lp.x1, lp.y1, lp.x2, lp.y2, 
                 lp.x1 & ~line_subpixel_mask, lp.y1 & ~line_subpixel_mask)
        {
            m_li.adjust_forward();
        }

        //---------------------------------------------------------------------
        bool step_hor()
        {
            int dist;
            int dy;
            int s1 = step_hor_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            *p1++ = m_ren.alpha(s1);

            dy = 1;
            while((dist = m_dist[dy] - s1) <= m_width)
            {
                *p1++ = m_ren.alpha(dist);
                ++dy;
            }

            dy = 1;
            while((dist = m_dist[dy] + s1) <= m_width)
            {
                *--p0 = m_ren.alpha(dist);
                ++dy;
            }
            m_ren.blend_solid_vspan(m_x, 
                                    m_y - dy + 1, 
                                    unsigned(p1 - p0), 
                                    p0);
            return ++m_step < m_count;
        }

        //---------------------------------------------------------------------
        bool step_ver()
        {
            int dist;
            int dx;
            int s1 = step_ver_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            *p1++ = m_ren.alpha(s1);

            dx = 1;
            while((dist = m_dist[dx] - s1) <= m_width)
            {
                *p1++ = m_ren.alpha(dist);
                ++dx;
            }

            dx = 1;
            while((dist = m_dist[dx] + s1) <= m_width)
            {
                *--p0 = m_ren.alpha(dist);
                ++dx;
            }
            m_ren.blend_solid_hspan(m_x - dx + 1, 
                                    m_y,
                                    unsigned(p1 - p0), 
                                    p0);
            return ++m_step < m_count;
        }

    private:
        //---------------------------------------------------------------------
        distance_interpolator1 m_di; 
    };






    //=========================================================================
    template<class Renderer> class line_interpolator_aa1 :
    public line_interpolator_aa_base<Renderer>
    {
    public:
        typedef Renderer renderer_type;
        typedef typename Renderer::color_type color_type;
        typedef typename Renderer::alpha_type alpha_type;

        //---------------------------------------------------------------------
        line_interpolator_aa1(renderer_type& ren, const line_parameters& lp, 
                              int sx, int sy) :
            line_interpolator_aa_base<Renderer>(ren, lp),
            m_di(lp.x1, lp.y1, lp.x2, lp.y2, sx, sy,
                 lp.x1 & ~line_subpixel_mask, lp.y1 & ~line_subpixel_mask)
        {
            int dist1_start;
            int dist2_start;

            int npix = 1;

            if(lp.vertical)
            {
                do
                {
                    --m_li;
                    m_y -= lp.inc;
                    m_x = (m_lp.x1 + m_li.y()) >> line_subpixel_shift;

                    if(lp.inc > 0) m_di.dec_y(m_x - m_old_x);
                    else           m_di.inc_y(m_x - m_old_x);

                    m_old_x = m_x;

                    dist1_start = dist2_start = m_di.dist_start(); 

                    int dx = 0;
                    if(dist1_start < 0) ++npix;
                    do
                    {
                        dist1_start += m_di.dy_start();
                        dist2_start -= m_di.dy_start();
                        if(dist1_start < 0) ++npix;
                        if(dist2_start < 0) ++npix;
                        ++dx;
                    }
                    while(m_dist[dx] <= m_width);
                    --m_step;
                    if(npix == 0) break;
                    npix = 0;
                }
                while(m_step >= -m_max_extent);
            }
            else
            {
                do
                {
                    --m_li;
                    m_x -= lp.inc;
                    m_y = (m_lp.y1 + m_li.y()) >> line_subpixel_shift;

                    if(lp.inc > 0) m_di.dec_x(m_y - m_old_y);
                    else           m_di.inc_x(m_y - m_old_y);

                    m_old_y = m_y;

                    dist1_start = dist2_start = m_di.dist_start(); 

                    int dy = 0;
                    if(dist1_start < 0) ++npix;
                    do
                    {
                        dist1_start -= m_di.dx_start();
                        dist2_start += m_di.dx_start();
                        if(dist1_start < 0) ++npix;
                        if(dist2_start < 0) ++npix;
                        ++dy;
                    }
                    while(m_dist[dy] <= m_width);
                    --m_step;
                    if(npix == 0) break;
                    npix = 0;
                }
                while(m_step >= -m_max_extent);
            }
            m_li.adjust_forward();
        }

        //---------------------------------------------------------------------
        bool step_hor()
        {
            int dist_start;
            int dist;
            int dy;
            int s1 = step_hor_base(m_di);

            dist_start = m_di.dist_start();
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            *p1 = 0;
            if(dist_start <= 0)
            {
                *p1 = m_ren.alpha(s1);
            }
            ++p1;

            dy = 1;
            while((dist = m_dist[dy] - s1) <= m_width)
            {
                dist_start -= m_di.dx_start();
                *p1 = 0;
                if(dist_start <= 0)
                {   
                    *p1 = m_ren.alpha(dist);
                }
                ++p1;
                ++dy;
            }

            dy = 1;
            dist_start = m_di.dist_start();
            while((dist = m_dist[dy] + s1) <= m_width)
            {
                dist_start += m_di.dx_start();
                *--p0 = 0;
                if(dist_start <= 0)
                {   
                    *p0 = m_ren.alpha(dist);
                }
                ++dy;
            }

            m_ren.blend_solid_vspan(m_x, 
                                    m_y - dy + 1,
                                    unsigned(p1 - p0), 
                                    p0);
            return ++m_step < m_count;
        }

        //---------------------------------------------------------------------
        bool step_ver()
        {
            int dist_start;
            int dist;
            int dx;
            int s1 = step_ver_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            dist_start = m_di.dist_start();

            *p1 = 0;
            if(dist_start <= 0)
            {
                *p1 = m_ren.alpha(s1);
            }
            ++p1;

            dx = 1;
            while((dist = m_dist[dx] - s1) <= m_width)
            {
                dist_start += m_di.dy_start();
                *p1 = 0;
                if(dist_start <= 0)
                {   
                    *p1 = m_ren.alpha(dist);
                }
                ++p1;
                ++dx;
            }

            dx = 1;
            dist_start = m_di.dist_start();
            while((dist = m_dist[dx] + s1) <= m_width)
            {
                dist_start -= m_di.dy_start();
                *--p0 = 0;
                if(dist_start <= 0)
                {   
                    *p0 = m_ren.alpha(dist);
                }
                ++dx;
            }
            m_ren.blend_solid_hspan(m_x - dx + 1, 
                                    m_y,
                                    unsigned(p1 - p0), 
                                    p0);
            return ++m_step < m_count;
        }

    private:
        //---------------------------------------------------------------------
        distance_interpolator2 m_di; 
    };











    //=========================================================================
    template<class Renderer> class line_interpolator_aa2 :
    public line_interpolator_aa_base<Renderer>
    {
    public:
        typedef Renderer renderer_type;
        typedef typename Renderer::color_type color_type;
        typedef typename Renderer::alpha_type alpha_type;

        //---------------------------------------------------------------------
        line_interpolator_aa2(renderer_type& ren, const line_parameters& lp, 
                              int ex, int ey) :
            line_interpolator_aa_base<Renderer>(ren, lp),
            m_di(lp.x1, lp.y1, lp.x2, lp.y2, ex, ey, 
                 lp.x1 & ~line_subpixel_mask, lp.y1 & ~line_subpixel_mask,
                 0)
        {
            m_li.adjust_forward();
            m_step -= m_max_extent;
        }

        //---------------------------------------------------------------------
        bool step_hor()
        {
            int dist_end;
            int dist;
            int dy;
            int s1 = step_hor_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            dist_end = m_di.dist_end();

            int npix = 0;
            *p1 = 0;
            if(dist_end > 0)
            {
                *p1 = m_ren.alpha(s1);
                ++npix;
            }
            ++p1;

            dy = 1;
            while((dist = m_dist[dy] - s1) <= m_width)
            {
                dist_end -= m_di.dx_end();
                *p1 = 0;
                if(dist_end > 0)
                {   
                    *p1 = m_ren.alpha(dist);
                    ++npix;
                }
                ++p1;
                ++dy;
            }

            dy = 1;
            dist_end = m_di.dist_end();
            while((dist = m_dist[dy] + s1) <= m_width)
            {
                dist_end += m_di.dx_end();
                *--p0 = 0;
                if(dist_end > 0)
                {   
                    *p0 = m_ren.alpha(dist);
                    ++npix;
                }
                ++dy;
            }
            m_ren.blend_solid_vspan(m_x,
                                    m_y - dy + 1, 
                                    unsigned(p1 - p0), 
                                    p0);
            return npix && ++m_step < m_count;
        }

        //---------------------------------------------------------------------
        bool step_ver()
        {
            int dist_end;
            int dist;
            int dx;
            int s1 = step_ver_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            dist_end = m_di.dist_end();

            int npix = 0;
            *p1 = 0;
            if(dist_end > 0)
            {
                *p1 = m_ren.alpha(s1);
                ++npix;
            }
            ++p1;

            dx = 1;
            while((dist = m_dist[dx] - s1) <= m_width)
            {
                dist_end += m_di.dy_end();
                *p1 = 0;
                if(dist_end > 0)
                {   
                    *p1 = m_ren.alpha(dist);
                    ++npix;
                }
                ++p1;
                ++dx;
            }

            dx = 1;
            dist_end = m_di.dist_end();
            while((dist = m_dist[dx] + s1) <= m_width)
            {
                dist_end -= m_di.dy_end();
                *--p0 = 0;
                if(dist_end > 0)
                {   
                    *p0 = m_ren.alpha(dist);
                    ++npix;
                }
                ++dx;
            }
            m_ren.blend_solid_hspan(m_x - dx + 1,
                                    m_y, 
                                    unsigned(p1 - p0), 
                                    p0);
            return npix && ++m_step < m_count;
        }

    private:
        //---------------------------------------------------------------------
        distance_interpolator2 m_di; 
    };










    //=========================================================================
    template<class Renderer> class line_interpolator_aa3 :
    public line_interpolator_aa_base<Renderer>
    {
    public:
        typedef Renderer renderer_type;
        typedef typename Renderer::color_type color_type;
        typedef typename Renderer::alpha_type alpha_type;

        //---------------------------------------------------------------------
        line_interpolator_aa3(renderer_type& ren, const line_parameters& lp, 
                              int sx, int sy, int ex, int ey) :
            line_interpolator_aa_base<Renderer>(ren, lp),
            m_di(lp.x1, lp.y1, lp.x2, lp.y2, sx, sy, ex, ey, 
                 lp.x1 & ~line_subpixel_mask, lp.y1 & ~line_subpixel_mask)
        {
            int dist1_start;
            int dist2_start;
            int npix = 1;
            if(lp.vertical)
            {
                do
                {
                    --m_li;
                    m_y -= lp.inc;
                    m_x = (m_lp.x1 + m_li.y()) >> line_subpixel_shift;

                    if(lp.inc > 0) m_di.dec_y(m_x - m_old_x);
                    else           m_di.inc_y(m_x - m_old_x);

                    m_old_x = m_x;

                    dist1_start = dist2_start = m_di.dist_start(); 

                    int dx = 0;
                    if(dist1_start < 0) ++npix;
                    do
                    {
                        dist1_start += m_di.dy_start();
                        dist2_start -= m_di.dy_start();
                        if(dist1_start < 0) ++npix;
                        if(dist2_start < 0) ++npix;
                        ++dx;
                    }
                    while(m_dist[dx] <= m_width);
                    if(npix == 0) break;
                    npix = 0;
                }
                while(--m_step >= -m_max_extent);
            }
            else
            {
                do
                {
                    --m_li;
                    m_x -= lp.inc;
                    m_y = (m_lp.y1 + m_li.y()) >> line_subpixel_shift;

                    if(lp.inc > 0) m_di.dec_x(m_y - m_old_y);
                    else           m_di.inc_x(m_y - m_old_y);

                    m_old_y = m_y;

                    dist1_start = dist2_start = m_di.dist_start(); 

                    int dy = 0;
                    if(dist1_start < 0) ++npix;
                    do
                    {
                        dist1_start -= m_di.dx_start();
                        dist2_start += m_di.dx_start();
                        if(dist1_start < 0) ++npix;
                        if(dist2_start < 0) ++npix;
                        ++dy;
                    }
                    while(m_dist[dy] <= m_width);
                    if(npix == 0) break;
                    npix = 0;
                }
                while(--m_step >= -m_max_extent);
            }
            m_li.adjust_forward();
            m_step -= m_max_extent;
        }


        //---------------------------------------------------------------------
        bool step_hor()
        {
            int dist_start;
            int dist_end;
            int dist;
            int dy;
            int s1 = step_hor_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            dist_start = m_di.dist_start();
            dist_end   = m_di.dist_end();

            int npix = 0;
            *p1 = 0;
            if(dist_end > 0)
            {
                if(dist_start <= 0)
                {
                    *p1 = m_ren.alpha(s1);
                }
                ++npix;
            }
            ++p1;

            dy = 1;
            while((dist = m_dist[dy] - s1) <= m_width)
            {
                dist_start -= m_di.dx_start();
                dist_end   -= m_di.dx_end();
                *p1 = 0;
                if(dist_end > 0 && dist_start <= 0)
                {   
                    *p1 = m_ren.alpha(dist);
                    ++npix;
                }
                ++p1;
                ++dy;
            }

            dy = 1;
            dist_start = m_di.dist_start();
            dist_end   = m_di.dist_end();
            while((dist = m_dist[dy] + s1) <= m_width)
            {
                dist_start += m_di.dx_start();
                dist_end   += m_di.dx_end();
                *--p0 = 0;
                if(dist_end > 0 && dist_start <= 0)
                {   
                    *p0 = m_ren.alpha(dist);
                    ++npix;
                }
                ++dy;
            }
            m_ren.blend_solid_vspan(m_x,
                                    m_y - dy + 1, 
                                    unsigned(p1 - p0), 
                                    p0);
            return npix && ++m_step < m_count;
        }

        //---------------------------------------------------------------------
        bool step_ver()
        {
            int dist_start;
            int dist_end;
            int dist;
            int dx;
            int s1 = step_ver_base(m_di);
            alpha_type* p0 = m_alphas + max_half_width + 2;
            alpha_type* p1 = p0;

            dist_start = m_di.dist_start();
            dist_end   = m_di.dist_end();

            int npix = 0;
            *p1 = 0;
            if(dist_end > 0)
            {
                if(dist_start <= 0)
                {
                    *p1 = m_ren.alpha(s1);
                }
                ++npix;
            }
            ++p1;

            dx = 1;
            while((dist = m_dist[dx] - s1) <= m_width)
            {
                dist_start += m_di.dy_start();
                dist_end   += m_di.dy_end();
                *p1 = 0;
                if(dist_end > 0 && dist_start <= 0)
                {   
                    *p1 = m_ren.alpha(dist);
                    ++npix;
                }
                ++p1;
                ++dx;
            }

            dx = 1;
            dist_start = m_di.dist_start();
            dist_end   = m_di.dist_end();
            while((dist = m_dist[dx] + s1) <= m_width)
            {
                dist_start -= m_di.dy_start();
                dist_end   -= m_di.dy_end();
                *--p0 = 0;
                if(dist_end > 0 && dist_start <= 0)
                {   
                    *p0 = m_ren.alpha(dist);
                    ++npix;
                }
                ++dx;
            }
            m_ren.blend_solid_hspan(m_x - dx + 1,
                                    m_y, 
                                    unsigned(p1 - p0), 
                                    p0);
            return npix && ++m_step < m_count;
        }

    private:
        //---------------------------------------------------------------------
        distance_interpolator3 m_di; 
    };




    //=========================================================================
    class line_profile_aa
    {
    public:
        //---------------------------------------------------------------------
        typedef int8u value_type;
        enum
        {
            subpixel_shift = line_subpixel_shift,
            subpixel_size  = 1 << subpixel_shift,
            subpixel_mask  = subpixel_size - 1
        };

        enum
        {
            aa_shift = 8,
            aa_num   = 1 << aa_shift,
            aa_mask  = aa_num - 1
        };
        
        //---------------------------------------------------------------------
        ~line_profile_aa() { delete [] m_profile; }

        //---------------------------------------------------------------------
        line_profile_aa() : 
            m_size(0), 
            m_profile(0), 
            m_subpixel_width(0),
            m_min_width(1.0),
            m_smoother_width(1.0)
        {
            int i;
            for(i = 0; i < aa_num; i++) m_gamma[i] = i;
        }

        //---------------------------------------------------------------------
        template<class GammaF> 
        line_profile_aa(double w, const GammaF& gamma_function) : 
            m_size(0), 
            m_profile(0), 
            m_subpixel_width(0),
            m_min_width(1.0),
            m_smoother_width(1.0)
        {
            gamma(gamma_function);
            width(w);
        }

        //---------------------------------------------------------------------
        void min_width(double w) { m_min_width = w; }
        void smoother_width(double w) { m_smoother_width = w; }

        //---------------------------------------------------------------------
        template<class GammaF> void gamma(const GammaF& gamma_function)
        { 
            int i;
            for(i = 0; i < aa_num; i++)
            {
                m_gamma[i] = int(gamma_function(double(i) / aa_mask) * aa_mask);
            }
        }

        void width(double w);

        unsigned profile_size() const { return m_size; }
        int subpixel_width() const { return m_subpixel_width; }

        //---------------------------------------------------------------------
        double min_width() const { return m_min_width; }
        double smoother_width() const { return m_smoother_width; }

        //---------------------------------------------------------------------
        value_type value(int dist) const
        {
            return m_profile[dist + subpixel_size*2];
        }

    private:
        value_type* profile(double w);
        void set(double center_width, double smoother_width);

        line_profile_aa(const line_profile_aa&);
        const line_profile_aa& operator = (const line_profile_aa&);

        //---------------------------------------------------------------------
        unsigned    m_size;
        value_type* m_profile;
        value_type  m_gamma[aa_num];
        int         m_subpixel_width;
        double      m_min_width;
        double      m_smoother_width;
    };






    //=========================================================================
    template<class BaseRenderer> class renderer_outline_aa
    {
    public:
        //---------------------------------------------------------------------
        typedef BaseRenderer base_ren_type;
        typedef renderer_outline_aa<base_ren_type> self_type;
        typedef typename base_ren_type::color_type color_type;
        typedef typename base_ren_type::color_type::alpha_type alpha_type;

        //---------------------------------------------------------------------
        renderer_outline_aa(base_ren_type& ren, line_profile_aa& prof) :
            m_ren(&ren),
            m_profile(&prof)
        {
        }

        //---------------------------------------------------------------------
        void color(const color_type& c) { m_color = c; }
        const color_type& color() const { return m_color; }

        //---------------------------------------------------------------------
        void profile(const line_profile_aa& prof) { m_profile = &prof; }
        const line_profile_aa& profile() const { return *m_profile; }
        line_profile_aa& profile() { return *m_profile; }

        //---------------------------------------------------------------------
        int subpixel_width() const { return m_profile->subpixel_width(); }

        //---------------------------------------------------------------------
        int alpha(int d) const
        {
            return m_profile->value(d);
        }

        //-------------------------------------------------------------------------
        void blend_solid_hspan(int x, int y, unsigned len, const alpha_type* covers)
        {
            m_ren->blend_solid_hspan(x, y, len, m_color, covers);
        }

        //-------------------------------------------------------------------------
        void blend_solid_vspan(int x, int y, unsigned len, const alpha_type* covers)
        {
            m_ren->blend_solid_vspan(x, y, len, m_color, covers);
        }

        //-------------------------------------------------------------------------
        static bool accurate_join_only() { return false; }

        //-------------------------------------------------------------------------
        template<class Cmp>
        void semidot_hline(Cmp cmp,
                           int xc1, int yc1, int xc2, int yc2, 
                           int x1,  int y1,  int x2)
        {
            alpha_type alphas[line_interpolator_aa_base<self_type>::max_half_width * 2 + 4];
            alpha_type* p0 = alphas;
            alpha_type* p1 = alphas;
            int x = x1 << line_subpixel_shift;
            int y = y1 << line_subpixel_shift;
            int w = subpixel_width();
            distance_interpolator0 di(xc1, yc1, xc2, yc2, x, y);
            x += line_subpixel_size/2;
            y += line_subpixel_size/2;

            int x0 = x1;
            int dx = x - xc1;
            int dy = y - yc1;
            do
            {
                int d = int(fast_sqrt(dx*dx + dy*dy));
                *p1 = 0;
                if(cmp(di.dist()) && d <= w)
                {
                    *p1 = alpha(d);
                }
                ++p1;
                dx += line_subpixel_size;
                di.inc_x();
            }
            while(++x1 <= x2);
            m_ren->blend_solid_hspan(x0, y1, 
                                     unsigned(p1 - p0), 
                                     color(), 
                                     p0);
        }

        //-------------------------------------------------------------------------
        template<class Cmp> 
        void semidot(Cmp cmp, int xc1, int yc1, int xc2, int yc2)
        {
            int r = ((subpixel_width() + line_subpixel_mask) >> line_subpixel_shift);
            if(r < 1) r = 1;
            ellipse_bresenham_interpolator ei(r, r);
            int dx = 0;
            int dy = -r;
            int dy0 = dy;
            int dx0 = dx;
            int x = xc1 >> line_subpixel_shift;
            int y = yc1 >> line_subpixel_shift;

            do
            {
                dx += ei.dx();
                dy += ei.dy();

                if(dy != dy0)
                {
                    semidot_hline(cmp, xc1, yc1, xc2, yc2, x-dx0, y+dy0, x+dx0);
                    semidot_hline(cmp, xc1, yc1, xc2, yc2, x-dx0, y-dy0, x+dx0);
                }
                dx0 = dx;
                dy0 = dy;
                ++ei;
            }
            while(dy < 0);
            semidot_hline(cmp, xc1, yc1, xc2, yc2, x-dx0, y+dy0, x+dx0);
        }

        //-------------------------------------------------------------------------
        void line0(const line_parameters& lp)
        {
            line_interpolator_aa0<self_type> li(*this, lp);
            if(li.count())
            {
                if(li.vertical())
                {
                    while(li.step_ver());
                }
                else
                {
                    while(li.step_hor());
                }
            }
        }

        //-------------------------------------------------------------------------
        void line1(const line_parameters& lp, int sx, int sy)
        {
            line_interpolator_aa1<self_type> li(*this, lp, sx, sy);
            if(li.vertical())
            {
                while(li.step_ver());
            }
            else
            {
                while(li.step_hor());
            }
        }

        //-------------------------------------------------------------------------
        void line2(const line_parameters& lp, int ex, int ey)
        {
            line_interpolator_aa2<self_type> li(*this, lp, ex, ey);
            if(li.vertical())
            {
                while(li.step_ver());
            }
            else
            {
                while(li.step_hor());
            }
        }

        //-------------------------------------------------------------------------
        void line3(const line_parameters& lp, 
                   int sx, int sy, int ex, int ey)
        {
            line_interpolator_aa3<self_type> li(*this, lp, sx, sy, ex, ey);
            if(li.vertical())
            {
                while(li.step_ver());
            }
            else
            {
                while(li.step_hor());
            }
        }

    private:
        base_ren_type*   m_ren;
        line_profile_aa* m_profile; 
        color_type       m_color;
    };



}

#endif
