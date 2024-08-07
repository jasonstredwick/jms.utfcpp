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

#include <algorithm>
#include <tuple>

#include "utfcpp/utfcpp.hpp"
#include "ftest.h"

TEST(IteratorTests, code_point_append_iterator)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    std::u32string out{};
    out.reserve(16);
    auto out_it = code_point_append_iterator(out);
    bool found_error = false;
    for (size_t i=0; i<sv8.size(); ++out_it) {
        DecodeData data = utf8_decode(std::u8string_view{sv8.begin() + i, sv8.end()});
        *out_it = data.code_point;
        if (data.error_code != UTF_ERROR::OK) { found_error = true; }
        i += data.consumed;
    }
    EXPECT_FALSE(found_error);
    EXPECT_EQ(out, std::u32string{U"abcdxyzшницла水手𐌀"});
    EXPECT_EQ(out.size(), 16);

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    std::u32string out2{};
    auto out2_it = code_point_append_iterator(out2);
    for (size_t i=0; i<sv16.size(); ++out2_it) {
        DecodeData data = utf16_decode(std::u16string_view{sv16.begin() + i, sv16.end()});
        *out2_it = data.code_point;
        i += data.consumed;
    }
    EXPECT_EQ(out2, std::u32string{U"abcdxyzшницла水手𐌀"});

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    std::u32string out3{};
    auto out3_it = code_point_append_iterator(out3);
    for (size_t i=0; i<sv32.size(); ++out3_it) {
        *out3_it = sv32[i];
        i += 1;
    }
    EXPECT_EQ(out3, std::u32string{U"abcdxyzшницла水手𐌀"});
    auto out4_it = code_point_append_iterator(out3);
    for (size_t i=0; i<sv32.size(); ++out4_it) {
        *out4_it = sv32[i];
        i += 1;
    }
    EXPECT_EQ(out3, std::u32string{U"abcdxyzшницла水手𐌀abcdxyzшницла水手𐌀"});
    for (size_t i=0; i<sv32.size(); ++out4_it) {
        *out4_it = sv32[i];
        i += 1;
    }
    EXPECT_EQ(out3, std::u32string{U"abcdxyzшницла水手𐌀abcdxyzшницла水手𐌀abcdxyzшницла水手𐌀"});
}

TEST(IteratorTests, utf_input_iterator_default_construct)
{
    using namespace utfcpp;
    utf_input_iterator<char8_t> it8{};
    EXPECT_EQ(it8.data().size(), 0);
    utf_input_iterator<char16_t> it16{};
    EXPECT_EQ(it16.data().size(), 0);
    utf_input_iterator<char32_t> it32{};
    EXPECT_EQ(it32.data().size(), 0);
}

TEST(IteratorTests, utf_input_iterator_view_construct)
{
    using namespace utfcpp;
    EXPECT_EQ(utf_input_iterator{std::u8string_view{}}.data().size(), 0);
    EXPECT_EQ(utf_input_iterator{std::u16string_view{}}.data().size(), 0);
    EXPECT_EQ(utf_input_iterator{std::u32string_view{}}.data().size(), 0);

    EXPECT_EQ(utf_input_iterator{std::u8string_view{u8""}}.data().size(), 0);
    EXPECT_EQ(utf_input_iterator{std::u16string_view{u""}}.data().size(), 0);
    EXPECT_EQ(utf_input_iterator{std::u32string_view{U""}}.data().size(), 0);

    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    EXPECT_EQ(it8.data().size(), sv8.size());
    EXPECT_EQ(it8.data(), sv8);
    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    EXPECT_EQ(it16.data().size(), sv16.size());
    EXPECT_EQ(it16.data(), sv16);
    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    EXPECT_EQ(it32.data().size(), sv32.size());
    EXPECT_EQ(it32.data(), sv32);
}

TEST(IteratorTests, utf_input_iterator_pre_inc)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    for (int count=0; count < 15; ++count) ++it8;
    EXPECT_TRUE(it8.data().size() > 0);
    ++it8;
    EXPECT_EQ(it8.data().size(), 0);
    ++it8;
    EXPECT_EQ(it8.data().size(), 0);

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    for (int count=0; count < 15; ++count) ++it16;
    EXPECT_TRUE(it16.data().size() > 0);
    ++it16;
    EXPECT_EQ(it16.data().size(), 0);
    ++it16;
    EXPECT_EQ(it16.data().size(), 0);

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    for (int count=0; count < 15; ++count) ++it32;
    EXPECT_TRUE(it32.data().size() > 0);
    ++it32;
    EXPECT_EQ(it32.data().size(), 0);
    ++it32;
    EXPECT_EQ(it32.data().size(), 0);
}

