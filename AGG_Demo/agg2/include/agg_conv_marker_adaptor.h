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

#ifndef AGG_CONV_MARKER_ADAPTOR_INCLUDED
#define AGG_CONV_MARKER_ADAPTOR_INCLUDED

#include "agg_basics.h"
#include "agg_conv_generator.h"
#include "agg_gen_vertex_sequence.h"

namespace agg
{

    //========================================================================
    template<class VertexSource, class Markers=null_markers>
    struct conv_marker_adaptor : 
    public conv_generator<VertexSource, gen_vertex_sequence, Markers>
    {
        typedef Markers marker_type;

        conv_marker_adaptor(VertexSource& vs) : 
            conv_generator<VertexSource, gen_vertex_sequence, Markers>(vs)
        {
        }

        void shorten(double s) { generator().shorten(s); }
        double shorten() const { return generator().shorten(); }
    };


}

#endif
