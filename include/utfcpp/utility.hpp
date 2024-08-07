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


#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "utfcpp/concepts.hpp"
#include "utfcpp/core.hpp"
//#include "utfcpp/decode_encode.hpp"
#include "utfcpp/iterator.hpp"
#include "utfcpp/views.hpp"


namespace utfcpp {


template <IsUTF_c T, template<typename> typename Iter_t=UTFInputIterator>
constexpr size_t FindInvalid(std::basic_string_view<T> src) {
    UTFView<T, Iter_t> view{src};
    auto src_iter = view.begin();
    for (; src_iter != view.end(); ++src_iter) {
        auto error_code = src_iter.DecodeError();
        if (error_code != UTF_ERROR::OK) {
            break;
        }
    }
    auto src_remaining = src_iter.Data();
    return src.size() - src_remaining.size();
}


template <IsUTF_c T, template<typename> typename Iter_t=UTFInputIterator>
constexpr bool IsValid(std::basic_string_view<T> src) noexcept {
    return FindInvalid<T, Iter_t>(src) >= src.size();
}


// Assumes input is validated; will replace invalid code points with REPLACEMENT_CHARACTER.
// User can consider using shrink_to_fit to reduce overall allocated size if concerned about footprint.
template <typename Src_t, IsUTF_c Dst_t, template<typename> typename Iter_t=UTFInputIterator>
constexpr std::basic_string<Dst_t> UTFConvertTo(std::basic_string_view<Src_t> src) {
    std::basic_string<Dst_t> result{};
    result.reserve(src.size()); // TODO: pre-allocate return string in a smarter way
    std::ranges::transform(UTFView<Src_t, Iter_t>{src}, CodePointAppender(result), std::identity{});
    return result;
}

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u8string utf8_to_8(std::u8string_view sv)     { return UTFConvertTo<char8_t,  char8_t,  Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u16string utf8_to_16(std::u8string_view sv)   { return UTFConvertTo<char8_t,  char16_t, Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u32string utf8_to_32(std::u8string_view sv)   { return UTFConvertTo<char8_t,  char32_t, Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u8string utf16_to_8(std::u16string_view sv)   { return UTFConvertTo<char16_t, char8_t,  Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u16string utf16_to_16(std::u16string_view sv) { return UTFConvertTo<char16_t, char16_t, Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u32string utf16_to_32(std::u16string_view sv) { return UTFConvertTo<char16_t, char32_t, Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u8string utf32_to_8(std::u32string_view sv)   { return UTFConvertTo<char32_t, char8_t,  Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u16string utf32_to_16(std::u32string_view sv) { return UTFConvertTo<char32_t, char16_t, Iter_t>(sv); }

template <template<typename> typename Iter_t=UTFInputIterator>
constexpr std::u32string utf32_to_32(std::u32string_view sv) { return UTFConvertTo<char32_t, char32_t, Iter_t>(sv); }


#if 0
template <IsUTF_c Src_t, IsUTF_c Dst_t, template<typename> typename Iter_t=UTFInputIterator>
constexpr std::tuple<size_t, UTF_ERROR> UTFAttemptConvertTo(std::basic_string_view<Src_t>& src,
                                                            std::basic_string<Dst_t>& dst) {
    auto src_end_iter = Iter_t<Src_t>::Sentinel();
    auto dst_iter = CodePointAppender(dst);
    for (Iter_t src_iter{src}; src_iter != src_end_iter; ++src_iter, ++dst_iter) {
        auto [code_point, error_code] = src_iter.Decode();
        if (error_code != UTF_ERROR::OK) {
            auto src_remaining = src_iter.Data();
            size_t consumed = src.size() - src_remaining.size();
            return {consumed, error_code};
        }
        *dst_iter = code_point;
    }
    src = {};
    return {src.size(), UTF_ERROR::OK};
}
#endif


} // namespace utfcpp
