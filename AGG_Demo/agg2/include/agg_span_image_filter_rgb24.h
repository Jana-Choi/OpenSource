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

#ifndef AGG_SPAN_IMAGE_FILTER_RGB24_INCLUDED
#define AGG_SPAN_IMAGE_FILTER_RGB24_INCLUDED

#include "agg_basics.h"
#include "agg_pixfmt_rgb24.h"
#include "agg_span_image_filter.h"


namespace agg
{


    //========================================================================
    template<class Order, 
             class Interpolator,
             class Allocator = span_allocator<rgba8> > 
    class span_image_filter_rgb24_nn : 
    public span_image_filter<rgba8, Interpolator, Allocator>
    {
    public:
        typedef Interpolator interpolator_type;
        typedef Allocator alloc_type;
        typedef span_image_filter<rgba8, Interpolator, alloc_type> base_type;
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;

        //--------------------------------------------------------------------
        span_image_filter_rgb24_nn(alloc_type& alloc) : base_type(alloc) {}

        //--------------------------------------------------------------------
        span_image_filter_rgb24_nn(alloc_type& alloc,
                                   const rendering_buffer& src, 
                                   const color_type& back_color,
                                   interpolator_type& inter) :
            base_type(alloc, src, back_color, inter, 0) 
        {}

        //--------------------------------------------------------------------
        color_type* generate(int x, int y, unsigned len)
        {
            interpolator().begin(x, y, len);

            int fg[3];
            int src_alpha;

            const unsigned char *fg_ptr;
            color_type* span = allocator().span();

            int maxx = source_image().width() - 1;
            int maxy = source_image().height() - 1;

            do
            {
                interpolator().coordinates(&x, &y);

                x >>= image_subpixel_shift;
                y >>= image_subpixel_shift;

                if(x >= 0    && y >= 0 &&
                   x <= maxx && y <= maxy) 
                {
                    fg_ptr = source_image().row(y) + x + x + x;
                    fg[0] = *fg_ptr++;
                    fg[1] = *fg_ptr++;
                    fg[2] = *fg_ptr++;
                    src_alpha = 255;
                }
                else
                {
                    fg[Order::R] = background_color().r;
                    fg[Order::G] = background_color().g;
                    fg[Order::B] = background_color().b;
                    src_alpha    = background_color().a;
                }
                span->r = fg[Order::R];
                span->g = fg[Order::G];
                span->b = fg[Order::B];
                span->a = src_alpha;
                ++span;
                ++interpolator();

            } while(--len);

            return allocator().span();
        }
    };




