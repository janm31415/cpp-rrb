#include "rrb_tests.h"
#include "test_assert.h"
#include <iostream>
#include <immutable/rrb.h>
#include <immutable/rrb_debug.h>
#include <immutable/rrb_transient.h>
#include <vector>

namespace immutable
  {

  void test_size_table_construction()
    {
    using namespace rrb_details;

    ref<rrb_size_table<true>> p_table = size_table_create<true>(5);

    for (uint32_t i = 0; i < 5; ++i)
      {
      p_table->size[i] = i;
      TEST_EQ(i, p_table->size[i]);
      }

    ref<rrb_size_table<true>> p_clone = size_table_clone<true>(p_table.ptr, 5);
    for (uint32_t i = 0; i < 5; ++i)
      {
      TEST_EQ(i, p_clone->size[i]);
      }

    ref<rrb_size_table<true>> p_inc = size_table_inc<true>(p_table.ptr, 5);
    for (uint32_t i = 0; i < 5; ++i)
      {
      TEST_EQ(i, p_inc->size[i]);
      }
    p_inc->size[5] = 5;
    TEST_EQ(5, p_inc->size[5]);
    }

  void test_empty_rrb()
    {
    ref<rrb<char>> tree = rrb_create<char>();
    TEST_EQ(0, tree->cnt);
    }


  void test_push_tail()
    {
    ref<rrb<char>> tree = rrb_create<char>();

    ref<rrb<char>> pushed_tree_a = rrb_push(tree, 'a');
    ref<rrb<char>> pushed_tree_b = rrb_push(pushed_tree_a, 'b');
    ref<rrb<char>> pushed_tree_c = rrb_push(pushed_tree_b, 'c');

    TEST_EQ('a', rrb_nth(pushed_tree_a, 0));
    TEST_EQ('a', rrb_nth(pushed_tree_b, 0));
    TEST_EQ('a', rrb_nth(pushed_tree_c, 0));
    TEST_EQ('b', rrb_nth(pushed_tree_b, 1));
    TEST_EQ('b', rrb_nth(pushed_tree_c, 1));
    TEST_EQ('c', rrb_nth(pushed_tree_c, 2));
    }

  void test_push_tail2()
    {
    ref<rrb<char>> tree = rrb_create<char>();

    ref<rrb<char>> new_tree;
    for (int i = 0; i < 50; ++i)
      {
      new_tree = rrb_push(tree, (char)(i + 32));
      tree = new_tree;
      }

    for (int i = 0; i < 50; ++i)
      {
      char ch = rrb_nth(tree, i);
      TEST_EQ((char)(i + 32), ch);
      }
    }

  void test_internal_node()
    {
    using namespace rrb_details;

    ref<internal_node<char, true>> in = internal_node_create<char, true>(8);

    in->child[0] = internal_node_create<char, true>(5);
    TEST_ASSERT(in->_ref_count == 1);
    TEST_ASSERT(in->len == 8);

    ref<internal_node<char, true>> copy = internal_node_clone(in.ptr);

    TEST_ASSERT(copy->_ref_count == 1);
    TEST_ASSERT(copy->len == 8);
    TEST_ASSERT(in->child[0]->_ref_count == 2);

    ref<internal_node<char, true>> inc = internal_node_inc(in.ptr);
    TEST_ASSERT(inc->_ref_count == 1);
    TEST_ASSERT(inc->len == 9);
    TEST_ASSERT(in->child[0]->_ref_count == 3);

    ref<internal_node<char, true>> dec = internal_node_dec(in.ptr);
    TEST_ASSERT(dec->_ref_count == 1);
    TEST_ASSERT(dec->len == 7);
    TEST_ASSERT(in->child[0]->_ref_count == 4);
    }

  void test_push_tail3()
    {

    ref<rrb<char>> tree = rrb_create<char>();

    ref<rrb<char>> new_tree;
    for (int i = 0; i < 80; ++i)
      {
      new_tree = rrb_push(tree, (char)(i + 32));
      tree = new_tree;
      }

    for (int i = 0; i < 80; ++i)
      {
      char ch = rrb_nth(tree, i);
      TEST_EQ((char)(i + 32), ch);
      }
    }

  void test_push_tail4()
    {

    ref<rrb<char>> tree = rrb_create<char>();

    ref<rrb<char>> new_tree;
    for (int i = 0; i < 8000; ++i)
      {
      new_tree = rrb_push(tree, (char)((i % 90) + 32));
      tree = new_tree;
      }

    for (int i = 0; i < 8000; ++i)
      {
      char ch = rrb_nth(tree, i);
      TEST_EQ((char)((i % 90) + 32), ch);
      }

    }

  void test_push(uint32_t sz = 40000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (auto& v : list)
      {
      tree = rrb_push(tree, v);
      }

    for (uint32_t i = 0; i < sz; ++i)
      TEST_EQ(list[i], rrb_nth(tree, i));

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_peek(uint32_t sz = 40000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (auto& v : list)
      {
      tree = rrb_push(tree, v);
      int top = rrb_peek(tree);
      TEST_EQ(v, top);
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_count(uint32_t sz = 40000)
    {

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, 5);
      TEST_EQ(i + 1, rrb_count(tree));
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_pop(uint32_t sz = 40000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, list[i]);

      if (i > 0)
        {
        ref<rrb<int>> prev_tree = rrb_pop(tree);
        int val = rrb_peek(prev_tree);
        TEST_EQ(val, list[i - 1]);
        }
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_update(uint32_t sz = 400000, uint32_t updates = 133337)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, list[i]);
      }

    std::vector<int> updated_list(updates);
    std::vector<uint32_t> lookups(updates);
    for (uint32_t i = 0; i < updates; ++i)
      {
      updated_list[i] = rand();
      lookups[i] = (uint32_t)(rand() % sz);
      }

    for (uint32_t i = 0; i < updates; ++i)
      {
      const uint32_t idx = lookups[i];
      ref<rrb<int>> old_tree = tree;
      tree = rrb_update(tree, idx, updated_list[i]);
      int old_val = rrb_nth(old_tree, idx);
      int new_val = rrb_nth(tree, idx);
      TEST_EQ(old_val, list[idx]);
      TEST_EQ(new_val, updated_list[i]);
      list[idx] = new_val;
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_drop_left(uint32_t sz = 40000, uint32_t slice_pos = 1000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, list[i]);
      }

    tree = rrb_drop_left(tree, slice_pos);
    TEST_EQ(sz - slice_pos, tree->cnt);
    for (uint32_t i = slice_pos; i < sz; ++i)
      {
      TEST_EQ(list[i], rrb_nth(tree, i - slice_pos));
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_drop_right(uint32_t sz = 40000, uint32_t slice_pos = 1000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, list[i]);
      }

    tree = rrb_drop_right(tree, slice_pos);
    TEST_EQ(slice_pos, tree->cnt);
    for (uint32_t i = 0; i < slice_pos; ++i)
      {
      TEST_EQ(list[i], rrb_nth(tree, i));
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_slice(uint32_t sz = 40000, uint32_t slices = 10000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = rrb_push(tree, list[i]);
      }

    std::vector<int> from_list(slices);
    std::vector<int> to_list(slices);
    for (uint32_t i = 0; i < slices; ++i)
      {
      from_list[i] = rand() % sz;
      to_list[i] = (rand() % (sz - from_list[i])) + from_list[i];
      }

    for (uint32_t i = 0; i < slices; ++i)
      {
      ref<rrb<int>> sliced = rrb_slice(tree, from_list[i], to_list[i]);
      TEST_ASSERT(validate_rrb(sliced));
      for (uint32_t j = 0; j < rrb_count(sliced); ++j)
        {
        int sliced_val = rrb_nth(sliced, j);
        int original_val = rrb_nth(tree, j + from_list[i]);
        TEST_EQ(original_val, sliced_val);
        }
      }
    }

  void test_concat(uint32_t sz = 10000)
    {

    ref<rrb<int>> rrb1 = rrb_create<int>();
    ref<rrb<int>> rrb2 = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      {
      rrb1 = rrb_push(rrb1, (int)rand());
      rrb2 = rrb_push(rrb2, (int)rand());

      ref<rrb<int>> catted = rrb_concat(rrb1, rrb2);

      TEST_ASSERT(validate_rrb(catted));

      for (uint32_t j = 0; j < (i + 1) * 2; j++)
        {
        int val_cat = rrb_nth(catted, j);
        if (j <= i)
          {
          int val1 = rrb_nth(rrb1, j);
          TEST_EQ(val1, val_cat);
          }
        else
          {
          int val2 = rrb_nth(rrb2, j - i - 1);
          TEST_EQ(val2, val_cat);
          }
        }
      }
    }


  void test_catslice(uint32_t sz = 400, uint32_t sliced = 53, uint32_t catted = 2310, uint32_t tot_catted = 10)
    {
    ref<rrb<int>> tree = rrb_create<int>();
    for (uint32_t i = 0; i < sz; ++i)
      tree = rrb_push(tree, (int)rand() % 10000);

    std::vector<ref<rrb<int>>> sliced_rrb(sliced);

    for (uint32_t i = 0; i < sliced; ++i)
      {
      uint32_t from = (uint32_t)rand() % sz;
      uint32_t to = (uint32_t)(rand() % (sz - from)) + from;
      sliced_rrb[i] = rrb_slice(tree, from, to);
      TEST_ASSERT(validate_rrb(sliced_rrb[i]));
      }

    for (uint32_t i = 0; i < catted; ++i)
      {
      uint32_t tot_cats = (uint32_t)rand() % tot_catted;
      ref<rrb<int>> multicat = rrb_create<int>();
      std::vector< ref<rrb<int>>> catsteps(tot_cats);
      std::vector<uint32_t> merged_in(tot_cats);
      for (uint32_t cat_num = 0; cat_num < tot_cats; ++cat_num)
        {
        merged_in[cat_num] = (uint32_t)rand() % sliced;
        multicat = rrb_concat(multicat, sliced_rrb[merged_in[cat_num]]);

        catsteps[cat_num] = multicat;
        TEST_ASSERT(validate_rrb(multicat));
        }

      // checking consistency here
      uint32_t pos = 0;
      uint32_t merged_pos = 0;
      while (pos < rrb_count(multicat))
        {
        ref<rrb<int>> merged = sliced_rrb[merged_in[merged_pos]];

        for (uint32_t merged_i = 0; merged_i < rrb_count(merged); merged_i++, pos++)
          {
          int expected = rrb_nth(merged, merged_i);
          int actual = rrb_nth(multicat, pos);
          TEST_EQ(expected, actual);
          }
        merged_pos++;
        }
      }
    }

  void test_fibocat(uint32_t rrb_counter = 2600, uint32_t predef_rrbs = 200)
    {

    uint32_t max_init_size = 16;

    std::vector<ref<rrb<int>>> rrbs(rrb_counter);
    for (uint32_t i = 0; i < predef_rrbs; ++i)
      {
      const uint32_t local_sz = (rand() % max_init_size);
      rrbs[i] = rrb_create<int>();
      for (uint32_t j = 0; j < local_sz; ++j)
        {
        rrbs[i] = rrb_push(rrbs[i], rand() & 0xffff);
        }
      TEST_ASSERT(validate_rrb(rrbs[i]));
      }

    for (uint32_t i = predef_rrbs; i < rrb_counter; ++i)
      rrbs[i] = rrb_concat(rrbs[i - predef_rrbs], rrbs[i - predef_rrbs + 1]);

    for (uint32_t i = predef_rrbs; i < rrb_counter; ++i)
      {
      ref<rrb<int>> merged = rrbs[i];
      ref<rrb<int>> left = rrbs[i - predef_rrbs];
      ref<rrb<int>> right = rrbs[i - predef_rrbs + 1];
      uint32_t merged_idx = 0;
      for (uint32_t left_idx = 0; left_idx < rrb_count(left); left_idx++, merged_idx++)
        {
        int expected = rrb_nth(left, left_idx);
        int actual = rrb_nth(merged, merged_idx);
        TEST_EQ(expected, actual);
        }

      for (uint32_t right_idx = 0; right_idx < rrb_count(right); right_idx++, merged_idx++)
        {
        int expected = rrb_nth(right, right_idx);
        int actual = rrb_nth(merged, merged_idx);
        TEST_EQ(expected, actual);
        }
      }
    }

  void test_transient_push(uint32_t sz = 13000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<transient_rrb<int>> tree = rrb_to_transient(rrb_create<int>());
    for (uint32_t i = 0; i < sz; ++i)
      {
      tree = transient_rrb_push(tree, list[i]);
      for (uint32_t j = 0; j <= i; ++j)
        {
        int val = transient_rrb_nth(tree, j);
        TEST_EQ(list[j], val);
        }
      }
    }

  void test_transient_pop(uint32_t sz = 400000)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<transient_rrb<int>> trrb = rrb_to_transient(rrb_create<int>());

    for (uint32_t i = 0; i < sz; ++i)
      {
      trrb = transient_rrb_push(trrb, list[i]);
      }
    ref<rrb<int>> tree = transient_to_rrb(trrb);
    trrb = rrb_to_transient(tree);
    for (uint32_t i = sz; i > 0; --i)
      {
      int val = transient_rrb_peek(trrb);
      TEST_EQ(list[i - 1], val);
      trrb = transient_rrb_pop(trrb);
      }
    }

  void test_transient_update(uint32_t sz = 400000, uint32_t updates = 133337)
    {

    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    ref<transient_rrb<int>> trrb = rrb_to_transient(rrb_create<int>());
    for (uint32_t i = 0; i < sz; ++i)
      {
      trrb = transient_rrb_push(trrb, list[i]);
      }
    ref<rrb<int>> tree = transient_to_rrb(trrb);

    std::vector<int> updated_list(updates);
    std::vector<uint32_t> lookups(updates);
    for (uint32_t i = 0; i < updates; ++i)
      {
      updated_list[i] = rand();
      lookups[i] = (uint32_t)(rand() % sz);
      }

    trrb = rrb_to_transient(tree);

    for (uint32_t i = 0; i < updates; ++i)
      {
      const uint32_t idx = lookups[i];
      trrb = transient_rrb_update(trrb, idx, updated_list[i]);
      int old_val = rrb_nth(tree, idx);
      int new_val = transient_rrb_nth(trrb, idx);
      TEST_EQ(old_val, list[idx]);
      TEST_EQ(new_val, updated_list[i]);
      }

    TEST_ASSERT(validate_rrb(tree));
    }

  void test_transient_push_2(uint32_t tests = 400, uint32_t sz = 40, uint32_t max_extra_pushes = 50)
    {

    for (uint32_t t = 0; t < tests; ++t)
      {
      uint32_t extra_pushes = 32 + rand() % max_extra_pushes;
      uint32_t rrb_size = sz % rand();

      std::vector<int> list(rrb_size);
      std::vector<int> extra_vals(extra_pushes);

      for (uint32_t i = 0; i < rrb_size; ++i)
        list[i] = rand() % 0x1000;

      for (uint32_t i = 0; i < extra_pushes; ++i)
        extra_vals[i] = rand() % 0x1000;

      uint32_t rand_cut = rand() % rrb_size;
      ref<rrb<int>> left = rrb_create<int>();
      ref<rrb<int>> right = rrb_create<int>();

      ref<transient_rrb<int>> tmp = rrb_to_transient(left);
      for (uint32_t i = 0; i < rand_cut; i++) 
        {
        transient_rrb_push(tmp, list[i]);
        }
      left = transient_to_rrb(tmp);

      tmp = rrb_to_transient(right);
      for (uint32_t i = 0; i < rrb_size - rand_cut; i++) 
        {
        transient_rrb_push(tmp, list[i + rand_cut]);
        }
      right = transient_to_rrb(tmp);

      ref<rrb<int>> cat = rrb_concat(left, right);

      ref<transient_rrb<int>> trrb = rrb_to_transient(cat);
      for (uint32_t i = 0; i < extra_pushes; i++) 
        {
        trrb = transient_rrb_push(trrb, extra_vals[i]);
        }

      TEST_EQ(extra_pushes + rrb_size, transient_rrb_count(trrb));

      for (uint32_t i = 0; i < rrb_size; i++) 
        {
        int actual = transient_rrb_nth(trrb, i);
        int in_cat = rrb_nth(cat, i);
        int expected = list[i];
        TEST_EQ(expected, actual);
        TEST_EQ(in_cat, actual);
        }

      for (uint32_t i = 0; i < extra_pushes; i++) 
        {
        int actual = transient_rrb_nth(trrb, i + rrb_size);
        int expected = extra_vals[i];
        TEST_EQ(expected, actual);
        }

      ref<rrb<int>> cat_pushed = transient_to_rrb(trrb);
      TEST_ASSERT(validate_rrb(cat_pushed));

      uint32_t left_offset, right_offset;
      left_offset = (uint32_t)rand() % rrb_size;
      right_offset = (uint32_t)(rand() % (rrb_size - left_offset)) + left_offset;

      ref<rrb<int>> slice = rrb_slice(cat, left_offset, right_offset);

      trrb = rrb_to_transient(slice);
      for (uint32_t i = 0; i < extra_pushes; i++) 
        {
        trrb = transient_rrb_push(trrb, extra_vals[i]);
        }

      for (uint32_t i = 0; i < rrb_count(slice); i++) 
        {
        int actual = transient_rrb_nth(trrb, i);
        int in_slice = rrb_nth(slice, i);
        int expected = list[i + left_offset];
        TEST_EQ(expected, actual);
        TEST_EQ(in_slice, actual);
        }

      for (uint32_t i = 0; i < extra_pushes; i++) 
        {
        int actual = transient_rrb_nth(trrb, i + rrb_count(slice));
        int expected = extra_vals[i];
        TEST_EQ(expected, actual);
        }

      ref<rrb<int>> slice_pushed = transient_to_rrb(trrb);
      TEST_ASSERT(validate_rrb(slice_pushed));
      }
    }
  }

void run_all_rrb_tests()
  {
  using namespace immutable;
  
  test_size_table_construction();
  test_empty_rrb();
  test_push_tail();
  test_push_tail2();
  test_internal_node();
  test_push_tail3();
  test_push_tail4();
  test_push(20);
  test_peek(20);
  test_count(20);
  test_pop(20);
  test_update(20, 10);
  test_drop_left(20, 5);
  test_drop_right(20, 5);
  test_slice(20, 5);
  test_concat(20);
  test_transient_push(20);
  test_transient_pop(20);

  test_push(200);
  test_peek(200);
  test_count(200);
  test_pop(200);
  test_update(200, 100);
  test_drop_left(200, 50);
  test_drop_right(200, 50);
  test_slice(200, 50);
  test_concat(200);
  test_transient_push(200);
  test_transient_pop(200);

  test_push();
  test_peek();
  test_count();
  test_pop();
  test_update();
  test_drop_left();
  test_drop_right();

  test_catslice();
  test_fibocat();
  
  test_transient_push();
  test_transient_pop();

#ifdef NDEBUG
  test_slice();
  test_concat();
#endif

  test_transient_update();
  test_transient_push_2();
  }