TEST(IteratorTests, utf_input_iterator_post_inc)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    auto tmp8 = it8++;
    EXPECT_EQ(tmp8.data().size(), sv8.size());
    EXPECT_TRUE(tmp8.data().size() > it8.data().size());
    for (int count=1; count < 15; ++count) it8++;
    EXPECT_TRUE(it8.data().size() > 0);
    it8++;
    EXPECT_EQ(it8.data().size(), 0);
    it8++;
    EXPECT_EQ(it8.data().size(), 0);

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    auto tmp16 = it16++;
    EXPECT_EQ(tmp16.data().size(), sv16.size());
    EXPECT_TRUE(tmp16.data().size() > it16.data().size());
    for (int count=1; count < 15; ++count) it16++;
    EXPECT_TRUE(it16.data().size() > 0);
    it16++;
    EXPECT_EQ(it16.data().size(), 0);
    it16++;
    EXPECT_EQ(it16.data().size(), 0);

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    auto tmp32 = it32++;
    EXPECT_EQ(tmp32.data().size(), sv32.size());
    EXPECT_TRUE(tmp32.data().size() > it32.data().size());
    for (int count=1; count < 15; ++count) it32++;
    EXPECT_TRUE(it32.data().size() > 0);
    it32++;
    EXPECT_EQ(it32.data().size(), 0);
    it32++;
    EXPECT_EQ(it32.data().size(), 0);
}

