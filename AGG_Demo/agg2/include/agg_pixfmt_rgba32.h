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

#ifndef AGG_PIXFMT_RGBA32_INCLUDED
#define AGG_PIXFMT_RGBA32_INCLUDED

#include <string.h>
#include "agg_basics.h"
#include "agg_color_rgba8.h"
#include "agg_rendering_buffer.h"

namespace agg
{
    struct order_rgba32 { enum { R=0, G=1, B=2, A=3, rgba32_tag }; };
    struct order_argb32 { enum { A=0, R=1, G=2, B=3, rgba32_tag }; };
    struct order_abgr32 { enum { A=0, B=1, G=2, R=3, rgba32_tag }; };
    struct order_bgra32 { enum { B=0, G=1, R=2, A=3, rgba32_tag }; };

    //========================================================================
    template<class Order> class pixel_formats_rgba32
    {
    public:
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;

        //--------------------------------------------------------------------
        pixel_formats_rgba32(rendering_buffer& rb)
            : m_rbuf(&rb)
        {
        }

        //--------------------------------------------------------------------
        unsigned width()  const { return m_rbuf->width();  }
        unsigned height() const { return m_rbuf->height(); }

        //--------------------------------------------------------------------
        color_type pixel(int x, int y)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
            return color_type(p[Order::R], p[Order::G], p[Order::B], p[Order::A]);
        }

