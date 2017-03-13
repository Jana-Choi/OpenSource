#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u8.h"
#include "agg_renderer_scanline.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"
#include "agg_trans_bilinear.h"
#include "agg_trans_perspective.h"
#include "agg_span_interpolator_trans.h"
#include "agg_pixfmt_rgb24.h"
#include "agg_span_image_filter_rgb24.h"
#include "ctrl/agg_rbox_ctrl.h"
#include "platform/agg_platform_support.h"
#include "interactive_polygon.h"


enum { flip_y = true };

agg::rasterizer_scanline_aa<> g_rasterizer;
agg::scanline_u8  g_scanline;
double            g_x1 = 0;
double            g_y1 = 0;
double            g_x2 = 0;
double            g_y2 = 0;



class the_application : public agg::platform_support
{
public:
    typedef agg::pixfmt_bgr24 pixfmt;
    typedef agg::renderer_base<pixfmt> renderer_base;
    typedef agg::renderer_scanline_u_solid<renderer_base> renderer_solid;

    agg::interactive_polygon   m_quad;
    agg::rbox_ctrl<agg::rgba8> m_trans_type;

    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y),
        m_quad(4, 5.0),
        m_trans_type(420, 5.0, 420+130.0, 55.0, !flip_y)
    {
        m_trans_type.add_item("Bilinear");
        m_trans_type.add_item("Perspective");
        m_trans_type.cur_item(1);
        add_ctrl(m_trans_type);
    }


    virtual void on_init()
    {
        g_x1 = 0.0;
        g_y1 = 0.0;
        g_x2 = rbuf_img(0).width();
        g_y2 = rbuf_img(0).height();
        double dx = width()  / 2.0 - (g_x2 - g_x1) / 2.0;
        double dy = height() / 2.0 - (g_y2 - g_y1) / 2.0;
        m_quad.xn(0) = g_x1 + dx;
        m_quad.yn(0) = g_y1 + dy;
        m_quad.xn(1) = g_x2 + dx;
        m_quad.yn(1) = g_y1 + dy;
        m_quad.xn(2) = g_x2 + dx;
        m_quad.yn(2) = g_y2 + dy;
        m_quad.xn(3) = g_x1 + dx;
        m_quad.yn(3) = g_y2 + dy;
    }

    virtual void on_draw()
    {
        pixfmt pixf(rbuf_window());
        renderer_base rb(pixf);
        renderer_solid r(rb);
        rb.clear(agg::rgba(1, 1, 1));

        g_rasterizer.clip_box(0, 0, width(), height());

        typedef agg::span_allocator<agg::rgba8> span_alloc_type;
        span_alloc_type sa;

        if(m_trans_type.cur_item() == 0)
        {
            agg::trans_bilinear tr(m_quad.polygon(), g_x1, g_y1, g_x2, g_y2);
            if(tr.is_valid())
            {
                typedef agg::span_interpolator_trans<agg::trans_bilinear> interpolator_type;
                interpolator_type interpolator(tr);

                // "hardcoded" bilinear filter
                //------------------------------------------
                typedef agg::span_image_filter_rgb24_bilinear<agg::order_bgr24, 
                                                              interpolator_type> span_gen_type;
                typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

                span_gen_type sg(sa, 
                                 rbuf_img(0), 
                                 agg::rgba(1, 1, 1, 0),
                                 interpolator);

                renderer_type ri(rb, sg);

                g_rasterizer.reset();
                g_rasterizer.move_to_d(m_quad.xn(0), m_quad.yn(0));
                g_rasterizer.line_to_d(m_quad.xn(1), m_quad.yn(1));
                g_rasterizer.line_to_d(m_quad.xn(2), m_quad.yn(2));
                g_rasterizer.line_to_d(m_quad.xn(3), m_quad.yn(3));

                g_rasterizer.render(g_scanline, ri);
            }
        }
        else
        {
            agg::trans_perspective tr(m_quad.polygon(), g_x1, g_y1, g_x2, g_y2);

            if(tr.is_valid())
            {
                typedef agg::span_interpolator_trans<agg::trans_perspective> interpolator_type;
                interpolator_type interpolator(tr);

                // "hardcoded" bilinear filter
                //------------------------------------------
                typedef agg::span_image_filter_rgb24_bilinear<agg::order_bgr24, 
                                                              interpolator_type> span_gen_type;
                typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

                span_gen_type sg(sa, 
                                 rbuf_img(0), 
                                 agg::rgba(1, 1, 1, 0),
                                 interpolator);

                renderer_type ri(rb, sg);

                g_rasterizer.reset();
                g_rasterizer.move_to_d(m_quad.xn(0), m_quad.yn(0));
                g_rasterizer.line_to_d(m_quad.xn(1), m_quad.yn(1));
                g_rasterizer.line_to_d(m_quad.xn(2), m_quad.yn(2));
                g_rasterizer.line_to_d(m_quad.xn(3), m_quad.yn(3));

                g_rasterizer.render(g_scanline, ri);
            }

        }


        //--------------------------
        // Render the "quad" tool and controls
        g_rasterizer.add_path(m_quad);
        r.color(agg::rgba(0, 0.3, 0.5, 0.6));
        g_rasterizer.render(g_scanline, r);
        g_rasterizer.render_ctrl(g_scanline, r, m_trans_type);
        //--------------------------
    }



    virtual void on_mouse_button_down(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            if(m_quad.on_mouse_button_down(x, y))
            {
                force_redraw();
            }
        }
    }


    virtual void on_mouse_move(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            if(m_quad.on_mouse_move(x, y))
            {
                force_redraw();
            }
        }
        if((flags & agg::mouse_left) == 0)
        {
            on_mouse_button_up(x, y, flags);
        }
    }


    virtual void on_mouse_button_up(int x, int y, unsigned flags)
    {
        if(m_quad.on_mouse_button_up(x, y))
        {
            force_redraw();
        }
    }

};






int agg_main(int argc, char* argv[])
{
    the_application app(agg::pix_format_bgr24, flip_y);
    app.caption("AGG Example. Image Perspective Transformations");

    const char* img_name = "spheres";
    if(argc >= 2) img_name = argv[1];
    if(!app.load_img(0, img_name)) 
    {
        char buf[256];
        if(strcmp(img_name, "spheres") == 0)
        {
            sprintf(buf, "File not found: %s%s. Download http://www.antigrain.com/%s%s\n"
                         "or copy it from another directory if available.",
                    img_name, app.img_ext(), img_name, app.img_ext());
        }
        else
        {
            sprintf(buf, "File not found: %s%s", img_name, app.img_ext());
        }
        app.message(buf);
        return 1;
    }
    
    if(app.init(600, 600, agg::window_resize))
    {
        return app.run();
    }
    return 1;
}






