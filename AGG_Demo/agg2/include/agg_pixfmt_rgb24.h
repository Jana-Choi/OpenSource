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

#ifndef AGG_PIXFMT_RGB24_INCLUDED
#define AGG_PIXFMT_RGB24_INCLUDED

#include <string.h>
#include "agg_basics.h"
#include "agg_color_rgba8.h"
#include "agg_rendering_buffer.h"

namespace agg
{
    struct order_rgb24 { enum { R=0, G=1, B=2, rgb24_tag }; };
    struct order_bgr24 { enum { B=0, G=1, R=2, rgb24_tag }; };

    //========================================================================
    template<class Order> class pixel_formats_rgb24
    {
    public:
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;

        //--------------------------------------------------------------------
        pixel_formats_rgb24(rendering_buffer& rb)
            : m_rbuf(&rb)
        {
        }

        //--------------------------------------------------------------------
        unsigned width()  const { return m_rbuf->width();  }
        unsigned height() const { return m_rbuf->height(); }

        //--------------------------------------------------------------------
        color_type pixel(int x, int y)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            return color_type(p[Order::R], p[Order::G], p[Order::B]);
        }

        //--------------------------------------------------------------------
        void copy_pixel(int x, int y, const color_type& c)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            p[Order::R] = c.r;
            p[Order::G] = c.g;
            p[Order::B] = c.b;
        }

        //--------------------------------------------------------------------
        void blend_pixel(int x, int y, 
                         const color_type& c, 
                         int alpha)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            alpha *= c.a;
            if(alpha == 255*255)
            {
                p[Order::R] = c.r;
                p[Order::G] = c.g;
                p[Order::B] = c.b;
            }
            else
            {
                int r = p[Order::R];
                int g = p[Order::G];
                int b = p[Order::B];
                p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
            }
        }


        //--------------------------------------------------------------------
        void copy_hline(int x, int y, 
                        unsigned len, 
                        const color_type& c)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            do
            {
                p[Order::R] = c.r; 
                p[Order::G] = c.g; 
                p[Order::B] = c.b;
                p += 3;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void copy_vline(int x, int y,
                        unsigned len, 
                        const color_type& c)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            do
            {
                p[Order::R] = c.r; 
                p[Order::G] = c.g; 
                p[Order::B] = c.b;
                p += m_rbuf->stride();
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_hline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int alpha)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            alpha *= c.a;
            if(alpha == 255*255)
            {
                do
                {
                    p[Order::R] = c.r; 
                    p[Order::G] = c.g; 
                    p[Order::B] = c.b;
                    p += 3;
                }
                while(--len);
            }
            else
            {
                do
                {
                    int r = p[Order::R];
                    int g = p[Order::G];
                    int b = p[Order::B];
                    p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                    p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                    p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    p += 3;
                }
                while(--len);
            }
        }


        //--------------------------------------------------------------------
        void blend_vline(int x, int y,
                         unsigned len, 
                         const color_type& c,
                         int alpha)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            alpha *= c.a;
            if(alpha == 255*255)
            {
                do
                {
                    p[Order::R] = c.r; 
                    p[Order::G] = c.g; 
                    p[Order::B] = c.b;
                    p += m_rbuf->stride();
                }
                while(--len);
            }
            else
            {
                do
                {
                    int r = p[Order::R];
                    int g = p[Order::G];
                    int b = p[Order::B];
                    p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                    p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                    p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    p += m_rbuf->stride();
                }
                while(--len);
            }
        }


        //--------------------------------------------------------------------
        void copy_from(const rendering_buffer& from, 
                       int xdst, int ydst,
                       int xsrc, int ysrc,
                       unsigned len)
        {
            memmove(m_rbuf->row(ydst) + xdst * 3, 
                    (const void*)(from.row(ysrc) + xsrc * 3), len * 3);
        }


        //--------------------------------------------------------------------
        void blend_solid_hspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            do 
            {
                int alpha = (*covers++) * c.a;

                if(alpha)
                {
                    if(alpha == 255*255)
                    {
                        p[Order::R] = c.r;
                        p[Order::G] = c.g;
                        p[Order::B] = c.b;
                    }
                    else
                    {
                        int r = p[Order::R];
                        int g = p[Order::G];
                        int b = p[Order::B];
                        p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                        p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                        p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    }
                }
                p += 3;
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_solid_vspan(int x, int y,
                               unsigned len, 
                               const color_type& c,
                               const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            do 
            {
                int alpha = (*covers++) * c.a;

                if(alpha)
                {
                    if(alpha == 255*255)
                    {
                        p[Order::R] = c.r;
                        p[Order::G] = c.g;
                        p[Order::B] = c.b;
                    }
                    else
                    {
                        int r = p[Order::R];
                        int g = p[Order::G];
                        int b = p[Order::B];
                        p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                        p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                        p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    }
                }
                p += m_rbuf->stride();
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_color_hspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            if(covers)
            {
                do 
                {
                    int alpha = (*covers++) * colors->a;

                    if(alpha)
                    {
                        if(alpha == 255*255)
                        {
                            p[Order::R] = colors->r;
                            p[Order::G] = colors->g;
                            p[Order::B] = colors->b;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            p[Order::R] = (((colors->r - r) * alpha) + (r << 16)) >> 16;
                            p[Order::G] = (((colors->g - g) * alpha) + (g << 16)) >> 16;
                            p[Order::B] = (((colors->b - b) * alpha) + (b << 16)) >> 16;
                        }
                    }
                    p += 3;
                    ++colors;
                }
                while(--len);
            }
            else
            {
                do 
                {
                    if(colors->a)
                    {
                        if(colors->a == 255)
                        {
                            p[Order::R] = colors->r;
                            p[Order::G] = colors->g;
                            p[Order::B] = colors->b;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            p[Order::R] = (((colors->r - r) * colors->a) + (r << 8)) >> 8;
                            p[Order::G] = (((colors->g - g) * colors->a) + (g << 8)) >> 8;
                            p[Order::B] = (((colors->b - b) * colors->a) + (b << 8)) >> 8;
                        }
                    }
                    p += 3;
                    ++colors;
                }
                while(--len);
            }
        }



        //--------------------------------------------------------------------
        void blend_color_vspan(int x, int y,
                               unsigned len, 
                               const color_type* colors,
                               const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + x + x + x;
            if(covers)
            {
                do 
                {
                    int alpha = (*covers++) * colors->a;

                    if(alpha)
                    {
                        if(alpha == 255*255)
                        {
                            p[Order::R] = colors->r;
                            p[Order::G] = colors->g;
                            p[Order::B] = colors->b;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            p[Order::R] = (((colors->r - r) * alpha) + (r << 16)) >> 16;
                            p[Order::G] = (((colors->g - g) * alpha) + (g << 16)) >> 16;
                            p[Order::B] = (((colors->b - b) * alpha) + (b << 16)) >> 16;
                        }
                    }
                    p += m_rbuf->stride();
                    ++colors;
                }
                while(--len);
            }
            else
            {
                do 
                {
                    if(colors->a)
                    {
                        if(colors->a == 255)
                        {
                            p[Order::R] = colors->r;
                            p[Order::G] = colors->g;
                            p[Order::B] = colors->b;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            p[Order::R] = (((colors->r - r) * colors->a) + (r << 8)) >> 8;
                            p[Order::G] = (((colors->g - g) * colors->a) + (g << 8)) >> 8;
                            p[Order::B] = (((colors->b - b) * colors->a) + (b << 8)) >> 8;
                        }
                    }
                    p += m_rbuf->stride();
                    ++colors;
                }
                while(--len);
            }
        }

    private:
        rendering_buffer* m_rbuf;

    };


    typedef pixel_formats_rgb24<order_rgb24> pixfmt_rgb24;
    typedef pixel_formats_rgb24<order_bgr24> pixfmt_bgr24;

}

#endif

