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
// Arc generator. Produces at most 4 consecutive cubic bezier curves, i.e., 
// 4, 7, 10, or 13 vertices.
//
//----------------------------------------------------------------------------

#ifndef AGG_BEZIER_ARC_INCLUDED
#define AGG_BEZIER_ARC_INCLUDED

#include "agg_basics.h"

namespace agg
{

    //------------------------------------------------------------------------
    void arc_to_bezier(double cx, double cy, double rx, double ry, 
                       double start_angle, double sweep_angle,
                       double* curve);


    //========================================================================
    class bezier_arc
    {
    public:
        bezier_arc() : m_vertex(26) {}
        bezier_arc(double x,  double y, 
                   double rx, double ry, 
                   double a1, double a2, 
                   bool ccw=true)
        {
            init(x, y, rx, ry, a1, a2, ccw);
        }

        void init(double x,  double y, 
                  double rx, double ry, 
                  double a1, double a2, 
                  bool ccw=true);

        //--------------------------------------------------------------------
        void rewind(unsigned)
        {
            m_vertex = 0;
        }

        //--------------------------------------------------------------------
        unsigned vertex(double* x, double* y)
        {
            if(m_vertex >= m_num_vertices) return path_cmd_stop;
            *x = m_vertices[m_vertex];
            *y = m_vertices[m_vertex + 1];
            m_vertex += 2;
            return (m_vertex == 2) ? path_cmd_move_to : path_cmd_curve4;
        }

    private:
        unsigned m_vertex;
        unsigned m_num_vertices;
        double   m_vertices[26];
    };

}


#endif