        //--------------------------------------------------------------------
        void copy_pixel(int x, int y, const color_type& c)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
            p[Order::R] = c.r;
            p[Order::G] = c.g;
            p[Order::B] = c.b;
            p[Order::A] = c.a;
        }

        //--------------------------------------------------------------------
        void blend_pixel(int x, int y, const color_type& c, int alpha)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
            alpha *= c.a;
            if(alpha == 255*255)
            {
                p[Order::R] = c.r;
                p[Order::G] = c.g;
                p[Order::B] = c.b;
                p[Order::A] = c.a;
            }
            else
            {
                int r = p[Order::R];
                int g = p[Order::G];
                int b = p[Order::B];
                int a = p[Order::A];
                p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                p[Order::A] = (((c.a - a) * alpha) + (a << 16)) >> 16;
            }
        }

        //--------------------------------------------------------------------
        void copy_hline(int x, int y, unsigned len, const color_type& c)
        {
            int32u v;
            int8u* p8 = (int8u*)&v;
            p8[Order::R] = c.r;
            p8[Order::G] = c.g;
            p8[Order::B] = c.b;
            p8[Order::A] = c.a;
            int32u* p32 = (int32u*)(m_rbuf->row(y)) + x;
            do
            {
                *p32++ = v;
            }
            while(--len);
        }

        //--------------------------------------------------------------------
        void copy_vline(int x, int y, unsigned len, const color_type& c)
        {
            int32u v;
            int8u* p8 = (int8u*)&v;
            p8[Order::R] = c.r;
            p8[Order::G] = c.g;
            p8[Order::B] = c.b;
            p8[Order::A] = c.a;
            int8u* p = m_rbuf->row(y) + (x << 2);
            do
            {
                *(int32u*)p = v; 
                p += m_rbuf->stride();
            }
            while(--len);
        }

        //--------------------------------------------------------------------
        void blend_hline(int x, int y, unsigned len, 
                         const color_type& c, int alpha)
        {
            alpha *= c.a;
            if(alpha == 255*255)
            {
                int32u v;
                int8u* p8 = (int8u*)&v;
                p8[Order::R] = c.r;
                p8[Order::G] = c.g;
                p8[Order::B] = c.b;
                p8[Order::A] = c.a;
                int32u* p32 = (int32u*)(m_rbuf->row(y)) + x;
                do
                {
                    *p32++ = v;
                }
                while(--len);
            }
            else
            {
                int8u* p = m_rbuf->row(y) + (x << 2);
                do
                {
                    int r = p[Order::R];
                    int g = p[Order::G];
                    int b = p[Order::B];
                    int a = p[Order::A];
                    p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                    p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                    p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    p[Order::A] = (((c.a - a) * alpha) + (a << 16)) >> 16;
                    p += 4;
                }
                while(--len);
            }
        }

        //--------------------------------------------------------------------
        void blend_vline(int x, int y, unsigned len, 
                         const color_type& c, int alpha)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
            alpha *= c.a;
            if(alpha == 255*255)
            {
                int32u v;
                int8u* p8 = (int8u*)&v;
                p8[Order::R] = c.r;
                p8[Order::G] = c.g;
                p8[Order::B] = c.b;
                p8[Order::A] = c.a;
                do
                {
                    *(int32u*)p = v; 
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
                    int a = p[Order::A];
                    p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                    p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                    p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                    p[Order::A] = (((c.a - a) * alpha) + (a << 16)) >> 16;
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
            memmove(m_rbuf->row(ydst) + xdst * 4, 
                    (const void*)(from.row(ysrc) + xsrc * 4), len * 4);
        }


        //--------------------------------------------------------------------
        void blend_solid_hspan(int x, int y, unsigned len, 
                               const color_type& c, const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
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
                        p[Order::A] = c.a;
                    }
                    else
                    {
                        int r = p[Order::R];
                        int g = p[Order::G];
                        int b = p[Order::B];
                        int a = p[Order::A];
                        p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                        p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                        p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                        p[Order::A] = (((c.a - a) * alpha) + (a << 16)) >> 16;
                    }
                }
                p += 4;
            }
            while(--len);
        }



        //--------------------------------------------------------------------
        void blend_solid_vspan(int x, int y, unsigned len, 
                               const color_type& c, const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
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
                        p[Order::A] = c.a;
                    }
                    else
                    {
                        int r = p[Order::R];
                        int g = p[Order::G];
                        int b = p[Order::B];
                        int a = p[Order::A];
                        p[Order::R] = (((c.r - r) * alpha) + (r << 16)) >> 16;
                        p[Order::G] = (((c.g - g) * alpha) + (g << 16)) >> 16;
                        p[Order::B] = (((c.b - b) * alpha) + (b << 16)) >> 16;
                        p[Order::A] = (((c.a - a) * alpha) + (a << 16)) >> 16;
                    }
                }
                p += m_rbuf->stride();
            }
            while(--len);
        }


        //--------------------------------------------------------------------
        void blend_color_hspan(int x, int y, unsigned len, 
                               const color_type* colors, const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
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
                            p[Order::A] = colors->a;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            int a = p[Order::A];
                            p[Order::R] = (((colors->r - r) * alpha) + (r << 16)) >> 16;
                            p[Order::G] = (((colors->g - g) * alpha) + (g << 16)) >> 16;
                            p[Order::B] = (((colors->b - b) * alpha) + (b << 16)) >> 16;
                            p[Order::A] = (((colors->a - a) * alpha) + (a << 16)) >> 16;
                        }
                    }
                    p += 4;
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
                            p[Order::A] = colors->a;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            int a = p[Order::A];
                            p[Order::R] = (((colors->r - r) * colors->a) + (r << 8)) >> 8;
                            p[Order::G] = (((colors->g - g) * colors->a) + (g << 8)) >> 8;
                            p[Order::B] = (((colors->b - b) * colors->a) + (b << 8)) >> 8;
                            p[Order::A] = (((colors->a - a) * colors->a) + (a << 8)) >> 8;
                        }
                    }
                    p += 4;
                    ++colors;
                }
                while(--len);
            }
        }


        //--------------------------------------------------------------------
        void blend_color_vspan(int x, int y, unsigned len, 
                               const color_type* colors, const int8u* covers)
        {
            int8u* p = m_rbuf->row(y) + (x << 2);
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
                            p[Order::A] = colors->a;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            int a = p[Order::A];
                            p[Order::R] = (((colors->r - r) * alpha) + (r << 16)) >> 16;
                            p[Order::G] = (((colors->g - g) * alpha) + (g << 16)) >> 16;
                            p[Order::B] = (((colors->b - b) * alpha) + (b << 16)) >> 16;
                            p[Order::A] = (((colors->a - a) * alpha) + (a << 16)) >> 16;
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
                            p[Order::A] = colors->a;
                        }
                        else
                        {
                            int r = p[Order::R];
                            int g = p[Order::G];
                            int b = p[Order::B];
                            int a = p[Order::A];
                            p[Order::R] = (((colors->r - r) * colors->a) + (r << 8)) >> 8;
                            p[Order::G] = (((colors->g - g) * colors->a) + (g << 8)) >> 8;
                            p[Order::B] = (((colors->b - b) * colors->a) + (b << 8)) >> 8;
                            p[Order::A] = (((colors->a - a) * colors->a) + (a << 8)) >> 8;
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

    typedef pixel_formats_rgba32<order_rgba32> pixfmt_rgba32;
    typedef pixel_formats_rgba32<order_argb32> pixfmt_argb32;
    typedef pixel_formats_rgba32<order_abgr32> pixfmt_abgr32;
    typedef pixel_formats_rgba32<order_bgra32> pixfmt_bgra32;
}

#endif

