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
#include <ranges>
#include <string>
#include <string_view>

#include "utfcpp/core.hpp"


namespace utfcpp {


struct DecodeData {
    size_t consumed{0};
    char32_t code_point{REPLACEMENT_CHARACTER};
    UTF_ERROR error_code{UTF_ERROR::OK};
};


constexpr DecodeData DecodeUTF8(std::u8string_view utf8str) noexcept {
    if (utf8str.empty()) { return DecodeData{.error_code=UTF_ERROR::INCOMPLETE_SEQUENCE}; }

    // Expected byte length of the utf-8 sequence, according to the lead byte
    const char8_t lead = utf8str[0];
    const size_t length = SequenceLength(lead);
    if (!length) { return DecodeData{.consumed=1, .error_code=UTF_ERROR::INVALID_LEAD}; }
    else if (length == 1) {
        // No need to go through error checks here
        return DecodeData{
            .consumed=1,
            .code_point=static_cast<char32_t>(lead),
            .error_code=UTF_ERROR::OK
        };
    }

    // Incomplete sequence may mean:
    // 1) utf8str does not contain the required number of bytes, or
    // 2) some of the expected trail bytes have invalid value
    if (length > utf8str.length()) { return DecodeData{.consumed=1, .error_code=UTF_ERROR::INCOMPLETE_SEQUENCE}; }
    for (char8_t cp : utf8str | std::views::take(length) | std::views::drop(1)) {
        if (!IsTrailUTF8(cp)) {
            return DecodeData{.consumed=1, .error_code=UTF_ERROR::INCOMPLETE_SEQUENCE};
        }
    }

    // initialize to common value used in all initializations.
    char32_t code_point = static_cast<char32_t>(lead);
    switch (length) {
    case 2:
        code_point = ((code_point << 6) & 0x7ff) +
                     (static_cast<char32_t>(utf8str[1]) & 0x3f);
        break;
    case 3:
        code_point = ((code_point << 12) & 0xffff) +
                     ((static_cast<char32_t>(utf8str[1]) << 6) & 0xfff) +
                     (static_cast<char32_t>(utf8str[2]) & 0x3f);
        break;
    case 4:
        code_point = ((code_point << 18) & 0x1fffff) +
                     ((static_cast<char32_t>(utf8str[1]) << 12) & 0x3ffff) +
                     ((static_cast<char32_t>(utf8str[2]) << 6) & 0xfff) +
                     (static_cast<char32_t>(utf8str[3]) & 0x3f);
        break;
    default:
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INVALID_LEAD};
    }

    // Decoding succeeded. Now, security checks...
    if (!is_code_point_valid(code_point))        {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INVALID_CODE_POINT};
    }
    if(IsOverlongSequence(code_point, length)) {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::OVERLONG_SEQUENCE};
    }

    // Success! Change utf8str to point to the remainder of the original sequence
    return DecodeData{
        .consumed=length,
        .code_point=code_point,
        .error_code=UTF_ERROR::OK
    };
}

// Assumes code_point has been validated; will be converted to REPLACEMENT_CHARACTER if not valid.
constexpr std::u8string EncodeUTF8(char32_t code_point) {
    if (!is_code_point_valid(code_point)) { code_point = REPLACEMENT_CHARACTER; }
    if (code_point < 0x80) {                     // 1 byte
        return std::u8string{{static_cast<char8_t>(code_point)}};
    } else if (code_point < 0x800) {             // 2 bytes
        return std::u8string{{
            static_cast<char8_t>((code_point >> 6)          | 0xc0),
            static_cast<char8_t>((code_point & 0x3f)        | 0x80)
        }};
    } else if (code_point < 0x10000) {           // 3 bytes
        return std::u8string{{
            static_cast<char8_t>((code_point >> 12)         | 0xe0),
            static_cast<char8_t>(((code_point >> 6) & 0x3f) | 0x80),
            static_cast<char8_t>((code_point & 0x3f)        | 0x80)
        }};
    }                                            // 4 bytes
    return std::u8string{{
        static_cast<char8_t>((code_point >> 18)             | 0xf0),
        static_cast<char8_t>(((code_point >> 12) & 0x3f)    | 0x80),
        static_cast<char8_t>(((code_point >> 6) & 0x3f)     | 0x80),
        static_cast<char8_t>((code_point & 0x3f)            | 0x80)
    }};
}

constexpr DecodeData DecodeUTF16(std::u16string_view utf16str) noexcept {
    if (utf16str.empty()) { return DecodeData{.error_code=UTF_ERROR::INCOMPLETE_SEQUENCE}; }

    const char16_t first_word = utf16str[0];
    if (!IsSurrogateUTF16(first_word)) {
        return DecodeData{
            .consumed=1,
            .code_point=static_cast<char32_t>(first_word),
            .error_code=UTF_ERROR::OK
        };
    }

    if (utf16str.length() < 2) {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INCOMPLETE_SEQUENCE};
    } else if (!IsLeadSurrogateUTF16(first_word)) {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INVALID_LEAD};
    }

    const char16_t second_word = utf16str[1];
    if (!IsTrailSurrogateUTF16(second_word)) {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INCOMPLETE_SEQUENCE};
    }

    char32_t code_point = SURROGATE_OFFSET +
                            (static_cast<char32_t>(first_word) << 10) +
                            static_cast<char32_t>(second_word);
    if (!is_code_point_valid(code_point)) {
        return DecodeData{.consumed=1, .error_code=UTF_ERROR::INVALID_CODE_POINT};
    }

    return DecodeData{
        .consumed=2,
        .code_point=code_point,
        .error_code=UTF_ERROR::OK
    };
}

// Assumes code_point has been validated; will be converted to REPLACEMENT_CHARACTER if not valid.
constexpr std::u16string EncodeUTF16(char32_t code_point) {
    if (!is_code_point_valid(code_point)) { code_point = REPLACEMENT_CHARACTER; }
    if (IsInBMP(code_point)) { return std::u16string{{static_cast<char16_t>(code_point)}}; }
    // Code points from the supplementary planes are encoded via surrogate pairs
    return std::u16string{{
        static_cast<char16_t>(LEAD_OFFSET + (code_point >> 10)),
        static_cast<char16_t>(TRAIL_SURROGATE_MIN + (code_point & 0x3FF))
    }};
}


} // namespace utfcpp
