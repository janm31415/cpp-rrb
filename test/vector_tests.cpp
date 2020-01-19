#include "vector_tests.h"
#include "test_assert.h"
#include <iostream>
#include <immutable/vector.h>
#include <vector>

namespace
  {

  template <bool atomic_ref_counting, int N>
  void test_empty_vector()
    {
    immutable::vector<int, atomic_ref_counting, N> v;
    TEST_ASSERT(v.empty());
    TEST_EQ(0, v.size());
    }

  template <bool atomic_ref_counting, int N>
  void test_push_back_simple()
    {
    immutable::vector<int, atomic_ref_counting, N> v;
    auto v3 = v.push_back(3);
    auto v37 = v3.push_back(7);
    TEST_ASSERT(v.empty());
    TEST_EQ(0, v.size());
    TEST_EQ(1, v3.size());
    TEST_EQ(2, v37.size());
    TEST_EQ(3, v3[0]);
    TEST_EQ(3, v37[0]);
    TEST_EQ(7, v37[1]);
    }

  template <bool atomic_ref_counting, int N>
  void test_push_back(uint32_t sz = 40000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (auto& v : list)
      {
      vec = vec.push_back(v);
      }

    for (uint32_t i = 0; i < sz; ++i)
      TEST_EQ(list[i], vec[i]);
    }

  template <bool atomic_ref_counting, int N>
  void test_front(uint32_t sz = 40000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (auto& v : list)
      {
      vec = vec.push_back(v);
      int top = vec.front();
      TEST_EQ(list[0], top);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_back(uint32_t sz = 40000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (auto& v : list)
      {
      vec = vec.push_back(v);
      int top = vec.back();
      TEST_EQ(v, top);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_size(uint32_t sz = 40000)
    {
    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(5);
      TEST_EQ(i + 1, vec.size());
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_pop_back(uint32_t sz = 40000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);

      if (i > 0)
        {
        immutable::vector<int, atomic_ref_counting, N> prev_vec = vec.pop_back();
        int val = prev_vec.back();
        TEST_EQ(val, list[i - 1]);
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_update(uint32_t sz = 400000, uint32_t updates = 133337)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
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
      immutable::vector<int, atomic_ref_counting, N> old_vec = vec;
      vec = vec.set(idx, updated_list[i]);
      int old_val = old_vec[idx];
      int new_val = vec[idx];
      TEST_EQ(old_val, list[idx]);
      TEST_EQ(new_val, updated_list[i]);
      list[idx] = new_val;
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_erase_single(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    list.erase(list.begin() + sz / 2);

    vec = vec.erase(sz / 2);

    TEST_EQ(vec.size(), list.size());

    for (uint32_t i = 0; i < sz - 1; ++i)
      TEST_EQ(list[i], vec[i]);
    }

  template <bool atomic_ref_counting, int N>
  void test_erase(uint32_t sz = 10000, uint32_t slices = 2500)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    std::vector<uint32_t> from_list(slices);
    std::vector<uint32_t> to_list(slices);
    for (uint32_t i = 0; i < slices; ++i)
      {
      from_list[i] = rand() % sz;
      to_list[i] = (rand() % (sz - from_list[i])) + from_list[i];
      }

    for (uint32_t i = 0; i < slices; ++i)
      {
      immutable::vector<int, atomic_ref_counting, N> erased = vec.erase(from_list[i], to_list[i]);
      for (uint32_t j = 0; j < from_list[i]; ++j)
        {
        int sliced_val = erased[j];
        int original_val = vec[j];
        TEST_EQ(original_val, sliced_val);
        }
      for (uint32_t j = to_list[i]; j < sz; ++j)
        {
        int sliced_val = erased[j - to_list[i] + from_list[i]];
        int original_val = vec[j];
        TEST_EQ(original_val, sliced_val);
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_insert_single(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    list.insert(list.begin() + sz / 2, 666);

    vec = vec.insert(sz / 2, 666);

    TEST_EQ(vec.size(), list.size());

    for (uint32_t i = 0; i < sz + 1; ++i)
      TEST_EQ(list[i], vec[i]);
    }

  template <bool atomic_ref_counting, int N>
  void test_concat(uint32_t sz = 2000)
    {
    immutable::vector<int, atomic_ref_counting, N> v1;
    immutable::vector<int, atomic_ref_counting, N> v2;
    for (uint32_t i = 0; i < sz; ++i)
      {
      v1 = v1.push_back((int)rand());
      v2 = v2.push_back((int)rand());

      immutable::vector<int, atomic_ref_counting, N> catted = v1 + v2;

      for (uint32_t j = 0; j < (i + 1) * 2; j++)
        {
        int val_cat = catted[j];
        if (j <= i)
          {
          int val1 = v1[j];
          TEST_EQ(val1, val_cat);
          }
        else
          {
          int val2 = v2[j - i - 1];
          TEST_EQ(val2, val_cat);
          }
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_drop(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    vec = vec.drop(3);

    for (uint32_t i = 0; i < sz - 3; ++i)
      TEST_EQ(list[i + 3], vec[i]);

    TEST_EQ(sz - 3, vec.size());
    }

  template <bool atomic_ref_counting, int N>
  void test_take(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    vec = vec.take(3);

    for (uint32_t i = 0; i < 3; ++i)
      TEST_EQ(list[i], vec[i]);

    TEST_EQ(3, vec.size());
    }

  template <bool atomic_ref_counting, int N>
  void test_iterator(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = vec.begin();
    auto it_end = vec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_iterator_2(uint32_t sz = 1000)
    {
    std::vector<int> list1(sz), list2(sz), list3(sz), list4(sz);
    for (auto& v : list1)
      v = rand();
    for (auto& v : list2)
      v = rand();
    for (auto& v : list3)
      v = rand();
    for (auto& v : list4)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec1, vec2, vec3, vec4;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec1 = vec1.push_back(list1[i]);
      vec2 = vec2.push_back(list2[i]);
      vec3 = vec3.push_back(list3[i]);
      vec4 = vec4.push_back(list4[i]);
      }

    std::vector<int> list;
    list.insert(list.end(), list1.begin(), list1.end());
    list.insert(list.end(), list2.begin(), list2.end());
    list.insert(list.end(), list3.begin(), list3.end());
    list.insert(list.end(), list4.begin(), list4.end());

    immutable::vector<int, atomic_ref_counting, N> vec = vec1 + vec2 + vec3 + vec4;

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = vec.begin();
    auto it_end = vec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_iterator_3(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    list.erase(list.begin() + sz / 4, list.begin() + sz / 2);
    vec = vec.erase(sz / 4, sz / 2);

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = vec.begin();
    auto it_end = vec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_reverse_iterator(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    auto std_it = list.rbegin();
    auto std_end = list.rend();

    auto it = vec.rbegin();
    auto it_end = vec.rend();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_vector_equality()
    {
    immutable::vector<int, atomic_ref_counting, N> vec;
    immutable::vector<int, atomic_ref_counting, N> vec2 = vec;
    TEST_ASSERT(vec == vec2);
    immutable::vector<int, atomic_ref_counting, N> vec3 = vec.push_back(0);
    immutable::vector<int, atomic_ref_counting, N> vec4 = vec.push_back(0);
    TEST_ASSERT(vec3 == vec4);
    immutable::vector<int, atomic_ref_counting, N> vec5 = vec3;
    TEST_ASSERT(vec5 == vec3);
    TEST_ASSERT(vec5 == vec4);
    immutable::vector<int, atomic_ref_counting, N> vec6;
    TEST_ASSERT(vec == vec6);
    vec6 = vec.push_back(3);
    TEST_ASSERT(vec != vec6);
    }

  template <bool atomic_ref_counting, int N>
  void test_insert_vector(uint32_t sz = 10)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    std::vector<int> list2(sz);
    for (auto& v : list2)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    immutable::vector<int, atomic_ref_counting, N> vec2;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec2 = vec2.push_back(list2[i]);
      }

    list.insert(list.begin() + sz / 2, list2.begin(), list2.end());
    auto vec3 = vec.insert(sz / 2, vec2);

    TEST_EQ(vec3.size(), list.size());
    for (uint32_t i = 0; i < list.size(); ++i)
      {
      TEST_EQ(vec3[i], list[i]);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_vector_push(uint32_t sz = 13000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;

    auto tvec = vec.transient();

    for (uint32_t i = 0; i < sz; ++i)
      {
      tvec.push_back(list[i]);
      for (uint32_t j = 0; j <= i; ++j)
        {
        int val = tvec[j];
        TEST_EQ(list[j], val);
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_vector_pop(uint32_t sz = 400000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;

    auto tvec = vec.transient();

    for (uint32_t i = 0; i < sz; ++i)
      {
      tvec.push_back(list[i]);
      }
    vec = tvec.persistent();
    tvec = vec.transient();
    for (uint32_t i = sz; i > 0; --i)
      {
      int val = tvec.back();
      TEST_EQ(list[i - 1], val);
      tvec.pop_back();
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_vector_update(uint32_t sz = 400000, uint32_t updates = 133337)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;

    auto tvec = vec.transient();

    for (uint32_t i = 0; i < sz; ++i)
      {
      tvec.push_back(list[i]);
      }
    vec = tvec.persistent();

    std::vector<int> updated_list(updates);
    std::vector<uint32_t> lookups(updates);
    for (uint32_t i = 0; i < updates; ++i)
      {
      updated_list[i] = rand();
      lookups[i] = (uint32_t)(rand() % sz);
      }

    tvec = vec.transient();

    for (uint32_t i = 0; i < updates; ++i)
      {
      const uint32_t idx = lookups[i];
      tvec.set(idx, updated_list[i]);
      int old_val = vec[idx];
      int new_val = tvec[idx];
      TEST_EQ(old_val, list[idx]);
      TEST_EQ(new_val, updated_list[i]);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_vector_push_2(uint32_t tests = 400, uint32_t sz = 40, uint32_t max_extra_pushes = 50)
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

      immutable::vector<int, atomic_ref_counting, N> left;
      immutable::vector<int, atomic_ref_counting, N> right;

      auto tmp = left.transient();
      for (uint32_t i = 0; i < rand_cut; i++)
        {
        tmp.push_back(list[i]);
        }
      left = tmp.persistent();

      tmp = right.transient();
      for (uint32_t i = 0; i < rrb_size - rand_cut; i++)
        {
        tmp.push_back(list[i + rand_cut]);
        }
      right = tmp.persistent();

      auto cat = left + right;

      auto trrb = cat.transient();

      for (uint32_t i = 0; i < extra_pushes; i++)
        {
        trrb.push_back(extra_vals[i]);
        }

      TEST_EQ(extra_pushes + rrb_size, trrb.size());

      for (uint32_t i = 0; i < rrb_size; i++)
        {
        int actual = trrb[i];
        int in_cat = cat[i];
        int expected = list[i];
        TEST_EQ(expected, actual);
        TEST_EQ(in_cat, actual);
        }

      for (uint32_t i = 0; i < extra_pushes; i++)
        {
        int actual = trrb[i + rrb_size];
        int expected = extra_vals[i];
        TEST_EQ(expected, actual);
        }

      auto cat_pushed = trrb.persistent();

      uint32_t left_offset, right_offset;
      left_offset = (uint32_t)rand() % rrb_size;
      right_offset = (uint32_t)(rand() % (rrb_size - left_offset)) + left_offset;

      auto slice = cat.take(right_offset).drop(left_offset);

      trrb = slice.transient();
      for (uint32_t i = 0; i < extra_pushes; i++)
        {
        trrb.push_back(extra_vals[i]);
        }

      for (uint32_t i = 0; i < slice.size(); i++)
        {
        int actual = trrb[i];
        int in_slice = slice[i];
        int expected = list[i + left_offset];
        TEST_EQ(expected, actual);
        TEST_EQ(in_slice, actual);
        }

      for (uint32_t i = 0; i < extra_pushes; i++)
        {
        int actual = trrb[i + slice.size()];
        int expected = extra_vals[i];
        TEST_EQ(expected, actual);
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_iterator(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    auto tvec = vec.transient();

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = tvec.begin();
    auto it_end = tvec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_iterator_2(uint32_t sz = 1000)
    {
    std::vector<int> list1(sz), list2(sz), list3(sz), list4(sz);
    for (auto& v : list1)
      v = rand();
    for (auto& v : list2)
      v = rand();
    for (auto& v : list3)
      v = rand();
    for (auto& v : list4)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec1, vec2, vec3, vec4;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec1 = vec1.push_back(list1[i]);
      vec2 = vec2.push_back(list2[i]);
      vec3 = vec3.push_back(list3[i]);
      vec4 = vec4.push_back(list4[i]);
      }

    std::vector<int> list;
    list.insert(list.end(), list1.begin(), list1.end());
    list.insert(list.end(), list2.begin(), list2.end());
    list.insert(list.end(), list3.begin(), list3.end());
    list.insert(list.end(), list4.begin(), list4.end());

    immutable::vector<int, atomic_ref_counting, N> vec = vec1 + vec2 + vec3 + vec4;

    auto tvec = vec.transient();

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = tvec.begin();
    auto it_end = tvec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_iterator_3(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    list.erase(list.begin() + sz / 4, list.begin() + sz / 2);
    vec = vec.erase(sz / 4, sz / 2);

    auto tvec = vec.transient();

    auto std_it = list.begin();
    auto std_end = list.end();

    auto it = tvec.begin();
    auto it_end = tvec.end();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_transient_reverse_iterator(uint32_t sz = 10000)
    {
    std::vector<int> list(sz);
    for (auto& v : list)
      v = rand();

    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      {
      vec = vec.push_back(list[i]);
      }

    auto tvec = vec.transient();

    auto std_it = list.rbegin();
    auto std_end = list.rend();

    auto it = tvec.rbegin();
    auto it_end = tvec.rend();

    for (; it != it_end; ++it, ++std_it)
      {
      TEST_EQ(*it, *std_it);
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_catslice(uint32_t sz = 400, uint32_t sliced = 53, uint32_t catted = 2310, uint32_t tot_catted = 10)
    {
    immutable::vector<int, atomic_ref_counting, N> vec;
    for (uint32_t i = 0; i < sz; ++i)
      vec = vec.push_back((int)rand() % 10000);

    std::vector<immutable::vector<int, atomic_ref_counting, N>> sliced_rrb(sliced);

    for (uint32_t i = 0; i < sliced; ++i)
      {
      uint32_t from = (uint32_t)rand() % sz;
      uint32_t to = (uint32_t)(rand() % (sz - from)) + from;
      sliced_rrb[i] = vec.take(to).drop(from);      
      }

    for (uint32_t i = 0; i < catted; ++i)
      {
      uint32_t tot_cats = (uint32_t)rand() % tot_catted;
      immutable::vector<int, atomic_ref_counting, N> multicat;
      std::vector<immutable::vector<int, atomic_ref_counting, N>> catsteps(tot_cats);
      std::vector<uint32_t> merged_in(tot_cats);
      for (uint32_t cat_num = 0; cat_num < tot_cats; ++cat_num)
        {
        merged_in[cat_num] = (uint32_t)rand() % sliced;
        multicat = multicat + sliced_rrb[merged_in[cat_num]];

        catsteps[cat_num] = multicat;
        }

      uint32_t pos = 0;
      uint32_t merged_pos = 0;
      while (pos < multicat.size())
        {
        immutable::vector<int, atomic_ref_counting, N> merged = sliced_rrb[merged_in[merged_pos]];

        for (uint32_t merged_i = 0; merged_i < merged.size(); merged_i++, pos++)
          {
          int expected = merged[merged_i];
          int actual = multicat[pos];
          TEST_EQ(expected, actual);
          }
        merged_pos++;
        }
      }
    }
  
  template <bool atomic_ref_counting, int N>
  void test_fibocat(uint32_t rrb_counter = 2600, uint32_t predef_rrbs = 200)
    {
    uint32_t max_init_size = 16;

    std::vector<immutable::vector<int, atomic_ref_counting, N>> vecs(rrb_counter);
    for (uint32_t i = 0; i < predef_rrbs; ++i)
      {
      const uint32_t local_sz = (rand() % max_init_size);
      for (uint32_t j = 0; j < local_sz; ++j)
        {
        vecs[i] = vecs[i].push_back(rand() & 0xffff);
        }
      }

    for (uint32_t i = predef_rrbs; i < rrb_counter; ++i)
      vecs[i] = vecs[i - predef_rrbs] + vecs[i - predef_rrbs + 1];

    for (uint32_t i = predef_rrbs; i < rrb_counter; ++i)
      {
      auto merged = vecs[i];
      auto left = vecs[i - predef_rrbs];
      auto right = vecs[i - predef_rrbs + 1];
      uint32_t merged_idx = 0;
      for (uint32_t left_idx = 0; left_idx < left.size(); left_idx++, merged_idx++)
        {
        int expected = left[left_idx];
        int actual = merged[merged_idx];
        TEST_EQ(expected, actual);
        }

      for (uint32_t right_idx = 0; right_idx < right.size(); right_idx++, merged_idx++)
        {
        int expected = right[right_idx];
        int actual = merged[merged_idx];
        TEST_EQ(expected, actual);
        }
      }
    }

  template <bool atomic_ref_counting, int N>
  void test_invalid_states()
    {
    immutable::vector<int, atomic_ref_counting, N> v;
    auto tv = v.transient();
    tv.push_back(0);
    tv.push_back(1);
    tv.push_back(2);
    v = tv.persistent();
    bool error_catch = false;
    try
      {
      tv.push_back(3);
      }
    catch (std::runtime_error e)
      {
      error_catch = true;
      }
    TEST_ASSERT(error_catch);
    TEST_EQ(0, v.at(0));
    TEST_EQ(1, v.at(1));
    TEST_EQ(2, v.at(2));
    error_catch = false;
    try
      {
      v.at(3);
      }
    catch (std::out_of_range e)
      {
      error_catch = true;
      }
    TEST_ASSERT(error_catch);
    }
    
  template <bool atomic_ref_counting, int N>
  void run_tests()
    {
    test_empty_vector<atomic_ref_counting, N>();
    test_push_back_simple<atomic_ref_counting, N>();
    test_push_back<atomic_ref_counting, N>();
    test_front<atomic_ref_counting, N>();
    test_back<atomic_ref_counting, N>();
    test_size<atomic_ref_counting, N>();
    test_pop_back<atomic_ref_counting, N>();
    test_update<atomic_ref_counting, N>();
    test_erase_single<atomic_ref_counting, N>();
    test_erase<atomic_ref_counting, N>(200, 50);
    test_insert_single<atomic_ref_counting, N>();
    test_concat<atomic_ref_counting, N>(200);
    test_drop<atomic_ref_counting, N>();
    test_take<atomic_ref_counting, N>();
    test_iterator<atomic_ref_counting, N>();
    test_iterator_2<atomic_ref_counting, N>();
    test_iterator_3<atomic_ref_counting, N>();
    test_reverse_iterator<atomic_ref_counting, N>();
    test_vector_equality<atomic_ref_counting, N>();
    test_insert_vector<atomic_ref_counting, N>();
    test_transient_vector_push<atomic_ref_counting, N>();
    test_transient_vector_pop<atomic_ref_counting, N>();
    test_transient_vector_update<atomic_ref_counting, N>();
    test_transient_vector_push_2<atomic_ref_counting, N>();
    test_transient_iterator<atomic_ref_counting, N>();
    test_transient_iterator_2<atomic_ref_counting, N>();
    test_transient_iterator_3<atomic_ref_counting, N>();
    test_transient_reverse_iterator<atomic_ref_counting, N>();
    test_catslice<atomic_ref_counting, N>();
    test_fibocat<atomic_ref_counting, N>();
    test_invalid_states<atomic_ref_counting, N>();
    test_erase<atomic_ref_counting, N>();
    test_concat<atomic_ref_counting, N>();
    }

  }

void run_all_vector_tests()
  {  
  run_tests<true, 5>();
  run_tests<false, 5>();
  run_tests<false, 6>();
  }