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

#ifndef AGG_CONV_CONCAT_INCLUDED
#define AGG_CONV_CONCAT_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_iterator.h"

namespace agg
{
    //========================================================================
    // Concatenation of two paths. Usually used to combine lines or curves 
    // with markers such as arrowheads
    template<class VS1, class VS2> class conv_concat
    {
    public:
        conv_concat(VS1& source1, VS2& source2) :
            m_source1(&source1), m_source2(&source2), m_status(2) { }

        void rewind(unsigned id)
        { 
            m_source1->rewind(id);
            m_source2->rewind(0);
            m_status = 0;
        }

        unsigned vertex(double* x, double* y)
        {
            unsigned cmd;
            if(m_status == 0)
            {
                cmd = m_source1->vertex(x, y);
                if(!is_stop(cmd)) return cmd;
                m_status = 1;
            }
            if(m_status == 1)
            {
                cmd = m_source2->vertex(x, y);
                if(!is_stop(cmd)) return cmd;
                m_status = 2;
            }
            return path_cmd_stop;
        }

        typedef conv_concat<VS1, VS2> source_type;
        typedef vertex_iterator<source_type> iterator;
        iterator begin(unsigned id) { return iterator(*this, id); }
        iterator end() { return iterator(path_cmd_stop); }

    private:
        VS1* m_source1;
        VS2* m_source2;
        int  m_status;

    };
}


#endif
