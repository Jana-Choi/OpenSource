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
// Horizontal span interpolator for use with an arbitrary transformer
// The efficiency highly depends on the operations done in the transformer
//
//----------------------------------------------------------------------------

#ifndef AGG_SPAN_INTERPOLATOR_TRANS_INCLUDED
#define AGG_SPAN_INTERPOLATOR_TRANS_INCLUDED

#include "agg_basics.h"

namespace agg
{
    //========================================================================
    template<class Transformer, unsigned SubpixelShift = 8> 
    class span_interpolator_trans
    {
    public:
        typedef Transformer trans_type;

        enum
        {
            subpixel_shift = SubpixelShift,
            subpixel_size  = 1 << subpixel_shift
        };

        //--------------------------------------------------------------------
        span_interpolator_trans() {}
        span_interpolator_trans(const trans_type& trans) : m_trans(&trans) {}
        span_interpolator_trans(const trans_type& trans,
                                double x, double y, unsigned) :
            m_trans(&trans)
        {
            begin(x, y, 0);
        }

        //----------------------------------------------------------------
        const trans_type& transformer() const { return *m_trans; }
        void transformer(const trans_type& trans) { m_trans = &trans; }

        //----------------------------------------------------------------
        void begin(double x, double y, unsigned)
        {
            m_x = x;
            m_y = y;
            transform();
        }

        //----------------------------------------------------------------
        void operator++()
        {
            m_x += 1.0;
            transform();
        }

        //----------------------------------------------------------------
        void coordinates(int* x, int* y) const
        {
            *x = m_ix;
            *y = m_iy;
        }

    private:
        //----------------------------------------------------------------
        void transform()
        {
            double x = m_x;
            double y = m_y;
            m_trans->transform(&x, &y);
            m_ix = int(x * subpixel_size);
            m_iy = int(y * subpixel_size);
        }
        
        const trans_type* m_trans;
        double            m_x;
        double            m_y;
        int               m_ix;
        int               m_iy;
    };

}

#endif
