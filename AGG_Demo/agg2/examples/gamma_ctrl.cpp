#include <stdio.h>
#include <stdlib.h>
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p32.h"
#include "agg_renderer_scanline.h"
#include "agg_gsv_text.h"
#include "agg_conv_stroke.h"
#include "agg_color_rgba8.h"
#include "agg_path_storage.h"
#include "ctrl/agg_gamma_ctrl.h"
#include "platform/agg_platform_support.h"

//#define AGG_BGR24 
//#define AGG_RGB24
#define AGG_BGRA32 
//#define AGG_RGBA32 
//#define AGG_ARGB32 
//#define AGG_ABGR32
//#define AGG_RGB565
//#define AGG_RGB555
#include "pixel_formats.h"

enum { flip_y = true };


agg::gamma_ctrl<agg::rgba8> g_ctrl(10.0, 10.0, 300.0, 200.0, !flip_y);

void read_gamma()
{
    FILE* fd = fopen("gamma.txt", "rb");
    if(fd)
    {
        char buf[32];
        double kx1, ky1, kx2, ky2;
        fgets(buf, 30, fd); kx1 = atof(buf);
        fgets(buf, 30, fd); ky1 = atof(buf);
        fgets(buf, 30, fd); kx2 = atof(buf);
        fgets(buf, 30, fd); ky2 = atof(buf);
        g_ctrl.values(kx1, ky1, kx2, ky2);
        fclose(fd);
    }
}



void write_gamma()
{
    const unsigned char* gamma = g_ctrl.gamma();
    FILE* fd = fopen("gamma.bin", "wb");
    if(fd)
    {
        fwrite(gamma, 256, 1, fd);
        fclose(fd);
    }

    fd = fopen("gamma.txt", "wt");
    if(fd)
    {
        double kx1, ky1, kx2, ky2;
        g_ctrl.values(&kx1, &ky1, &kx2, &ky2);
        fprintf(fd, "%5.3f\n", kx1);
        fprintf(fd, "%5.3f\n", ky1);
        fprintf(fd, "%5.3f\n", kx2);
        fprintf(fd, "%5.3f\n", ky2);
        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                fprintf(fd, "%3d,", gamma[i*16 + j]);
            }
            fprintf(fd, "\n");
        }
        fclose(fd);
    }
}



class the_application : public agg::platform_support
{
public:
    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y)        
    {
        add_ctrl(g_ctrl);
    }

    virtual void on_init()
    {
        read_gamma();
    }

    virtual ~the_application()
    {
        write_gamma();
    }

    virtual void on_draw()
    {
        int width = rbuf_window().width();
        int height = rbuf_window().height();
        double ewidth = initial_width() / 2 - 10;
        double ecenter = initial_width() / 2;

        typedef agg::renderer_base<pixfmt> ren_base;
        typedef agg::renderer_scanline_p_solid<ren_base> renderer;

        pixfmt pixf(rbuf_window());
        ren_base rb(pixf);
        renderer r(rb);
        rb.clear(agg::rgba(1, 1, 1));

        g_ctrl.text_size(10.0, 12.0);

        agg::rasterizer_scanline_aa<> ras;
        agg::scanline_p8 sl;

        ras.render_ctrl(sl, r, g_ctrl);

        ras.gamma(g_ctrl);

        agg::ellipse ellipse;
        agg::conv_stroke<agg::ellipse> poly(ellipse);
        agg::conv_transform<agg::conv_stroke<agg::ellipse> > tpoly(poly, resizing_matrix());
        r.color(agg::rgba8(0, 0, 0));

        ellipse.init(ecenter, 220, ewidth, 15.5, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 220, 11, 11, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        r.color(agg::rgba8(127, 127, 127));

        ellipse.init(ecenter, 260, ewidth, 15.5, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 260, 11, 11, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        r.color(agg::rgba8(192, 192, 192));

        ellipse.init(ecenter, 300, ewidth, 15.5, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 300, 11, 11, 100);
        poly.width(2.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        r.color(agg::rgba(0.0, 0.0, 0.4));

        ellipse.init(ecenter, 340, ewidth, 15.5, 100);
        poly.width(1.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 340, 11, 11, 100);
        poly.width(1.0);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 380, ewidth, 15.5, 100);
        poly.width(0.4);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 380, 11, 11, 100);
        poly.width(0.4);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 420, ewidth, 15.5, 100);
        poly.width(0.1);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        ellipse.init(ecenter, 420, 11, 11, 100);
        poly.width(0.1);
        ras.add_path(tpoly, 0);
        ras.render(sl, r);

        agg::affine_matrix mtx;
        mtx *= agg::skewing_matrix(0.15, 0.0);
        mtx *= resizing_matrix();
        agg::gsv_text text;
        agg::gsv_text_outline<agg::affine_matrix> text1(text, mtx);
        text.text("Text 2345");
        text.size(50, 20);
        text1.width(2.0);
        text.start_point(320, 10);

        r.color(agg::rgba(0.0, 0.5, 0.0));
        ras.add_path(text1, 0);
        ras.render(sl, r);


        r.color(agg::rgba(0.5, 0.0, 0.0));
        agg::path_storage path;
        path.move_to(30, -1.0);
        path.line_to(60, 0.0);
        path.line_to(30, 1.0);

        path.move_to(27, -1.0);
        path.line_to(10, 0.0);
        path.line_to(27, 1.0);

        agg::conv_transform<agg::path_storage>  trans(path, mtx);

        for(int i = 0; i < 35; i++)
        {
            mtx.reset();
            mtx *= agg::rotation_matrix(double(i) / 35.0 * agg::pi * 2.0);
            mtx *= agg::translation_matrix(400, 130);
            mtx *= resizing_matrix();
            ras.add_path(trans, 0);
            ras.render(sl, r);
        }
    }
};



int agg_main(int argc, char* argv[])
{
    the_application app(pix_format, flip_y);
    app.caption("Anti-Aliasing Gamma Correction");

    if(app.init(500, 400, agg::window_resize))
    {
        return app.run();
    }
    return 1;
}


