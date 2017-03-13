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


#include <math.h>
#include "agg_bezier_arc.h"


namespace agg
{

    //------------------------------------------------------------------------
    void arc_to_bezier(double cx, double cy, double rx, double ry, 
                       double start_angle, double sweep_angle,
                       double* curve)
    {
        double x0 = cos(sweep_angle / 2.0);
        double y0 = sin(sweep_angle / 2.0);
        double tx = (1.0 - x0) * 4.0 / 3.0;
        double ty = y0 - tx * x0 / y0;
        double px[4];
        double py[4];
        px[0] =  x0;
        py[0] = -y0;
        px[1] =  x0 + tx;
        py[1] = -ty;
        px[2] =  x0 + tx;
        py[2] =  ty;
        px[3] =  x0;
        py[3] =  y0;

        double sn = sin(start_angle + sweep_angle / 2.0);
        double cs = cos(start_angle + sweep_angle / 2.0);

        unsigned i;
        for(i = 0; i < 4; i++)
        {
            curve[i * 2]     = cx + rx * (px[i] * cs - py[i] * sn);
            curve[i * 2 + 1] = cy + ry * (px[i] * sn + py[i] * cs);
        }
    }



    //------------------------------------------------------------------------
    void bezier_arc::init(double x,  double y, 
                          double rx, double ry, 
                          double a1, double a2, 
                          bool ccw)
    {
        double sweep = 0.0;
        double start = 0.0;
        double angle1 = 0.0;
        double angle2;
        bool done = false;
        m_num_vertices = 2;
        if(ccw)
        {
            while(a2 < a1) a2 += pi * 2.0;
            sweep = a2 - a1;
            while(sweep > pi * 2.0) sweep -= pi * 2.0;
            start = a1;
            do
            {
                angle2 = angle1 + pi * 0.5;
                if(angle2 >= sweep) 
                {
                    angle2 = sweep;
                    done = true;
                }
                arc_to_bezier(x, y, rx, ry, 
                              start + angle1, 
                              angle2 - angle1, 
                              m_vertices + m_num_vertices - 2);
                m_num_vertices += 6;
                angle1 = angle2;
            }
            while(!done && m_num_vertices < 26);
        }
        else
        {
            while(a1 < a2) a1 += pi * 2.0;
            sweep = a2 - a1;
            while(sweep < -pi * 2.0) sweep += pi * 2.0;
            start = a2;
            do
            {
                angle2 = angle1 - pi * 0.5;
                if(angle2 <= sweep) 
                {
                    angle2 = sweep;
                    done = true;
                }
                arc_to_bezier(x, y, rx, ry, 
                              start - angle1, 
                              angle1 - angle2, 
                              m_vertices + m_num_vertices - 2);
                m_num_vertices += 6;
                angle1 = angle2;
            }
            while(!done && m_num_vertices < 26);
        }
    }

}
