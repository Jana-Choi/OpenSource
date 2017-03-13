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
// class bspline
//
//----------------------------------------------------------------------------


#include "agg_bspline.h"

namespace agg
{

    //------------------------------------------------------------------------
    bspline::~bspline()
    {
        delete [] m_am;
    }


    //------------------------------------------------------------------------
    bspline::bspline() :
        m_num(0),
        m_x(0),
        m_y(0),
        m_am(0)
    {
    }


    //------------------------------------------------------------------------
    void bspline::init(int num, const double* x, const double* y)
    {
        if(num > 2 && x && y)
        {
            int i, k, n1;
            double* temp; 
            double* r; 
            double* s;
            double* al; 
            double h, p, d, f, e;

            m_x = x;
            m_y = y;
            if(num != m_num)
            {
                delete [] m_am;
                m_am = new double[m_num = num];
            }
    
            for(k = 0; k < m_num; k++) 
            {
                m_am[k] = 0.0;
            }

            n1 = 3 * m_num;

            al = new double[n1];
            temp = al;

            for(k = 0; k < n1; k++) 
            {
                temp[k] = 0.0;
            }

            temp += m_num; r = temp;
            temp += m_num; s = temp;

            n1 = m_num - 1;
            d = m_x[1] - m_x[0];
            e = (m_y[1] - m_y[0]) / d;

            for(k = 1; k < n1; k++) 
            {
                h     = d;
                d     = m_x[k + 1] - m_x[k];
                f     = e;
                e     = (m_y[k + 1] - m_y[k]) / d;
                al[k] = d / (d + h);
                r[k]  = 1.0 - al[k];
                s[k]  = 6.0 * (e - f) / (h + d);
            }

            for(k = 1; k < n1; k++) 
            {
                p = 1.0 / (r[k] * al[k - 1] + 2.0);
                al[k] *= -p;
                s[k] = (s[k] - r[k] * s[k - 1]) * p; 
            }

            m_am[n1]     = 0.0;
            al[n1 - 1]   = s[n1 - 1];
            m_am[n1 - 1] = al[n1 - 1];

            for(k = n1 - 2, i = 0; i < m_num - 2; i++, k--) 
            {
                al[k]   = al[k] * al[k + 1] + s[k];
                m_am[k] = al[k];
            }
            delete al;
        }
    }




    //------------------------------------------------------------------------
    void bspline::bsearch(int n, const double *x, double x0, int *i, int *j) 
    {
        int k;

        *i = 0;  
        *j = n-1;

        for(;;) 
        {
            if((*j - *i) == 1) return;

            if(x0 < x[k = (*i + *j) >> 1]) 
            {
                *j = k; 
            }
            else  
            {
                *i = k;
            }
        }
    }



    //------------------------------------------------------------------------
    double bspline::interpolation(double x, int i, int j) const
    {
        double d = m_x[i] - m_x[j];
        double h = x - m_x[j];
        double r = m_x[i] - x;
        double p = d * d / 6.0;
        return (m_am[j] * r * r * r + m_am[i] * h * h * h) / 6.0 / d +
               ((m_y[j] - m_am[j] * p) * r + (m_y[i] - m_am[i] * p) * h) / d;
    }


    //------------------------------------------------------------------------
    double bspline::get(double x) const
    {
        if(m_num > 2 && m_am)
        {
            int i = -1, j, n;
            double d;

            n  = m_num;

            // Extrapolation on the left
            if(x < m_x[0]) 
            {
                d = m_x[1] - m_x[0];
                return (-d * m_am[1] / 6 + (m_y[1] - m_y[0]) / d) * 
                       (x - m_x[0]) + 
                       m_y[0];
            }

            // Extrapolation on the right
            if(x >= m_x[n-1]) 
            {
                d = m_x[n - 1] - m_x[n - 2];
                return (d * m_am[n - 2] / 6 + (m_y[n - 1] - m_y[n - 2]) / d) * 
                       (x - m_x[n - 1]) + 
                       m_y[n - 1];
            }

            // Interpolation
            bsearch(n, m_x, x, &i, &j);
            return interpolation(x, i, j);
        }
        return 0.0;
    }


}

