//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.0 
// Copyright (C) 2002 Maxim Shemanarev (McSeem)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
// The author gratefully acknowleges the support of David Turner, 
// Robert Wilhelm, and Werner Lemberg - the authors of the FreeType 
// libray - in producing this work. See http://www.freetype.org for details.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// Class rasterizer_scanline_aa
//         
//
//----------------------------------------------------------------------------
#ifndef AGG_RASTERIZER_SCANLINE_AA_INCLUDED
#define AGG_RASTERIZER_SCANLINE_AA_INCLUDED

#include <string.h>
#include <math.h>
#include "agg_basics.h"
#include "agg_math.h"
#include "agg_gamma_functions.h"
#include "agg_clip_liang_barsky.h"


namespace agg
{

    //------------------------------------------------------------------------
    // These constants determine the subpixel accuracy, to be more precise, 
    // the number of bits of the fractional part of the coordinates. 
    // The possible coordinate capacity in bits can be calculated by formula:
    // sizeof(int) * 8 - poly_base_shift * 2, i.e, for 32-bit integers and
    // 8-bits fractional part the capacity is 16 bits or [-32768...32767].
    enum
    {
        poly_base_shift = 8,
        poly_base_size  = 1 << poly_base_shift,
        poly_base_mask  = poly_base_size - 1
    };
    
    //------------------------------------------------------------------------
    inline int poly_coord(double c)
    {
        return int(c * poly_base_size);
    }

    //------------------------------------------------------------------------
    // A pixel cell. There're no constructors defined and it was done 
    // intentionally in order to avoid extra overhead when allocating an 
    // array of cells.
    struct cell_aa
    {
        int16 x;
        int16 y;
        int   packed_coord;
        int   cover;
        int   area;

        void set(int x, int y, int c, int a);
        void set_coord(int x, int y);
        void set_cover(int c, int a);
        void add_cover(int c, int a);
    };


    //------------------------------------------------------------------------
    // An internal class that implements the main rasterization algorithm.
    // Used in the rasterizer. Should not be used direcly.
    class outline_aa
    {
        enum
        {
            cell_block_shift = 12,
            cell_block_size  = 1 << cell_block_shift,
            cell_block_mask  = cell_block_size - 1,
            cell_block_pool  = 256,
            cell_block_limit = 1024
        };

    public:

        ~outline_aa();
        outline_aa();

        void reset();

        void move_to(int x, int y);
        void line_to(int x, int y);

        int min_x() const { return m_min_x; }
        int max_x() const { return m_max_x; }

        unsigned num_cells() const { return m_num_cells; }
        const cell_aa* const* cells();
        bool sorted() const { return m_sorted; }

    private:
        outline_aa(const outline_aa&);
        const outline_aa& operator = (const outline_aa&);

        void set_cur_cell(int x, int y);
        void add_cur_cell();
        void sort_cells();
        void render_hline(int ey, int x1, int y1, int x2, int y2);
        void render_line(int x1, int y1, int x2, int y2);
        void allocate_block();
        
        static void qsort_cells(cell_aa** start, unsigned num);

    private:
        unsigned  m_num_blocks;
        unsigned  m_max_blocks;
        unsigned  m_cur_block;
        unsigned  m_num_cells;
        cell_aa** m_cells;
        cell_aa*  m_cur_cell_ptr;
        cell_aa** m_sorted_cells;
        unsigned  m_sorted_size;
        cell_aa   m_cur_cell;
        int       m_cur_x;
        int       m_cur_y;
        int       m_min_x;
        int       m_max_x;
        bool      m_sorted;
    };


