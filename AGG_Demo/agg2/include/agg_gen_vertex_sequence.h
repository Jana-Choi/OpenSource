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

#ifndef AGG_GEN_VERTEX_SEQUENCE_INCLUDED
#define AGG_GEN_VERTEX_SEQUENCE_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_sequence.h"
#include "agg_shorten_path.h"

namespace agg
{

    //========================================================================
    class gen_vertex_sequence
    {
    public:
        typedef vertex_dist_cmd                 vertex_type;
        typedef vertex_sequence<vertex_type, 6> vertex_storage;

        gen_vertex_sequence() :
            m_flags(0),
            m_cur_vertex(0),
            m_shorten(0.0),
            m_ready(false)
        {
        }

        // Vertex Generator Interface
        void remove_all();
        void add_vertex(double x, double y, unsigned cmd);

        // Vertex Source Interface
        void     rewind(unsigned id);
        unsigned vertex(double* x, double* y);

        void shorten(double s) { m_shorten = s; }
        double shorten() const { return m_shorten; }

    private:
        vertex_storage m_src_vertices;
        unsigned       m_flags;
        unsigned       m_cur_vertex;
        double         m_shorten;
        bool           m_ready;
    };


    //------------------------------------------------------------------------
    inline void gen_vertex_sequence::remove_all()
    {
        m_ready = false;
        m_src_vertices.remove_all();
        m_cur_vertex = 0;
        m_flags = 0;
    }

    //------------------------------------------------------------------------
    inline void gen_vertex_sequence::add_vertex(double x, double y, unsigned cmd)
    {
        m_ready = false;
        if(is_move_to(cmd))
        {
            m_src_vertices.modify_last(vertex_dist_cmd(x, y, cmd));
        }
        else
        {
            if(is_vertex(cmd))
            {
                m_src_vertices.add(vertex_dist_cmd(x, y, cmd));
            }
            else
            {
                m_flags = cmd & path_flags_mask;
            }
        }
    }


    //------------------------------------------------------------------------
    inline void gen_vertex_sequence::rewind(unsigned) 
    { 
        if(!m_ready)
        {
            m_src_vertices.close(is_closed(m_flags));
            shorten_path(m_src_vertices, m_shorten, get_close_flag(m_flags));
        }
        m_ready = true;
        m_cur_vertex = 0; 
    }

    //------------------------------------------------------------------------
    inline unsigned gen_vertex_sequence::vertex(double* x, double* y)
    {
        if(!m_ready)
        {
            rewind(0);
        }

        if(m_cur_vertex == m_src_vertices.size())
        {
            ++m_cur_vertex;
            return path_cmd_end_poly | m_flags;
        }

        if(m_cur_vertex > m_src_vertices.size())
        {
            return path_cmd_stop;
        }

        vertex_type& v = m_src_vertices[m_cur_vertex++];
        *x = v.x;
        *y = v.y;
        return v.cmd;
    }


}

#endif
