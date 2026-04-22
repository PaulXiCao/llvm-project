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

void test() {
  { // cartesian_product_view<First, Vs>::begin() requires !__simple_view<First> || ... || !__simple_view<Vs>
    std::ranges::cartesian_product_view<NonSimpleView> view{NonSimpleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.begin();
  }

  { // cartesian_product_view<First, Vs>::begin() const requires range<const First> && ... && range<const Vs>
    const std::ranges::cartesian_product_view<ConstAccessibleView> view{ConstAccessibleView{}};
    // expected-warning@+1 {{ignoring return value of function declared with 'nodiscard' attribute}}
    view.begin();
  }
}
