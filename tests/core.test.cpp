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

TEST(CoreTests, test_IsTrailUTF8)
{
    using namespace utfcpp;
    EXPECT_FALSE(IsTrailUTF8('a'));
    EXPECT_FALSE(IsTrailUTF8('\x7F'));

    EXPECT_TRUE(IsTrailUTF8(static_cast<char8_t>('\x80')));
    EXPECT_TRUE(IsTrailUTF8(static_cast<char8_t>('\x99')));
}

TEST(CoreTests, test_IsLeadSurrogateUTF16)
{
    using namespace utfcpp;
    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\u0000'));
    EXPECT_FALSE(IsLeadSurrogateUTF16(u'z'));
    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\ud7ff'));

    EXPECT_TRUE(IsLeadSurrogateUTF16(u'\xd800'));
    EXPECT_TRUE(IsLeadSurrogateUTF16(u'\xdab0'));
    EXPECT_TRUE(IsLeadSurrogateUTF16(u'\xdbff'));

    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\xdc00'));
    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\xdfff'));

    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\uefff'));
}

TEST(CoreTests, test_IsTrailSurrogateUTF16)
{
    using namespace utfcpp;
    EXPECT_FALSE(IsTrailSurrogateUTF16(u'\u0000'));
    EXPECT_FALSE(IsTrailSurrogateUTF16(u'z'));
    EXPECT_FALSE(IsTrailSurrogateUTF16(u'\ud7ff'));

    EXPECT_FALSE(IsTrailSurrogateUTF16(u'\xd800'));
    EXPECT_FALSE(IsTrailSurrogateUTF16(u'\xdbff'));

    EXPECT_TRUE(IsTrailSurrogateUTF16(u'\xdc00'));
    EXPECT_TRUE(IsTrailSurrogateUTF16(u'\xde02'));
    EXPECT_TRUE(IsTrailSurrogateUTF16(u'\xdfff'));
    EXPECT_FALSE(IsTrailSurrogateUTF16(u'\xe000'));

    EXPECT_FALSE(IsLeadSurrogateUTF16(u'\uefff'));
}

TEST(CoreTests, test_IsSurrogateUTF16)
{
    using namespace utfcpp;
    EXPECT_FALSE(IsSurrogateUTF16(u'\u0000'));
    EXPECT_FALSE(IsSurrogateUTF16(U'z'));
    EXPECT_FALSE(IsSurrogateUTF16(u'\ud7ff'));

    EXPECT_TRUE(IsSurrogateUTF16(u'\xd800'));
    EXPECT_TRUE(IsSurrogateUTF16(u'\xdbff'));
    EXPECT_TRUE(IsSurrogateUTF16(u'\xdc00'));
    EXPECT_TRUE(IsSurrogateUTF16(u'\xde02'));
    EXPECT_TRUE(IsSurrogateUTF16(u'\xdfff'));
    EXPECT_FALSE(IsSurrogateUTF16(u'\xe000'));

    EXPECT_FALSE(IsSurrogateUTF16(u'\uefff'));
    EXPECT_FALSE(IsSurrogateUTF16(U'\U001012af'));
}

TEST(CoreTests, test_is_code_point_valid)
{
    using namespace utfcpp;
    EXPECT_FALSE(is_code_point_valid(U'\xdbff'));
    EXPECT_FALSE(is_code_point_valid(U'\x11ffff'));

    EXPECT_TRUE(is_code_point_valid(U'\x80'));
    EXPECT_TRUE(is_code_point_valid(U'\x99'));
}

TEST(CoreTests, test_SequenceLength)
{
    using namespace utfcpp;
    EXPECT_EQ(SequenceLength(u8'Z'), 1);
    EXPECT_EQ(SequenceLength(static_cast<char8_t>(0x79)), 1);
    EXPECT_EQ(SequenceLength(static_cast<char8_t>(0xc2)), 2);
    EXPECT_EQ(SequenceLength(static_cast<char8_t>(0xe0)), 3);
    EXPECT_EQ(SequenceLength(static_cast<char8_t>(0xf0)), 4);
}

TEST(CoreTests, test_DecodeUTF8)
{
    using namespace utfcpp;
    char32_t cp = 0;

    std::u8string_view ascii{u8"abcdxyz"};
    DecodeData data = DecodeUTF8(ascii);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'a');
    EXPECT_EQ(data.consumed, 1);

    std::u8string_view cyrillic {u8"шницла"}; // "steak"
    data = DecodeUTF8(cyrillic);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'ш');
    EXPECT_EQ(data.consumed, 2);

    std::u8string_view chinese {u8"水手"}; // "sailor"
    data = DecodeUTF8(chinese);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'水');
    EXPECT_EQ(data.consumed, 3);

    std::u8string_view etruscan {u8"𐌀"};
    data = DecodeUTF8(etruscan);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'𐌀');
    EXPECT_EQ(data.consumed, 4);
}

TEST(CoreTests, test_EncodeUTF8)
{
    using namespace utfcpp;
    std::u8string utf8str{};

    utf8str = EncodeUTF8(U'a');
    EXPECT_EQ(utf8str, u8"a");

    utf8str = EncodeUTF8(U'ц');
    EXPECT_EQ(utf8str, u8"ц");

    utf8str = EncodeUTF8(U'水');
    EXPECT_EQ(utf8str, u8"水");

    utf8str = EncodeUTF8(U'𐌀');
    EXPECT_EQ(utf8str, u8"𐌀");
}

TEST(CoreTests, test_DecodeUTF16)
{
    using namespace utfcpp;
    char32_t cp = 0;

    std::u16string_view ascii{u"abcdxyz"};
    DecodeData data = DecodeUTF16(ascii);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'a');
    EXPECT_EQ(data.consumed, 1);

    std::u16string_view cyrillic {u"шницла"}; // "steak"
    data = DecodeUTF16(cyrillic);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'ш');
    EXPECT_EQ(data.consumed, 1);

    std::u16string_view etruscan {u"𐌀"};
    data = DecodeUTF16(etruscan);
    EXPECT_EQ(data.error_code, UTF_ERROR::OK);
    EXPECT_EQ(data.code_point, U'𐌀');
    EXPECT_EQ(data.consumed, 2);
}