    //------------------------------------------------------------------------
    enum filling_rule_e
    {
        fill_non_zero,
        fill_even_odd
    };


    
    //------------------------------------------------------------------------
    // Determine the clipping code of the vertex according to the 
    // Cyrus-Beck line clipping algorithm
    //
    //        |        |
    //  0110  |  0010  | 0011
    //        |        |
    // -------+--------+-------- clip_box.y2
    //        |        |
    //  0100  |  0000  | 0001
    //        |        |
    // -------+--------+-------- clip_box.y1
    //        |        |
    //  1100  |  1000  | 1001
    //        |        |
    //  clip_box.x1  clip_box.x2
    //
    // 
    inline unsigned clipping_flags(int x, int y, const rect& clip_box)
    {
        return  (x > clip_box.x2) |
               ((y > clip_box.y2) << 1) |
               ((x < clip_box.x1) << 2) |
               ((y < clip_box.y1) << 3);
    }


    //========================================================================
    // Polygon rasterizer that is used to render filled polygons with 
    // high-quality Anti-Aliasing. Internally, by default, the class uses 
    // integer coordinates in format 24.8, i.e. 24 bits for integer part 
    // and 8 bits for fractional - see poly_base_shift. This class can be 
    // used in the following  way:
    //
    // 1. filling_rule(filling_rule_e ft) - optional.
    //
    // 2. gamma() - optional.
    //
    // 3. reset()
    //
    // 4. move_to(x, y) / line_to(x, y) - make the polygon. One can create 
    //    more than one contour, but each contour must consist of at least 3
    //    vertices, i.e. move_to(x1, y1); line_to(x2, y2); line_to(x3, y3);
    //    is the absolute minimum of vertices that define a triangle.
    //    The algorithm does not check either the number of vertices nor
    //    coincidence of their coordinates, but in the worst case it just 
    //    won't draw anything.
    //    The orger of the vertices (clockwise or counterclockwise) 
    //    is important when using the non-zero filling rule (fill_non_zero).
    //    In this case the vertex order of all the contours must be the same
    //    if you want your intersecting polygons to be without "holes".
    //    You actually can use different vertices order. If the contours do not 
    //    intersect each other the order is not important anyway. If they do, 
    //    contours with the same vertex order will be rendered without "holes" 
    //    while the intersecting contours with different orders will have "holes".
    //
    // filling_rule() and gamma() can be called anytime before "sweeping".
    //------------------------------------------------------------------------
    template<unsigned AA_Shift=8> class rasterizer_scanline_aa
    {
        enum status
        {
            status_initial,
            status_line_to,
            status_closed
        };

    public:
        enum
        {
            aa_shift = AA_Shift,
            aa_num   = 1 << aa_shift,
            aa_mask  = aa_num - 1,
            aa_2num  = aa_num * 2,
            aa_2mask = aa_2num - 1
        };

        //--------------------------------------------------------------------
        rasterizer_scanline_aa() : 
            m_filling_rule(fill_non_zero),
            m_clipped_start_x(0),
            m_clipped_start_y(0),
            m_start_x(0),
            m_start_y(0),
            m_prev_x(0),
            m_prev_y(0),
            m_prev_flags(0),
            m_status(status_initial),
            m_clipping(false)
        {
            int i;
            for(i = 0; i < aa_num; i++) m_gamma[i] = i;
        }

        //--------------------------------------------------------------------
        template<class GammaF> 
        rasterizer_scanline_aa(const GammaF& gamma_function) : 
            m_filling_rule(fill_non_zero),
            m_clipped_start_x(0),
            m_clipped_start_y(0),
            m_start_x(0),
            m_start_y(0),
            m_prev_x(0),
            m_prev_y(0),
            m_prev_flags(0),
            m_status(status_initial),
            m_clipping(false)
        {
            gamma(gamma_function);
        }

        //--------------------------------------------------------------------
        void reset(); 
        void filling_rule(filling_rule_e filling_rule);
        void clip_box(double x1, double y1, double x2, double y2);
        void reset_clipping();

        //--------------------------------------------------------------------
        template<class GammaF> void gamma(const GammaF& gamma_function)
        { 
            int i;
            for(i = 0; i < aa_num; i++)
            {
                m_gamma[i] = int(gamma_function(double(i) / aa_mask) * aa_mask);
            }
        }

        //--------------------------------------------------------------------
        void add_vertex(double x, double y, unsigned cmd);
        void move_to(int x, int y);
        void line_to(int x, int y);
        void close_polygon();
        void move_to_d(double x, double y);
        void line_to_d(double x, double y);
        

        //--------------------------------------------------------------------
        int min_x() const { return m_outline.min_x(); }
        int max_x() const { return m_outline.max_x(); }

        //--------------------------------------------------------------------
        unsigned calculate_alpha(int area) const
        {
            int cover = area >> (poly_base_shift*2 + 1 - aa_shift);

            if(cover < 0) cover = -cover;
            if(m_filling_rule == fill_even_odd)
            {
                cover &= aa_2mask;
                if(cover > aa_num)
                {
                    cover = aa_2num - cover;
                }
            }
            if(cover > aa_mask) cover = aa_mask;
            return m_gamma[cover];
        }

        //--------------------------------------------------------------------
        void sort()
        {
            if(m_outline.num_cells() == 0) return;
            m_outline.cells();
        }
        
        //--------------------------------------------------------------------
        template<class Scanline, class Renderer> 
        void render(Scanline& sl, Renderer& r, int dx=0, int dy=0)
        {
            close_polygon();
            if(m_outline.num_cells() == 0) return;
            const cell_aa* const* cells = m_outline.cells();

            int x, y;
            int cover;
            int alpha;
            int area;

            sl.reset(min_x(), max_x(), dx, dy);
            r.prepare(unsigned(max_x() - min_x() + 2));

            cover = 0;
            const cell_aa* cur_cell = *cells++;
            for(;;)
            {
                const cell_aa* start_cell = cur_cell;

                int coord  = cur_cell->packed_coord;
                x = cur_cell->x;
                y = cur_cell->y;

                area   = start_cell->area;
                cover += start_cell->cover;

                //accumulate all start cells
                while((cur_cell = *cells++) != 0)
                {
                    if(cur_cell->packed_coord != coord) break;
                    area  += cur_cell->area;
                    cover += cur_cell->cover;
                }

                if(area)
                {
                    alpha = calculate_alpha((cover << (poly_base_shift + 1)) - area);
                    if(alpha)
                    {
                        if(sl.is_ready(y))
                        {
                            sl.prepare();
                            r.render(sl);
                            sl.reset_spans();
                        }
                        sl.add_cell(x, y, alpha);//m_gamma.calculate(alpha));
                    }
                    x++;
                }

                if(!cur_cell) break;

                if(cur_cell->x > x)
                {
                    alpha = calculate_alpha(cover << (poly_base_shift + 1));
                    if(alpha)
                    {
                        if(sl.is_ready(y))
                        {
                            sl.prepare();
                            r.render(sl);
                            sl.reset_spans();
                        }
                        sl.add_span(x, y, 
                                    cur_cell->x - x, 
                                    alpha);//m_gamma.calculate(alpha));
                    }
                }
            } 
        
            if(sl.num_spans())
            {
                sl.prepare();
                r.render(sl);
            }
        }


        //--------------------------------------------------------------------
        bool hit_test(int tx, int ty);


        //--------------------------------------------------------------------
        void add_xy(const double* x, const double* y, unsigned n)
        {
            if(n > 2)
            {
                move_to_d(*x++, *y++);
                --n;
                do
                {
                    line_to_d(*x++, *y++);
                }
                while(--n);
            }
        }

        //-------------------------------------------------------------------
        template<class VertexSource>
        void add_path(VertexSource& vs, unsigned id=0)
        {
            double x;
            double y;

            unsigned cmd;
            vs.rewind(id);
            while(!is_stop(cmd = vs.vertex(&x, &y)))
            {
                add_vertex(x, y, cmd);
            }
        }


        //--------------------------------------------------------------------
        template<class VertexSource>
        void add_path(VertexSource& vs, double dx, double dy, unsigned id=0)
        {
            double x;
            double y;

            unsigned cmd;
            vs.rewind(id);
            while(!is_stop(cmd = vs.vertex(&x, &y)))
            {
                x += dx;
                y += dy;
                add_vertex(x, y, cmd);
            }
        }


        //--------------------------------------------------------------------
        template<class VertexIterator>
        void add_path(VertexIterator begin, VertexIterator end, 
                      double dx=0, double dy=0)
        {
            while(begin != end)
            {
                double x = begin->x + dx;
                double y = begin->y + dy;
                add_vertex(x, y, cmd);
                ++begin;
            }
        }


        //--------------------------------------------------------------------
        template<class Scanline, class Renderer, 
                 class VertexSource, class ColorStorage, class PathId>
        void render_all_paths(Scanline& sl,
                              Renderer& r, 
                              VertexSource& vs, 
                              const ColorStorage& as, 
                              const PathId& id,
                              unsigned num_paths)
        {
            for(unsigned i = 0; i < num_paths; i++)
            {
                reset();
                add_path(vs, id[i]);
                r.color(as[i]);
                render(sl, r);
            }
        }


        //--------------------------------------------------------------------
        template<class Scanline, class Renderer, 
                 class VertexSource, class ColorStorage, class PathId>
        void render_all_paths(Scanline& sl,
                              Renderer& r, 
                              VertexSource& vs, 
                              const ColorStorage& as, 
                              const PathId& id,
                              unsigned num_paths,
                              double dx,
                              double dy)
        {
            for(unsigned i = 0; i < num_paths; i++)
            {
                reset();
                add_path(vs, dx, dy, id[i]);
                r.color(as[i]);
                render(sl, r);
            }
        }



        //--------------------------------------------------------------------
        template<class Scanline, class Renderer, class Ctrl> 
        void render_ctrl(Scanline& sl, Renderer& r, Ctrl& c)
        {
            unsigned i;
            for(i = 0; i < c.num_paths(); i++)
            {
                reset();
                add_path(c, i);
                r.color(c.color(i));
                render(sl, r);
            }
        }


    private:
        //--------------------------------------------------------------------
        // Disable copying
        rasterizer_scanline_aa(const rasterizer_scanline_aa&);
        const rasterizer_scanline_aa& operator = (const rasterizer_scanline_aa&);

        //--------------------------------------------------------------------
        void move_to_no_clip(int x, int y);
        void line_to_no_clip(int x, int y);
        void close_polygon_no_clip();
        void clip_segment(int x, int y);

    private:
        outline_aa     m_outline;
        int            m_gamma[aa_num];
        filling_rule_e m_filling_rule;
        int            m_clipped_start_x;
        int            m_clipped_start_y;
        int            m_start_x;
        int            m_start_y;
        int            m_prev_x;
        int            m_prev_y;
        unsigned       m_prev_flags;
        unsigned       m_status;
        rect           m_clip_box;
        bool           m_clipping;
    };
















    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::reset() 
    { 
        m_outline.reset(); 
        m_status = status_initial;
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::filling_rule(filling_rule_e filling_rule) 
    { 
        m_filling_rule = filling_rule; 
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::clip_box(double x1, double y1, double x2, double y2)
    {
        reset();
        m_clip_box = rect(poly_coord(x1), poly_coord(y1),
                          poly_coord(x2), poly_coord(y2));
        m_clip_box.normalize();
        m_clipping = true;
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::reset_clipping()
    {
        reset();
        m_clipping = false;
    }



    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::move_to_no_clip(int x, int y)
    {
        if(m_status == status_line_to)
        {
            close_polygon_no_clip();
        }
        m_outline.move_to(x, y); 
        m_clipped_start_x = x;
        m_clipped_start_y = y;
        m_status = status_line_to;
    }


    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::line_to_no_clip(int x, int y)
    {
        if(m_status != status_initial)
        {
            m_outline.line_to(x, y); 
            m_status = status_line_to;
        }
    }


    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::close_polygon_no_clip()
    {
        if(m_status == status_line_to)
        {
            m_outline.line_to(m_clipped_start_x, m_clipped_start_y);
            m_status = status_closed;
        }
    }


    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::clip_segment(int x, int y) 
    {
        unsigned flags = clipping_flags(x, y, m_clip_box);
        if(m_prev_flags == flags)
        {
            if(flags == 0)
            {
                if(m_status == status_initial)
                {
                    move_to_no_clip(x, y);
                }
                else
                {
                    line_to_no_clip(x, y);
                }
            }
        }
        else
        {
            int cx[4];
            int cy[4];
            unsigned n = clip_liang_barsky(m_prev_x, m_prev_y, 
                                           x, y, 
                                           m_clip_box, 
                                           cx, cy);
            const int* px = cx;
            const int* py = cy;
            while(n--)
            {
                if(m_status == status_initial)
                {
                    move_to_no_clip(*px++, *py++);
                }
                else
                {
                    line_to_no_clip(*px++, *py++);
                }
            }
        }
        m_prev_flags = flags;
        m_prev_x = x;
        m_prev_y = y;
    }



    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::add_vertex(double x, double y, unsigned cmd)
    {
        if(is_close(cmd))
        {
            close_polygon();
        }
        else
        {
            if(is_move_to(cmd)) 
            {
                move_to(poly_coord(x), poly_coord(y));
            }
            else 
            {
                if(is_vertex(cmd))
                {
                    line_to(poly_coord(x), poly_coord(y));
                }
            }
        }
    }



    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::move_to(int x, int y) 
    { 
        if(m_clipping)
        {
            if(m_outline.sorted()) 
            {
                reset();
            }
            if(m_status == status_line_to)
            {
                close_polygon();
            }
            m_prev_x = m_start_x = x;
            m_prev_y = m_start_y = y;
            m_status = status_initial;
            m_prev_flags = clipping_flags(x, y, m_clip_box);
            if(m_prev_flags == 0)
            {
                move_to_no_clip(x, y);
            }
        }
        else
        {
            move_to_no_clip(x, y);
        }
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::line_to(int x, int y) 
    { 
        if(m_clipping)
        {
            clip_segment(x, y);
        }
        else
        {
            line_to_no_clip(x, y);
        }
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::close_polygon() 
    { 
        if(m_clipping)
        {
            clip_segment(m_start_x, m_start_y);
        }
        close_polygon_no_clip();
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::move_to_d(double x, double y) 
    { 
        move_to(poly_coord(x), poly_coord(y)); 
    }

    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    void rasterizer_scanline_aa<AA_Shift>::line_to_d(double x, double y) 
    { 
        line_to(poly_coord(x), poly_coord(y)); 
    }


    //------------------------------------------------------------------------
    template<unsigned AA_Shift> 
    bool rasterizer_scanline_aa<AA_Shift>::hit_test(int tx, int ty)
    {
        close_polygon();
        if(m_outline.num_cells() == 0) return false;
        const cell_aa* const* cells = m_outline.cells();

        int x, y;
        int cover;
        int alpha;
        int area;

        cover = 0;
        const cell_aa* cur_cell = *cells++;
        for(;;)
        {
            const cell_aa* start_cell = cur_cell;

            int coord  = cur_cell->packed_coord;
            x = cur_cell->x;
            y = cur_cell->y;

            if(y > ty) return false;

            area   = start_cell->area;
            cover += start_cell->cover;

            while((cur_cell = *cells++) != 0)
            {
                if(cur_cell->packed_coord != coord) break;
                area  += cur_cell->area;
                cover += cur_cell->cover;
            }

            if(area)
            {
                alpha = calculate_alpha((cover << (poly_base_shift + 1)) - area);
                if(alpha)
                {
                    if(tx == x && ty == y) return true;
                }
                x++;
            }

            if(!cur_cell) break;

            if(cur_cell->x > x)
            {
                alpha = calculate_alpha(cover << (poly_base_shift + 1));
                if(alpha)
                {
                    if(ty == y && tx >= x && tx <= cur_cell->x) return true;
                }
            }
        }
        return false;
    }

}



#endif

