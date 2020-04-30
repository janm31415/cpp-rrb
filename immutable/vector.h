#pragma once

#include "rrb.h"
#include "rrb_transient.h"

#include <tuple>

namespace immutable
  {

  template <typename T, bool atomic_ref_counting, int N>
  class vector;

  template <typename T, bool atomic_ref_counting, int N>
  class transient_vector;

  template <typename T, bool atomic_ref_counting, int N>
  class vector_iterator
    {
    public:
      typedef vector_iterator<T, atomic_ref_counting, N> self_type;
      typedef std::random_access_iterator_tag iterator_category;
      typedef T value_type;
      typedef uint32_t size_type;
      typedef const T* pointer;
      typedef const T* const_pointer;
      typedef const T& reference;
      typedef const T& const_reference;
      typedef std::ptrdiff_t difference_type;

      typedef std::tuple<pointer, size_type, size_type> region_type;

      struct end_type {};

      vector_iterator() = default;

      vector_iterator(const ref<rrb<T, atomic_ref_counting, N>>& impl) : _impl(impl), _index(0), _cursor {nullptr, (size_type)-1, (size_type)-1} {}

      vector_iterator(const ref<rrb<T, atomic_ref_counting, N>>& impl, end_type) : _impl(impl), _index(impl->cnt), _cursor{ nullptr, (size_type)-1, (size_type)-1 } {}

      reference operator* () const
        {
        if (_index < std::get<1>(_cursor) || _index >= std::get<2>(_cursor))
          {
          _cursor = rrb_region_for(_impl, _index);
          }
        return std::get<0>(_cursor)[_index - std::get<1>(_cursor)];
        }

      pointer operator ->() const
        {
        return &(this->operator*());
        }

      self_type operator++(int)
        {
        self_type tmp(*this);
        ++(*this);
        return tmp;
        }

      self_type operator--(int)
        {
        self_type tmp(*this);
        --(*this);
        return tmp;
        }

      self_type& operator++()
        {
        ++_index;
        return *this;
        }

      self_type& operator--()
        {
        --_index;
        return *this;
        }

      self_type operator + (difference_type n) const
        {
        self_type tmp(*this);
        tmp._index += (size_type)n;
        return tmp;
        }

      self_type& operator+= (difference_type n)
        {
        _index += (size_type)n;
        return *this;
        }

      self_type operator- (difference_type n) const
        {
        self_type tmp(*this);
        tmp._index -= (size_type)n;
        return tmp;
        }

      self_type& operator-= (difference_type n)
        {
        _index -= (size_type)n;
        return *this;
        }

      difference_type operator - (const self_type& c) const
        {
        return _index - c._index;
        }

      bool operator == (const self_type &other) const
        {
        return (_index == other._index) && (_impl.ptr == other._impl.ptr);
        }

      bool operator != (const self_type& other) const
        {
        return !(*this == other);
        }

      bool operator > (const self_type &other) const
        {
        return _index > other._index;
        }

      bool operator >= (const self_type &other) const
        {
        return _index >= other._index;
        }

      bool operator < (const self_type &other) const
        {
        return _index < other._index;
        }

      bool operator <= (const self_type &other) const
        {
        return _index <= other._index;
        }

    private:
      ref<rrb<T, atomic_ref_counting, N>> _impl;
      size_type _index;
      mutable region_type _cursor;
    };

  template <typename T, bool atomic_ref_counting = true, int N = 5>
  class vector
    {
    public:
      using value_type = T;
      using reference = const T&;
      using const_reference = const T&;
      using size_type = uint32_t;
      using iterator = vector_iterator<T, atomic_ref_counting, N>;
      using const_iterator = iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using transient_type = transient_vector<T, atomic_ref_counting, N>;

      vector() = default;

      iterator begin() const
        {
        return iterator(_impl);
        }

      iterator end() const
        {
        return iterator(_impl, typename iterator::end_type());
        }

      reverse_iterator rbegin() const 
        { 
        return reverse_iterator{ end() }; 
        }

      reverse_iterator rend() const
        {
        return reverse_iterator{ begin() };
        }

      bool empty() const
        {
        return _impl->cnt == 0;
        }

      size_type size() const
        {
        return _impl->cnt;
        }

      const_reference back() const
        {
        return rrb_peek(_impl);
        }

      const_reference front() const
        {
        return rrb_nth(_impl, 0);
        }

      const_reference operator [] (size_type index) const
        {
        return rrb_nth(_impl, index);
        }

      const_reference at(size_type index) const
        {
        if (index >= size())
          throw std::out_of_range("invalid vector<T> index");
        return rrb_nth(_impl, index);
        }

      vector push_back(value_type value) const
        {
        return rrb_push(_impl, value);
        }

      vector pop_back() const
        {
        return rrb_pop(_impl);
        }

      vector set(size_type index, value_type value) const
        {
        return rrb_update(_impl, index, value);
        }

      vector erase(size_type pos) const
        {        
        return take(pos) + drop(pos + 1);
        }

      vector erase(size_type from, size_type to) const
        {
        return to > from ? take(from) + drop(to) : *this;
        }

      vector insert(size_type pos, value_type value) const
        {        
        return take(pos).push_back(value) + drop(pos);
        }

      vector insert(size_type pos, vector value) const
        {
        return take(pos) + std::move(value) + drop(pos);
        }

      // drops first 'elems' items from the vector
      vector drop(size_type elems) const
        {
        return rrb_slice(_impl, elems, _impl->cnt);
        }

      // takes first 'elems' items from the vector
      vector take(size_type elems) const
        {       
        return rrb_slice(_impl, 0, elems);
        }

      // returns the slice from "from" to "to"
      vector slice(size_type from, size_type to) const
        {
        return rrb_slice(_impl, from, to);
        }

      bool operator == (const vector& other) const
        {
        if (size() != other.size())
          return false;
        if (size() == 0)
          return true;
        auto it = begin();
        auto it2 = other.begin();
        auto it_end = end();
        for (; it != it_end; ++it, ++it2)
          {
          if (*it != *it2)
            return false;
          }
        return true;
        }

      bool operator != (const vector& other) const
        {
        return !(*this == other);
        }

      transient_type transient() const
        {
        return transient_type(_impl);
        }

      ref<rrb<T, atomic_ref_counting, N>> raw() const
        {
        return _impl;
        }

    private:
      vector(const ref<rrb<T, atomic_ref_counting, N>>& impl) : _impl(impl)
        {
        }

      vector(const ref<transient_rrb<T, atomic_ref_counting, N>>& impl) 
        {
        _impl = transient_to_rrb(impl);
        }

    private:
      ref<rrb<T, atomic_ref_counting, N>> _impl = rrb_create<T, atomic_ref_counting, N>();

      template <typename T_2, bool atomic_ref_counting_2, int N_2>
      friend class transient_vector;

      template <typename T_2, bool atomic_ref_counting_2, int N_2>
      friend vector<T_2, atomic_ref_counting_2, N_2> operator + (const vector<T_2, atomic_ref_counting_2, N_2>& left, const vector<T_2, atomic_ref_counting_2, N_2>& right);
    };

  template <typename T, bool atomic_ref_counting, int N>
  vector<T, atomic_ref_counting, N> operator + (const vector<T, atomic_ref_counting, N>& left, const vector<T, atomic_ref_counting, N>& right)
    {
    return rrb_concat(left._impl, right._impl);
    }


  template <typename T, bool atomic_ref_counting = true, int N = 5>
  class transient_vector
    {
    public:
      using value_type = T;
      using reference = const T&;
      using const_reference = const T&;
      using size_type = uint32_t;
      using iterator = vector_iterator<T, atomic_ref_counting, N>;
      using const_iterator = iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using persistent_type = vector<T, atomic_ref_counting, N>;

      transient_vector() = default;

      iterator begin() const
        {
        return iterator((const ref<rrb<T, atomic_ref_counting, N>>&)_impl);
        }

      iterator end() const
        {
        return iterator((const ref<rrb<T, atomic_ref_counting, N>>&)_impl, typename iterator::end_type());
        }

      reverse_iterator rbegin() const
        {
        return reverse_iterator{ end() };
        }

      reverse_iterator rend() const
        {
        return reverse_iterator{ begin() };
        }

      bool empty() const
        {
        return _impl->cnt == 0;
        }

      size_type size() const
        {
        return _impl->cnt;
        }

      const_reference back() const
        {
        return transient_rrb_peek(_impl);
        }

      const_reference front() const
        {
        return transient_rrb_nth(_impl, 0);
        }

      const_reference operator [] (size_type index) const
        {
        return transient_rrb_nth(_impl, index);
        }

      const_reference at(size_type index) const
        {
        if (index >= size())
          throw std::out_of_range("invalid transient_vector<T> index");
        return transient_rrb_nth(_impl, index);
        }

      void push_back(value_type value)
        {
        transient_rrb_push(_impl, value);
        }

      void pop_back()
        {
        transient_rrb_pop(_impl);
        }

      void set(size_type index, value_type value)
        {
        transient_rrb_update(_impl, index, value);
        }

      persistent_type persistent() const
        {
        return persistent_type(_impl);
        }

    private:
      transient_vector(const ref<transient_rrb<T, atomic_ref_counting, N>>& impl) : _impl(impl)
        {
        }

      transient_vector(const ref<rrb<T, atomic_ref_counting, N>>& impl) 
        {
        _impl = rrb_to_transient(impl);
        }

    private:
      ref<transient_rrb<T, atomic_ref_counting, N>> _impl;

      template <typename T_2, bool atomic_ref_counting_2, int N_2>
      friend class vector;
    };

  }