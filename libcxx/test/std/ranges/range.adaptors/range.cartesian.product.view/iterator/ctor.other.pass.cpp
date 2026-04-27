//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// REQUIRES: std-at-least-c++23

// constexpr iterator(iterator<!Const> i)
//   requires Const && (convertible_to<iterator_t<First>, iterator_t<const First>> && ...
//                   && convertible_to<iterator_t<Vs>,    iterator_t<const Vs>>);

#include <array>
#include <cassert>
#include <ranges>
#include <tuple>
#include <utility>

#include "../../range_adaptor_types.h"

using ConstIterIncompatibleView =
    BasicView<forward_iterator<int*>,
              forward_iterator<int*>,
              random_access_iterator<const int*>,
              random_access_iterator<const int*>>;
static_assert(!std::convertible_to<std::ranges::iterator_t<ConstIterIncompatibleView>,
                                   std::ranges::iterator_t<const ConstIterIncompatibleView>>);

constexpr bool test() {
  std::array buffer{1, 2, 3};

  { // non-const to const conversion succeeds when all underlying iterators are convertible
    std::ranges::cartesian_product_view v(NonSimpleCommon{buffer}, NonSimpleCommon{buffer});
    auto iter1                                       = v.begin();
    std::ranges::iterator_t<const decltype(v)> iter2 = iter1;
    assert(iter1 == iter2);

    static_assert(!std::is_same_v<decltype(iter1), decltype(iter2)>);
    // Cannot go the other way (const -> non-const).
    static_assert(!std::constructible_from<decltype(iter1), decltype(iter2)>);
  }

  { // const-incompatible underlying iterator: neither direction is constructible
    std::ranges::cartesian_product_view v(ConstIterIncompatibleView{buffer});
    auto iter1 = v.begin();
    auto iter2 = std::as_const(v).begin();

    static_assert(!std::is_same_v<decltype(iter1), decltype(iter2)>);
    static_assert(!std::constructible_from<decltype(iter1), decltype(iter2)>);
    static_assert(!std::constructible_from<decltype(iter2), decltype(iter1)>);
  }

  { // 3-range conversion
    std::ranges::cartesian_product_view v(NonSimpleCommon{buffer}, NonSimpleCommon{buffer}, NonSimpleCommon{buffer});
    auto iter1                                       = v.begin();
    std::ranges::iterator_t<const decltype(v)> iter2 = iter1;
    assert(iter1 == iter2);
  }

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());
  return 0;
}