TEST(IteratorTests, utf_input_iterator_dereference)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    EXPECT_EQ(*it8, U'a'); ++it8;
    EXPECT_EQ(*it8, U'b'); ++it8;
    EXPECT_EQ(*it8, U'c'); ++it8;
    EXPECT_EQ(*it8, U'd'); ++it8;
    EXPECT_EQ(*it8, U'x'); ++it8;
    EXPECT_EQ(*it8, U'y'); ++it8;
    EXPECT_EQ(*it8, U'z'); ++it8;
    EXPECT_EQ(*it8, U'ш'); ++it8;
    EXPECT_EQ(*it8, U'н'); ++it8;
    EXPECT_EQ(*it8, U'и'); ++it8;
    EXPECT_EQ(*it8, U'ц'); ++it8;
    EXPECT_EQ(*it8, U'л'); ++it8;
    EXPECT_EQ(*it8, U'а'); ++it8;
    EXPECT_EQ(*it8, U'水'); ++it8;
    EXPECT_EQ(*it8, U'手'); ++it8;
    EXPECT_EQ(*it8, U'𐌀'); ++it8;
    EXPECT_EQ(*it8, REPLACEMENT_CHARACTER); ++it8;
    EXPECT_EQ(*it8, REPLACEMENT_CHARACTER);

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    EXPECT_EQ(*it16, U'a'); ++it16;
    EXPECT_EQ(*it16, U'b'); ++it16;
    EXPECT_EQ(*it16, U'c'); ++it16;
    EXPECT_EQ(*it16, U'd'); ++it16;
    EXPECT_EQ(*it16, U'x'); ++it16;
    EXPECT_EQ(*it16, U'y'); ++it16;
    EXPECT_EQ(*it16, U'z'); ++it16;
    EXPECT_EQ(*it16, U'ш'); ++it16;
    EXPECT_EQ(*it16, U'н'); ++it16;
    EXPECT_EQ(*it16, U'и'); ++it16;
    EXPECT_EQ(*it16, U'ц'); ++it16;
    EXPECT_EQ(*it16, U'л'); ++it16;
    EXPECT_EQ(*it16, U'а'); ++it16;
    EXPECT_EQ(*it16, U'水'); ++it16;
    EXPECT_EQ(*it16, U'手'); ++it16;
    EXPECT_EQ(*it16, U'𐌀'); ++it16;
    EXPECT_EQ(*it16, REPLACEMENT_CHARACTER); ++it16;
    EXPECT_EQ(*it16, REPLACEMENT_CHARACTER);

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    EXPECT_EQ(*it32, U'a'); ++it32;
    EXPECT_EQ(*it32, U'b'); ++it32;
    EXPECT_EQ(*it32, U'c'); ++it32;
    EXPECT_EQ(*it32, U'd'); ++it32;
    EXPECT_EQ(*it32, U'x'); ++it32;
    EXPECT_EQ(*it32, U'y'); ++it32;
    EXPECT_EQ(*it32, U'z'); ++it32;
    EXPECT_EQ(*it32, U'ш'); ++it32;
    EXPECT_EQ(*it32, U'н'); ++it32;
    EXPECT_EQ(*it32, U'и'); ++it32;
    EXPECT_EQ(*it32, U'ц'); ++it32;
    EXPECT_EQ(*it32, U'л'); ++it32;
    EXPECT_EQ(*it32, U'а'); ++it32;
    EXPECT_EQ(*it32, U'水'); ++it32;
    EXPECT_EQ(*it32, U'手'); ++it32;
    EXPECT_EQ(*it32, U'𐌀'); ++it32;
    EXPECT_EQ(*it32, REPLACEMENT_CHARACTER); ++it32;
    EXPECT_EQ(*it32, REPLACEMENT_CHARACTER);

    // Empty iterator should return REPLACEMENT_CHARACTER
    utf_input_iterator it82{std::u8string_view{}};
    EXPECT_EQ(*it82, REPLACEMENT_CHARACTER);
    utf_input_iterator it162{std::u16string_view{}};
    EXPECT_EQ(*it162, REPLACEMENT_CHARACTER);
    utf_input_iterator it322{std::u32string_view{}};
    EXPECT_EQ(*it322, REPLACEMENT_CHARACTER);

    // Invalid characters should return REPLACEMENT_CHARACTER
    char8_t cdata8[1] = {static_cast<char8_t>(0xf9)};
    std::u8string str8{cdata8};
    utf_input_iterator it83{std::u8string_view{str8}};
    EXPECT_EQ(*it83, REPLACEMENT_CHARACTER);
    char16_t cdata16[1] = {static_cast<char16_t>(0xdf11)};
    std::u16string str16{cdata16};
    utf_input_iterator it163{std::u16string_view{str16}};
    EXPECT_EQ(*it163, REPLACEMENT_CHARACTER);
    char32_t cdata32[1] = {static_cast<char32_t>(0x001fffff)};
    std::u32string str32{cdata32};
    utf_input_iterator it323{std::u32string_view{str32}};
    EXPECT_EQ(*it323, REPLACEMENT_CHARACTER);
}

TEST(IteratorTests, utf_input_iterator_decode)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    auto F = [](char32_t c, UTF_ERROR e) -> std::tuple<char32_t, UTF_ERROR> { return {c, e}; };
    EXPECT_EQ(it8.decode(), F(U'a', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'b', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'c', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'd', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'x', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'y', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'z', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'ш', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'н', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'и', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'ц', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'л', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'а', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'水', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'手', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(U'𐌀', UTF_ERROR::OK)); ++it8;
    EXPECT_EQ(it8.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT)); ++it8;
    EXPECT_EQ(it8.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    EXPECT_EQ(it16.decode(), F(U'a', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'b', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'c', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'd', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'x', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'y', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'z', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'ш', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'н', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'и', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'ц', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'л', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'а', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'水', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'手', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(U'𐌀', UTF_ERROR::OK)); ++it16;
    EXPECT_EQ(it16.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT)); ++it16;
    EXPECT_EQ(it16.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    EXPECT_EQ(it32.decode(), F(U'a', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'b', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'c', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'd', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'x', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'y', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'z', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'ш', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'н', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'и', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'ц', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'л', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'а', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'水', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'手', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(U'𐌀', UTF_ERROR::OK)); ++it32;
    EXPECT_EQ(it32.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT)); ++it32;
    EXPECT_EQ(it32.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));

    // Empty iterator should return REPLACEMENT_CHARACTER
    utf_input_iterator it82{std::u8string_view{}};
    EXPECT_EQ(it82.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));
    utf_input_iterator it162{std::u16string_view{}};
    EXPECT_EQ(it162.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));
    utf_input_iterator it322{std::u32string_view{}};
    EXPECT_EQ(it322.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));

    // Invalid characters should return REPLACEMENT_CHARACTER
    char8_t cdata8[1] = {static_cast<char8_t>(0xf9)};
    std::u8string str8{cdata8};
    utf_input_iterator it83{std::u8string_view{str8}};
    EXPECT_EQ(it83.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_LEAD));
    char16_t cdata16[1] = {static_cast<char16_t>(0xdf11)};
    std::u16string str16{cdata16};
    utf_input_iterator it163{std::u16string_view{str16}};
    EXPECT_EQ(it163.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_LEAD));
    char32_t cdata32[1] = {static_cast<char32_t>(0x001fffff)};
    std::u32string str32{cdata32};
    utf_input_iterator it323{std::u32string_view{str32}};
    EXPECT_EQ(it323.decode(), F(REPLACEMENT_CHARACTER, UTF_ERROR::INVALID_CODE_POINT));
}

