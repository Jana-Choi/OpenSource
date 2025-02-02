#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_path_storage.h"
#include "agg_affine_matrix.h"
#include "agg_conv_transform.h"
#include "agg_span_image_filter_rgba32.h"
#include "agg_span_interpolator_linear.h"
#include "agg_scanline_u8.h"
#include "agg_renderer_scanline.h"
#include "ctrl/agg_slider_ctrl.h"
#include "ctrl/agg_rbox_ctrl.h"
#include "ctrl/agg_cbox_ctrl.h"
#include "platform/agg_platform_support.h"

enum { flip_y = true };




class the_application : public agg::platform_support
{
    agg::slider_ctrl<agg::rgba8> m_polygon_angle;
    agg::slider_ctrl<agg::rgba8> m_polygon_scale;

    agg::slider_ctrl<agg::rgba8> m_image_angle;
    agg::slider_ctrl<agg::rgba8> m_image_scale;

    agg::cbox_ctrl<agg::rgba8> m_rotate_polygon;
    agg::cbox_ctrl<agg::rgba8> m_rotate_image;

    agg::rbox_ctrl<agg::rgba8> m_example;

    double m_image_center_x;
    double m_image_center_y;

    double m_polygon_cx;
    double m_polygon_cy;

    double m_image_cx;
    double m_image_cy;

    double m_dx;
    double m_dy;

