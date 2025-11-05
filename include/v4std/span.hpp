/**
 * @file span.hpp
 * @brief Lightweight span implementation for C++17
 *
 * Provides a minimal std::span-like class for C++17 compatibility.
 * This is a non-owning view over a contiguous sequence of elements.
 *
 * @copyright Copyright 2025 V4 Project
 * @license Dual-licensed under MIT or Apache-2.0
 */

#ifndef V4STD_SPAN_HPP
#define V4STD_SPAN_HPP

#include <cstddef>

namespace v4std
{

/**
 * @brief Lightweight non-owning view over a contiguous sequence
 *
 * Minimal std::span-like implementation for C++17.
 * Provides begin(), end(), size(), empty(), and operator[].
 */
template <typename T>
class span
{
 public:
  using element_type = T;
  using value_type = T;
  using size_type = size_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = const T*;

  // Default constructor (empty span)
  constexpr span() noexcept : data_(nullptr), size_(0) {}

  // Constructor from pointer and size
  constexpr span(pointer ptr, size_type count) noexcept
      : data_(ptr), size_(count)
  {
  }

  // Constructor from C-array
  template <size_t N>
  constexpr span(element_type (&arr)[N]) noexcept : data_(arr), size_(N)
  {
  }

  // Iterators
  constexpr iterator begin() const noexcept { return data_; }
  constexpr iterator end() const noexcept { return data_ + size_; }
  constexpr const_iterator cbegin() const noexcept { return data_; }
  constexpr const_iterator cend() const noexcept { return data_ + size_; }

  // Element access
  constexpr reference operator[](size_type idx) const noexcept
  {
    return data_[idx];
  }

  constexpr reference front() const noexcept { return data_[0]; }
  constexpr reference back() const noexcept { return data_[size_ - 1]; }
  constexpr pointer data() const noexcept { return data_; }

  // Capacity
  constexpr size_type size() const noexcept { return size_; }
  constexpr bool empty() const noexcept { return size_ == 0; }

 private:
  pointer data_;
  size_type size_;
};

}  // namespace v4std

#endif  // V4STD_SPAN_HPP