TEST(IteratorTests, utf_input_iterator_decode_error)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    utf_input_iterator it8{sv8};
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::OK); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::INVALID_CODE_POINT); ++it8;
    EXPECT_EQ(it8.decode_error(), UTF_ERROR::INVALID_CODE_POINT);

    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    utf_input_iterator it16{sv16};
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::OK); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::INVALID_CODE_POINT); ++it16;
    EXPECT_EQ(it16.decode_error(), UTF_ERROR::INVALID_CODE_POINT);

    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    utf_input_iterator it32{sv32};
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::OK); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::INVALID_CODE_POINT); ++it32;
    EXPECT_EQ(it32.decode_error(), UTF_ERROR::INVALID_CODE_POINT);

    // Empty iterator should return UTF_ERROR::INVALID_CODE_POINT
    utf_input_iterator it82{std::u8string_view{}};
    EXPECT_EQ(it82.decode_error(), UTF_ERROR::INVALID_CODE_POINT);
    utf_input_iterator it162{std::u16string_view{}};
    EXPECT_EQ(it162.decode_error(), UTF_ERROR::INVALID_CODE_POINT);
    utf_input_iterator it322{std::u32string_view{}};
    EXPECT_EQ(it322.decode_error(), UTF_ERROR::INVALID_CODE_POINT);

    // Invalid characters
    char8_t cdata8[1] = {static_cast<char8_t>(0xf9)};
    std::u8string str8{cdata8};
    utf_input_iterator it83{std::u8string_view{str8}};
    EXPECT_EQ(it83.decode_error(), UTF_ERROR::INVALID_LEAD);
    char16_t cdata16[1] = {static_cast<char16_t>(0xdf11)};
    std::u16string str16{cdata16};
    utf_input_iterator it163{std::u16string_view{str16}};
    EXPECT_EQ(it163.decode_error(), UTF_ERROR::INVALID_LEAD);
    char32_t cdata32[1] = {static_cast<char32_t>(0x001fffff)};
    std::u32string str32{cdata32};
    utf_input_iterator it323{std::u32string_view{str32}};
    EXPECT_EQ(it323.decode_error(), UTF_ERROR::INVALID_CODE_POINT);
}

TEST(IteratorTests, utf_input_iterator_sentinel)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"a"};
    utf_input_iterator it8{sv8};
    EXPECT_FALSE(decltype(it8)::sentinel{} == it8);;
    EXPECT_FALSE(it8 == decltype(it8)::sentinel{}); ++it8;
    EXPECT_TRUE(it8 == decltype(it8)::sentinel{});
    EXPECT_TRUE(decltype(it8)::sentinel{} == it8);

    std::u16string_view sv16{u"a"};
    utf_input_iterator it16{sv16};
    EXPECT_FALSE(decltype(it16)::sentinel{} == it16);;
    EXPECT_FALSE(it16 == decltype(it16)::sentinel{}); ++it16;
    EXPECT_TRUE(it16 == decltype(it16)::sentinel{});
    EXPECT_TRUE(decltype(it16)::sentinel{} == it16);

    std::u32string_view sv32{U"a"};
    utf_input_iterator it32{sv32};
    EXPECT_FALSE(decltype(it32)::sentinel{} == it32);;
    EXPECT_FALSE(it32 == decltype(it32)::sentinel{}); ++it32;
    EXPECT_TRUE(it32 == decltype(it32)::sentinel{});
    EXPECT_TRUE(decltype(it32)::sentinel{} == it32);
}
