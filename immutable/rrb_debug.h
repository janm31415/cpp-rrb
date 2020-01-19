
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

namespace immutable
  {

  template <typename T, bool atomic_ref_counting, int N>
  bool validate_rrb(const ref<rrb<T, atomic_ref_counting, N>>& rrb);

  namespace rrb_details
    {

    template <typename T, bool atomic_ref_counting, int N>
    bool validate_subtree(const ref<tree_node<T, atomic_ref_counting>>& root, uint32_t expected_size, uint32_t root_shift)
      {
      if (root_shift == 0)
        { // leaf node
        if (root->type != LEAF_NODE)
          {
          printf("Expected this node to be a leaf node, but it claims to be something else.\n");
          printf("Will treat it like a leaf node, so may segfault.\n");
          return false;
          }
        ref<leaf_node<T, atomic_ref_counting>> leaf = root;
        if (leaf->len != expected_size)
          {
          printf("Leaf node claims to be %u elements long, but was expected to be %u\n elements long. Will attempt to read %u elements.\n",
            leaf->len, expected_size, std::max<uint32_t>(leaf->len, expected_size));
          return false;
          }
        }
      else
        {
        if (root->type != INTERNAL_NODE)
          {
          printf("Expected this node to be an internal node, but it claims to be something else.\n");
          printf("Will treat it like an internal node, so may segfault.\n");
          return false;
          }
        ref<internal_node<T, atomic_ref_counting>> internal = root;
        if (internal->size_table.ptr != nullptr)
          {
          // expected size should be consistent with what's in the last size table
          // slot
          if (internal->size_table->size[internal->len - 1] != expected_size)
            {
            printf("Expected subtree to be of size %u, but its size table says it is %u.\n", expected_size,
              internal->size_table->size[internal->len - 1]);
            return false;
            }
          for (uint32_t i = 0; i < internal->len; i++)
            {
            uint32_t size_sub_trie = internal->size_table->size[i] - (i == 0 ? 0 : internal->size_table->size[i - 1]);
            ref<tree_node<T, atomic_ref_counting>> child = internal->child[i];
            if (!validate_subtree<T, atomic_ref_counting, N>(child, size_sub_trie, root_shift - bits<N>::rrb_bits))
              return false;
            }
          }
        else
          {
          // internal->size_table == NULL
          // this tree may contain at most (internal->len << shift) elements, not
          // more. Effectively, the tree contains (len - 1) << shift + last_tree_len
          // (1 << shift) >= last_tree_len > 0
          const uint32_t child_shift = root_shift - bits<N>::rrb_bits;
          const uint32_t child_max_size = 1 << root_shift;

          if (expected_size > internal->len * child_max_size)
            {
            printf("Expected size (%u) is larger than what can possibly be inside this subtree: %u.\n", expected_size,
              internal->len * child_max_size);
            return false;
            }
          else if (expected_size < ((internal->len - 1) * child_max_size))
            {
            printf("Expected size (%u) is smaller than %u, implying that some non-rightmost node\n is not completely populated.\n",
              expected_size, ((internal->len - 1) << root_shift));
            return false;
            }
          for (uint32_t i = 0; i < internal->len - 1; i++)
            {
            ref<tree_node<T, atomic_ref_counting>> child = internal->child[i];
            if (!validate_subtree<T, atomic_ref_counting, N>(child, child_max_size, child_shift))
              return false;
            }
          ref<tree_node<T, atomic_ref_counting>> child = internal->child[internal->len - 1];
          if (!validate_subtree<T, atomic_ref_counting, N>(child, expected_size - ((internal->len - 1) * child_max_size), child_shift))
            return false;
          }
        }
      return true;
      }

    }

  template <typename T, bool atomic_ref_counting, int N>
  bool validate_rrb(const ref<rrb<T, atomic_ref_counting, N>>& rrb)
    {
    using namespace rrb_details;
    // ensure the rrb tree is consistent      
    // the rrb tree should always have a tail
    if (rrb->tail->len != rrb->tail_len)
      {
      printf("The tail of this rrb-tree says it is of length %u, but the rrb head claims it\nis %u elements long.", rrb->tail->len, rrb->tail_len);
      return false;
      }
    else
      {
      ref<tree_node<T, atomic_ref_counting>> tail = rrb->tail;
      if (!validate_subtree<T, atomic_ref_counting, N>(tail, rrb->tail_len, 0))
        return false;
      }
    if (rrb->root.ptr == nullptr)
      {
      if (rrb->cnt - rrb->tail_len != 0)
        {
        printf("Root is null, but the size of the vector (excluding its tail) is %u.\n", rrb->cnt - rrb->tail_len);
        return false;
        }
      }
    else
      {
      if (!validate_subtree<T, atomic_ref_counting, N>(rrb->root, rrb->cnt - rrb->tail_len, rrb->shift))
        return false;
      }
    return true;
    }

  }