    int m_flag;


public:
    //------------------------------------------------------------------------
    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y),
        m_polygon_angle(5,  5,      145, 11,    !flip_y),
        m_polygon_scale(5,  5+14,   145, 12+14, !flip_y),
        m_image_angle  (155,  5,    300, 12,    !flip_y),
        m_image_scale  (155,  5+14, 300, 12+14, !flip_y),
        m_rotate_polygon(5, 5+14+14,    "Rotate Polygon", !flip_y),
        m_rotate_image  (5, 5+14+14+14, "Rotate Image", !flip_y),
        m_example(-3.0, 14+14+14+14, -3.0, 14+14+14+14, !flip_y),
        m_flag(0)
    {
        add_ctrl(m_polygon_angle);
        add_ctrl(m_polygon_scale);
        add_ctrl(m_image_angle);
        add_ctrl(m_image_scale);
        add_ctrl(m_rotate_polygon);
        add_ctrl(m_rotate_image);
        add_ctrl(m_example);

        m_polygon_angle.label("Polygon Angle=%3.2f");
        m_polygon_scale.label("Polygon Scale=%3.2f");
        m_polygon_angle.range(-180.0, 180.0);
        m_polygon_scale.range(0.1, 5.0);
        m_polygon_scale.value(1.0);

        m_image_angle.label("Image Angle=%3.2f");
        m_image_scale.label("Image Scale=%3.2f");
        m_image_angle.range(-180.0, 180.0);
        m_image_scale.range(0.1, 5.0);
        m_image_scale.value(1.0);

        m_example.add_item("0");
        m_example.add_item("1");
        m_example.add_item("2");
        m_example.add_item("3");
        m_example.add_item("4");
        m_example.add_item("5");
        m_example.add_item("6");
        m_example.cur_item(0);
    }


    //------------------------------------------------------------------------
    virtual ~the_application()
    {
    }


    //------------------------------------------------------------------------
    virtual void on_init()
    {
        m_image_center_x = initial_width() / 2.0;
        m_image_center_y = initial_height() / 2.0;
        m_polygon_cx = m_image_cx = initial_width() / 2.0;
        m_polygon_cy = m_image_cy = initial_height() / 2.0;
    }


    //------------------------------------------------------------------------
    void create_star(agg::path_storage& ps)
    {
        double r = initial_width();
        if(initial_height() < r) r = initial_height();

        double r1 = r / 3 - 8.0;
        double r2 = r1 / 1.45;
        unsigned nr = 14;

        unsigned i;
        for(i = 0; i < nr; i++)
        {
            double a = agg::pi * 2.0 * i / nr - agg::pi / 2.0;
            double dx = cos(a);
            double dy = sin(a);

            if(i & 1)
            {
                ps.line_to(m_polygon_cx + dx * r1, m_polygon_cy + dy * r1);
            }
            else
            {
                if(i) ps.line_to(m_polygon_cx + dx * r2, m_polygon_cy + dy * r2);
                else  ps.move_to(m_polygon_cx + dx * r2, m_polygon_cy + dy * r2);
            }
        }
    }





    //------------------------------------------------------------------------
    virtual void on_draw()
    {
        double width = rbuf_window().width();
        double height = rbuf_window().height();
    
        typedef agg::pixfmt_bgra32 pixfmt; 
        typedef agg::renderer_base<pixfmt> renderer_base;
        typedef agg::renderer_scanline_u_solid<renderer_base> renderer_solid;
       
        pixfmt pixf(rbuf_window());
        renderer_base rb(pixf);
        renderer_solid rs(rb);

        rb.clear(agg::rgba(1.0, 1.0, 1.0));

        agg::affine_matrix image_mtx;
        agg::affine_matrix polygon_mtx;

        polygon_mtx *= agg::translation_matrix(-m_polygon_cx, -m_polygon_cy);
        polygon_mtx *= agg::rotation_matrix(m_polygon_angle.value() * agg::pi / 180.0);
        polygon_mtx *= agg::scaling_matrix(m_polygon_scale.value());
        polygon_mtx *= agg::translation_matrix(m_polygon_cx, m_polygon_cy);



        switch(m_example.cur_item())
        {
        default:
        case 0:
// --------------(Example 0, Identity matrix)
        break;


        case 1:
// --------------(Example 1)
        image_mtx *= agg::translation_matrix(-m_image_center_x, -m_image_center_y);
        image_mtx *= agg::rotation_matrix(m_polygon_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_polygon_scale.value());
        image_mtx *= agg::translation_matrix(m_polygon_cx, m_polygon_cy);
        image_mtx.invert();
        break;



        case 2:
// --------------(Example 2)
        image_mtx *= agg::translation_matrix(-m_image_center_x, -m_image_center_y);
        image_mtx *= agg::rotation_matrix(m_image_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_image_scale.value());
        image_mtx *= agg::translation_matrix(m_image_cx, m_image_cy);
        image_mtx.invert();
        break;



        case 3:
// --------------(Example 3)
        image_mtx *= agg::translation_matrix(-m_image_center_x, -m_image_center_y);
        image_mtx *= agg::rotation_matrix(m_image_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_image_scale.value());
        image_mtx *= agg::translation_matrix(m_polygon_cx, m_polygon_cy);
        image_mtx.invert();
        break;



        case 4:
// --------------(Example 4)
        image_mtx *= agg::translation_matrix(-m_image_cx, -m_image_cy);
        image_mtx *= agg::rotation_matrix(m_polygon_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_polygon_scale.value());
        image_mtx *= agg::translation_matrix(m_polygon_cx, m_polygon_cy);
        image_mtx.invert();
        break;



        case 5:
// --------------(Example 5)
        image_mtx *= agg::translation_matrix(-m_image_center_x, -m_image_center_y);
        image_mtx *= agg::rotation_matrix(m_image_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::rotation_matrix(m_polygon_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_image_scale.value());
        image_mtx *= agg::scaling_matrix(m_polygon_scale.value());
        image_mtx *= agg::translation_matrix(m_image_cx, m_image_cy);
        image_mtx.invert();
        break;



        case 6:
// --------------(Example 6)
        image_mtx *= agg::translation_matrix(-m_image_cx, -m_image_cy);
        image_mtx *= agg::rotation_matrix(m_image_angle.value() * agg::pi / 180.0);
        image_mtx *= agg::scaling_matrix(m_image_scale.value());
        image_mtx *= agg::translation_matrix(m_image_cx, m_image_cy);
        image_mtx.invert();
        break;

        }

        typedef agg::span_interpolator_linear<> interpolator_type;
        interpolator_type interpolator(image_mtx);
        agg::span_allocator<agg::rgba8> sa;


/*
        // nearest neighbor
        //------------------------------------------
        typedef agg::span_image_filter_rgba32_nn<agg::order_bgra32, interpolator_type> span_gen_type;
        typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

        span_gen_type sg(sa, rbuf_img(0), agg::rgba(1,1,1,0), interpolator);
        //------------------------------------------
*/




        // "hardcoded" bilinear filter
        //------------------------------------------
        typedef agg::span_image_filter_rgba32_bilinear<agg::order_bgra32, interpolator_type> span_gen_type;
        typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

        span_gen_type sg(sa, rbuf_img(0), agg::rgba(1,1,1,0), interpolator);
        //------------------------------------------




/*
        // arbitrary filter
        //------------------------------------------
        typedef agg::span_image_filter_rgba32<agg::order_bgra32, interpolator_type> span_gen_type;
        typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

        agg::image_filter<agg::image_filter_spline36> filter;

        span_gen_type sg(sa, rbuf_img(0), agg::rgba(1,1,1,0), interpolator, filter);
        //------------------------------------------
*/



        renderer_type ri(rb, sg);

        agg::rasterizer_scanline_aa<> pf;
        agg::scanline_u8 sl;
        agg::path_storage ps;
        create_star(ps);

        agg::conv_transform<agg::path_storage> tr(ps, polygon_mtx);

        pf.add_path(tr);
        pf.render(sl, ri);

        agg::ellipse e1(m_image_cx, m_image_cy, 5, 5, 20);
        agg::ellipse e2(m_image_cx, m_image_cy, 2, 2, 20);
        agg::conv_stroke<agg::ellipse> c1(e1);

        rs.color(agg::rgba(0.7,0.8,0));
        pf.add_path(e1);
        pf.render(sl, rs);

        rs.color(agg::rgba(0,0,0));
        pf.add_path(c1);
        pf.render(sl, rs);

        pf.add_path(e2);
        pf.render(sl, rs);

        pf.render_ctrl(sl, rs, m_polygon_angle);
        pf.render_ctrl(sl, rs, m_polygon_scale);
        pf.render_ctrl(sl, rs, m_image_angle);
        pf.render_ctrl(sl, rs, m_image_scale);
        pf.render_ctrl(sl, rs, m_rotate_polygon);
        pf.render_ctrl(sl, rs, m_rotate_image);
        pf.render_ctrl(sl, rs, m_example);
    }



    //------------------------------------------------------------------------
    virtual void on_mouse_button_down(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            if(sqrt((x - m_image_cx) * (x - m_image_cx) + 
                    (y - m_image_cy) * (y - m_image_cy) ) < 5.0)
            {
                m_dx = x - m_image_cx;
                m_dy = y - m_image_cy;
                m_flag = 1;
            }
            else
            {
                agg::rasterizer_scanline_aa<> pf;
                agg::affine_matrix polygon_mtx;

                polygon_mtx *= agg::translation_matrix(-m_polygon_cx, -m_polygon_cy);
                polygon_mtx *= agg::rotation_matrix(m_polygon_angle.value() * agg::pi / 180.0);
                polygon_mtx *= agg::scaling_matrix(m_polygon_scale.value(), m_polygon_scale.value());
                polygon_mtx *= agg::translation_matrix(m_polygon_cx, m_polygon_cy);

                agg::path_storage ps;
                create_star(ps);

                agg::conv_transform<agg::path_storage> tr(ps, polygon_mtx);
                pf.add_path(tr);
                if(pf.hit_test(x, y))
                {
                    m_dx = x - m_polygon_cx;
                    m_dy = y - m_polygon_cy;
                    m_flag = 2;
                }
            }
        }
    }


    //------------------------------------------------------------------------
    virtual void on_mouse_move(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            if(m_flag == 1)
            {
                m_image_cx = x - m_dx;
                m_image_cy = y - m_dy;
                force_redraw();
            }

            if(m_flag == 2)
            {
                m_polygon_cx = x - m_dx;
                m_polygon_cy = y - m_dy;
                force_redraw();
            }
        }
        else
        {
            on_mouse_button_up(x, y, flags);
        }
    }






    //------------------------------------------------------------------------
    virtual void on_mouse_button_up(int x, int y, unsigned flags)
    {
        m_flag = 0;
    }



    //------------------------------------------------------------------------
    virtual void on_ctrl_change() 
    {
        if(m_rotate_polygon.status() || m_rotate_image.status())
        {
            wait_mode(false);
        }
        else
        {
            wait_mode(true);
        }
        force_redraw();
    }

    //------------------------------------------------------------------------
    virtual void on_idle() 
    {
        bool redraw = false;
        if(m_rotate_polygon.status())
        {
            m_polygon_angle.value(m_polygon_angle.value() + 0.5);
            if(m_polygon_angle.value() >= 180.0) 
            {
                m_polygon_angle.value(m_polygon_angle.value() - 360.0);
            }
            redraw = true;
        }

        if(m_rotate_image.status())
        {
            m_image_angle.value(m_image_angle.value() + 0.5);
            if(m_image_angle.value() >= 180.0) 
            {
                m_image_angle.value(m_image_angle.value() - 360.0);
            }
            redraw = true;
        }

        if(redraw) force_redraw();

    }


};





int agg_main(int argc, char* argv[])
{
    the_application app(agg::pix_format_bgra32, flip_y);
    app.caption("Image Affine Transformations with filtering");

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

    if(app.init(app.rbuf_img(0).width(), 
                app.rbuf_img(0).height(), 
                0))
    {
        return app.run();
    }
    return 0;
}


