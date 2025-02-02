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

#ifndef AGG_CONV_UNCLOSE_POLYGON_INCLUDED
#define AGG_CONV_UNCLOSE_POLYGON_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_iterator.h"

namespace agg
{
    //============================================================================
    template<class VertexSource> class conv_unclose_polygon
    {
    public:
        conv_unclose_polygon(VertexSource& vs) : m_source(&vs) {}

        void rewind(unsigned path_id)
        {
            m_source->rewind(path_id);
        }

        unsigned vertex(double* x, double* y)
        {
            unsigned cmd = m_source->vertex(x, y);
            if(is_end_poly(cmd)) cmd &= ~path_flags_close;
            return cmd;
        }

        typedef conv_unclose_polygon<VertexSource> source_type;
        typedef vertex_iterator<source_type> iterator;
        iterator begin(unsigned id) { return iterator(*this, id); }
        iterator end() { return iterator(path_cmd_stop); }

    private:
        VertexSource* m_source;
    };

}

#endif
