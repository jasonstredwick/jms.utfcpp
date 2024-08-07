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


#include <cstddef>
#include <string_view>
#include <tuple>
#include <utility>

#include "utfcpp/concepts.hpp"
#include "utfcpp/core.hpp"
#include "utfcpp/decode_encode.hpp"


namespace utfcpp {


// Derived from c++ standard library; back_insert_iterator / back_inserter
template <IsUTF_c T>
class CodePointAppendIterator { // wrap pushes to back of container as output iterator
public:
    using iterator_category = std::output_iterator_tag;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;

    using container_type = std::basic_string<T>;
    using difference_type = ptrdiff_t;

    constexpr explicit CodePointAppendIterator(container_type& str) noexcept : container(std::addressof(str)) {}

    constexpr CodePointAppendIterator& operator=(const char32_t& code_point) {
        if constexpr (std::is_same_v<T, char8_t>)       { container->append_range(EncodeUTF8(code_point)); }
        else if constexpr (std::is_same_v<T, char16_t>) { container->append_range(EncodeUTF16(code_point)); }
        else { container->push_back(code_point); }
        return *this;
    }

    [[nodiscard]] constexpr CodePointAppendIterator& operator*() noexcept { return *this; }
    constexpr CodePointAppendIterator& operator++()    noexcept { return *this; }
    constexpr CodePointAppendIterator  operator++(int) noexcept { return *this; }

protected:
    container_type* container;
};


template <IsUTF_c T>
[[nodiscard]] constexpr CodePointAppendIterator<T> CodePointAppender(std::basic_string<T>& str) noexcept {
    return CodePointAppendIterator<T>(str);
}


template <IsUTF_c T>
class UTFInputIterator {
public:
    using self_t            = UTFInputIterator<T>;
    using string_view_type  = std::basic_string_view<T>;
    using value_type        = char32_t;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    struct sentinel {};
    friend constexpr bool operator==(const sentinel& s, const self_t& iter) noexcept { return iter.rng.empty(); }
    friend constexpr bool operator==(const self_t& iter, const sentinel& s) noexcept { return iter.rng.empty(); }

    constexpr UTFInputIterator() noexcept = default;
    constexpr UTFInputIterator(string_view_type str_view) noexcept : rng{str_view} {
        if (!rng.empty()) {
            DecodeData data = _Decode();
            next_index = data.consumed;
            code_point = data.code_point;
            error_code = data.error_code;
        }
    }
    constexpr auto operator<=>(const UTFInputIterator&) const noexcept = default;

    constexpr auto& operator++() noexcept {
        if (!rng.empty()) {
            rng = _Advance(next_index);
            if (!rng.empty()) {
                DecodeData data = _Decode();
                next_index = data.consumed;
                code_point = data.code_point;
                error_code = data.error_code;
            }
        }
        return *this;
    }

    constexpr auto operator++(int) noexcept { auto tmp = *this; ++*this; return tmp; }

    constexpr value_type operator*() const noexcept { return rng.empty() ? REPLACEMENT_CHARACTER : code_point; }

    constexpr string_view_type Data() const noexcept { return rng; }

    constexpr std::tuple<value_type, UTF_ERROR> Decode() const noexcept {
        return rng.empty() ?
            std::tuple{REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT} :
            std::tuple{code_point, error_code};
    }

    constexpr UTF_ERROR DecodeError() const noexcept {
        return rng.empty() ? UTF_ERROR::INVALID_CODE_POINT : error_code;
    }

private:
    string_view_type rng{};
    size_t next_index{0};
    value_type code_point{REPLACEMENT_CHARACTER};
    UTF_ERROR error_code{UTF_ERROR::INVALID_CODE_POINT};

    constexpr string_view_type _Advance(const size_t consumed) const noexcept {
        return string_view_type{rng.begin() + (consumed ? consumed : 1), rng.end()};
    }

    constexpr DecodeData _Decode() const noexcept {
        if constexpr (std::is_same_v<T, char8_t>) {
            return DecodeUTF8(rng);
        } else if constexpr (std::is_same_v<T, char16_t>) {
            return DecodeUTF16(rng);
        } else {
            value_type code_point = rng.front();
            if (!is_code_point_valid(code_point)) {
                return DecodeData{
                    .error_code=UTF_ERROR::INVALID_CODE_POINT,
                    .code_point=REPLACEMENT_CHARACTER,
                    .consumed=1
                };
            }
            return DecodeData{
                .error_code=UTF_ERROR::OK,
                .code_point=code_point,
                .consumed=1
            };
        }
    }
};


template <>
class UTFInputIterator<char32_t> {
public:
    using self_t            = UTFInputIterator<char32_t>;
    using string_view_type  = std::basic_string_view<char32_t>;
    using value_type        = char32_t;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    struct sentinel {};
    friend constexpr bool operator==(const sentinel& s, const self_t& iter) noexcept { return iter.rng.empty(); }
    friend constexpr bool operator==(const self_t& iter, const sentinel& s) noexcept { return iter.rng.empty(); }

    constexpr UTFInputIterator() noexcept = default;
    constexpr UTFInputIterator(string_view_type str_view) noexcept : rng{str_view} {}
    constexpr auto operator<=>(const UTFInputIterator&) const noexcept = default;

    constexpr UTFInputIterator& operator++() noexcept {
        if (!rng.empty()) {
            rng = string_view_type{rng.begin() + 1, rng.end()};
        }
        return *this;
    }

    constexpr UTFInputIterator operator++(int) noexcept { auto tmp = *this; ++*this; return tmp; }

    constexpr value_type operator*() const noexcept {
        if (rng.empty()) { return REPLACEMENT_CHARACTER; }
        value_type code_point = rng.front();
        return is_code_point_valid(code_point) ? code_point : REPLACEMENT_CHARACTER;
    }

    constexpr string_view_type Data() const noexcept { return rng; }

    constexpr std::tuple<value_type, UTF_ERROR> Decode() const noexcept {
        if (rng.empty()) { return {REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT}; }
        value_type code_point = rng.front();
        return is_code_point_valid(code_point) ?
            std::tuple{code_point, UTF_ERROR::OK} :
            std::tuple{REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT};
    }

    constexpr UTF_ERROR DecodeError() const noexcept {
        if (rng.empty()) { return UTF_ERROR::INVALID_CODE_POINT; }
        return is_code_point_valid(rng.front()) ? UTF_ERROR::OK : UTF_ERROR::INVALID_CODE_POINT;
    }

private:
    string_view_type rng{};
};


} // namespace utfcpp
