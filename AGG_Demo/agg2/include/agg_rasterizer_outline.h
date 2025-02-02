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
#ifndef AGG_RASTERIZER_OUTLINE_INCLUDED
#define AGG_RASTERIZER_OUTLINE_INCLUDED

#include "agg_basics.h"

namespace agg
{
    //============================================================================
    template<class Renderer> class rasterizer_outline
    {
    public:
        rasterizer_outline(Renderer& ren) : m_ren(&ren), m_start_x(0), m_start_y(0)
        {
        }

        //------------------------------------------------------------------------
        void move_to(int x, int y)
        {
            m_ren->move_to(m_start_x = x, m_start_y = y);
        }

        //------------------------------------------------------------------------
        void line_to(int x, int y)
        {
            m_ren->line_to(x, y);
        }

        //------------------------------------------------------------------------
        void move_to_d(double x, double y)
        {
            move_to(m_ren->coord(x), m_ren->coord(y));
        }

        //------------------------------------------------------------------------
        void line_to_d(double x, double y)
        {
            line_to(m_ren->coord(x), m_ren->coord(y));
        }

        //------------------------------------------------------------------------
        void close()
        {
            line_to(m_start_x, m_start_y);
        }

        //------------------------------------------------------------------------
        void add_vertex(double x, double y, unsigned cmd)
        {
            if(is_move_to(cmd)) 
            {
                move_to_d(x, y);
            }
            else 
            {
                if(is_end_poly(cmd))
                {
                    if(is_closed(cmd)) close();
                }
                else
                {
                    line_to_d(x, y);
                }
            }
        }


        //------------------------------------------------------------------------
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


        //------------------------------------------------------------------------
        template<class VertexSource>
        void add_path(VertexSource& vs, double dx, double dy, unsigned id=0)
        {
            double x;
            double y;

            unsigned cmd;
            bool closed = 0;
            vs.rewind(id);
            while(!is_stop(cmd = vs.vertex(&x, &y)))
            {
                x += dx;
                y += dy;
                add_vertex(x, y, cmd);
            }
        }


        //------------------------------------------------------------------------
        template<class VertexIterator>
        void add_path(VertexIterator begin, VertexIterator end, 
                      double dx=0, double dy=0)
        {
            while(begin != end)
            {
                double x = begin->x + dx;
                double y = begin->y + dy;
                add_vertex(x, y, begin->cmd);
                ++begin;
            }
        }


        //------------------------------------------------------------------------
        template<class VertexSource, class ColorStorage, class PathId>
        void render_all_paths(VertexSource& vs, 
                              const ColorStorage& colors, 
                              const PathId& id,
                              unsigned num_paths)
        {
            for(unsigned i = 0; i < num_paths; i++)
            {
                m_ren->line_color(colors[i]);
                add_path(vs, id[i]);
            }
        }


        //------------------------------------------------------------------------
        template<class VertexSource, class ColorStorage, class PathId>
        void render_all_paths(VertexSource& vs, 
                              const ColorStorage& colors, 
                              const PathId& id,
                              unsigned num_paths,
                              double dx,
                              double dy)
        {
            for(unsigned i = 0; i < num_paths; i++)
            {
                m_ren->line_color(colors[i]);
                add_path(vs, id[i], dx, dy);
            }
        }



        //------------------------------------------------------------------------
        template<class Ctrl> void render_ctrl(Ctrl& c)
        {
            unsigned i;
            for(i = 0; i < c.num_paths(); i++)
            {
                m_ren->line_color(c.color(i));
                add_path(c, i);
            }
        }


    private:
        Renderer* m_ren;
        int       m_start_x;
        int       m_start_y;
    };


}


#endif

