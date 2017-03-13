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
// POD deque & array
//
//----------------------------------------------------------------------------
#ifndef AGG_ARRAY_INCLUDED
#define AGG_ARRAY_INCLUDED

#include <string.h>
#include "agg_basics.h"

namespace agg
{

    //------------------------------------------------------------------------
    // A simple class template to store Plain Old Data, a vector
    // of a fixed size. The data is continous in memory
    //------------------------------------------------------------------------
    template<class T> class pod_array
    {
    public:
        typedef T value_type;

        ~pod_array() { delete [] m_array; }
        pod_array() : m_size(0), m_capacity(0), m_array(0) {}
        pod_array(unsigned cap, unsigned extra_tail=0);

        unsigned capacity() const { return m_capacity; }
        void capacity(unsigned cap, unsigned extra_tail=0);

        void add(const T& v)  { m_array[m_size++] = v; }
        void inc_size(unsigned size) { m_size += size; } 
        unsigned size() const { return m_size; }
        const T& operator [] (unsigned idx) const { return m_array[idx]; }
              T& operator [] (unsigned idx)       { return m_array[idx]; }

        void remove_all()         { m_size = 0; }
        void cut_at(unsigned num) { if(num < m_size) m_size = num; }

    private:
        // Prohibit copying
        pod_array(const pod_array<T>&);
        const pod_array<T>& operator = (const pod_array<T>&);

        unsigned m_size;
        unsigned m_capacity;
        T*       m_array;
    };


    template<class T> 
    void pod_array<T>::capacity(unsigned cap, unsigned extra_tail)
    {
        m_size = 0;
        if(cap > m_capacity)
        {
            delete [] m_array;
            m_array = new T [m_capacity = cap + extra_tail];
        }
    }

    template<class T> pod_array<T>::pod_array(unsigned cap, unsigned extra_tail) :
        m_size(0), m_capacity(0), m_array(0)
    {
        capacity(cap, extra_tail);
    }




    //------------------------------------------------------------------------
    // A simple class template to store Plain Old Data, similar to std::deque
    // It doesn't reallocate memory but instead, uses blocks of data of size 
    // of (1 << S), that is, power of two. The data is NOT continuous in memory, 
    // so the only valid access method is operator [].
    //------------------------------------------------------------------------
    template<class T, unsigned S=6> class pod_deque
    {
        enum 
        {   
            block_shift = S,
            block_size  = 1 << block_shift,
            block_mask  = block_size - 1
        };

    public:
        typedef T value_type;

        ~pod_deque();
        pod_deque();

        void remove_all() { m_size = 0; }
        void add(const T& val);
        void modify_last(const T& val);
        void remove_last();

        void cut_at(unsigned size)
        {
            if(size < m_size) m_size = size;
        }

        unsigned size() const { return m_size; }

        const T& operator [] (unsigned idx) const
        {
            return m_blocks[idx >> block_shift][idx & block_mask];
        }

        T& operator [] (unsigned idx)
        {
            return m_blocks[idx >> block_shift][idx & block_mask];
        }

        const T& curr(unsigned idx) const
        {
            return (*this)[idx];
        }

        T& curr(unsigned idx)
        {
            return (*this)[idx];
        }

        const T& prev(unsigned idx) const
        {
            return (*this)[(idx + m_size - 1) % m_size];
        }

        T& prev(unsigned idx)
        {
            return (*this)[(idx + m_size - 1) % m_size];
        }

        const T& next(unsigned idx) const
        {
            return (*this)[(idx + 1) % m_size];
        }

        T& next(unsigned idx)
        {
            return (*this)[(idx + 1) % m_size];
        }

    private:
        void allocate_block(unsigned nb);
        T*   data_ptr();

        // Prohibit copying
        pod_deque(const pod_deque<T, S>&);
        const pod_deque<T, S>& operator = (const pod_deque<T, S>&);

        unsigned        m_size;
        unsigned        m_total_blocks;
        unsigned        m_max_blocks;
        T**             m_blocks;
    };




