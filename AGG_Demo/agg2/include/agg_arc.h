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

#ifndef AGG_ARC_INCLUDED
#define AGG_ARC_INCLUDED

#include <math.h>
#include "agg_basics.h"

namespace agg
{

    //------------------------------------------------------------------------
    class arc
    {
    public:
        arc() : m_scale(1.0) {}
        arc(double x,  double y, 
            double rx, double ry, 
            double a1, double a2, 
            bool ccw=true);

        void init(double x,  double y, 
                  double rx, double ry, 
                  double a1, double a2, 
                  bool ccw=true);

        void approximation_scale(double s) { m_scale = s; }
        double approximation_scale() const { return m_scale;  }

        void rewind(unsigned);
        unsigned vertex(double* x, double* y);

    private:
        void normalize(double a1, double a2, bool ccw);

        double   m_x;
        double   m_y;
        double   m_rx;
        double   m_ry;
        double   m_angle;
        double   m_start;
        double   m_end;
        double   m_scale;
        double   m_da;
        bool     m_ccw;
        unsigned m_path_cmd;
    };


}


#endif
