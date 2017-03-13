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
// scanline_u8 class
//
//----------------------------------------------------------------------------
#ifndef AGG_SCANLINE_U8_INCLUDED
#define AGG_SCANLINE_U8_INCLUDED

#include "agg_basics.h"

namespace agg
{
    //========================================================================
    //
    // This class is used to transfer data from class outline (or a similar one)
    // to the rendering buffer. It's organized very simple. The class stores 
    // information of horizontal spans to render it into a pixel-map buffer. 
    // Each span has initial X, length, and an array of bytes that determine the 
    // alpha-values for each pixel. So, the restriction of using this class is 256 
    // levels of Anti-Aliasing, which is quite enough for any practical purpose.
    // Before using this class you should know the minimal and maximal pixel 
    // coordinates of your scanline. The protocol of using is:
    // 1. reset(min_x, max_x)
    // 2. add_cell() / add_span() - accumulate scanline. You pass Y-coordinate 
    //    into these functions in order to make scanline know the last Y. Before 
    //    calling add_cell() / add_span() you should check with method is_ready(y)
    //    if the last Y has changed. It also checks if the scanline is not empty. 
    //    When forming one scanline the next X coordinate must be always greater
    //    than the last stored one, i.e. it works only with ordered coordinates.
    // 3. If the current scanline is_ready() you should render it and then call 
    //    reset_spans() before adding new cells/spans.
    //    
    // 4. Rendering:
    // 
    // Scanline provides an iterator class that allows you to extract
    // the spans and the cover values for each pixel. Be aware that clipping
    // has not been done yet, so you should perform it yourself.
    // Use scanline_u8::iterator to render spans:
    //-------------------------------------------------------------------------
    //
    // int y = sl.y();                    // Y-coordinate of the scanline
    //
    // ************************************
    // ...Perform vertical clipping here...
    // ************************************
    //
    // scanline_u8::const_iterator span = sl.begin();
    // 
    // unsigned char* row = m_rbuf->row(y); // The the address of the beginning 
    //                                      // of the current row
    // 
    // unsigned num_spans = sl.num_spans(); // Number of spans. It's guaranteed that
    //                                      // num_spans is always greater than 0.
    //
    // do
    // {
    //     const scanline_u8::cover_type* covers =
    //         span->covers;                     // The array of the cover values
    //
    //     int num_pix = span->len;              // Number of pixels of the span.
    //                                           // Always greater than 0, still it's
    //                                           // better to use "int" instead of 
    //                                           // "unsigned" because it's more
    //                                           // convenient for clipping
    //     int x = span->x;
    //
    //     **************************************
    //     ...Perform horizontal clipping here...
    //     ...you have x, covers, and pix_count..
    //     **************************************
    //
    //     unsigned char* dst = row + x;  // Calculate the start address of the row.
    //                                    // In this case we assume a simple 
    //                                    // grayscale image 1-byte per pixel.
    //     do
    //     {
    //         *dst++ = *covers++;        // Hypotetical rendering. 
    //     }
    //     while(--num_pix);
    //
    //     ++span;
    // } 
    // while(--num_spans);  // num_spans cannot be 0, so this loop is quite safe
    //------------------------------------------------------------------------
    //
    // The question is: why should we accumulate the whole scanline when we
    // could render just separate spans when they're ready?
    // That's because using the scaline is in general faster. When is consists 
    // of more than one span the conditions for the processor cash system
    // are better, because switching between two different areas of memory 
    // (that can be large ones) occures less frequently.
    //------------------------------------------------------------------------
    class scanline_u8
    {
    public:
        enum { aa_shift = 8 };
        typedef int8u cover_type;

        //--------------------------------------------------------------------
        struct const_span
        {
            int x;
            unsigned len;
            const cover_type* covers;
        };

        //--------------------------------------------------------------------
        class const_iterator
        {
        public:
            const_iterator(const scanline_u8& sl) :
                m_covers(sl.m_covers),
                m_cur_count(sl.m_counts),
                m_cur_start_ptr(sl.m_start_ptrs),
                m_base_x(sl.m_min_x + sl.m_dx)
            { 
                ++(*this);
            }

            const const_iterator& operator++()
            {
                m_span.covers = *(++m_cur_start_ptr);
                m_span.x = int(*m_cur_start_ptr - m_covers) + m_base_x;
                m_span.len = *(++m_cur_count);
                return *this;
            }

