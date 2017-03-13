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
// Polygon clipping converter
// There an optimized Liang-Basky algorithm is used. 
// The algorithm doesn't optimize the degenerate edges, i.e. it will never
// break a closed polygon into two or more ones, instead, there will be 
// degenerate edges coinciding with the respective clipping boundaries.
// This is a sub-optimal solution, because that optimization would require 
// extra, rather expensive math while the rasterizer tolerates it quite well, 
// without any considerable overhead.
//
//----------------------------------------------------------------------------
#ifndef AGG_CONV_CLIP_POLYGON_INCLUDED
#define AGG_CONV_CLIP_POLYGON_INCLUDED

#include "agg_basics.h"
#include "agg_conv_adaptor_vpgen.h"
#include "agg_vpgen_clip_polygon.h"
#include "agg_vertex_iterator.h"

namespace agg
{

    //============================================================================
    template<class VertexSource> 
    struct conv_clip_polygon : public conv_adaptor_vpgen<VertexSource, vpgen_clip_polygon>
    {
        conv_clip_polygon(VertexSource& vs) : 
            conv_adaptor_vpgen<VertexSource, vpgen_clip_polygon>(vs) {}

        void clip_box(double x1, double y1, double x2, double y2)
        {
            vpgen().clip_box(x1, y1, x2, y2);
        }

        double x1() const { return vpgen().x1(); }
        double y1() const { return vpgen().y1(); }
        double x2() const { return vpgen().x2(); }
        double y2() const { return vpgen().y2(); }

        typedef conv_clip_polygon<VertexSource> source_type;
        typedef vertex_iterator<source_type> iterator;
        iterator begin(unsigned id) { return iterator(*this, id); }
        iterator end() { return iterator(path_cmd_stop); }

    };

}

#endif