    //========================================================================
    template<class Order, 
             class Interpolator, 
             class Allocator = span_allocator<rgba8> > 
    class span_image_filter_rgb24_bilinear : 
    public span_image_filter<rgba8, Interpolator, Allocator>
    {
    public:
        typedef Interpolator interpolator_type;
        typedef Allocator alloc_type;
        typedef span_image_filter<rgba8, Interpolator, Allocator> base_type;
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;

        //--------------------------------------------------------------------
        span_image_filter_rgb24_bilinear(alloc_type& alloc) : base_type(alloc) {}

        //--------------------------------------------------------------------
        span_image_filter_rgb24_bilinear(alloc_type& alloc,
                                         const rendering_buffer& src, 
                                         const color_type& back_color,
                                         interpolator_type& inter) :
            base_type(alloc, src, back_color, inter, 0) 
        {}

        //--------------------------------------------------------------------
        color_type* generate(int x, int y, unsigned len)
        {
            interpolator().begin(x, y, len);

            int fg[3];
            int src_alpha;
            int back_r = background_color().r;
            int back_g = background_color().g;
            int back_b = background_color().b;
            int back_a = background_color().a;

            const unsigned char *fg_ptr;

            int stride = source_image().stride() - 2 * 3;
            color_type* span = allocator().span();

            int maxx = source_image().width() - 1;
            int maxy = source_image().height() - 1;

            do
            {
                int x_hr;
                int y_hr;
                
                interpolator().coordinates(&x_hr, &y_hr);

                int x_lr = x_hr >> image_subpixel_shift;
                int y_lr = y_hr >> image_subpixel_shift;
                int weight;

                if(x_lr >= 0    && y_lr >= 0 &&
                   x_lr <  maxx && y_lr <  maxy) 
                {
                    fg[0] = 
                    fg[1] = 
                    fg[2] = image_subpixel_size * image_subpixel_size / 2;

                    x_hr &= image_subpixel_mask;
                    y_hr &= image_subpixel_mask;
                    fg_ptr = source_image().row(y_lr) + x_lr + x_lr + x_lr;

                    weight = (image_subpixel_size - x_hr) * 
                             (image_subpixel_size - y_hr);
                    fg[0] += weight * *fg_ptr++;
                    fg[1] += weight * *fg_ptr++;
                    fg[2] += weight * *fg_ptr++;

                    weight = x_hr * (image_subpixel_size - y_hr);
                    fg[0] += weight * *fg_ptr++;
                    fg[1] += weight * *fg_ptr++;
                    fg[2] += weight * *fg_ptr++;

                    fg_ptr += stride;

                    weight = (image_subpixel_size - x_hr) * y_hr;
                    fg[0] += weight * *fg_ptr++;
                    fg[1] += weight * *fg_ptr++;
                    fg[2] += weight * *fg_ptr++;

                    weight = x_hr * y_hr;
                    fg[0] += weight * *fg_ptr++;
                    fg[1] += weight * *fg_ptr++;
                    fg[2] += weight * *fg_ptr++;

                    fg[0] >>= image_subpixel_shift * 2;
                    fg[1] >>= image_subpixel_shift * 2;
                    fg[2] >>= image_subpixel_shift * 2;
                    src_alpha = 255;
                }
                else
                {
                    if(x_lr < -1   || y_lr < -1 ||
                       x_lr > maxx || y_lr > maxy)
                    {
                        fg[Order::R] = back_r;
                        fg[Order::G] = back_g;
                        fg[Order::B] = back_b;
                        src_alpha    = back_a;
                    }
                    else
                    {
                        fg[0] = 
                        fg[1] = 
                        fg[2] = 
                        src_alpha = image_subpixel_size * image_subpixel_size / 2;

                        x_hr &= image_subpixel_mask;
                        y_hr &= image_subpixel_mask;

                        weight = (image_subpixel_size - x_hr) * 
                                 (image_subpixel_size - y_hr);
                        if(x_lr >= 0    && y_lr >= 0 &&
                           x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = source_image().row(y_lr) + x_lr + x_lr + x_lr;
                            fg[0] += weight * *fg_ptr++;
                            fg[1] += weight * *fg_ptr++;
                            fg[2] += weight * *fg_ptr++;
                            src_alpha += weight * 255;
                        }
                        else
                        {
                            fg[Order::R] += back_r * weight;
                            fg[Order::G] += back_g * weight;
                            fg[Order::B] += back_b * weight;
                            src_alpha    += back_a * weight;
                        }

                        x_lr++;

                        weight = x_hr * (image_subpixel_size - y_hr);
                        if(x_lr >= 0    && y_lr >= 0 &&
                           x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = source_image().row(y_lr) + x_lr + x_lr + x_lr;
                            fg[0] += weight * *fg_ptr++;
                            fg[1] += weight * *fg_ptr++;
                            fg[2] += weight * *fg_ptr++;
                            src_alpha += weight * 255;
                        }
                        else
                        {
                            fg[Order::R] += back_r * weight;
                            fg[Order::G] += back_g * weight;
                            fg[Order::B] += back_b * weight;
                            src_alpha    += back_a * weight;
                        }

                        x_lr--;
                        y_lr++;

                        weight = (image_subpixel_size - x_hr) * y_hr;
                        if(x_lr >= 0    && y_lr >= 0 &&
                           x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = source_image().row(y_lr) + x_lr + x_lr + x_lr;
                            fg[0] += weight * *fg_ptr++;
                            fg[1] += weight * *fg_ptr++;
                            fg[2] += weight * *fg_ptr++;
                            src_alpha += weight * 255;
                        }
                        else
                        {
                            fg[Order::R] += back_r * weight;
                            fg[Order::G] += back_g * weight;
                            fg[Order::B] += back_b * weight;
                            src_alpha    += back_a * weight;
                        }

                        x_lr++;

                        weight = x_hr * y_hr;
                        if(x_lr >= 0    && y_lr >= 0 &&
                           x_lr <= maxx && y_lr <= maxy)
                        {
                            fg_ptr = source_image().row(y_lr) + x_lr + x_lr + x_lr;
                            fg[0] += weight * *fg_ptr++;
                            fg[1] += weight * *fg_ptr++;
                            fg[2] += weight * *fg_ptr++;
                            src_alpha += weight * 255;
                        }
                        else
                        {
                            fg[Order::R] += back_r * weight;
                            fg[Order::G] += back_g * weight;
                            fg[Order::B] += back_b * weight;
                            src_alpha    += back_a * weight;
                        }

                        fg[0] >>= image_subpixel_shift * 2;
                        fg[1] >>= image_subpixel_shift * 2;
                        fg[2] >>= image_subpixel_shift * 2;
                        src_alpha >>= image_subpixel_shift * 2;
                    }
                }

                span->r = fg[Order::R];
                span->g = fg[Order::G];
                span->b = fg[Order::B];
                span->a = src_alpha;
                ++span;
                ++interpolator();

            } while(--len);

            return allocator().span();
        }
    };





