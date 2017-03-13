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
// Function render_ctrl
//
//----------------------------------------------------------------------------

#ifndef AGG_CTRL_INCLUDED
#define AGG_CTRL_INCLUDED

#include "agg_affine_matrix.h"

namespace agg
{

    //------------------------------------------------------------------------
    class ctrl
    {
    public:
        //--------------------------------------------------------------------
        ctrl(double x1, double y1, double x2, double y2, bool flip_y) :
            m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2), 
            m_flip_y(flip_y),
            m_mtx(0)
        {
        }

        //--------------------------------------------------------------------
        virtual bool in_rect(double x, double y) const = 0;
        virtual bool on_mouse_button_down(double x, double y) = 0;
        virtual bool on_mouse_button_up(double x, double y) = 0;
        virtual bool on_mouse_move(double x, double y, bool button_flag) = 0;
        virtual bool on_arrow_keys(bool left, bool right, bool down, bool up) = 0;

        //--------------------------------------------------------------------
        void transform(const affine_matrix& mtx) { m_mtx = &mtx; }
        void no_transform() { m_mtx = 0; }

        //--------------------------------------------------------------------
        void transform_xy(double* x, double* y) const
        {
            if(m_flip_y) *y = m_y1 + m_y2 - *y;
            if(m_mtx) m_mtx->transform(x, y);
        }

        //--------------------------------------------------------------------
        void inverse_transform_xy(double* x, double* y) const
        {
            if(m_mtx) m_mtx->inverse_transform(x, y);
            if(m_flip_y) *y = m_y1 + m_y2 - *y;
        }

    private:
        ctrl(const ctrl&);
        const ctrl& operator = (const ctrl&);

    protected:
        double m_x1;
        double m_y1;
        double m_x2;
        double m_y2;

    private:
        bool m_flip_y;
        const affine_matrix* m_mtx;
    };


}


#endif
