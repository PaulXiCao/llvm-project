//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// Check that functions are marked [[nodiscard]]

#include <ranges>

struct NonSimpleView : std::ranges::view_base {
  int* begin();
  int* end();
};

struct ConstAccessibleView : std::ranges::view_base {
  int* begin() const;
  int* end() const;
};

// A view with a distinct sentinel type so cartesian_product_is_common is false,
// causing end() to return default_sentinel_t.
struct NonCommonView : std::ranges::view_base {
  struct Sentinel {
    friend bool operator==(int*, Sentinel) noexcept;
  };
  int* begin() const;
  Sentinel end() const;
};

struct SizedView : std::ranges::view_base {
  int* begin() const;
  int* end() const;
  unsigned size() const;
};

void test_view() {
  { // cartesian_product_view::begin() requires (!__simple_view<First> || ... || !__simple_view<Vs>)
    static_assert(!std::ranges::__simple_view<NonSimpleView>);
    std::ranges::cartesian_product_view<NonSimpleView> view{NonSimpleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.begin();
  }

  { // cartesian_product_view::begin() const requires (range<const First> && ... && range<const Vs>)
    static_assert(std::ranges::range<const ConstAccessibleView>);
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.begin();
  }

  { // cartesian_product_view::end() requires (!__simple_view<First> || ... || !__simple_view<Vs>) && cartesian_product_is_common<First, Vs...>
    static_assert(!std::ranges::__simple_view<NonSimpleView>);
    static_assert(std::ranges::cartesian_product_common_arg<NonSimpleView>);
    std::ranges::cartesian_product_view<NonSimpleView> view{NonSimpleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.end();
  }

  { // cartesian_product_view::end() const requires cartesian_product_is_common<const First, const Vs...>
    static_assert(std::ranges::cartesian_product_common_arg<const ConstAccessibleView>);
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.end();
  }

  { // cartesian_product_view::end() const noexcept -> default_sentinel_t
    // Selected when neither end() overload applies, i.e. cartesian_product_is_common is false.
    static_assert(!std::ranges::cartesian_product_common_arg<NonCommonView>);
    const std::ranges::cartesian_product_view<NonCommonView> view{NonCommonView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.end();
  }
  
  { // cartesian_product_view::size() requires cartesian_product_is_sized<First, Vs...>
    static_assert(std::ranges::cartesian_product_is_sized<SizedView>);
    std::ranges::cartesian_product_view<SizedView> view{SizedView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.size();
  }
  
  { // cartesian_product_view::size() const requires cartesian_product_is_sized<const First, const Vs...>
    static_assert(std::ranges::cartesian_product_is_sized<const SizedView>);
    const std::ranges::cartesian_product_view<SizedView> view{SizedView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.size();
  }
}

void test_iterator() {

  { // cartesian_product_view::iterator::operator*() const
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    const auto iter = view.begin();
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    *iter;
  }

  { // cartesian_product_view::iterator::operator++(int) requires forward_range<const First>
    static_assert(std::ranges::forward_range<const ConstAccessibleView>);
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    auto iter = view.begin();
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    iter++;
  }

  { // cartesian_product_view::iterator::operator--(int) requires cartesian_product_is_bidirectional<const, First, Vs...>
    static_assert(std::ranges::cartesian_product_is_bidirectional<true, ConstAccessibleView>);
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    auto iter = view.begin();
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    iter--;
  }

  { // cartesian_product_view::iterator::operator[](difference_type) const requires cartesian_product_is_random_access<const, First, Vs...>
    static_assert(std::ranges::cartesian_product_is_random_access<true, ConstAccessibleView>);
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    const auto iter = view.begin();
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    iter[0];
  }
}

void test() {
  test_view();
  test_iterator();
}
