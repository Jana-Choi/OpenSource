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

#include <math.h>
#include "agg_trans_warp_magnifier.h"

namespace agg
{

    void trans_warp_magnifier::transform(double* x, double* y) const
    {
        double dx = *x - m_xc;
        double dy = *y - m_yc;
        double r = sqrt(dx * dx + dy * dy);
        if(r < m_radius)
        {
            *x = m_xc + dx * m_magn;
            *y = m_yc + dy * m_magn;
            return;
        }

        if(m_warp)
        {
            double m = (r + m_radius * (m_magn - 1.0)) / r;
            *x = m_xc + dx * m;
            *y = m_yc + dy * m;
            return;
        }

        if(r < m_radius * m_magn)
        {
            double m = m_radius * m_magn / r;
            *x = m_xc + dx * m;
            *y = m_yc + dy * m;
        }
    }

}