            const const_span& operator*()  const { return m_span;  }
            const const_span* operator->() const { return &m_span; }

        private:
            const int8u*        m_covers;
            const int16u*       m_cur_count;
            const int8u* const* m_cur_start_ptr;
            int                 m_base_x;
            const_span          m_span;
        };

        //--------------------------------------------------------------------
        struct span
        {
            cover_type* covers;
            int x;
            unsigned len;
        };

        //--------------------------------------------------------------------
        class iterator
        {
        public:
            iterator(scanline_u8& sl) :
                m_covers(sl.m_covers),
                m_cur_count(sl.m_counts),
                m_cur_start_ptr(sl.m_start_ptrs),
                m_base_x(sl.m_min_x + sl.m_dx)
            { 
                ++(*this);
            }

            iterator& operator++()
            {
                m_span.covers = *(++m_cur_start_ptr);
                m_span.x = int(*m_cur_start_ptr - m_covers) + m_base_x;
                m_span.len = *(++m_cur_count);
                return *this;
            }

            span& operator*()  { return m_span;  }
            span* operator->() { return &m_span; }

        private:
            int8u*        m_covers;
            const int16u* m_cur_count;
            int8u**       m_cur_start_ptr;
            int           m_base_x;
            span          m_span;
        };


        friend class iterator;
        friend class const_iterator;

        //--------------------------------------------------------------------
        ~scanline_u8();
        scanline_u8();

        void     reset(int min_x, int max_x, int dx, int dy);
        void     reset_spans();
        void     add_cell(int x, int y, unsigned cover);
        void     add_span(int x, int y, unsigned len, unsigned cover);
        int      is_ready(int y) const;
        void     prepare() {}

        int      y()           const { return m_last_y + m_dy; }
        unsigned num_spans()   const { return m_num_spans;     }
        const_iterator begin() const { return const_iterator(*this); }
        iterator       begin()       { return iterator(*this); }

    private:
        scanline_u8(const scanline_u8&);
        const scanline_u8& operator = (const scanline_u8&);

    private:
        int      m_min_x;
        unsigned m_max_len;
        int      m_dx;
        int      m_dy;
        int      m_last_x;
        int      m_last_y;
        int8u*   m_covers;
        int8u**  m_start_ptrs;
        int16u*  m_counts;
        unsigned m_num_spans;
        int8u**  m_cur_start_ptr;
        int16u*  m_cur_count;
    };


    //------------------------------------------------------------------------
    inline void scanline_u8::reset_spans()
    {
        m_last_x        = 0x7FFF;
        m_last_y        = 0x7FFF;
        m_cur_count     = m_counts;
        m_cur_start_ptr = m_start_ptrs;
        m_num_spans     = 0;
    }


    //------------------------------------------------------------------------
    inline void scanline_u8::add_cell(int x, int y, unsigned cover)
    {
        x -= m_min_x;
        m_covers[x] = (unsigned char)cover;
        if(x == m_last_x+1)
        {
            (*m_cur_count)++;
        }
        else
        {
            *++m_cur_count = 1;
            *++m_cur_start_ptr = m_covers + x;
            m_num_spans++;
        }
        m_last_x = x;
        m_last_y = y;
    }


    //------------------------------------------------------------------------
    inline int scanline_u8::is_ready(int y) const
    {
        return m_num_spans && (y ^ m_last_y);
    }






    //========================================================================

    //------------------------------------------------------------------------
    template<class AlphaMask> class scanline_u8_am : public scanline_u8
    {
    public:
        typedef AlphaMask alpha_mask_type;

        scanline_u8_am() : scanline_u8(), m_alpha_mask(0) {}
        scanline_u8_am(const AlphaMask& am) : scanline_u8(), m_alpha_mask(&am) {}

        //--------------------------------------------------------------------
        void prepare()
        {
            if(m_alpha_mask)
            {
                iterator span = begin();
                unsigned count = num_spans();
                do
                {
                    m_alpha_mask->combine_span(span->x, y(), span->covers, span->len);
                    ++span;
                }
                while(--count);
            }
        }

    private:
        const AlphaMask* m_alpha_mask;
    };




}

#endif

