//    Copyright 2024 Nemanja Trifunovic

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#pragma once


#include <cstddef> // std::size_t
#include <ranges>
#include <string>
#include <string_view>

#include "utfcpp/concepts.hpp"
#include "utfcpp/core.hpp"
#include "utfcpp/decode_encode.hpp"
#include "utfcpp/iterator.hpp"


namespace utfcpp {


template <is_utf_c T, template <typename> typename Iter_t=utf_input_iterator>
class utf_view : public std::ranges::view_interface<utf_view<T, Iter_t>> {
public:
    using string_view_type = std::basic_string_view<T>;
    using utf_interator_type = Iter_t<T>;

    constexpr utf_view(string_view_type str_view) noexcept : str_view{str_view} {}

    constexpr auto begin() const noexcept { return utf_interator_type{str_view}; }
    constexpr auto end() const noexcept { return typename utf_interator_type::sentinel{}; }

    constexpr bool empty() const noexcept { return str_view.empty(); }
    constexpr      operator bool() const noexcept { return !str_view.empty(); }
    constexpr auto data() const noexcept { return str_view; }
    constexpr size_t size() const noexcept { return str_view.size(); }

private:
    string_view_type str_view;
};

template <template<typename> typename Iter_t=utf_input_iterator> using utf8_view = utf_view<char8_t, Iter_t>;
template <template<typename> typename Iter_t=utf_input_iterator> using utf16_view = utf_view<char16_t, Iter_t>;
template <template<typename> typename Iter_t=utf_input_iterator> using utf32_view = utf_view<char32_t, Iter_t>;


} // namespace utfcpp
