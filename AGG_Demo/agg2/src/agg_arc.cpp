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
// Arc vertex generator
//
//----------------------------------------------------------------------------

#include <math.h>
#include "agg_arc.h"


namespace agg
{
    //------------------------------------------------------------------------
    arc::arc(double x,  double y, 
             double rx, double ry, 
             double a1, double a2, 
             bool ccw) :
        m_x(x), m_y(y), m_rx(rx), m_ry(ry), m_scale(1.0)
    {
        normalize(a1, a2, ccw);
    }

    //------------------------------------------------------------------------
    void arc::init(double x,  double y, 
                   double rx, double ry, 
                   double a1, double a2, 
                   bool ccw)
    {
        m_x   = x;  m_y  = y;
        m_rx  = rx; m_ry = ry; 
        normalize(a1, a2, ccw);
    }

    //------------------------------------------------------------------------
    void arc::rewind(unsigned)
    {
        m_path_cmd = path_cmd_move_to; 
        m_angle = m_start;
    }

    //------------------------------------------------------------------------
    unsigned arc::vertex(double* x, double* y)
    {
        if(is_stop(m_path_cmd)) return path_cmd_stop;
        if((m_angle < m_end) != m_ccw)
        {
            *x = m_x + cos(m_end) * m_rx;
            *y = m_y + sin(m_end) * m_ry;
            m_path_cmd = path_cmd_stop;
            return path_cmd_line_to;
        }

        *x = m_x + cos(m_angle) * m_rx;
        *y = m_y + sin(m_angle) * m_ry;

        m_angle += m_da;

        unsigned pf = m_path_cmd;
        m_path_cmd = path_cmd_line_to;
        return pf;
    }

    //------------------------------------------------------------------------
    void arc::normalize(double a1, double a2, bool ccw)
    {
        m_da = fabs(1.0 / ((m_rx + m_ry) * 0.5 * m_scale));
        if(ccw)
        {
            while(a2 < a1) a2 += pi * 2.0;
        }
        else
        {
            while(a1 < a2) a1 += pi * 2.0;
            m_da   = -m_da;
        }
        m_ccw   = ccw;
        m_start = a1;
        m_end   = a2;
    }

}
