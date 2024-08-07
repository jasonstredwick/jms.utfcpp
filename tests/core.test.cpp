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

#include "utfcpp/utfcpp.hpp"
#include "ftest.h"

TEST(CoreTests, test_is_utf8_trail)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_utf8_trail('a'));
    EXPECT_FALSE(is_utf8_trail('\x7F'));

    EXPECT_TRUE(is_utf8_trail(static_cast<char8_t>('\x80')));
    EXPECT_TRUE(is_utf8_trail(static_cast<char8_t>('\x99')));
}

TEST(CoreTests, test_is_utf16_lead_surrogate)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'z'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\ud7ff'));

    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xd800'));
    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xdab0'));
    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xdbff'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\xdc00'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\xdfff'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\uefff'));
}

TEST(CoreTests, test_is_utf16_trail_surrogate)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'z'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\ud7ff'));

    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xd800'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xdbff'));

    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xdc00'));
    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xde02'));
    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xdfff'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xe000'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\uefff'));
}

TEST(CoreTests, test_is_utf16_surrogate)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_utf16_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_surrogate(U'z'));
    EXPECT_FALSE(is_utf16_surrogate(u'\ud7ff'));

    EXPECT_TRUE(is_utf16_surrogate(u'\xd800'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdbff'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdc00'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xde02'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdfff'));
    EXPECT_FALSE(is_utf16_surrogate(u'\xe000'));

    EXPECT_FALSE(is_utf16_surrogate(u'\uefff'));
    EXPECT_FALSE(is_utf16_surrogate(U'\U001012af'));
}

TEST(CoreTests, test_is_code_point_valid)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_code_point_valid(U'\xdbff'));
    EXPECT_FALSE(is_code_point_valid(U'\x11ffff'));

    EXPECT_TRUE(is_code_point_valid(U'\x80'));
    EXPECT_TRUE(is_code_point_valid(U'\x99'));
}

TEST(CoreTests, test_sequence_length)
{
    using namespace utfcpp;
    EXPECT_EQ(sequence_length(u8'Z'), 1);
    EXPECT_EQ(sequence_length(static_cast<char8_t>(0x79)), 1);
    EXPECT_EQ(sequence_length(static_cast<char8_t>(0xc2)), 2);
    EXPECT_EQ(sequence_length(static_cast<char8_t>(0xe0)), 3);
    EXPECT_EQ(sequence_length(static_cast<char8_t>(0xf0)), 4);
}

TEST(CoreTests, test_utf8_decode)
{
    using namespace utfcpp;
    char32_t cp = 0;

    std::u8string_view ascii{u8"abcdxyz"};
    DecodeData data = utf8_decode(ascii);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'a');
    EXPECT_EQ(data.consumed, 1);

    std::u8string_view cyrillic {u8"шницла"}; // "steak"
    data = utf8_decode(cyrillic);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'ш');
    EXPECT_EQ(data.consumed, 2);

    std::u8string_view chinese {u8"水手"}; // "sailor"
    data = utf8_decode(chinese);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'水');
    EXPECT_EQ(data.consumed, 3);

    std::u8string_view etruscan {u8"𐌀"};
    data = utf8_decode(etruscan);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'𐌀');
    EXPECT_EQ(data.consumed, 4);
}

TEST(CoreTests, test_utf8_encode)
{
    using namespace utfcpp;
    std::u8string utf8str{};

    utf8str = utf8_encode(U'a');
    EXPECT_EQ(utf8str, u8"a");

    utf8str = utf8_encode(U'ц');
    EXPECT_EQ(utf8str, u8"ц");

    utf8str = utf8_encode(U'水');
    EXPECT_EQ(utf8str, u8"水");

    utf8str = utf8_encode(U'𐌀');
    EXPECT_EQ(utf8str, u8"𐌀");
}

TEST(CoreTests, test_utf16_decode)
{
    using namespace utfcpp;
    char32_t cp = 0;

    std::u16string_view ascii{u"abcdxyz"};
    DecodeData data = utf16_decode(ascii);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'a');
    EXPECT_EQ(data.consumed, 1);

    std::u16string_view cyrillic {u"шницла"}; // "steak"
    data = utf16_decode(cyrillic);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'ш');
    EXPECT_EQ(data.consumed, 1);

    std::u16string_view etruscan {u"𐌀"};
    data = utf16_decode(etruscan);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'𐌀');
    EXPECT_EQ(data.consumed, 2);
}
