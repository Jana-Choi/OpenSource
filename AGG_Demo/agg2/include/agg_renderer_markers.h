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
// class renderer_markers
//
//----------------------------------------------------------------------------

#ifndef AGG_RENDERER_MARKERS_INCLUDED
#define AGG_RENDERER_MARKERS_INCLUDED

#include "agg_basics.h"
#include "agg_renderer_primitives.h"

namespace agg
{

    //------------------------------------------------------------------------
    enum marker_e
    {
        marker_square,
        marker_diamond,
        marker_circle,
        marker_crossed_circle,
        marker_semiellipse_left,
        marker_semiellipse_right,
        marker_semiellipse_up,
        marker_semiellipse_down,
        marker_triangle_left,
        marker_triangle_right,
        marker_triangle_up,
        marker_triangle_down,
        marker_four_rays,
        marker_cross,
        marker_x,
        marker_dash,
        marker_dot,
        marker_pixel,
        
        end_of_markers
    };



    //------------------------------------------------------------------------
    template<class BaseRenderer> class renderer_markers :
    public renderer_primitives<BaseRenderer>
    {
    public:
        typedef renderer_primitives<BaseRenderer> base_type;
        typedef BaseRenderer base_ren_type;
        typedef typename base_ren_type::color_type color_type;

        //--------------------------------------------------------------------
        renderer_markers(base_ren_type& rbuf) :
            base_type(rbuf)
        {
        }

        //--------------------------------------------------------------------
        bool visible(int x, int y, int r) const
        {
            rect rc(x-r, y-r, x+y, y+r);
            return rc.clip(ren().bounding_clip_box());  
        }

        //--------------------------------------------------------------------
        void square(int x, int y, int r)
        {
            if(visible(x, y, r)) 
            {  
                if(r) outlined_rectangle(x-r, y-r, x+r, y+r);
                else  ren().blend_pixel(x, y, fill_color(), color_type::opaque());
            }
        }

        //--------------------------------------------------------------------
        void diamond(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    do
                    {
                        ren().blend_pixel(x - dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dx, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y - dy, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_hline(x-dx+1, y+dy, x+dx-1, fill_color(), color_type::opaque());
                            ren().blend_hline(x-dx+1, y-dy, x+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        ++dx;
                    }
                    while(dy <= 0);
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }

        //--------------------------------------------------------------------
        void circle(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r) outlined_ellipse(x, y, r, r);
                else  ren().blend_pixel(x, y, fill_color(), color_type::opaque());
            }
        }



        //--------------------------------------------------------------------
        void crossed_circle(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    outlined_ellipse(x, y, r, r);
                    int r6 = r + (r >> 1);
                    if(r <= 2) r6++;
                    r >>= 1;
                    ren().blend_hline(x-r6, y, x-r,  line_color(), color_type::opaque());
                    ren().blend_hline(x+r,  y, x+r6, line_color(), color_type::opaque());
                    ren().blend_vline(x, y-r6, y-r,  line_color(), color_type::opaque());
                    ren().blend_vline(x, y+r,  y+r6, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //------------------------------------------------------------------------
        void semiellipse_left(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int r8 = r * 4 / 5;
                    int dy = -r;
                    int dx = 0;
                    ellipse_bresenham_interpolator ei(r * 3 / 5, r+r8);
                    do
                    {
                        dx += ei.dx();
                        dy += ei.dy();
                        
                        ren().blend_pixel(x + dy, y + dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dy, y - dx, line_color(), color_type::opaque());
                        
                        if(ei.dy() && dx)
                        {
                            ren().blend_vline(x+dy, y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                        }
                        ++ei;
                    }
                    while(dy < r8);
                    ren().blend_vline(x+dy, y-dx, y+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void semiellipse_right(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int r8 = r * 4 / 5;
                    int dy = -r;
                    int dx = 0;
                    ellipse_bresenham_interpolator ei(r * 3 / 5, r+r8);
                    do
                    {
                        dx += ei.dx();
                        dy += ei.dy();
                        
                        ren().blend_pixel(x - dy, y + dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y - dx, line_color(), color_type::opaque());
                        
                        if(ei.dy() && dx)
                        {
                            ren().blend_vline(x-dy, y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                        }
                        ++ei;
                    }
                    while(dy < r8);
                    ren().blend_vline(x-dy, y-dx, y+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void semiellipse_up(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int r8 = r * 4 / 5;
                    int dy = -r;
                    int dx = 0;
                    ellipse_bresenham_interpolator ei(r * 3 / 5, r+r8);
                    do
                    {
                        dx += ei.dx();
                        dy += ei.dy();
                        
                        ren().blend_pixel(x + dx, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dx, y - dy, line_color(), color_type::opaque());
                        
                        if(ei.dy() && dx)
                        {
                            ren().blend_hline(x-dx+1, y-dy, x+dx-1, fill_color(), color_type::opaque());
                        }
                        ++ei;
                    }
                    while(dy < r8);
                    ren().blend_hline(x-dx, y-dy-1, x+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void semiellipse_down(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int r8 = r * 4 / 5;
                    int dy = -r;
                    int dx = 0;
                    ellipse_bresenham_interpolator ei(r * 3 / 5, r+r8);
                    do
                    {
                        dx += ei.dx();
                        dy += ei.dy();
                        
                        ren().blend_pixel(x + dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dx, y + dy, line_color(), color_type::opaque());
                        
                        if(ei.dy() && dx)
                        {
                            ren().blend_hline(x-dx+1, y+dy, x+dx-1, fill_color(), color_type::opaque());
                        }
                        ++ei;
                    }
                    while(dy < r8);
                    ren().blend_hline(x-dx, y+dy+1, x+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void triangle_left(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    int flip = 0;
                    int r6 = r * 3 / 5;
                    do
                    {
                        ren().blend_pixel(x + dy, y - dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dy, y + dx, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_vline(x+dy, y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        dx += flip;
                        flip ^= 1;
                    }
                    while(dy < r6);
                    ren().blend_vline(x+dy, y-dx, y+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void triangle_right(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    int flip = 0;
                    int r6 = r * 3 / 5;
                    do
                    {
                        ren().blend_pixel(x - dy, y - dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y + dx, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_vline(x-dy, y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        dx += flip;
                        flip ^= 1;
                    }
                    while(dy < r6);
                    ren().blend_vline(x-dy, y-dx, y+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void triangle_up(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    int flip = 0;
                    int r6 = r * 3 / 5;
                    do
                    {
                        ren().blend_pixel(x - dx, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y - dy, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_hline(x-dx+1, y-dy, x+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        dx += flip;
                        flip ^= 1;
                    }
                    while(dy < r6);
                    ren().blend_hline(x-dx, y-dy, x+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void triangle_down(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    int flip = 0;
                    int r6 = r * 3 / 5;
                    do
                    {
                        ren().blend_pixel(x - dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y + dy, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_hline(x-dx+1, y+dy, x+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        dx += flip;
                        flip ^= 1;
                    }
                    while(dy < r6);
                    ren().blend_hline(x-dx, y+dy, x+dx, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void four_rays(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r;
                    int dx = 0;
                    int flip = 0;
                    int r3 = -(r / 3);
                    do
                    {
                        ren().blend_pixel(x - dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dx, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dx, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dy, y - dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dy, y + dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y - dx, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y + dx, line_color(), color_type::opaque());
                        
                        if(dx)
                        {
                            ren().blend_hline(x-dx+1, y+dy,   x+dx-1, fill_color(), color_type::opaque());
                            ren().blend_hline(x-dx+1, y-dy,   x+dx-1, fill_color(), color_type::opaque());
                            ren().blend_vline(x+dy,   y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                            ren().blend_vline(x-dy,   y-dx+1, y+dx-1, fill_color(), color_type::opaque());
                        }
                        ++dy;
                        dx += flip;
                        flip ^= 1;
                    }
                    while(dy <= r3);
                    solid_rectangle(x+r3+1, y+r3+1, x-r3-1, y-r3-1);
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }


        //--------------------------------------------------------------------
        void cross(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    ren().blend_vline(x, y-r, y+r, line_color(), color_type::opaque());
                    ren().blend_hline(x-r, y, x+r, line_color(), color_type::opaque());
                }
                else
                {
                    ren().blend_pixel(x, y, fill_color(), color_type::opaque());
                }
            }
        }
        
        
        //--------------------------------------------------------------------
        void xing(int x, int y, int r)
        {
            if(visible(x, y, r))
            {
                if(r)
                {
                    int dy = -r * 7 / 10;
                    do
                    {
                        ren().blend_pixel(x + dy, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y + dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x + dy, y - dy, line_color(), color_type::opaque());
                        ren().blend_pixel(x - dy, y - dy, line_color(), color_type::opaque());
                        ++dy;
                    }
                    while(dy < 0);
                }
                ren().blend_pixel(x, y, fill_color(), color_type::opaque());
            }
        }
        
        
        //--------------------------------------------------------------------
        void dash(int x, int y, int r)
        {
            if(visible(x, y, r)) 
            {
                if(r) ren().blend_hline(x-r, y, x+r, line_color(), color_type::opaque());
                else  ren().blend_pixel(x, y, fill_color(), color_type::opaque());
            }
        }
        
        
        //--------------------------------------------------------------------
        void dot(int x, int y, int r)
        {
            if(visible(x, y, r)) 
            {
                if(r) solid_ellipse(x, y, r, r);
                else  ren().blend_pixel(x, y, fill_color(), color_type::opaque());
            }
        }
        
        //--------------------------------------------------------------------
        void pixel(int x, int y, int)
        {
            ren().blend_pixel(x, y, fill_color(), color_type::opaque());
        }
        
        //--------------------------------------------------------------------
        void marker(int x, int y, int r, marker_e type)
        {
            switch(type)
            {
                case marker_square:            square(x, y, r);            break;
                case marker_diamond:           diamond(x, y, r);           break;
                case marker_circle:            circle(x, y, r);            break;
                case marker_crossed_circle:    crossed_circle(x, y, r);    break;
                case marker_semiellipse_left:  semiellipse_left(x, y, r);  break;
                case marker_semiellipse_right: semiellipse_right(x, y, r); break;
                case marker_semiellipse_up:    semiellipse_up(x, y, r);    break;
                case marker_semiellipse_down:  semiellipse_down(x, y, r);  break;
                case marker_triangle_left:     triangle_left(x, y, r);     break;
                case marker_triangle_right:    triangle_right(x, y, r);    break;
                case marker_triangle_up:       triangle_up(x, y, r);       break;
                case marker_triangle_down:     triangle_down(x, y, r);     break;
                case marker_four_rays:         four_rays(x, y, r);         break;
                case marker_cross:             cross(x, y, r);             break;
                case marker_x:                 xing(x, y, r);              break;
                case marker_dash:              dash(x, y, r);              break;
                case marker_dot:               dot(x, y, r);               break;
                case marker_pixel:             pixel(x, y, r);             break;
            }
        }


        //--------------------------------------------------------------------
        template<class T>
        void markers(int n, const T* x, const T* y, T r, marker_e type)
        {
            if(n <= 0) return;
            if(r == 0)
            {
                do
                {
                    ren().blend_pixel(int(*x), int(*y), fill_color(), color_type::opaque());
                    ++x;
                    ++y;
                }
                while(--n);
                return;
            }
            
            switch(type)
            {
                case marker_square:            do { square           (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_diamond:           do { diamond          (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_circle:            do { circle           (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_crossed_circle:    do { crossed_circle   (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_semiellipse_left:  do { semiellipse_left (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_semiellipse_right: do { semiellipse_right(int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_semiellipse_up:    do { semiellipse_up   (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_semiellipse_down:  do { semiellipse_down (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_triangle_left:     do { triangle_left    (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_triangle_right:    do { triangle_right   (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_triangle_up:       do { triangle_up      (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_triangle_down:     do { triangle_down    (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_four_rays:         do { four_rays        (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_cross:             do { cross            (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_x:                 do { xing             (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_dash:              do { dash             (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_dot:               do { dot              (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
                case marker_pixel:             do { pixel            (int(*x), int(*y), int(r)); ++x; ++y; } while(--n); break;
            }                                                                                  
        }
        
        //--------------------------------------------------------------------
        template<class T>
        void markers(int n, const T* x, const T* y, const T* r, marker_e type)
        {
            if(n <= 0) return;
            switch(type)
            {
                case marker_square:            do { square           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_diamond:           do { diamond          (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_circle:            do { circle           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_crossed_circle:    do { crossed_circle   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_semiellipse_left:  do { semiellipse_left (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_semiellipse_right: do { semiellipse_right(int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_semiellipse_up:    do { semiellipse_up   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_semiellipse_down:  do { semiellipse_down (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_triangle_left:     do { triangle_left    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_triangle_right:    do { triangle_right   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_triangle_up:       do { triangle_up      (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_triangle_down:     do { triangle_down    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_four_rays:         do { four_rays        (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_cross:             do { cross            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_x:                 do { xing             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_dash:              do { dash             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_dot:               do { dot              (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
                case marker_pixel:             do { pixel            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; } while(--n); break;
            }                                                                                  
        }
        
        //--------------------------------------------------------------------
        template<class T>
        void markers(int n, const T* x, const T* y, const T* r, const color_type* fc, marker_e type)
        {
            if(n <= 0) return;
            switch(type)
            {
                case marker_square:            do { fill_color(*fc); square           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_diamond:           do { fill_color(*fc); diamond          (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_circle:            do { fill_color(*fc); circle           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_crossed_circle:    do { fill_color(*fc); crossed_circle   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_semiellipse_left:  do { fill_color(*fc); semiellipse_left (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_semiellipse_right: do { fill_color(*fc); semiellipse_right(int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_semiellipse_up:    do { fill_color(*fc); semiellipse_up   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_semiellipse_down:  do { fill_color(*fc); semiellipse_down (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_triangle_left:     do { fill_color(*fc); triangle_left    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_triangle_right:    do { fill_color(*fc); triangle_right   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_triangle_up:       do { fill_color(*fc); triangle_up      (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_triangle_down:     do { fill_color(*fc); triangle_down    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_four_rays:         do { fill_color(*fc); four_rays        (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_cross:             do { fill_color(*fc); cross            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_x:                 do { fill_color(*fc); xing             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_dash:              do { fill_color(*fc); dash             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_dot:               do { fill_color(*fc); dot              (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
                case marker_pixel:             do { fill_color(*fc); pixel            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; } while(--n); break;
            }
        }
        
        //--------------------------------------------------------------------
        template<class T>
        void markers(int n, const T* x, const T* y, const T* r, const color_type* fc, const color_type* lc, marker_e type)
        {
            if(n <= 0) return;
            switch(type)
            {
                case marker_square:            do { fill_color(*fc); line_color(*lc); square           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_diamond:           do { fill_color(*fc); line_color(*lc); diamond          (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_circle:            do { fill_color(*fc); line_color(*lc); circle           (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_crossed_circle:    do { fill_color(*fc); line_color(*lc); crossed_circle   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_semiellipse_left:  do { fill_color(*fc); line_color(*lc); semiellipse_left (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_semiellipse_right: do { fill_color(*fc); line_color(*lc); semiellipse_right(int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_semiellipse_up:    do { fill_color(*fc); line_color(*lc); semiellipse_up   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_semiellipse_down:  do { fill_color(*fc); line_color(*lc); semiellipse_down (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_triangle_left:     do { fill_color(*fc); line_color(*lc); triangle_left    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_triangle_right:    do { fill_color(*fc); line_color(*lc); triangle_right   (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_triangle_up:       do { fill_color(*fc); line_color(*lc); triangle_up      (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_triangle_down:     do { fill_color(*fc); line_color(*lc); triangle_down    (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_four_rays:         do { fill_color(*fc); line_color(*lc); four_rays        (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_cross:             do { fill_color(*fc); line_color(*lc); cross            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_x:                 do { fill_color(*fc); line_color(*lc); xing             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_dash:              do { fill_color(*fc); line_color(*lc); dash             (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_dot:               do { fill_color(*fc); line_color(*lc); dot              (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
                case marker_pixel:             do { fill_color(*fc); line_color(*lc); pixel            (int(*x), int(*y), int(*r)); ++x; ++y; ++r; ++fc; ++lc; } while(--n); break;
            }
        }
    };

}

#endif
