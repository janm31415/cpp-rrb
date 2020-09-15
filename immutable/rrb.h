
/*
 * CPP port of the c-rrb library (https://github.com/hypirion/c-rrb)
 * CPP port by Jan Maes, January, 2020.
 *
 *
 * Copyright (c) 2013-2014 Jean Niklas L'orange. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */


#pragma once

#include <stdint.h>
#include <cassert>
#include <vector>
#include <algorithm>
#include <atomic>
#include <tuple>

#ifndef _WIN32
#include <string.h>
#endif

namespace immutable
  {

  template <typename T, bool atomic_ref_counting, int N>
  struct rrb;

  template <typename T, bool atomic_ref_counting, int N>
  struct transient_rrb;

  template <typename T>
  struct ref
    {
    ref();

    ref(T* p);

    template <typename U>
    ref(U* p);

    ref(const ref<T>& r);

    template <typename U>
    ref(const ref<U>& r);

    ~ref();

    T* operator->() const;

    void swap(ref<T>& r);

    ref<T>& operator = (const ref<T>& r);

    template <typename U>
    ref<T>& operator = (const ref<U>& r);

    bool unique();

    void inc() const;

    void dec() const;

    T* ptr;
    };

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N> > rrb_create();

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N> > rrb_push(const ref<rrb<T, atomic_ref_counting, N> >& in, T element);

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N> > rrb_pop(const ref<rrb<T, atomic_ref_counting, N> >& in);

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N> > rrb_update(const ref<rrb<T, atomic_ref_counting, N> >& in, uint32_t index, T element);

  template <typename T, bool atomic_ref_counting, int N>
  const T& rrb_nth(const ref<rrb<T, atomic_ref_counting, N> >& in, uint32_t index);

  template <typename T, bool atomic_ref_counting, int N>
  std::tuple<const T*, uint32_t, uint32_t> rrb_region_for(const ref<rrb<T, atomic_ref_counting, N> >& in, uint32_t index);

  template <typename T, bool atomic_ref_counting, int N>
  uint32_t rrb_count(const ref<rrb<T, atomic_ref_counting, N> >& rrb);

  template <typename T, bool atomic_ref_counting, int N>
  const T& rrb_peek(const ref<rrb<T, atomic_ref_counting, N> >& rrb);

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N>> rrb_slice(const ref<rrb<T, atomic_ref_counting, N> >& rrb, uint32_t from, uint32_t to);

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N>> rrb_concat(const ref<rrb<T, atomic_ref_counting, N> >& left, const ref<rrb<T, atomic_ref_counting, N> >& right);

  namespace rrb_details
    {

    typedef uint32_t guid_type;

    template <int N>
    struct bits
      {
      enum
        {
        rrb_bits = N,
        rrb_branching = (1 << N),
        rrb_mask = (1 << N) - 1,
        rrb_max_height = (32 + (N - 1)) / N,
        rrb_invariant = 1,
        rrb_extras = 2
        };

      }; // bits

    typedef enum { LEAF_NODE, INTERNAL_NODE } node_type;

    template <bool atomic_ref_counting>
    struct rrb_size_table;

    template <typename T, bool atomic_ref_counting>
    struct leaf_node;

    template <typename T, bool atomic_ref_counting>
    struct internal_node;

    template <typename T, bool atomic_ref_counting>
    struct tree_node;

    void release(const rrb_size_table<true>* p_table);

    template <typename T>
    void release(const leaf_node<T, true>* p_node);

    template <typename T>
    void release(const internal_node<T, true>* p_node);

    template <typename T>
    void release(const tree_node<T, true>* p_node);

    template <typename T, int N>
    void release(const rrb<T, true, N>* p_node);

    void release(const rrb_size_table<false>* p_table);

    template <typename T>
    void release(const leaf_node<T, false>* p_node);

    template <typename T>
    void release(const internal_node<T, false>* p_node);

    template <typename T>
    void release(const tree_node<T, false>* p_node);

    template <typename T, int N>
    void release(const rrb<T, false, N>* p_node);

    void addref(const rrb_size_table<true>* p_node);

    template <typename T>
    void addref(const leaf_node<T, true>* p_node);

    template <typename T>
    void addref(const internal_node<T, true>* p_node);

    template <typename T>
    void addref(const tree_node<T, true>* p_node);

    template <typename T, int N>
    void addref(const rrb<T, true, N>* p_node);

    void addref(const rrb_size_table<false>* p_node);

    template <typename T>
    void addref(const leaf_node<T, false>* p_node);

    template <typename T>
    void addref(const internal_node<T, false>* p_node);

    template <typename T>
    void addref(const tree_node<T, false>* p_node);

    template <typename T, int N>
    void addref(const rrb<T, false, N>* p_node);

    template <typename T, int N>
    void release(const transient_rrb<T, true, N>* p_node);

    template <typename T, int N>
    void release(const transient_rrb<T, false, N>* p_node);

    template <typename T, int N>
    void addref(const transient_rrb<T, true, N>* p_node);

    template <typename T, int N>
    void addref(const transient_rrb<T, false, N>* p_node);

    template <bool atomic_ref_counting>
    struct rrb_size_table
      {
      uint32_t* size;
      mutable std::atomic<uint32_t> _ref_count;
      guid_type guid;
      };

    template <>
    struct rrb_size_table<false>
      {
      uint32_t* size;
      mutable uint32_t _ref_count;
      guid_type guid;
      };

    template <typename T, bool atomic_ref_counting>
    struct internal_node
      {
      node_type type;
      uint32_t len;
      mutable std::atomic<uint32_t> _ref_count;
      guid_type guid;
      ref<rrb_size_table<atomic_ref_counting> > size_table;
      ref<internal_node<T, atomic_ref_counting> >* child;
      };

    template <typename T>
    struct internal_node<T, false>
      {
      node_type type;
      uint32_t len;
      mutable uint32_t _ref_count;
      guid_type guid;
      ref<rrb_size_table<false> > size_table;
      ref<internal_node<T, false> >* child;
      };

    template <typename T, bool atomic_ref_counting>
    struct tree_node
      {
      node_type type;
      uint32_t len;
      mutable std::atomic<uint32_t> _ref_count;
      guid_type guid;
      };

    template <typename T>
    struct tree_node<T, false>
      {
      node_type type;
      uint32_t len;
      mutable uint32_t _ref_count;
      guid_type guid;
      };

    template <typename T, bool atomic_ref_counting>
    struct leaf_node
      {
      node_type type;
      uint32_t len;
      mutable std::atomic<uint32_t> _ref_count;
      guid_type guid;
      T* child;
      };

    template <typename T>
    struct leaf_node<T, false>
      {
      node_type type;
      uint32_t len;
      mutable uint32_t _ref_count;
      guid_type guid;
      T* child;
      };

    inline void release(const rrb_size_table<true>* p_table)
      {
      if (p_table)
        {
        if (1 == p_table->_ref_count.fetch_sub(1, std::memory_order_acq_rel))
          {
          free((void*)p_table);
          }
        }
      }

    inline void release(const rrb_size_table<false>* p_table)
      {
      if (p_table)
        {
        if (1 == p_table->_ref_count--)
          {
          free((void*)p_table);
          }
        }
      }

    template <typename T>
    inline void release(const leaf_node<T, true>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count.fetch_sub(1, std::memory_order_acq_rel))
          {
          for (uint32_t i = 0; i < p_node->len; ++i)
            p_node->child[i].~T();
          free((void*)p_node);
          }
        }
      }

    template <typename T>
    inline void release(const leaf_node<T, false>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count--)
          {
          for (uint32_t i = 0; i < p_node->len; ++i)
            p_node->child[i].~T();
          free((void*)p_node);
          }
        }
      }

    template <typename T>
    inline void release(const internal_node<T, true>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count.fetch_sub(1, std::memory_order_acq_rel))
          {
          p_node->size_table.dec();
          for (uint32_t i = 0; i < p_node->len; ++i)
            {
            if (p_node->child[i].ptr && p_node->child[i]->type == LEAF_NODE)
              {
              release((leaf_node<T, true>*)p_node->child[i].ptr);
              }
            else
              release(p_node->child[i].ptr);
            }
          free((void*)p_node);
          }
        }
      }

    template <typename T>
    inline void release(const internal_node<T, false>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count--)
          {
          p_node->size_table.dec();
          for (uint32_t i = 0; i < p_node->len; ++i)
            {
            if (p_node->child[i].ptr && p_node->child[i]->type == LEAF_NODE)
              {
              release((leaf_node<T, false>*)p_node->child[i].ptr);
              }
            else
              release(p_node->child[i].ptr);
            }
          free((void*)p_node);
          }
        }
      }

    template <typename T>
    inline void release(const tree_node<T, true>* p_node)
      {
      if (p_node)
        {
        if (p_node->type == LEAF_NODE)
          release((leaf_node<T, true>*)p_node);
        else
          release((internal_node<T, true>*)p_node);
        }
      }

    template <typename T>
    inline void release(const tree_node<T, false>* p_node)
      {
      if (p_node)
        {
        if (p_node->type == LEAF_NODE)
          release((leaf_node<T, false>*)p_node);
        else
          release((internal_node<T, false>*)p_node);
        }
      }

    template <typename T, int N>
    inline void release(const rrb<T, true, N>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count.fetch_sub(1, std::memory_order_acq_rel))
          {
          release(p_node->tail.ptr);
          release<T>(p_node->root.ptr);
          free((void*)p_node);
          }
        }
      }

    template <typename T, int N>
    inline void release(const rrb<T, false, N>* p_node)
      {
      if (p_node)
        {
        if (1 == p_node->_ref_count--)
          {
          release(p_node->tail.ptr);
          release<T>(p_node->root.ptr);
          free((void*)p_node);
          }
        }
      }


    inline void addref(const rrb_size_table<true>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T>
    inline void addref(const leaf_node<T, true>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T>
    inline void addref(const internal_node<T, true>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T>
    inline void addref(const tree_node<T, true>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T, int N>
    inline void addref(const rrb<T, true, N>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    inline void addref(const rrb_size_table<false>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <typename T>
    inline void addref(const leaf_node<T, false>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <typename T>
    inline void addref(const internal_node<T, false>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <typename T>
    inline void addref(const tree_node<T, false>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <typename T, int N>
    inline void addref(const rrb<T, false, N>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <bool atomic_ref_counting>
    inline rrb_size_table<atomic_ref_counting>* size_table_create(uint32_t size)
      {
      rrb_size_table<atomic_ref_counting>* table = (rrb_size_table<atomic_ref_counting>*)malloc(sizeof(rrb_size_table<atomic_ref_counting>) + size * sizeof(uint32_t));
      table->size = (uint32_t*)((char*)table + sizeof(rrb_size_table<atomic_ref_counting>));
      table->guid = 0;
      return table;
      }

    template <bool atomic_ref_counting>
    inline rrb_size_table<atomic_ref_counting>* size_table_clone(const rrb_size_table<atomic_ref_counting>* original, uint32_t len)
      {
      rrb_size_table<atomic_ref_counting>* clone = (rrb_size_table<atomic_ref_counting>*)malloc(sizeof(rrb_size_table<atomic_ref_counting>) + len * sizeof(uint32_t));
      clone->size = (uint32_t*)((char*)clone + sizeof(rrb_size_table<atomic_ref_counting>));
      memcpy(clone->size, original->size, sizeof(uint32_t) * len);
      clone->guid = 0;
      return clone;
      }

    template <bool atomic_ref_counting>
    inline rrb_size_table<atomic_ref_counting>* size_table_inc(const rrb_size_table<atomic_ref_counting> *original, uint32_t len)
      {
      rrb_size_table<atomic_ref_counting>* table = (rrb_size_table<atomic_ref_counting>*)malloc(sizeof(rrb_size_table<atomic_ref_counting>) + (len + 1) * sizeof(uint32_t));
      table->size = (uint32_t*)((char*)table + sizeof(rrb_size_table<atomic_ref_counting>));
      memcpy(table->size, original->size, sizeof(uint32_t) * len);
      table->guid = 0;
      return table;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline rrb<T, atomic_ref_counting, N>* rrb_head_clone(const rrb<T, atomic_ref_counting, N>* original)
      {
      rrb<T, atomic_ref_counting, N>* clone = (rrb<T, atomic_ref_counting, N>*)malloc(sizeof(rrb<T, atomic_ref_counting, N>));
      memcpy(clone, original, sizeof(rrb<T, atomic_ref_counting, N>));
      clone->root.inc();
      clone->tail.inc();
      return clone;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* create_empty_leaf()
      {
      leaf_node<T, atomic_ref_counting>* empty = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>));
      empty->type = LEAF_NODE;
      empty->len = 0;
      empty->child = nullptr;
      empty->guid = 0;
      return empty;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* leaf_node_inc(const leaf_node<T, atomic_ref_counting>* original)
      {
      leaf_node<T, atomic_ref_counting>* inc = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (original->len + 1) * sizeof(T));
      memset(inc, 0, sizeof(leaf_node<T, atomic_ref_counting>) + (original->len + 1) * sizeof(T));
      inc->len = original->len + 1;
      inc->type = LEAF_NODE;
      inc->child = (T*)((char*)inc + sizeof(leaf_node<T, atomic_ref_counting>));
      inc->guid = 0;
      //memcpy(inc->child, original->child, original->len * sizeof(T));
      for (uint32_t i = 0; i < original->len; ++i)
        inc->child[i] = original->child[i]; // don't memcpy, but use copy constructor
      T* loc = (T*)((char*)inc->child + original->len * sizeof(T));
      loc = new(loc) T(); // placement new      
      return inc;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* leaf_node_create(uint32_t len)
      {
      leaf_node<T, atomic_ref_counting>* leaf = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (len) * sizeof(T));
      leaf->len = len;
      leaf->type = LEAF_NODE;
      leaf->child = (T*)((char*)leaf + sizeof(leaf_node<T, atomic_ref_counting>));
      leaf->guid = 0;
      for (uint32_t i = 0; i < len; ++i)
        {
        T* loc = (T*)((char*)leaf->child + i * sizeof(T));
        loc = new(loc) T(); // placement new
        }
      return leaf;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* leaf_node_clone(const leaf_node<T, atomic_ref_counting>* original)
      {
      leaf_node<T, atomic_ref_counting>* clone = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (original->len) * sizeof(T));
      memset(clone, 0, sizeof(leaf_node<T, atomic_ref_counting>) + (original->len) * sizeof(T));
      clone->len = original->len;
      clone->type = LEAF_NODE;
      clone->child = (T*)((char*)clone + sizeof(leaf_node<T, atomic_ref_counting>));
      clone->guid = 0;
      //memcpy(clone->child, original->child, original->len * sizeof(T));
      for (uint32_t i = 0; i < original->len; ++i)
        clone->child[i] = original->child[i]; // don't memcpy, but use copy constructor
      return clone;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* leaf_node_dec(const leaf_node<T, atomic_ref_counting>* original)
      {
      leaf_node<T, atomic_ref_counting>* dec = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (original->len - 1) * sizeof(T));
      memset(dec, 0, sizeof(leaf_node<T, atomic_ref_counting>) + (original->len - 1) * sizeof(T));
      dec->len = original->len - 1;
      dec->type = LEAF_NODE;
      dec->child = (T*)((char*)dec + sizeof(leaf_node<T, atomic_ref_counting>));
      dec->guid = 0;
      //memcpy(dec->child, original->child, (original->len - 1) * sizeof(T));
      for (uint32_t i = 0; i < original->len - 1; ++i)
        dec->child[i] = original->child[i]; // don't memcpy, but use copy constructor
      return dec;
      }

    template <typename T, bool atomic_ref_counting>
    inline leaf_node<T, atomic_ref_counting>* leaf_node_merge(const leaf_node<T, atomic_ref_counting>* left, const leaf_node<T, atomic_ref_counting>* right)
      {
      leaf_node<T, atomic_ref_counting>* merged = leaf_node_create<T, atomic_ref_counting>(left->len + right->len);
      //memcpy(merged->child, left->child, left->len * sizeof(T));
      //memcpy(merged->child + left->len, right->child, right->len * sizeof(T));

      for (uint32_t i = 0; i < left->len; ++i)
        merged->child[i] = left->child[i]; // don't memcpy, but use copy constructor
      for (uint32_t i = 0; i < right->len; ++i)
        merged->child[i + left->len] = right->child[i]; // don't memcpy, but use copy constructor
      return merged;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_create(uint32_t len)
      {
      internal_node<T, atomic_ref_counting>* node = (internal_node<T, atomic_ref_counting>*)malloc(sizeof(internal_node<T, atomic_ref_counting>) + len * sizeof(ref<internal_node<T, atomic_ref_counting>>));
      node->len = len;
      node->type = INTERNAL_NODE;
      node->size_table.ptr = nullptr;
      node->guid = 0;
      node->child = (ref<internal_node<T, atomic_ref_counting>>*)((char*)node + sizeof(internal_node<T, atomic_ref_counting>));
      memset(node->child, 0, len * sizeof(ref<internal_node<T, atomic_ref_counting>>)); // init pointers to zero      
      return node;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_clone(const internal_node<T, atomic_ref_counting>* original)
      {
      internal_node<T, atomic_ref_counting>* node = (internal_node<T, atomic_ref_counting>*)malloc(sizeof(internal_node<T, atomic_ref_counting>) + original->len * sizeof(ref<internal_node<T, atomic_ref_counting>>));
      node->len = original->len;
      node->type = INTERNAL_NODE;
      node->size_table.ptr = nullptr;
      node->size_table = original->size_table;
      node->guid = 0;
      node->child = (ref<internal_node<T, atomic_ref_counting>>*)((char*)node + sizeof(internal_node<T, atomic_ref_counting>));
      memset(node->child, 0, original->len * sizeof(ref<internal_node<T, atomic_ref_counting>>)); // init pointers to zero      
      for (uint32_t i = 0; i < original->len; ++i)
        node->child[i] = original->child[i];
      return node;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_copy(const internal_node<T, atomic_ref_counting>* original, uint32_t start, uint32_t len)
      {
      internal_node<T, atomic_ref_counting>* node = internal_node_create<T, atomic_ref_counting>(len);
      for (uint32_t i = 0; i < len; ++i)
        node->child[i] = original->child[i + start];
      return node;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_inc(const internal_node<T, atomic_ref_counting>* original)
      {
      internal_node<T, atomic_ref_counting>* node = (internal_node<T, atomic_ref_counting>*)malloc(sizeof(internal_node<T, atomic_ref_counting>) + (original->len + 1) * sizeof(ref<internal_node<T, atomic_ref_counting>>));
      node->len = original->len + 1;
      node->type = INTERNAL_NODE;
      node->size_table.ptr = nullptr;
      if (original->size_table.ptr != nullptr)
        node->size_table = size_table_inc(original->size_table.ptr, original->len);
      node->child = (ref<internal_node<T, atomic_ref_counting>>*)((char*)node + sizeof(internal_node<T, atomic_ref_counting>));
      memset(node->child, 0, node->len * sizeof(ref<internal_node<T, atomic_ref_counting>>)); // init pointers to zero      
      for (uint32_t i = 0; i < original->len; ++i)
        node->child[i] = original->child[i];
      node->guid = 0;
      return node;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_dec(const internal_node<T, atomic_ref_counting>* original)
      {
      internal_node<T, atomic_ref_counting>* node = (internal_node<T, atomic_ref_counting>*)malloc(sizeof(internal_node<T, atomic_ref_counting>) + (original->len - 1) * sizeof(ref<internal_node<T, atomic_ref_counting>>));
      node->len = original->len - 1;
      node->type = INTERNAL_NODE;
      node->size_table.ptr = nullptr;
      node->size_table = original->size_table;
      node->child = (ref<internal_node<T, atomic_ref_counting>>*)((char*)node + sizeof(internal_node<T, atomic_ref_counting>));
      memset(node->child, 0, node->len * sizeof(ref<internal_node<T, atomic_ref_counting>>)); // init pointers to zero      
      for (uint32_t i = 0; i < node->len; ++i)
        node->child[i] = original->child[i];
      node->guid = 0;
      return node;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_new_above1(const ref<internal_node<T, atomic_ref_counting>>& child)
      {
      internal_node<T, atomic_ref_counting>* above = internal_node_create<T, atomic_ref_counting>(1);
      above->child[0] = child;
      return above;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_new_above(const ref<internal_node<T, atomic_ref_counting>>& left, const ref<internal_node<T, atomic_ref_counting>>& right)
      {
      internal_node<T, atomic_ref_counting>* above = internal_node_create<T, atomic_ref_counting>(2);
      above->child[0] = left;
      above->child[1] = right;
      return above;
      }

    template <typename T, bool atomic_ref_counting>
    inline internal_node<T, atomic_ref_counting>* internal_node_merge(const ref<internal_node<T, atomic_ref_counting>>& left, const ref<internal_node<T, atomic_ref_counting>>& centre, const ref<internal_node<T, atomic_ref_counting>>& right)
      {
      // If internal node is NULL, its size is zero.
      uint32_t left_len = (left.ptr == nullptr) ? 0 : left->len - 1;
      uint32_t centre_len = (centre.ptr == nullptr) ? 0 : centre->len;
      uint32_t right_len = (right.ptr == nullptr) ? 0 : right->len - 1;

      internal_node<T, atomic_ref_counting>* merged = internal_node_create<T, atomic_ref_counting>(left_len + centre_len + right_len);
      for (uint32_t i = 0; i < left_len; ++i)
        merged->child[i] = left->child[i];
      for (uint32_t i = 0; i < centre_len; ++i)
        merged->child[left_len + i] = centre->child[i];
      for (uint32_t i = 0; i < right_len; ++i)
        merged->child[left_len + centre_len + i] = right->child[i + 1];

      return merged;
      }

    template <typename T, bool atomic_ref_counting>
    inline ref<internal_node<T, atomic_ref_counting>>* append_empty(ref<internal_node<T, atomic_ref_counting>>* to_set, uint32_t empty_height)
      {
      if (0 < empty_height)
        {
        ref<internal_node<T, atomic_ref_counting>> leaf = internal_node_create<T, atomic_ref_counting>(1);
        ref<internal_node<T, atomic_ref_counting>> empty = leaf;
        for (uint32_t i = 1; i < empty_height; i++)
          {
          ref<internal_node<T, atomic_ref_counting>> new_empty = internal_node_create<T, atomic_ref_counting>(1);
          new_empty->child[0] = empty;
          empty = new_empty;
          }
        // this root node must be one larger, otherwise segfault        
        *to_set = empty;
        return &leaf->child[0];
        }
      else
        {
        return to_set;
        }
      }

    // - Height should be shift or height, not max element size
    // - copy_first_k returns a pointer to the next pointer to set
    // - append_empty now returns a pointer to the *void we're supposed to set

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>>* copy_first_k(const ref<rrb<T, atomic_ref_counting, N>>& in, const ref<rrb<T, atomic_ref_counting, N>>& new_rrb, const uint32_t k, const uint32_t tail_size)
      {
      ref<internal_node<T, atomic_ref_counting>> current = in->root;
      ref<internal_node<T, atomic_ref_counting>>* to_set = (ref<internal_node<T, atomic_ref_counting>>*)&new_rrb->root;
      uint32_t index = in->cnt - 1;
      uint32_t shift = in->shift;

      // Copy all non-leaf nodes first. Happens when shift > RRB_BRANCHING
      uint32_t i = 1;
      while (i <= k && shift != 0)
        {
        // First off, copy current node and stick it in.
        ref<internal_node<T, atomic_ref_counting>> new_current;
        if (i != k)
          {
          new_current = internal_node_clone(current.ptr);
          if (current->size_table.ptr != nullptr)
            {
            new_current->size_table = size_table_clone(new_current->size_table.ptr, new_current->len);
            new_current->size_table->size[new_current->len - 1] += tail_size;
            }
          }
        else
          { // increment size of last element -- will only happen if we append empties
          new_current = internal_node_inc(current.ptr);
          if (current->size_table.ptr != nullptr)
            {
            new_current->size_table->size[new_current->len - 1] = new_current->size_table->size[new_current->len - 2] + tail_size;
            }
          }
        *to_set = new_current;

        // calculate child index
        uint32_t child_index;
        if (current->size_table.ptr == nullptr)
          {
          child_index = (index >> shift) & bits<N>::rrb_mask;
          }
        else
          {
          // no need for sized_pos here, luckily.
          child_index = new_current->len - 1;
          // Decrement index
          if (child_index != 0)
            {
            index -= current->size_table->size[child_index - 1];
            }
          }
        to_set = &new_current->child[child_index];
        if (child_index < current->len)
          current = current->child[child_index];

        i++;
        shift -= bits<N>::rrb_bits;
        }
      return to_set;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<rrb<T, atomic_ref_counting, N>> push_down_tail(const ref<rrb<T, atomic_ref_counting, N>>& in, const ref<rrb<T, atomic_ref_counting, N>>& new_rrb, const ref<leaf_node<T, atomic_ref_counting>>& new_tail)
      {
      ref<leaf_node<T, atomic_ref_counting>> old_tail = new_rrb->tail;
      new_rrb->tail = new_tail;
      //if (in->cnt <= bits<N>::rrb_branching)
      if (in->root.ptr == nullptr) // [JanM] old code is commented out above. Fixed this due to bug, see unit test test_bug_concat in vector_tests.cpp
        {
        new_rrb->shift = 0;
        new_rrb->root = old_tail;
        return new_rrb;
        }

      // Copyable count starts here

      // TODO: Can find last rightmost jump in constant time for pvec subvecs:
      // use the fact that (index & large_mask) == 1 << (RRB_BITS * H) - 1 -> 0 etc.

      uint32_t index = in->cnt - 1;

      uint32_t nodes_to_copy = 0;
      uint32_t nodes_visited = 0;
      uint32_t pos = 0; // pos is the position we insert empty nodes in the bottom
                        // copyable node (or the element, if we can copy the leaf)
      ref<internal_node<T, atomic_ref_counting>> current = in->root;
      uint32_t shift = in->shift;

      // checking all non-leaf nodes (or if tail, all but the lowest two levels)
      while (shift > bits<N>::rrb_bits)
        {
        // calculate child index
        uint32_t child_index;
        if (current->size_table.ptr == nullptr)
          {
          // some check here to ensure we're not overflowing the pvec subvec.
          // important to realise that this only needs to be done once in a better
          // impl, the same way the size_table check only has to be done until it's
          // false.
          const uint32_t prev_shift = shift + bits<N>::rrb_bits;
          if (index >> prev_shift > 0)
            {
            nodes_visited++; // this could possibly be done earlier in the code.
            goto copyable_count_end;
            }
          child_index = (index >> shift) & bits<N>::rrb_mask;
          // index filtering is not necessary when the check above is performed at
          // most once.
          index &= ~(bits<N>::rrb_mask << shift);
          }
        else
          {
          // no need for sized_pos here, luckily.
          child_index = current->len - 1;
          // Decrement index
          if (child_index != 0)
            {
            index -= current->size_table->size[child_index - 1];
            }
          }
        nodes_visited++;
        if (child_index < bits<N>::rrb_mask)
          {
          nodes_to_copy = nodes_visited;
          pos = child_index;
          }

        if (child_index < current->len)
          current = current->child[child_index];
        else
          current = ref<internal_node<T, atomic_ref_counting>>();
        // This will only happen in a pvec subtree
        if (current.ptr == nullptr)
          {
          nodes_to_copy = nodes_visited;
          pos = child_index;

          // if next element we're looking at is null, we can copy all above. Good
          // times.
          goto copyable_count_end;
          }
        shift -= bits<N>::rrb_bits;
        }
      // if we're here, we're at the leaf node (or lowest non-leaf), which is
      // `current`

      // no need to even use index here: We know it'll be placed at current->len,
      // if there's enough space. That check is easy.
      if (shift != 0)
        {
        nodes_visited++;
        if (current->len < bits<N>::rrb_branching)
          {
          nodes_to_copy = nodes_visited;
          pos = current->len;
          }
        }

    copyable_count_end:
      // GURRHH, nodes_visited is not yet handled nicely. for loop down to get
      // nodes_visited set straight.
      while (shift > bits<N>::rrb_bits)
        {
        nodes_visited++;
        shift -= bits<N>::rrb_bits;
        }

      // Increasing height of tree.
      if (nodes_to_copy == 0)
        {
        ref<internal_node<T, atomic_ref_counting>> new_root = internal_node_create<T, atomic_ref_counting>(2);
        new_root->child[0] = in->root;
        new_rrb->root = new_root;
        new_rrb->shift = new_rrb->shift + bits<N>::rrb_bits;

        // create size table if the original rrb root has a size table.
        if (in->root->type != LEAF_NODE && ((const internal_node<T, atomic_ref_counting> *)in->root.ptr)->size_table.ptr != nullptr)
          {
          ref<rrb_size_table<atomic_ref_counting>> table = size_table_create<atomic_ref_counting>(2);
          table->size[0] = in->cnt - old_tail->len;
          // If we insert the tail, the old size minus the old tail size will be the
          // amount of elements in the left branch. If there is no tail, the size is
          // just the old rrb-tree.

          table->size[1] = in->cnt;
          // If we insert the tail, the old size would include the tail.
          // Consequently, it has to be the old size. If we have no tail, we append
          // a single element to the old vector, therefore it has to be one more
          // than the original.

          new_root->size_table = table;
          }

        // nodes visited == original rrb tree height. Nodes visited > 0.
        ref<internal_node<T, atomic_ref_counting>>* to_set = append_empty(&((internal_node<T, atomic_ref_counting> *)new_rrb->root.ptr)->child[1], nodes_visited);
        *to_set = old_tail;
        }
      else
        {
        ref<internal_node<T, atomic_ref_counting>>* node = copy_first_k(in, new_rrb, nodes_to_copy, old_tail->len);
        ref<internal_node<T, atomic_ref_counting>>* to_set = append_empty(node, nodes_visited - nodes_to_copy);
        *to_set = old_tail;
        }
      return new_rrb;
      }


    template <typename T, bool atomic_ref_counting, int N>
    inline rrb<T, atomic_ref_counting, N>* rrb_tail_push(const ref<rrb<T, atomic_ref_counting, N>>& in, T element)
      {
      rrb<T, atomic_ref_counting, N>* new_rrb = rrb_head_clone(in.ptr);
      leaf_node<T, atomic_ref_counting>* new_tail = leaf_node_inc(in->tail.ptr);
      new_tail->child[new_rrb->tail_len] = std::move(element);
      new_rrb->cnt++;
      new_rrb->tail_len++;
      new_rrb->tail = new_tail;
      return new_rrb;
      }

    template <typename T, bool atomic_ref_counting>
    inline uint32_t sized_pos(const internal_node<T, atomic_ref_counting>* node, uint32_t* index, uint32_t sp)
      {
      rrb_size_table<atomic_ref_counting>* table = node->size_table.ptr;
      uint32_t is = *index >> sp;
      while (table->size[is] <= *index)
        {
        is++;
        }
      if (is != 0)
        {
        *index -= table->size[is - 1];
        }
      return is;
      }

    template <typename T, bool atomic_ref_counting>
    inline const internal_node<T, atomic_ref_counting>* sized(const internal_node<T, atomic_ref_counting>* node, uint32_t* index, uint32_t sp)
      {
      uint32_t is = sized_pos(node, index, sp);
      return (internal_node<T, atomic_ref_counting>*)node->child[is].ptr;
      }

    /**
     * Destructively replaces the rightmost leaf as the new tail, discarding the
     * old.
     */
     // Note that this is very similar to the direct pop algorithm, which is
     // described further down in this file.
    template <typename T, bool atomic_ref_counting, int N>
    inline void promote_rightmost_leaf(ref<rrb<T, atomic_ref_counting, N>>& new_rrb)
      {
      ref<internal_node<T, atomic_ref_counting>> path[bits<N>::rrb_max_height + 1];
      path[0] = new_rrb->root;
      uint32_t i = 0, shift = 0;

      // populate path array
      for (i = 0, shift = 0; shift < new_rrb->shift; i++, shift += bits<N>::rrb_bits)
        {
        path[i + 1] = path[i]->child[path[i]->len - 1];
        }

      const uint32_t height = i;
      // Set the leaf node as tail.
      new_rrb->tail = path[height];
      new_rrb->tail_len = path[height]->len;
      const uint32_t tail_len = new_rrb->tail_len;

      // last element is now always null, in contrast to direct pop
      path[height] = ref<internal_node<T, atomic_ref_counting>>(nullptr);

      while (i-- > 0)
        {
        // TODO: First skip will always happen. Can we use that somehow?
        if (path[i + 1].ptr == nullptr)
          {
          if (path[i]->len == 1)
            {
            path[i] = ref<internal_node<T, atomic_ref_counting>>(nullptr);
            }
          else if (i == 0 && path[i]->len == 2)
            {
            path[i] = path[i]->child[0];
            new_rrb->shift -= bits<N>::rrb_bits;
            }
          else
            {
            path[i] = internal_node_dec(path[i].ptr);
            }
          }
        else
          {
          path[i] = internal_node_clone(path[i].ptr);
          path[i]->child[path[i]->len - 1] = path[i + 1];
          if (path[i]->size_table.ptr != nullptr)
            {
            path[i]->size_table = size_table_clone(path[i]->size_table.ptr, path[i]->len);
            // this line differs, as we remove `tail_len` elements from the trie,
            // instead of just 1 as in the direct pop algorithm.
            path[i]->size_table->size[path[i]->len - 1] -= tail_len;
            }
          }
        }

      new_rrb->root = path[0];
      }



    template <typename T, bool atomic_ref_counting, int N>
    inline ref<tree_node<T, atomic_ref_counting>> rrb_drop_left_rec(uint32_t *total_shift, const ref<tree_node<T, atomic_ref_counting>>& root, uint32_t left, uint32_t shift, bool has_right)
      {
      const uint32_t subshift = shift - bits<N>::rrb_bits;
      uint32_t subidx = left >> shift;
      if (shift > 0)
        {
        ref<internal_node<T, atomic_ref_counting>> internal_root = root;
        uint32_t idx = left;
        if (internal_root->size_table.ptr == nullptr)
          {
          idx -= subidx << shift;
          }
        else
          { // if (internal_root->size_table != NULL)
          const rrb_size_table<atomic_ref_counting> *table = internal_root->size_table.ptr;

          while (table->size[subidx] <= idx)
            {
            subidx++;
            }
          if (subidx != 0)
            {
            idx -= table->size[subidx - 1];
            }
          }

        const uint32_t last_slot = internal_root->len - 1;
        ref<tree_node<T, atomic_ref_counting>> child = internal_root->child[subidx];
        ref<tree_node<T, atomic_ref_counting>> left_hand_node = rrb_drop_left_rec<T, atomic_ref_counting, N>(total_shift, child, idx, subshift, (subidx != last_slot) | has_right);
        if (subidx == last_slot)
          { // No more slots left
          if (has_right)
            {
            ref<internal_node<T, atomic_ref_counting>> left_hand_parent = internal_node_create<T, atomic_ref_counting>(1);
            ref<internal_node<T, atomic_ref_counting>> internal_left_hand_node = left_hand_node;
            left_hand_parent->child[0] = internal_left_hand_node;

            if (subshift != 0 && internal_left_hand_node->size_table.ptr != nullptr)
              {
              ref<rrb_size_table<atomic_ref_counting>> sliced_table = size_table_create<atomic_ref_counting>(1);
              sliced_table->size[0] = internal_left_hand_node->size_table->size[internal_left_hand_node->len - 1];
              left_hand_parent->size_table = sliced_table;
              }
            *total_shift = shift;
            return left_hand_parent;
            }
          else
            { // if (!has_right)
            return left_hand_node;
            }
          }
        else
          { // if (subidx != last_slot)

          const uint32_t sliced_len = internal_root->len - subidx;
          ref<internal_node<T, atomic_ref_counting>> sliced_root = internal_node_create<T, atomic_ref_counting>(sliced_len);

          // TODO: Can shrink size here if sliced_len == 2, using the ambidextrous
          // vector technique w. offset. Takes constant time.

          for (uint32_t i = 0; i < (sliced_len - 1); ++i)
            sliced_root->child[1 + i] = internal_root->child[subidx + 1 + i];

          ref<rrb_size_table<atomic_ref_counting>> table = internal_root->size_table; // [JanM] copy seems unnecessary, todo

          // TODO: Can check if left is a power of the tree size. If so, all nodes
          // will be completely populated, and we can ignore the size table. Most
          // importantly, this will remove the need to alloc a size table, which
          // increases perf.
          ref<rrb_size_table<atomic_ref_counting>> sliced_table = size_table_create<atomic_ref_counting>(sliced_len);

          if (table.ptr == nullptr)
            {
            for (uint32_t i = 0; i < sliced_len; i++)
              {
              // left is total amount sliced off. By adding in subidx, we get faster
              // computation later on.
              sliced_table->size[i] = (subidx + 1 + i) << shift;
              // NOTE: This doesn't really work properly for top root, as last node
              // may have a higher count than it *actually* has. To remedy for this,
              // the top function performs a check afterwards, which may insert the
              // correct value if there's a size table in the root.
              }
            }
          else
            { // if (table != NULL)
            memcpy(sliced_table->size, &table->size[subidx], sliced_len * sizeof(uint32_t));
            }

          for (uint32_t i = 0; i < sliced_len; i++)
            {
            sliced_table->size[i] -= left;
            }

          sliced_root->size_table = sliced_table;
          sliced_root->child[0] = left_hand_node;
          *total_shift = shift;
          return sliced_root;
          }
        }
      else
        { // if (shift <= RRB_BRANCHING)
        ref<leaf_node<T, atomic_ref_counting>> leaf_root = root;
        const uint32_t right_vals_len = leaf_root->len - subidx;
        ref<leaf_node<T, atomic_ref_counting>> right_vals = leaf_node_create<T, atomic_ref_counting>(right_vals_len);

        //memcpy(right_vals->child, &leaf_root->child[subidx], right_vals_len * sizeof(T));
        for (uint32_t i = 0; i < right_vals_len; ++i)
          right_vals->child[i] = leaf_root->child[subidx + i]; // don't memcpy, but use copy constructor

        *total_shift = shift;

        return right_vals;
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<tree_node<T, atomic_ref_counting>> rrb_drop_right_rec(uint32_t *total_shift, const ref<tree_node<T, atomic_ref_counting>>& root, uint32_t right, uint32_t shift, bool has_left)
      {
      const uint32_t subshift = shift - bits<N>::rrb_bits;
      uint32_t subidx = right >> shift;
      if (shift > 0)
        {
        ref<internal_node<T, atomic_ref_counting>> internal_root = root;
        if (internal_root->size_table.ptr == nullptr)
          {
          ref<tree_node<T, atomic_ref_counting>> child = internal_root->child[subidx];
          ref<tree_node<T, atomic_ref_counting>> right_hand_node = rrb_drop_right_rec<T, atomic_ref_counting, N>(total_shift, child, right - (subidx << shift), subshift, (subidx != 0) | has_left);
          if (subidx == 0)
            {
            if (has_left)
              {
              ref<internal_node<T, atomic_ref_counting>> right_hand_parent = internal_node_create<T, atomic_ref_counting>(1);
              right_hand_parent->child[0] = right_hand_node;
              *total_shift = shift;
              return right_hand_parent;
              }
            else { // if (!has_left)
              return right_hand_node;
              }
            }
          else
            { // if (subidx != 0)
            ref<internal_node<T, atomic_ref_counting>> sliced_root = internal_node_create<T, atomic_ref_counting>(subidx + 1);
            for (uint32_t i = 0; i < subidx; ++i)
              sliced_root->child[i] = internal_root->child[i];
            sliced_root->child[subidx] = right_hand_node;
            *total_shift = shift;
            return sliced_root;
            }
          }
        else
          { // if (internal_root->size_table != NULL)
          rrb_size_table<atomic_ref_counting>* table = internal_root->size_table.ptr;
          uint32_t idx = right;

          while (table->size[subidx] <= idx)
            {
            subidx++;
            }
          if (subidx != 0)
            {
            idx -= table->size[subidx - 1];
            }

          ref<tree_node<T, atomic_ref_counting>> internal_root_child = internal_root->child[subidx];
          ref<tree_node<T, atomic_ref_counting>> right_hand_node = rrb_drop_right_rec<T, atomic_ref_counting, N>(total_shift, internal_root_child, idx, subshift, (subidx != 0) | has_left);
          if (subidx == 0)
            {
            if (has_left)
              {
              // As there is one above us, must place the right hand node in a
              // one-node
              ref<internal_node<T, atomic_ref_counting>> right_hand_parent = internal_node_create<T, atomic_ref_counting>(1);
              ref<rrb_size_table<atomic_ref_counting>> right_hand_table = size_table_create<atomic_ref_counting>(1);

              right_hand_table->size[0] = right + 1;
              // TODO: Not set size_table if the underlying node doesn't have a
              // table as well.
              right_hand_parent->size_table = right_hand_table;
              right_hand_parent->child[0] = right_hand_node;

              *total_shift = shift;
              return right_hand_parent;
              }
            else
              { // if (!has_left)
              return right_hand_node;
              }
            }
          else
            { // if (subidx != 0)
            ref<internal_node<T, atomic_ref_counting>> sliced_root = internal_node_create<T, atomic_ref_counting>(subidx + 1);
            ref<rrb_size_table<atomic_ref_counting>> sliced_table = size_table_create<atomic_ref_counting>(subidx + 1);

            memcpy(sliced_table->size, table->size, subidx * sizeof(uint32_t));
            sliced_table->size[subidx] = right + 1;

            for (uint32_t i = 0; i < subidx; ++i)
              sliced_root->child[i] = internal_root->child[i];
            sliced_root->size_table = sliced_table;
            sliced_root->child[subidx] = right_hand_node;

            *total_shift = shift;
            return sliced_root;
            }
          }
        }
      else
        { // if (shift <= RRB_BRANCHING)
        // Just pure copying into a new node
        ref<leaf_node<T, atomic_ref_counting>> leaf_root = root;
        ref<leaf_node<T, atomic_ref_counting>> left_vals = leaf_node_create<T, atomic_ref_counting>(subidx + 1);

        //memcpy(left_vals->child, leaf_root->child, (subidx + 1) * sizeof(T));
        for (uint32_t i = 0; i < subidx + 1; ++i)
          left_vals->child[i] = leaf_root->child[i]; // don't memcpy, but use copy constructor

        *total_shift = shift;
        return left_vals;
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<rrb<T, atomic_ref_counting, N>> rrb_drop_left(ref<rrb<T, atomic_ref_counting, N>> in, uint32_t left)
      {
      using namespace rrb_details;
      if (left >= in->cnt)
        {
        return rrb_create<T, atomic_ref_counting, N>();
        }
      else if (left > 0)
        {
        const uint32_t remaining = in->cnt - left;

        // If we slice into the tail, we just need to modify the tail itself
        if (remaining <= in->tail_len)
          {
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(remaining);
          //memcpy(new_tail->child, &in->tail->child[in->tail_len - remaining], remaining * sizeof(T));
          for (uint32_t i = 0; i < remaining; ++i)
            new_tail->child[i] = in->tail->child[in->tail_len - remaining + i]; // don't memcpy, but use copy constructor

          ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_create<T, atomic_ref_counting, N>();
          new_rrb->cnt = remaining;
          new_rrb->tail_len = remaining;
          new_rrb->tail = new_tail;
          return new_rrb;
          }
        // Otherwise, we don't really have to take the tail into consideration.
        // Good!

        ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_create<T, atomic_ref_counting, N>();
        ref<internal_node<T, atomic_ref_counting>> root = rrb_drop_left_rec<T, atomic_ref_counting, N>(&new_rrb->shift, in->root, left, in->shift, false);
        new_rrb->cnt = remaining;
        new_rrb->root = root;

        // Ensure last element in size table is correct size, if the root is an
        // internal node.
        if (new_rrb->shift != 0 && root->size_table.ptr != nullptr)
          {
          root->size_table->size[root->len - 1] = new_rrb->cnt - in->tail_len;
          }
        new_rrb->tail = in->tail;
        new_rrb->tail_len = in->tail_len;
        in = new_rrb;
        }

      // TODO: I think the code below also applies to root nodes where size_table
      // == NULL and (cnt - tail_len) & 0xff != 0, but it may be that this is
      // resolved by rrb_drop_right itself. Perhaps not promote in the right slicing,
      // but here instead?

      // This case handles leaf nodes < RRB_BRANCHING size, by redistributing
      // values from the tail into the actual leaf node.
      if (in->shift == 0 && in->root.ptr != nullptr)
        {
        // two cases to handle: cnt <= RRB_BRANCHING
        //     and (cnt - tail_len) < RRB_BRANCHING

        if (in->cnt <= bits<N>::rrb_branching)
          {
          // can put all into a new tail
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(in->cnt);
          //memcpy(&new_tail->child[0], &((leaf_node<T, atomic_ref_counting> *)in->root.ptr)->child[0], in->root->len * sizeof(T));
          for (uint32_t i = 0; i < in->root->len; ++i)
            new_tail->child[i] = ((leaf_node<T, atomic_ref_counting> *)in->root.ptr)->child[i]; // don't memcpy, but use copy constructor

          //memcpy(&new_tail->child[in->root->len], &in->tail->child[0], in->tail_len * sizeof(T));
          for (uint32_t i = 0; i < in->tail_len; ++i)
            new_tail->child[in->root->len + i] = in->tail->child[i]; // don't memcpy, but use copy constructor
          in->tail_len = in->cnt;
          in->root = ref<tree_node<T, atomic_ref_counting>>(nullptr);
          in->tail = new_tail;
          }
        // no need for <= here, because if the root node is == rrb_branching, the
        // invariant is kept.
        else if (in->cnt - in->tail_len < bits<N>::rrb_branching)
          {
          // create both a new tail and a new root node
          const uint32_t tail_cut = bits<N>::rrb_branching - in->root->len;
          ref<leaf_node<T, atomic_ref_counting>> new_root = leaf_node_create<T, atomic_ref_counting>(bits<N>::rrb_branching);
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(in->tail_len - tail_cut);

          //memcpy(&new_root->child[0], &((leaf_node<T, atomic_ref_counting> *)in->root.ptr)->child[0], in->root->len * sizeof(T));
          for (uint32_t i = 0; i < in->root->len; ++i)
            new_root->child[i] = ((leaf_node<T, atomic_ref_counting> *)in->root.ptr)->child[i]; // don't memcpy, but use copy constructor

          //memcpy(&new_root->child[in->root->len], &in->tail->child[0], tail_cut * sizeof(T));
          for (uint32_t i = 0; i < tail_cut; ++i)
            new_root->child[in->root->len + i] = in->tail->child[i]; // don't memcpy, but use copy constructor

          //memcpy(&new_tail->child[0], &in->tail->child[tail_cut], (in->tail_len - tail_cut) * sizeof(T));
          for (uint32_t i = 0; i < (in->tail_len - tail_cut); ++i)
            new_tail->child[i] = in->tail->child[tail_cut + i]; // don't memcpy, but use copy constructor

          in->tail_len = in->tail_len - tail_cut;
          in->tail = new_tail;
          in->root = new_root;
          }
        }
      return in;
      }


    template <typename T, bool atomic_ref_counting, int N>
    inline ref<rrb<T, atomic_ref_counting, N>> rrb_drop_right(ref<rrb<T, atomic_ref_counting, N>> in, const uint32_t right)
      {
      using namespace rrb_details;
      if (right == 0)
        {
        return rrb_create<T, atomic_ref_counting, N>();
        }
      else if (right < in->cnt)
        {
        const uint32_t tail_offset = in->cnt - in->tail_len;
        // Can just cut the tail slightly
        if (tail_offset < right)
          {
          ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_head_clone(in.ptr);
          const uint32_t new_tail_len = right - tail_offset;
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(new_tail_len);
          //memcpy(new_tail->child, in->tail->child, new_tail_len * sizeof(T));
          for (uint32_t i = 0; i < new_tail_len; ++i)
            new_tail->child[i] = in->tail->child[i]; // don't memcpy, but use copy constructor

          new_rrb->cnt = right;
          new_rrb->tail = new_tail;
          new_rrb->tail_len = new_tail_len;
          return new_rrb;
          }

        ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_create<T, atomic_ref_counting, N>();
        ref<tree_node<T, atomic_ref_counting>> root = rrb_drop_right_rec<T, atomic_ref_counting, N>(&new_rrb->shift, in->root, right - 1, in->shift, false);
        new_rrb->cnt = right;
        new_rrb->root = root;

        // Not sure if this is necessary in this part of the program, due to issues
        // wrt. rrb_drop_left and roots without size tables.
        promote_rightmost_leaf(new_rrb);
        new_rrb->tail_len = new_rrb->tail->len;
        return new_rrb;
        }
      else
        {
        return in;
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline uint32_t size_sub_trie(const ref<tree_node<T, atomic_ref_counting>>& node, uint32_t shift)
      {
      if (shift > 0)
        {
        ref<internal_node<T, atomic_ref_counting>> intern = node;
        if (intern->size_table.ptr == nullptr)
          {
          uint32_t len = intern->len;
          uint32_t child_shift = shift - bits<N>::rrb_bits;
          // TODO: for loopify recursive calls
          /* We're not sure how many are in the last child, so look it up */
          ref<tree_node<T, atomic_ref_counting>> child = intern->child[len - 1];
          uint32_t last_size = size_sub_trie<T, atomic_ref_counting, N>(child, child_shift);
          /* We know all but the last ones are filled, and they have child_shift
             elements in them. */
          return ((len - 1) << shift) + last_size;
          }
        else
          {
          return intern->size_table->size[intern->len - 1];
          }
        }
      else
        {
        leaf_node<T, atomic_ref_counting>* leaf = (leaf_node<T, atomic_ref_counting>*)node.ptr;
        return leaf->len;
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>> set_sizes(ref<internal_node<T, atomic_ref_counting>>& node, uint32_t shift)
      {
      uint32_t sum = 0;
      ref<rrb_size_table<atomic_ref_counting>> table = size_table_create<atomic_ref_counting>(node->len);
      const uint32_t child_shift = shift - bits<N>::rrb_bits;

      for (uint32_t i = 0; i < node->len; i++)
        {
        ref<tree_node<T, atomic_ref_counting>> child = node->child[i];
        sum += size_sub_trie<T, atomic_ref_counting, N>(child, child_shift);
        table->size[i] = sum;
        }
      node->size_table = table;
      return node;
      }

    // optimize this away?
    template <typename T, bool atomic_ref_counting, int N>
    uint32_t find_shift(const ref<tree_node<T, atomic_ref_counting>>& node)
      {
      if (node->type == LEAF_NODE)
        {
        return 0;
        }
      else
        { // must be internal node
        internal_node<T, atomic_ref_counting>* inode = (internal_node<T, atomic_ref_counting>*)node.ptr;
        ref<tree_node<T, atomic_ref_counting>> child = inode->child[0];
        return bits<N>::rrb_bits + find_shift<T, atomic_ref_counting, N>(child);
        }
      }


    /**
     * create_concat_plan takes in the large concatenated internal node and a
     * pointer to an uint32_t, which will contain the reduced size of the rebalanced
     * node. It returns a plan as an array of uint32_t's, and modifies the input
     * pointer to contain the length of said array.
     */

    template <typename T, bool atomic_ref_counting, int N>
    inline std::vector<uint32_t> create_concat_plan(const ref<internal_node<T, atomic_ref_counting>>& all, uint32_t* top_len)
      {
      std::vector<uint32_t> node_count(all->len);

      uint32_t total_nodes = 0;
      for (uint32_t i = 0; i < all->len; i++)
        {
        const uint32_t size = all->child[i]->len;
        node_count[i] = size;
        total_nodes += size;
        }

      const uint32_t optimal_slots = ((total_nodes - 1) / bits<N>::rrb_branching) + 1;

      uint32_t shuffled_len = all->len;
      uint32_t i = 0;
      while (optimal_slots + bits<N>::rrb_extras < shuffled_len)
        {

        // Skip over all nodes satisfying the invariant.
        while (node_count[i] > bits<N>::rrb_branching - bits<N>::rrb_invariant)
          {
          i++;
          }

        // Found short node, so redistribute over the next nodes
        uint32_t remaining_nodes = node_count[i];
        do
          {
          const uint32_t min_size = std::min<uint32_t>(remaining_nodes + node_count[i + 1], bits<N>::rrb_branching);
          node_count[i] = min_size;
          remaining_nodes = remaining_nodes + node_count[i + 1] - min_size;
          i++;
          } while (remaining_nodes > 0);

          // Shuffle up remaining node sizes
          for (uint32_t j = i; j < shuffled_len - 1; j++)
            {
            node_count[j] = node_count[j + 1]; // Could use memmove here I guess
            }
          shuffled_len--;
          i--;
        }

      *top_len = shuffled_len;
      return node_count;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>> execute_concat_plan(const ref<internal_node<T, atomic_ref_counting>>& all, const std::vector<uint32_t>& node_size, uint32_t slen, uint32_t shift)
      {
      // the all vector doesn't have sizes set yet.

      ref<internal_node<T, atomic_ref_counting>> new_all = internal_node_create<T, atomic_ref_counting>(slen);
      // Current old node index to copy from
      uint32_t idx = 0;

      // Offset is how long into the current old node we've already copied from
      uint32_t offset = 0;

      if (shift == bits<N>::rrb_bits)
        { // handle leaf nodes here
        for (uint32_t i = 0; i < slen; i++)
          {
          const uint32_t new_size = node_size[i];
          ref<leaf_node<T, atomic_ref_counting>> old = all->child[idx];

          if (offset == 0 && new_size == old->len)
            {
            // just pointer copy the node if there is no offset and both have same
            // size
            idx++;
            new_all->child[i] = old;
            }
          else
            {
            ref<leaf_node<T, atomic_ref_counting>> new_node = leaf_node_create<T, atomic_ref_counting>(new_size);
            uint32_t cur_size = 0;
            // cur_size is the current size of the new node
            // (the amount of elements copied into it so far)

            while (cur_size < new_size /*&& idx < all->len*/)
              {
              // the commented out check is verified by create_concat_plan --
              // otherwise the implementation is erroneous!
              ref<leaf_node<T, atomic_ref_counting>> old_node = all->child[idx];

              if (new_size - cur_size >= old_node->len - offset)
                {
                // if this node can contain all elements not copied in the old node,
                // copy all of them into this node

                //memcpy(&new_node->child[cur_size], &old_node->child[offset], (old_node->len - offset) * sizeof(T));
                for (uint32_t j = 0; j < old_node->len - offset; ++j)
                  new_node->child[cur_size + j] = old_node->child[offset + j]; // don't memcpy, but use copy constructor

                cur_size += old_node->len - offset;
                idx++;
                offset = 0;
                }
              else
                {
                // if this node can't contain all the elements not copied in the old
                // node, copy as many as we can and pass the old node over to the
                // new node after this one.

                //memcpy(&new_node->child[cur_size], &old_node->child[offset], (new_size - cur_size) * sizeof(T));
                for (uint32_t j = 0; j < new_size - cur_size; ++j)
                  new_node->child[cur_size + j] = old_node->child[offset + j]; // don't memcpy, but use copy constructor

                offset += new_size - cur_size;
                cur_size = new_size;
                }
              }

            new_all->child[i] = new_node;
            }
          }
        }
      else
        { // not at lowest non-leaf level
        // this is ALMOST equivalent with the leaf node copying, the only difference
        // is that this is with internal nodes and the fact that they have to create
        // their size tables.

        // As that's the only difference, I won't bother with comments here.
        for (uint32_t i = 0; i < slen; i++)
          {
          const uint32_t new_size = node_size[i];
          ref<internal_node<T, atomic_ref_counting>> old = all->child[idx];

          if (offset == 0 && new_size == old->len)
            {
            idx++;
            new_all->child[i] = old;
            }
          else
            {
            ref<internal_node<T, atomic_ref_counting>> new_node = internal_node_create<T, atomic_ref_counting>(new_size);
            uint32_t cur_size = 0;
            while (cur_size < new_size)
              {
              ref<internal_node<T, atomic_ref_counting>> old_node = all->child[idx];

              if (new_size - cur_size >= old_node->len - offset)
                {
                for (uint32_t k = 0; k < (old_node->len - offset); ++k)
                  new_node->child[cur_size + k] = old_node->child[offset + k];

                cur_size += old_node->len - offset;
                idx++;
                offset = 0;
                }
              else
                {
                for (uint32_t k = 0; k < (new_size - cur_size); ++k)
                  new_node->child[cur_size + k] = old_node->child[offset + k];

                offset += new_size - cur_size;
                cur_size = new_size;
                }
              }
            set_sizes<T, atomic_ref_counting, N>(new_node, shift - bits<N>::rrb_bits); // This is where we set sizes
            new_all->child[i] = new_node;
            }
          }
        }
      return new_all;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>> rebalance(const ref<internal_node<T, atomic_ref_counting>>& left, const ref<internal_node<T, atomic_ref_counting>>& centre, const ref<internal_node<T, atomic_ref_counting>>& right, uint32_t shift, bool is_top)
      {
      ref<internal_node<T, atomic_ref_counting>> all = internal_node_merge(left, centre, right);
      // top_len is children count of the internal node returned.
      uint32_t top_len; // populated through pointer manipulation.

      std::vector<uint32_t> node_count = create_concat_plan<T, atomic_ref_counting, N>(all, &top_len);

      ref<internal_node<T, atomic_ref_counting>> new_all = execute_concat_plan<T, atomic_ref_counting, N>(all, node_count, top_len, shift);
      if (top_len <= bits<N>::rrb_branching)
        {
        if (is_top == false)
          {
          return internal_node_new_above1(set_sizes<T, atomic_ref_counting, N>(new_all, shift));
          }
        else
          {
          return new_all;
          }
        }
      else
        {
        ref<internal_node<T, atomic_ref_counting>> new_left = internal_node_copy(new_all.ptr, 0, bits<N>::rrb_branching);
        ref<internal_node<T, atomic_ref_counting>> new_right = internal_node_copy(new_all.ptr, bits<N>::rrb_branching, top_len - bits<N>::rrb_branching);
        return internal_node_new_above<T, atomic_ref_counting>(set_sizes<T, atomic_ref_counting, N>(new_left, shift), set_sizes<T, atomic_ref_counting, N>(new_right, shift));
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>> concat_sub_tree(const ref<tree_node<T, atomic_ref_counting>>& left_node, uint32_t left_shift, const ref<tree_node<T, atomic_ref_counting>>& right_node, uint32_t right_shift, bool is_top)
      {
      if (left_shift > right_shift)
        {
        // Left tree is higher than right tree
        ref<internal_node<T, atomic_ref_counting>> left_internal = left_node;
        ref<tree_node<T, atomic_ref_counting>> left_node_temp = left_internal->child[left_internal->len - 1];
        ref<internal_node<T, atomic_ref_counting>> centre_node = concat_sub_tree<T, atomic_ref_counting, N>(left_node_temp, left_shift - bits<N>::rrb_bits, right_node, right_shift, false);
        ref<internal_node<T, atomic_ref_counting>> empty(nullptr);
        return rebalance<T, atomic_ref_counting, N>(left_internal, centre_node, empty, left_shift, is_top);
        }
      else if (left_shift < right_shift)
        {
        ref<internal_node<T, atomic_ref_counting>> right_internal = right_node;
        ref<tree_node<T, atomic_ref_counting>> right_node_temp = right_internal->child[0];
        ref<internal_node<T, atomic_ref_counting>> centre_node = concat_sub_tree<T, atomic_ref_counting, N>(left_node, left_shift, right_node_temp, right_shift - bits<N>::rrb_bits, false);
        ref<internal_node<T, atomic_ref_counting>> empty(nullptr);
        return rebalance<T, atomic_ref_counting, N>(empty, centre_node, right_internal, right_shift, is_top);
        }
      else
        { // we have same height
        if (left_shift == 0)
          { // We're dealing with leaf nodes
          ref<leaf_node<T, atomic_ref_counting>> left_leaf = left_node;
          ref<leaf_node<T, atomic_ref_counting>> right_leaf = right_node;
          // We don't do this if we're not at top, as we'd have to zip stuff above
          // as well.
          if (is_top && (left_leaf->len + right_leaf->len) <= bits<N>::rrb_branching)
            {
            // Can put them in a single node
            ref<internal_node<T, atomic_ref_counting>> merged = leaf_node_merge<T, atomic_ref_counting>(left_leaf.ptr, right_leaf.ptr);
            return internal_node_new_above1(merged);
            }
          else
            {
            ref<internal_node<T, atomic_ref_counting>> left_internal = left_node;
            ref<internal_node<T, atomic_ref_counting>> right_internal = right_node;
            return internal_node_new_above(left_internal, right_internal);
            }
          }
        else
          { // two internal nodes with same height. Move both down
          ref<internal_node<T, atomic_ref_counting>> left_internal = left_node;
          ref<internal_node<T, atomic_ref_counting>> right_internal = right_node;
          ref<tree_node<T, atomic_ref_counting>> left_node_temp = left_internal->child[left_internal->len - 1];
          ref<tree_node<T, atomic_ref_counting>> right_node_temp = right_internal->child[0];
          ref<internal_node<T, atomic_ref_counting>> centre_node = concat_sub_tree<T, atomic_ref_counting, N>(left_node_temp, left_shift - bits<N>::rrb_bits, right_node_temp, right_shift - bits<N>::rrb_bits, false);
          // can be optimised: since left_shift == right_shift, we'll end up in this
          // block again.
          return rebalance<T, atomic_ref_counting, N>(left_internal, centre_node, right_internal, left_shift, is_top);
          }
        }
      }



    } // namespace rrb_details


    template <typename T>
    ref<T>::ref() : ptr(nullptr) {}

    template <typename T>
    ref<T>::ref(T* p) : ptr(p)
      {
      if (ptr)
        ptr->_ref_count = 1;
      }

    template <typename T>
    template <typename U>
    ref<T>::ref(U* p) : ptr((T*)p)
      {
      if (ptr)
        ptr->_ref_count = 1;
      }

    template <typename T>
    ref<T>::ref(const ref<T>& r) : ptr(r.ptr)
      {
      rrb_details::addref(ptr);
      }

    template <typename T>
    template <typename U>
    ref<T>::ref(const ref<U>& r) : ptr((T*)r.ptr)
      {
      rrb_details::addref(ptr);
      }

    template <typename T>
    ref<T>::~ref()
      {
      rrb_details::release(ptr);
      }

    template <typename T>  
    T* ref<T>::operator->() const
      {
      assert(ptr != nullptr);
      return ptr;
      }

    template <typename T>  
    void ref<T>::swap(ref<T>& r)
      {
      T* temp = ptr;
      ptr = r.ptr;
      r.ptr = temp;
      }

    template <typename T>  
    ref<T>& ref<T>::operator = (const ref<T>& r)
      {
      ref<T> temp(r);
      swap(temp);
      return *this;
      }

    template <typename T>  
    template <typename U>
    ref<T>& ref<T>::operator = (const ref<U>& r)
      {
      ref<T> temp(r);
      swap(temp);
      return *this;
      }

    template <typename T>  
    bool ref<T>::unique()
      {
      return (ptr == nullptr) || (ptr->_ref_count == 1);
      }

    template <typename T>  
    void ref<T>::inc() const
      {
      rrb_details::addref(ptr);
      }

    template <typename T>  
    void ref<T>::dec() const
      {
      rrb_details::release(ptr);
      }


  template <typename T, bool atomic_ref_counting = true, int N = 5>
  struct rrb
    {
    uint32_t cnt;
    uint32_t shift;
    uint32_t tail_len;
    ref<rrb_details::leaf_node<T, atomic_ref_counting>> tail;
    ref<rrb_details::tree_node<T, atomic_ref_counting>> root;
    mutable std::atomic<uint32_t> _ref_count;
    };

  template <typename T, int N>
  struct rrb<T, false, N>
    {
    uint32_t cnt;
    uint32_t shift;
    uint32_t tail_len;
    ref<rrb_details::leaf_node<T, false>> tail;
    ref<rrb_details::tree_node<T, false>> root;
    mutable uint32_t _ref_count;
    };

  template <typename T, bool atomic_ref_counting = true, int N = 5>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_create()
    {
    rrb<T, atomic_ref_counting, N>* empty = (rrb<T, atomic_ref_counting, N>*)malloc(sizeof(rrb<T, atomic_ref_counting, N>));
    empty->cnt = 0;
    empty->shift = 0;
    empty->root.ptr = nullptr;
    empty->tail.ptr = nullptr;
    empty->tail_len = 0;
    empty->tail = rrb_details::create_empty_leaf<T, atomic_ref_counting>();
    return ref<rrb<T, atomic_ref_counting, N>>(empty);
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_push(const ref<rrb<T, atomic_ref_counting, N>>& in, T element)
    {
    using namespace rrb_details;
    if (in->tail_len < bits<N>::rrb_branching)
      {
      return rrb_details::rrb_tail_push(in, std::move(element));
      }
    ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_head_clone(in.ptr);
    new_rrb->cnt++;
    ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(1);
    new_tail->child[0] = std::move(element);
    new_rrb->tail_len = 1;
    return push_down_tail(in, new_rrb, new_tail);
    }

  // Also assume direct append
  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_pop(const ref<rrb<T, atomic_ref_counting, N>>& in)
    {
    using namespace rrb_details;
    if (in->cnt == 1)
      {
      return rrb_create<T, atomic_ref_counting, N>();
      }
    ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_head_clone(in.ptr);
    new_rrb->cnt--;

    if (in->tail_len == 1)
      {
      promote_rightmost_leaf(new_rrb);
      return new_rrb;
      }
    else
      {
      ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_dec(in->tail.ptr);
      new_rrb->tail_len--;
      new_rrb->tail = new_tail;
      return new_rrb;
      }
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_update(const ref<rrb<T, atomic_ref_counting, N>>& in, uint32_t index, T element)
    {
    using namespace rrb_details;
    assert(index < in->cnt);
    ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_head_clone(in.ptr);
    const uint32_t tail_offset = in->cnt - in->tail_len;
    if (tail_offset <= index)
      {
      ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_clone(in->tail.ptr);
      new_tail->child[index - tail_offset] = std::move(element);
      new_rrb->tail = new_tail;
      return new_rrb;
      }
    ref<internal_node<T, atomic_ref_counting>>* previous_pointer = (ref<internal_node<T, atomic_ref_counting>>*)&new_rrb->root;
    ref<internal_node<T, atomic_ref_counting>> current = in->root;
    for (uint32_t shift = in->shift; shift > 0; shift -= bits<N>::rrb_bits)
      {
      current = internal_node_clone(current.ptr);
      *previous_pointer = current;

      uint32_t child_index;
      if (current->size_table.ptr == nullptr)
        {
        child_index = (index >> shift) & bits<N>::rrb_mask;
        }
      else
        {
        child_index = sized_pos(current.ptr, &index, shift);
        }
      previous_pointer = &current->child[child_index];
      current = current->child[child_index];
      }
    ref<leaf_node<T, atomic_ref_counting>> leaf = current;
    leaf = leaf_node_clone(leaf.ptr);
    *previous_pointer = leaf;
    leaf->child[index & bits<N>::rrb_mask] = std::move(element);
    return new_rrb;
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline const T& rrb_nth(const ref<rrb<T, atomic_ref_counting, N>>& rrb, uint32_t index)
    {
    using namespace rrb_details;
    assert(index < rrb->cnt);
    const uint32_t tail_offset = rrb->cnt - rrb->tail_len;
    if (tail_offset <= index)
      {
      return rrb->tail->child[index - tail_offset];
      }
    else
      {
      const internal_node<T, atomic_ref_counting>* current = (const internal_node<T, atomic_ref_counting>*)rrb->root.ptr;
      for (uint32_t shift = rrb->shift; shift > 0; shift -= bits<N>::rrb_bits)
        {
        if (current->size_table.ptr == nullptr)
          {
          const uint32_t subidx = (index >> shift) & bits<N>::rrb_mask;
          current = current->child[subidx].ptr;
          }
        else
          {
          current = sized(current, &index, shift);
          }
        }
      return ((const leaf_node<T, atomic_ref_counting>*)current)->child[index & bits<N>::rrb_mask];
      }
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline std::tuple<const T*, uint32_t, uint32_t> rrb_region_for(const ref<rrb<T, atomic_ref_counting, N>>& rrb, uint32_t index)
    {
    using namespace rrb_details;
    assert(index < rrb->cnt);
    const uint32_t tail_offset = rrb->cnt - rrb->tail_len;
    if (tail_offset <= index)
      {
      return std::make_tuple(rrb->tail->child, tail_offset, rrb->cnt);
      }
    else
      {
      const uint32_t original_index = index;
      const internal_node<T, atomic_ref_counting>* current = (const internal_node<T, atomic_ref_counting>*)rrb->root.ptr;
      for (uint32_t shift = rrb->shift; shift > 0; shift -= bits<N>::rrb_bits)
        {
        if (current->size_table.ptr == nullptr)
          {
          const uint32_t subidx = (index >> shift) & bits<N>::rrb_mask;
          current = current->child[subidx].ptr;
          }
        else
          {
          current = sized(current, &index, shift);
          }
        }
      const uint32_t index_of_first_element = original_index - (index & bits<N>::rrb_mask);
      return std::make_tuple(((const leaf_node<T, atomic_ref_counting>*)current)->child, index_of_first_element, index_of_first_element + ((const leaf_node<T, atomic_ref_counting>*)current)->len);
      }
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline uint32_t rrb_count(const ref<rrb<T, atomic_ref_counting, N>>& rrb)
    {
    return rrb->cnt;
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline const T& rrb_peek(const ref<rrb<T, atomic_ref_counting, N>>& rrb)
    {
    return rrb->tail->child[rrb->tail_len - 1];
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_concat(const ref<rrb<T, atomic_ref_counting, N>>& left, const ref<rrb<T, atomic_ref_counting, N>>& right)
    {
    using namespace rrb_details;
    if (left->cnt == 0)
      {
      return right;
      }
    else if (right->cnt == 0)
      {
      return left;
      }
    else
      {
      if (right->root.ptr == nullptr)
        {
        // merge left and right tail, if possible
        ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_head_clone<T, atomic_ref_counting, N>(left.ptr);
        new_rrb->cnt += right->cnt;

        // skip merging if left tail is full.
        if (left->tail_len == bits<N>::rrb_branching)
          {
          new_rrb->tail_len = right->tail_len;
          return push_down_tail(left, new_rrb, right->tail);
          }
        // We can merge both tails into a single tail.
        else if (left->tail_len + right->tail_len <= bits<N>::rrb_branching)
          {
          const uint32_t new_tail_len = left->tail_len + right->tail_len;
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_merge<T, atomic_ref_counting>(left->tail.ptr, right->tail.ptr);
          new_rrb->tail = new_tail;
          new_rrb->tail_len = new_tail_len;
          return new_rrb;
          }
        else
          { // must push down something, and will have elements remaining in
            // the right tail
          ref<leaf_node<T, atomic_ref_counting>> push_down = leaf_node_create<T, atomic_ref_counting>(bits<N>::rrb_branching);
          //memcpy(&push_down->child[0], &left->tail->child[0], left->tail_len * sizeof(T));
          for (uint32_t i = 0; i < left->tail_len; ++i)
            push_down->child[i] = left->tail->child[i]; // don't memcpy, but use copy constructor
          const uint32_t right_cut = bits<N>::rrb_branching - left->tail_len;
          //memcpy(&push_down->child[left->tail_len], &right->tail->child[0], right_cut * sizeof(T));
          for (uint32_t i = 0; i < right_cut; ++i)
            push_down->child[left->tail_len + i] = right->tail->child[i]; // don't memcpy, but use copy constructor

          // this will be strictly positive.
          const uint32_t new_tail_len = right->tail_len - right_cut;
          ref<leaf_node<T, atomic_ref_counting>> new_tail = leaf_node_create<T, atomic_ref_counting>(new_tail_len);

          //memcpy(&new_tail->child[0], &right->tail->child[right_cut], new_tail_len * sizeof(T));
          for (uint32_t i = 0; i < new_tail_len; ++i)
            new_tail->child[i] = right->tail->child[right_cut + i]; // don't memcpy, but use copy constructor

          new_rrb->tail = push_down;
          new_rrb->tail_len = new_tail_len;

          // This is an imitation, so that push_down_tail works as we intend it
          // to: Whenever the height has to be increased, it calculates the size
          // table based upon the old rrb's size, minus the old tail. However,
          // since we manipulate the old tail to be longer than it actually was,
          // we have to reflect those changes in the cnt variable.

          ref<rrb<T, atomic_ref_counting, N>> left_imitation = rrb_head_clone(left.ptr);
          left_imitation->cnt = new_rrb->cnt - new_tail_len;

          return push_down_tail(left_imitation, new_rrb, new_tail);
          }
        }
      ref<leaf_node<T, atomic_ref_counting>> empty_leaf(nullptr);
      ref<rrb<T, atomic_ref_counting, N>> left_head = rrb_head_clone<T, atomic_ref_counting, N>(left.ptr);
      ref<rrb<T, atomic_ref_counting, N>> left2 = push_down_tail(left, left_head, empty_leaf);
      ref<rrb<T, atomic_ref_counting, N>> new_rrb = rrb_create<T, atomic_ref_counting, N>();
      new_rrb->cnt = left2->cnt + right->cnt;

      ref<internal_node<T, atomic_ref_counting>> root_candidate = concat_sub_tree<T, atomic_ref_counting, N>(left2->root, left2->shift, right->root, right->shift, true);

      ref<tree_node<T, atomic_ref_counting>> find_shift_arg = root_candidate;
      new_rrb->shift = find_shift<T, atomic_ref_counting, N>(find_shift_arg);
      // must be done before we set sizes.
      new_rrb->root = set_sizes<T, atomic_ref_counting, N>(root_candidate, new_rrb->shift);
      new_rrb->tail = right->tail;
      new_rrb->tail_len = right->tail_len;
      return new_rrb;
      }
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> rrb_slice(const ref<rrb<T, atomic_ref_counting, N>>& rrb, uint32_t from, uint32_t to)
    {
    using namespace rrb_details;
    return rrb_drop_left(rrb_drop_right(rrb, to), from);
    }

  }
