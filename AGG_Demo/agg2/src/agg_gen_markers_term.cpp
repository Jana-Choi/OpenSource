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
// Terminal markers generator (arrowhead/arrowtail)
//
//----------------------------------------------------------------------------

#include "agg_gen_markers_term.h"

namespace agg
{

    //------------------------------------------------------------------------
    void gen_markers_term::remove_all()
    {
        m_markers.remove_all();
    }


    //------------------------------------------------------------------------
    void gen_markers_term::add_vertex(double x, double y, unsigned cmd)
    {
        if(is_move_to(cmd))
        {
            if(m_markers.size() & 1)
            {
                // Initial state, the first coordinate was added.
                // If two of more calls of start_vertex() occures
                // we just modify the last one.
                m_markers.modify_last(coord_type(x, y));
            }
            else
            {
                m_markers.add(coord_type(x, y));
            }
        }
        else
        {
            if(is_vertex(cmd))
            {
                if(m_markers.size() & 1)
                {
                    // Initial state, the first coordinate was added.
                    // Add three more points, 0,1,1,0
                    m_markers.add(coord_type(x, y));
                    m_markers.add(m_markers[m_markers.size() - 1]);
                    m_markers.add(m_markers[m_markers.size() - 3]);
                }
                else
                {
                    if(m_markers.size())
                    {
                        // Replace two last points: 0,1,1,0 -> 0,1,2,1
                        m_markers[m_markers.size() - 1] = m_markers[m_markers.size() - 2];
                        m_markers[m_markers.size() - 2] = coord_type(x, y);
                    }
                }
            }
        }
    }


    //------------------------------------------------------------------------
    void gen_markers_term::rewind(unsigned id)
    {
        m_curr_id = id * 2;
        m_curr_idx = m_curr_id;
    }


    //------------------------------------------------------------------------
    unsigned gen_markers_term::vertex(double* x, double* y)
    {
        if(m_curr_id > 2 || m_curr_idx >= m_markers.size()) 
        {
            return path_cmd_stop;
        }
        const coord_type& c = m_markers[m_curr_idx];
        *x = c.x;
        *y = c.y;
        if(m_curr_idx & 1)
        {
            m_curr_idx += 3;
            return path_cmd_line_to;
        }
        ++m_curr_idx;
        return path_cmd_move_to;
    }


}