    //------------------------------------------------------------------------
    template<class T, unsigned S> pod_deque<T, S>::~pod_deque()
    {
        if(m_total_blocks)
        {
            T** blk = m_blocks + m_total_blocks - 1;
            while(m_total_blocks--)
            {
                delete [] *blk;
                --blk;
            }
            delete [] m_blocks;
        }
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S> pod_deque<T, S>::pod_deque() :
        m_size(0),
        m_total_blocks(0),
        m_max_blocks(0),
        m_blocks(0)
    {
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S>
    void pod_deque<T, S>::allocate_block(unsigned nb)
    {
        if(nb >= m_max_blocks) 
        {
            T** new_blocks = new T* [m_max_blocks + block_size];

            if(m_blocks)
            {
                memcpy(new_blocks, 
                       m_blocks, 
                       m_max_blocks * sizeof(T*));

                delete [] m_blocks;
            }
            m_blocks = new_blocks;
            m_max_blocks += block_size;
        }
        m_blocks[nb] = new T [block_size];
        m_total_blocks++;
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S>
    inline T* pod_deque<T, S>::data_ptr()
    {
        unsigned nb = m_size >> block_shift;
        if(nb >= m_total_blocks)
        {
            allocate_block(nb);
        }
        return m_blocks[nb] + (m_size & block_mask);
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    inline void pod_deque<T, S>::add(const T& val)
    {
        *data_ptr() = val;
        ++m_size;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    inline void pod_deque<T, S>::remove_last()
    {
        if(m_size) --m_size;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    void pod_deque<T, S>::modify_last(const T& val)
    {
        remove_last();
        add(val);
    }


    enum
    {
        quick_sort_threshold = 9
    };

    
    //------------------------------------------------------------------------
    template<class T> inline void swap_elements(T& a, T& b)
    {
        T temp = a;
        a = b;
        b = temp;
    }


    //------------------------------------------------------------------------
    template<class Array, class Less>
    void quick_sort(Array& arr, Less less)
    {
        if(arr.size() < 2) return;

        typename Array::value_type* e1;
        typename Array::value_type* e2;

        int  stack[80];
        int* top = stack; 
        int  limit = arr.size();
        int  base = 0;

        for(;;)
        {
            int len = limit - base;

            int i;
            int j;
            int pivot;

            if(len > quick_sort_threshold)
            {
                // we use base + len/2 as the pivot
                pivot = base + len / 2;
                swap_elements(arr[base], arr[pivot]);

                i = base + 1;
                j = limit - 1;

                // now ensure that *i <= *base <= *j 
                e1 = &(arr[j]); 
                e2 = &(arr[i]);
                if(less(*e1, *e2)) swap_elements(*e1, *e2);

                e1 = &(arr[base]); 
                e2 = &(arr[i]);
                if(less(*e1, *e2)) swap_elements(*e1, *e2);

                e1 = &(arr[j]); 
                e2 = &(arr[base]);
                if(less(*e1, *e2)) swap_elements(*e1, *e2);

                for(;;)
                {
                    do i++; while( less(arr[i], arr[base]) );
                    do j--; while( less(arr[base], arr[j]) );

                    if( i > j )
                    {
                        break;
                    }

                    swap_elements(arr[i], arr[j]);
                }

                swap_elements(arr[base], arr[j]);

                // now, push the largest sub-array
                if(j - base > limit - i)
                {
                    top[0] = base;
                    top[1] = j;
                    base   = i;
                }
                else
                {
                    top[0] = i;
                    top[1] = limit;
                    limit  = j;
                }
                top += 2;
            }
            else
            {
                // the sub-array is small, perform insertion sort
                j = base;
                i = j + 1;

                for(; i < limit; j = i, i++)
                {
                    for(; less(*(e1 = &(arr[j + 1])), *(e2 = &(arr[j]))); j--)
                    {
                        swap_elements(*e1, *e2);
                        if(j == base)
                        {
                            break;
                        }
                    }
                }
                if(top > stack)
                {
                    top  -= 2;
                    base  = top[0];
                    limit = top[1];
                }
                else
                {
                    break;
                }
            }
        }
    }




    //------------------------------------------------------------------------
    template<class Array, class Equal>
    unsigned compact(Array& arr, Equal equal)
    {
        if(arr.size() < 2) return arr.size();

        unsigned i, j;
        for(i = 1, j = 1; i < arr.size(); i++)
        {
            typename Array::value_type& e = arr[i];
            if(!equal(e, arr[i - 1]))
            {
                arr[j++] = e;
            }
        }
        return j;
    }




}

#endif
