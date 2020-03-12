
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

#include "rrb.h"
#include <thread>

namespace immutable
  {

  template <typename T, bool atomic_ref_counting, int N>
  struct transient_rrb;

  template <typename T, bool atomic_ref_counting, int N>
  ref<transient_rrb<T, atomic_ref_counting, N>> rrb_to_transient(const ref<rrb<T, atomic_ref_counting, N>>& in);

  template <typename T, bool atomic_ref_counting, int N>
  ref<rrb<T, atomic_ref_counting, N>> transient_to_rrb(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb);

  template <typename T, bool atomic_ref_counting, int N>
  ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_update(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, uint32_t index, T element);

  template <typename T, bool atomic_ref_counting, int N>
  ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_push(ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, T element);

  template <typename T, bool atomic_ref_counting, int N>
  const T& transient_rrb_nth(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, uint32_t index);

  template <typename T, bool atomic_ref_counting, int N>
  const T& transient_rrb_peek(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb);

  template <typename T, bool atomic_ref_counting, int N>
  uint32_t transient_rrb_count(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb);

  template <typename T, bool atomic_ref_counting, int N>
  ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_pop(ref<transient_rrb<T, atomic_ref_counting, N>>& trrb);

  namespace rrb_details
    {

    static std::atomic<guid_type> g_guid{ 1 };

    inline guid_type rrb_guid_create()
      {
      return g_guid.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T, int N>
    inline void release(const transient_rrb<T, true, N>* p_node)
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
    inline void release(const transient_rrb<T, false, N>* p_node)
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

    template <typename T, int N>
    inline void addref(const transient_rrb<T, true, N>* p_node)
      {
      if (p_node)
        p_node->_ref_count.fetch_add(1, std::memory_order_relaxed);
      }

    template <typename T, int N>
    inline void addref(const transient_rrb<T, false, N>* p_node)
      {
      if (p_node)
        ++p_node->_ref_count;
      }

    template <int N, bool atomic_ref_counting>
    inline rrb_size_table<atomic_ref_counting>* transient_size_table_create()
      {
      rrb_size_table<atomic_ref_counting>* table = (rrb_size_table<atomic_ref_counting>*)malloc(sizeof(rrb_size_table<atomic_ref_counting>) + bits<N>::rrb_branching * sizeof(uint32_t));
      table->size = (uint32_t*)((char*)table + sizeof(rrb_size_table<atomic_ref_counting>));
      return table;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline transient_rrb<T, atomic_ref_counting, N>* transient_rrb_head_create(const rrb<T, atomic_ref_counting, N>* original)
      {
      transient_rrb<T, atomic_ref_counting, N>* trrb = (transient_rrb<T, atomic_ref_counting, N>*)malloc(sizeof(transient_rrb<T, atomic_ref_counting, N>));
      memcpy(trrb, original, sizeof(rrb<T, atomic_ref_counting, N>));
      trrb->root.inc();
      trrb->tail.inc();
      trrb->owner = std::this_thread::get_id();
      return trrb;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline leaf_node<T, atomic_ref_counting>* transient_leaf_node_create()
      {
      leaf_node<T, atomic_ref_counting>* leaf = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (bits<N>::rrb_branching) * sizeof(T));
      leaf->len = 0;
      leaf->type = LEAF_NODE;
      leaf->child = (T*)((char*)leaf + sizeof(leaf_node<T, atomic_ref_counting>));
      return leaf;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline internal_node<T, atomic_ref_counting>* transient_internal_node_create()
      {
      internal_node<T, atomic_ref_counting>* node = (internal_node<T, atomic_ref_counting>*)malloc(sizeof(internal_node<T, atomic_ref_counting>) + (bits<N>::rrb_branching) * sizeof(ref<internal_node<T, atomic_ref_counting>>));
      node->type = INTERNAL_NODE;
      node->child = (ref<internal_node<T, atomic_ref_counting>>*)((char*)node + sizeof(internal_node<T, atomic_ref_counting>));
      node->size_table.ptr = nullptr;
      node->len = 0;
      memset(node->child, 0, bits<N>::rrb_branching * sizeof(ref<internal_node<T, atomic_ref_counting>>)); // init pointers to zero      
      return node;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline internal_node<T, atomic_ref_counting>* transient_internal_node_clone(const internal_node<T, atomic_ref_counting>* original, guid_type guid)
      {
      internal_node<T, atomic_ref_counting>* node = transient_internal_node_create<T, atomic_ref_counting, N>();
      node->len = original->len;
      node->size_table = original->size_table;
      for (uint32_t i = 0; i < original->len; ++i)
        node->child[i] = original->child[i];
      node->guid = guid;
      return node;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline leaf_node<T, atomic_ref_counting>* transient_leaf_node_clone(const leaf_node<T, atomic_ref_counting>* original, guid_type guid)
      {
      leaf_node<T, atomic_ref_counting>* clone = (leaf_node<T, atomic_ref_counting>*)malloc(sizeof(leaf_node<T, atomic_ref_counting>) + (bits<N>::rrb_branching) * sizeof(T));
      memset(clone, 0, sizeof(leaf_node<T, atomic_ref_counting>) + (bits<N>::rrb_branching) * sizeof(T));
      clone->len = original->len;
      clone->type = LEAF_NODE;
      clone->child = (T*)((char*)clone + sizeof(leaf_node<T, atomic_ref_counting>));
      //memcpy(clone->child, original->child, original->len * sizeof(T));
      for (uint32_t i = 0; i < original->len; ++i)
        clone->child[i] = original->child[i]; // don't memcpy, but use copy constructor
      clone->guid = guid;
      return clone;
      }

    template <int N, bool atomic_ref_counting>
    inline rrb_size_table<atomic_ref_counting>* transient_size_table_clone(const rrb_size_table<atomic_ref_counting>* original, uint32_t len, guid_type guid)
      {
      rrb_size_table<atomic_ref_counting>* clone = (rrb_size_table<atomic_ref_counting>*)malloc(sizeof(rrb_size_table<atomic_ref_counting>) + bits<N>::rrb_branching * sizeof(uint32_t));
      clone->size = (uint32_t*)((char*)clone + sizeof(rrb_size_table<atomic_ref_counting>));
      memcpy(clone->size, original->size, sizeof(uint32_t) * len);
      clone->guid = guid;
      return clone;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline void ensure_internal_editable(ref<internal_node<T, atomic_ref_counting>>& internal, guid_type guid)
      {
      if (internal->guid != guid)
        {
        internal = transient_internal_node_clone<T, atomic_ref_counting, N>(internal.ptr, guid);
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline void ensure_size_table_editable(ref<rrb_size_table<atomic_ref_counting>>& table, uint32_t len, guid_type guid)
      {
      if (table->guid != guid)
        {
        table = transient_size_table_clone<N>(table.ptr, len, guid);
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline void ensure_leaf_editable(ref<leaf_node<T, atomic_ref_counting>>& leaf, guid_type guid)
      {
      if (leaf->guid != guid)
        {
        leaf = transient_leaf_node_clone<T, atomic_ref_counting, N>(leaf.ptr, guid);
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline void check_transience(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
      {
      if (trrb->guid == 0)
        throw std::runtime_error("transient used after transient to persistent call");
      if (trrb->owner != std::this_thread::get_id())
        throw std::runtime_error("transient used by non-owner thread");
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>>* new_editable_path(ref<internal_node<T, atomic_ref_counting>>* to_set, uint32_t empty_height, guid_type guid)
      {
      if (0 < empty_height)
        {
        internal_node<T, atomic_ref_counting>* leaf = transient_internal_node_create<T, atomic_ref_counting, N>();
        leaf->guid = guid;
        leaf->len = 1;

        ref<internal_node<T, atomic_ref_counting>> empty = leaf;
        for (uint32_t i = 1; i < empty_height; i++)
          {
          ref<internal_node<T, atomic_ref_counting>> new_empty = transient_internal_node_create<T, atomic_ref_counting, N>();
          new_empty->len = 1;
          new_empty->guid = guid;
          new_empty->child[0] = empty;
          empty = new_empty;
          }
        *to_set = empty;
        return &leaf->child[0];
        }
      else
        {
        return to_set;
        }
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline ref<internal_node<T, atomic_ref_counting>>* mutate_first_k(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, const uint32_t k)
      {
      guid_type guid = trrb->guid;
      ref<internal_node<T, atomic_ref_counting>> current = trrb->root;
      ref<internal_node<T, atomic_ref_counting>>* to_set = (ref<internal_node<T, atomic_ref_counting>>*)&trrb->root;
      uint32_t index = trrb->cnt - 2;
      uint32_t shift = trrb->shift;

      // mutate all non-leaf nodes first. Happens when shift > RRB_BRANCHING
      uint32_t i = 1;
      while (i <= k && shift != 0)
        {
        // First off, ensure current node is editable
        ensure_internal_editable<T, atomic_ref_counting, N>(current, guid);
        *to_set = current;

        if (i == k)
          {
          // increase width of node
          current->len++;
          }

        if (current->size_table.ptr != nullptr)
          {
          // Ensure size table is editable too
          ensure_size_table_editable<T, atomic_ref_counting, N>(current->size_table, current->len, guid);
          if (i != k)
            {
            // Tail will always be 32 long, otherwise we insert a single element only
            current->size_table->size[current->len - 1] += bits<N>::rrb_branching;
            }
          else { // increment size of last element -- will only happen if we append empties
            current->size_table->size[current->len - 1] = current->size_table->size[current->len - 2] + bits<N>::rrb_branching;
            }
          }

        // calculate child index
        uint32_t child_index;
        if (current->size_table.ptr == nullptr)
          {
          child_index = (index >> shift) & bits<N>::rrb_mask;
          }
        else {
          // no need for sized_pos here, luckily.
          child_index = current->len - 1;
          // Decrement index
          if (child_index != 0)
            {
            index -= current->size_table->size[child_index - 1];
            }
          }
        to_set = &current->child[child_index];
        auto temp = current->child[child_index];
        current = temp;

        i++;
        shift -= bits<N>::rrb_bits;
        }

      // check if we need to mutate the leaf node. Very likely to happen (31/32)
      if (i == k)
        {
        ref<leaf_node<T, atomic_ref_counting>> leaf = current;
        ensure_leaf_editable<T, atomic_ref_counting, N>(leaf, guid);
        leaf->len++;
        *to_set = leaf;
        }
      return to_set;
      }

    template <typename T, bool atomic_ref_counting, int N>
    inline void transient_promote_rightmost_leaf(ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
      {
      guid_type guid = trrb->guid;
      ref<internal_node<T, atomic_ref_counting>> current = trrb->root;

      ref<internal_node<T, atomic_ref_counting>> path[bits<N>::rrb_max_height + 1];
      path[0] = trrb->root;
      uint32_t i = 0, shift = 0;

      // populate path array
      for (i = 0, shift = 0; shift < trrb->shift; i++, shift += bits<N>::rrb_bits)
        {
        path[i + 1] = path[i]->child[path[i]->len - 1];
        }

      const uint32_t height = i;

      // Set leaf node as tail.
      trrb->tail = path[height];
      trrb->tail_len = path[height]->len;
      const uint32_t tail_len = trrb->tail_len;

      path[height] = ref<internal_node<T, atomic_ref_counting>>(nullptr);

      while (i-- > 0)
        {
        if (path[i + 1].ptr == nullptr && path[i]->len == 1)
          {
          path[i] = ref<internal_node<T, atomic_ref_counting>>(nullptr);
          }
        else if (path[i + 1].ptr == nullptr && i == 0 && path[0]->len == 2)
          {
          path[i] = path[i]->child[0];
          trrb->shift -= bits<N>::rrb_bits;
          }
        else
          {
          ensure_internal_editable<T, atomic_ref_counting, N>(path[i], guid);
          path[i]->child[path[i]->len - 1] = path[i + 1];
          if (path[i + 1].ptr == nullptr)
            {
            path[i]->len--;
            }
          if (path[i]->size_table.ptr != nullptr)
            { // this is decrement-size-table*
            ensure_size_table_editable<T, atomic_ref_counting, N>(path[i]->size_table, path[i]->len, guid);
            path[i]->size_table->size[path[i]->len - 1] -= tail_len;
            }
          }
        }

      trrb->root = path[0];
      }    
    
    } // namespace rrb_details

  template <typename T, bool atomic_ref_counting=true, int N = 5>
  struct transient_rrb
    {
    uint32_t cnt;
    uint32_t shift;
    uint32_t tail_len;
    ref<rrb_details::leaf_node<T, atomic_ref_counting>> tail;
    ref<rrb_details::tree_node<T, atomic_ref_counting>> root;
    mutable std::atomic<uint32_t> _ref_count;
    rrb_details::guid_type guid;
    std::thread::id owner;
    };

  template <typename T, int N>
  struct transient_rrb<T, false, N>
    {
    uint32_t cnt;
    uint32_t shift;
    uint32_t tail_len;
    ref<rrb_details::leaf_node<T, false>> tail;
    ref<rrb_details::tree_node<T, false>> root;
    mutable uint32_t _ref_count;
    rrb_details::guid_type guid;
    std::thread::id owner;
    };

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<transient_rrb<T, atomic_ref_counting, N>> rrb_to_transient(const ref<rrb<T, atomic_ref_counting, N>>& in)
    {
    using namespace rrb_details;
    ref<transient_rrb<T, atomic_ref_counting, N>> trrb = transient_rrb_head_create(in.ptr);
    trrb->guid = rrb_guid_create();
    trrb->tail = transient_leaf_node_clone<T, atomic_ref_counting, N>(in->tail.ptr, trrb->guid);
    return trrb;
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<rrb<T, atomic_ref_counting, N>> transient_to_rrb(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
    {
    using namespace rrb_details;
    trrb->guid = 0;
    ref<leaf_node<T, atomic_ref_counting>> leaf = leaf_node_clone<T, atomic_ref_counting>(trrb->tail.ptr);
    trrb->tail = leaf;
    ref<rrb<T, atomic_ref_counting, N>> out = rrb_head_clone((const rrb<T, atomic_ref_counting, N>*)trrb.ptr);
    return out;
    }

  // transient_rrb_update is effectively the same as rrb_update, but may mutate
  // nodes if it's safe to do so (replacing clone calls with ensure_editable
  // calls)
  template <typename T, bool atomic_ref_counting, int N>
  inline ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_update(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, uint32_t index, T element)
    {
    using namespace rrb_details;
    assert(index < trrb->cnt);
    check_transience(trrb);
    guid_type guid = trrb->guid;
    const uint32_t tail_offset = trrb->cnt - trrb->tail_len;
    if (tail_offset <= index)
      {
      trrb->tail->child[index - tail_offset] = std::move(element);
      return trrb;
      }
    ref<internal_node<T, atomic_ref_counting>>* previous_pointer = (ref<internal_node<T, atomic_ref_counting>>*)&trrb->root;
    ref<internal_node<T, atomic_ref_counting>> current = trrb->root;
    for (uint32_t shift = trrb->shift; shift > 0; shift -= bits<N>::rrb_bits)
      {
      ensure_internal_editable<T, atomic_ref_counting, N>(current, guid);
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
    ensure_leaf_editable<T, atomic_ref_counting, N>(leaf, guid);
    *previous_pointer = leaf;
    leaf->child[index & bits<N>::rrb_mask] = std::move(element);
    return trrb;
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_push(ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, T element)
    {
    using namespace rrb_details;
    check_transience(trrb);
    if (trrb->tail_len < bits<N>::rrb_branching)
      {
      T* loc = (T*)((char*)trrb->tail->child + trrb->tail_len * sizeof(T));
      loc = new(loc) T(std::move(element)); // placement new      

      //trrb->tail->child[trrb->tail_len] = std::move(element);
      trrb->cnt++;
      trrb->tail_len++;
      trrb->tail->len++;
      // ^ consider deferring incrementing this until insertion and/or persistentified.
      return trrb;
      }

    trrb->cnt++;
    guid_type guid = trrb->guid;
    ref<leaf_node<T, atomic_ref_counting>> new_tail = transient_leaf_node_create<T, atomic_ref_counting, N>();
    new_tail->guid = guid;
    
    T* loc = (T*)((char*)new_tail->child);
    loc = new(loc) T(std::move(element)); // placement new   

    //new_tail->child[0] = std::move(element);
    new_tail->len = 1;
    trrb->tail_len = 1;

    ref<leaf_node<T, atomic_ref_counting>> old_tail = trrb->tail;
    trrb->tail = new_tail;

    if (trrb->root.ptr == nullptr)
      { // If it's  null, we can't just mutate it down.
      trrb->shift = 0;
      trrb->root = old_tail;
      return trrb;
      }
    // mutable count starts here

    // TODO: Can find last rightmost jump in constant time for pvec subvecs:
    // use the fact that (index & large_mask) == 1 << (RRB_BITS * H) - 1 -> 0 etc.

    uint32_t index = trrb->cnt - 2;

    uint32_t nodes_to_mutate = 0;
    uint32_t nodes_visited = 0;
    uint32_t pos = 0; // pos is the position we insert empty nodes in the bottom
                      // mutable node (or the element, if we can mutate the leaf)
    ref<internal_node<T, atomic_ref_counting>> current = trrb->root;

    uint32_t shift = trrb->shift;

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
          goto mutable_count_end;
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
        nodes_to_mutate = nodes_visited;
        pos = child_index;
        }

      current = current->child[child_index];
      // This will only happen in a pvec subtree
      if (current.ptr == nullptr)
        {
        nodes_to_mutate = nodes_visited;
        pos = child_index;

        // if next element we're looking at is null, we can mutate all above. Good
        // times.
        goto mutable_count_end;
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
        nodes_to_mutate = nodes_visited;
        pos = current->len;
        }
      }

  mutable_count_end:
    // GURRHH, nodes_visited is not yet handled nicely. loop down to get
    // nodes_visited set straight.
    while (shift > bits<N>::rrb_bits)
      {
      nodes_visited++;
      shift -= bits<N>::rrb_bits;
      }

    // Increasing height of tree.
    if (nodes_to_mutate == 0)
      {
      ref<internal_node<T, atomic_ref_counting>> old_root = trrb->root;
      ref<internal_node<T, atomic_ref_counting>> new_root = transient_internal_node_create<T, atomic_ref_counting, N>();
      new_root->guid = guid;
      new_root->len = 2;
      new_root->child[0] = trrb->root;
      trrb->root = new_root;
      trrb->shift = trrb->shift + bits<N>::rrb_bits;

      // create size table if the original rrb root has a size table.
      if (old_root->type != LEAF_NODE && ((const internal_node<T, atomic_ref_counting> *)old_root.ptr)->size_table.ptr != nullptr)
        {
        rrb_size_table<atomic_ref_counting>* table = transient_size_table_create<N, atomic_ref_counting>();
        table->guid = guid;
        table->size[0] = trrb->cnt - (old_tail->len + 1);
        // If we insert the tail, the old size minus (new size minus one) the old
        // tail size will be the amount of elements in the left branch. If there
        // is no tail, the size is just the old rrb-tree.

        table->size[1] = trrb->cnt - 1;
        // If we insert the tail, the old size would include the tail.
        // Consequently, it has to be the old size. If we have no tail, we append
        // a single element to the old vector, therefore it has to be one more
        // than the original (which means it is zero)

        new_root->size_table = table;
        }

      // nodes visited == original rrb tree height. Nodes visited > 0.
      ref<internal_node<T, atomic_ref_counting>>* to_set = new_editable_path<T, atomic_ref_counting, N>(&((internal_node<T, atomic_ref_counting> *)trrb->root.ptr)->child[1], nodes_visited, guid);
      *to_set = old_tail;
      }
    else
      {
      ref<internal_node<T, atomic_ref_counting>>* node = mutate_first_k(trrb, nodes_to_mutate);
      ref<internal_node<T, atomic_ref_counting>>* to_set = new_editable_path<T, atomic_ref_counting, N>(node, nodes_visited - nodes_to_mutate, guid);
      *to_set = old_tail;
      }

    return trrb;
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline const T& transient_rrb_nth(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb, uint32_t index)
    {
    using namespace rrb_details;
    check_transience(trrb);
    return rrb_nth((const ref<rrb<T, atomic_ref_counting, N>>&)trrb, index);
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline const T& transient_rrb_peek(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
    {
    using namespace rrb_details;
    check_transience(trrb);
    return rrb_peek((const ref<rrb<T, atomic_ref_counting, N>>&)trrb);
    }

  template <typename T, bool atomic_ref_counting, int N>
  inline uint32_t transient_rrb_count(const ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
    {
    using namespace rrb_details;
    check_transience(trrb);
    return rrb_count((const ref<rrb<T, atomic_ref_counting, N>>&)trrb);
    }

  template <typename T, bool atomic_ref_counting, int N>
  ref<transient_rrb<T, atomic_ref_counting, N>> transient_rrb_pop(ref<transient_rrb<T, atomic_ref_counting, N>>& trrb)
    {
    using namespace rrb_details;
    check_transience(trrb);
    if (trrb->cnt == 1)
      {
      trrb->cnt = 0;
      trrb->tail_len = 0;
      trrb->tail->child[0].~T();
      trrb->tail->len = 0;
      return trrb;
      }
    trrb->cnt--;

    if (trrb->tail_len == 1)
      {
      transient_promote_rightmost_leaf(trrb);
      return trrb;
      }
    else
      {
      trrb->tail->child[trrb->tail_len - 1].~T();
      trrb->tail_len--;
      trrb->tail->len--;

      return trrb;
      }
    }
  }