    //========================================================================
    template<class Order, 
             class Interpolator, 
             class Allocator = span_allocator<rgba8> > 
    class span_image_filter_rgb24 : 
    public span_image_filter<rgba8, Interpolator, Allocator>
    {
    public:
        typedef Interpolator interpolator_type;
        typedef Allocator alloc_type;
        typedef span_image_filter<rgba8, Interpolator, alloc_type> base_type;
        typedef rgba8 color_type;
        typedef color_type::alpha_type alpha_type;

        //--------------------------------------------------------------------
        span_image_filter_rgb24(alloc_type& alloc) : base_type(alloc) {}

        //--------------------------------------------------------------------
        span_image_filter_rgb24(alloc_type& alloc,
                                const rendering_buffer& src, 
                                const color_type& back_color,
                                interpolator_type& inter,
                                const image_filter_base& filter) :
            base_type(alloc, src, back_color, inter, &filter) 
        {}

        //--------------------------------------------------------------------
        color_type* generate(int x, int y, unsigned len)
        {
            interpolator().begin(x, y, len);

            int fg[3];
            int src_alpha;
            int back_r = background_color().r;
            int back_g = background_color().g;
            int back_b = background_color().b;
            int back_a = background_color().a;

            const unsigned char *fg_ptr;

            unsigned   dimension    = filter().dimension();
            int        start        = filter().start();
            int        start1       = start - 1;
            const int* weight_array = filter().weight_array_int();

            int stride = source_image().stride() - dimension * 3;
            color_type* span = allocator().span();

            int maxx = source_image().width() + start - 2;
            int maxy = source_image().height() + start - 2;

            int maxx2 = source_image().width() - start - 1;
            int maxy2 = source_image().height() - start - 1;

            int x_count; 
            int weight_y;

            do
            {
                interpolator().coordinates(&x, &y);

                int x_hr = x;
                int y_hr = y;
            
                int x_lr = x_hr >> image_subpixel_shift;
                int y_lr = y_hr >> image_subpixel_shift;

                fg[0] = fg[1] = fg[2] = image_filter_size / 2;

                int x_fract = x_hr & image_subpixel_mask;
                unsigned y_count = dimension;

                if(x_lr >= -start && y_lr >= -start &&
                   x_lr <= maxx   && y_lr <= maxy) 
                {
                    y_hr = image_subpixel_mask - (y_hr & image_subpixel_mask);
                    fg_ptr = source_image().row(y_lr + start) + (x_lr + start) * 3;
                    do
                    {
                        x_count = dimension;
                        weight_y = weight_array[y_hr];
                        x_hr = image_subpixel_mask - x_fract;

                        do
                        {
                            int weight = (weight_y * weight_array[x_hr] + 
                                         image_filter_size / 2) >> 
                                         image_filter_shift;
            
                            fg[0] += *fg_ptr++ * weight;
                            fg[1] += *fg_ptr++ * weight;
                            fg[2] += *fg_ptr++ * weight;

                            x_hr += image_subpixel_size;

                        } while(--x_count);

                        y_hr += image_subpixel_size;
                        fg_ptr += stride;

                    } while(--y_count);

                    fg[0] >>= image_filter_shift;
                    fg[1] >>= image_filter_shift;
                    fg[2] >>= image_filter_shift;

                    if(fg[0] < 0) fg[0] = 0;
                    if(fg[1] < 0) fg[1] = 0;
                    if(fg[2] < 0) fg[2] = 0;

                    if(fg[0] > 255) fg[0] = 255;
                    if(fg[1] > 255) fg[1] = 255;
                    if(fg[2] > 255) fg[2] = 255;
                    src_alpha = 255;
                }
                else
                {
                    if(x_lr < start1 || y_lr < start1 ||
                       x_lr > maxx2  || y_lr > maxy2) 
                    {
                        fg[Order::R] = back_r;
                        fg[Order::G] = back_g;
                        fg[Order::B] = back_b;
                        src_alpha    = back_a;
                    }
                    else
                    {
                        src_alpha = image_filter_size / 2;
                        y_lr = (y >> image_subpixel_shift) + start;
                        y_hr = image_subpixel_mask - (y_hr & image_subpixel_mask);

                        do
                        {
                            x_count = dimension;
                            weight_y = weight_array[y_hr];
                            x_lr = (x >> image_subpixel_shift) + start;
                            x_hr = image_subpixel_mask - x_fract;

                            do
                            {
                                int weight = (weight_y * weight_array[x_hr] + 
                                             image_filter_size / 2) >> 
                                             image_filter_shift;

                                if(x_lr >= 0 && y_lr >= 0 && 
                                   x_lr < int(source_image().width()) && 
                                   y_lr < int(source_image().height()))
                                {
                                    fg_ptr = source_image().row(y_lr) + x_lr * 3;
                                    fg[0] += *fg_ptr++ * weight;
                                    fg[1] += *fg_ptr++ * weight;
                                    fg[2] += *fg_ptr++ * weight;
                                    src_alpha += 255   * weight;
                                }
                                else
                                {
                                    fg[Order::R] += back_r * weight;
                                    fg[Order::G] += back_g * weight;
                                    fg[Order::B] += back_b * weight;
                                    src_alpha    += back_a * weight;
                                }
                                x_hr += image_subpixel_size;
                                x_lr++;

                            } while(--x_count);

                            y_hr += image_subpixel_size;
                            y_lr++;

                        } while(--y_count);


                        fg[0] >>= image_filter_shift;
                        fg[1] >>= image_filter_shift;
                        fg[2] >>= image_filter_shift;
                        src_alpha >>= image_filter_shift;

                        if(fg[0] < 0) fg[0] = 0;
                        if(fg[1] < 0) fg[1] = 0;
                        if(fg[2] < 0) fg[2] = 0;
                        if(src_alpha < 0) src_alpha = 0;

                        if(fg[0] > 255) fg[0] = 255;
                        if(fg[1] > 255) fg[1] = 255;
                        if(fg[2] > 255) fg[2] = 255;
                        if(src_alpha > 255) src_alpha = 255;
                    }
                }

                span->r = fg[Order::R];
                span->g = fg[Order::G];
                span->b = fg[Order::B];
                span->a = src_alpha;
                ++span;
                ++interpolator();

            } while(--len);

            return allocator().span();
        }
    };

}


#endif



