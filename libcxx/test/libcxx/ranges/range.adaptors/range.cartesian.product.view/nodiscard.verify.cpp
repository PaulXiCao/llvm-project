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

void test() {
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
}
