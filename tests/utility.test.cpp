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

TEST(UtilityTests, test_FindInvalid)
{
    using namespace utfcpp;
    EXPECT_EQ(FindInvalid(std::u8string_view{}), 0);
    EXPECT_EQ(FindInvalid(std::u16string_view{}), 0);
    EXPECT_EQ(FindInvalid(std::u32string_view{}), 0);

    std::u8string valid1{u8"abcdxyzшницла水手𐌀"};
    EXPECT_EQ(FindInvalid<char8_t>(valid1), valid1.size());
    std::u16string valid2{u"abcdxyzшницла水手𐌀"};
    EXPECT_EQ(FindInvalid<char16_t>(valid2), valid2.size());
    std::u32string valid3{U"abcdxyzшницла水手𐌀"};
    EXPECT_EQ(FindInvalid<char32_t>(valid3), valid3.size());

    // \xfa is invalid (unexpected continuation byte)
    std::u8string invalid1{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa}};
    std::u8string invalid2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa, 0xe6, 0x97, 0xa5}};
    // 0xdc07 is invalid (unexpected lead surrogate)
    std::u16string invalid3{{0x65e5, 0x0448, 0xdc07}};
    std::u16string invalid4{{0x65e5, 0x0448, 0xdc07, 0x65e5, 0x0448}};
    // 0x0011ffff is invalid; beyond last possible code point
    std::u32string invalid5{{0x000065e5, 0x00000448, 0x0011ffff}};
    std::u32string invalid6{{0x000065e5, 0x00000448, 0x0011ffff, 0x000065e5, 0x00000448}};

    EXPECT_EQ(FindInvalid<char8_t>(invalid1), 5);
    EXPECT_EQ(FindInvalid<char8_t>(invalid2), 5);
    EXPECT_EQ(FindInvalid<char16_t>(invalid3), 2);
    EXPECT_EQ(FindInvalid<char16_t>(invalid4), 2);
    EXPECT_EQ(FindInvalid<char32_t>(invalid5), 2);
    EXPECT_EQ(FindInvalid<char32_t>(invalid6), 2);
}

TEST(UtilityTests, test_is_invalid)
{
    using namespace utfcpp;
    EXPECT_TRUE(IsValid(std::u8string_view{}));
    EXPECT_TRUE(IsValid(std::u16string_view{}));
    EXPECT_TRUE(IsValid(std::u32string_view{}));

    std::u8string valid1{u8"abcdxyzшницла水手𐌀"};
    EXPECT_TRUE(IsValid<char8_t>(valid1));
    std::u16string valid2{u"abcdxyzшницла水手𐌀"};
    EXPECT_TRUE(IsValid<char16_t>(valid2));
    std::u32string valid3{U"abcdxyzшницла水手𐌀"};
    EXPECT_TRUE(IsValid<char32_t>(valid3));

    // \xfa is invalid (unexpected continuation byte)
    std::u8string invalid1{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa}};
    std::u8string invalid2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa, 0xe6, 0x97, 0xa5}};
    // 0xdc07 is invalid (unexpected lead surrogate)
    std::u16string invalid3{{0x65e5, 0x0448, 0xdc07}};
    std::u16string invalid4{{0x65e5, 0x0448, 0xdc07, 0x65e5, 0x0448}};
    // 0x0011ffff is invalid; beyond last possible code point
    std::u32string invalid5{{0x000065e5, 0x00000448, 0x0011ffff}};
    std::u32string invalid6{{0x000065e5, 0x00000448, 0x0011ffff, 0x000065e5, 0x00000448}};

    EXPECT_FALSE(IsValid<char8_t>(invalid1));
    EXPECT_FALSE(IsValid<char8_t>(invalid2));
    EXPECT_FALSE(IsValid<char16_t>(invalid3));
    EXPECT_FALSE(IsValid<char16_t>(invalid4));
    EXPECT_FALSE(IsValid<char32_t>(invalid5));
    EXPECT_FALSE(IsValid<char32_t>(invalid6));
}

/***
 * Test UTFConvertTo
 * NOTE: Need to include more codepoints that test u16 surrogate values.
 */
TEST(UtilityTests, test_utf8_to_8)
{
    using namespace utfcpp;
    EXPECT_EQ(utf8_to_8(u8""), u8"");
    EXPECT_EQ(utf8_to_8(u8"л"), u8"л");
    EXPECT_EQ(utf8_to_8(u8"aл"), u8"aл");
    EXPECT_EQ(utf8_to_8(u8"abcdxyzшницла水手𐌀"), u8"abcdxyzшницла水手𐌀");

    std::u8string invalid{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa}};
    std::u8string final{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd}};
    EXPECT_EQ(utf8_to_8(invalid), final);

    std::u8string invalid2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa, 0xe6, 0x97, 0xa5}};
    std::u8string final2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd, 0xe6, 0x97, 0xa5}};
    EXPECT_EQ(utf8_to_8(invalid2), final2);
}

TEST(UtilityTests, test_utf8_to_16)
{
    using namespace utfcpp;
    EXPECT_EQ(utf8_to_16(u8""), u"");
    EXPECT_EQ(utf8_to_16(u8"л"), u"л");
    EXPECT_EQ(utf8_to_16(u8"aл"), u"aл");
    EXPECT_EQ(utf8_to_16(u8"abcdxyzшницла水手𐌀"), u"abcdxyzшницла水手𐌀");

    // \xfa is invalid (unexpected continuation byte)
    std::u8string invalid{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa}};
    std::u16string final{{0x65e5, 0x0448, 0xfffd}};
    EXPECT_EQ(utf8_to_16(invalid), final);

    std::u8string invalid2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa, 0xe6, 0x97, 0xa5}};
    std::u16string final2{{0x65e5, 0x0448, 0xfffd, 0x65e5}};
    EXPECT_EQ(utf8_to_16(invalid2), final2);
}

TEST(UtilityTests, test_utf8_to_32)
{
    using namespace utfcpp;
    EXPECT_EQ(utf8_to_32(u8""), U"");
    EXPECT_EQ(utf8_to_32(u8"л"), U"л");
    EXPECT_EQ(utf8_to_32(u8"aл"), U"aл");
    EXPECT_EQ(utf8_to_32(u8"abcdxyzшницла水手𐌀"), U"abcdxyzшницла水手𐌀");

    // \xfa is invalid (unexpected continuation byte)
    std::u8string invalid{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa}};
    std::u32string final{{0x000065e5, 0x00000448, 0x0000fffd}};
    EXPECT_EQ(utf8_to_32(invalid), final);

    std::u8string invalid2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xfa, 0xe6, 0x97, 0xa5}};
    std::u32string final2{{0x000065e5, 0x00000448, 0x0000fffd, 0x000065e5}};
    EXPECT_EQ(utf8_to_32(invalid2), final2);
}

TEST(UtilityTests, test_utf16_to_8)
{
    using namespace utfcpp;
    EXPECT_EQ(utf16_to_8(u""), u8"");
    EXPECT_EQ(utf16_to_8(u"л"), u8"л");
    EXPECT_EQ(utf16_to_8(u"aл"), u8"aл");
    EXPECT_EQ(utf16_to_8(u"abcdxyzшницла水手𐌀"), u8"abcdxyzшницла水手𐌀");

    // 0xdc07 is invalid (unexpected lead surrogate)
    std::u16string invalid{{0x65e5, 0x0448, 0xdc07}};
    std::u8string final{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd}};
    EXPECT_EQ(utf16_to_8(invalid), final);

    std::u16string invalid2{{0x65e5, 0x0448, 0xdc07, 0x65e5, 0x0448}};
    std::u8string final2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd, 0xe6, 0x97, 0xa5, 0xd1, 0x88}};
    EXPECT_EQ(utf16_to_8(invalid2), final2);
}

TEST(UtilityTests, test_utf16_to_16)
{
    using namespace utfcpp;
    EXPECT_EQ(utf16_to_16(u""), u"");
    EXPECT_EQ(utf16_to_16(u"л"), u"л");
    EXPECT_EQ(utf16_to_16(u"aл"), u"aл");
    EXPECT_EQ(utf16_to_16(u"abcdxyzшницла水手𐌀"), u"abcdxyzшницла水手𐌀");

    // 0xdc07 is invalid (unexpected lead surrogate)
    std::u16string invalid{{0x65e5, 0x0448, 0xdc07}};
    std::u16string final{{0x65e5, 0x0448, 0xfffd}};
    EXPECT_EQ(utf16_to_16(invalid), final);

    std::u16string invalid2{{0x65e5, 0x0448, 0xdc07, 0x65e5, 0x0448}};
    std::u16string final2{{0x65e5, 0x0448, 0xfffd, 0x65e5, 0x0448}};
    EXPECT_EQ(utf16_to_16(invalid2), final2);
}

TEST(UtilityTests, test_utf16_to_32)
{
    using namespace utfcpp;
    EXPECT_EQ(utf16_to_32(u""), U"");
    EXPECT_EQ(utf16_to_32(u"л"), U"л");
    EXPECT_EQ(utf16_to_32(u"aл"), U"aл");
    EXPECT_EQ(utf16_to_32(u"abcdxyzшницла水手𐌀"), U"abcdxyzшницла水手𐌀");

    // 0xdc07 is invalid (unexpected lead surrogate)
    std::u16string invalid{{0x65e5, 0x0448, 0xdc07}};
    std::u32string final{{0x000065e5, 0x00000448, 0x0000fffd}};
    EXPECT_EQ(utf16_to_32(invalid), final);

    std::u16string invalid2{{0x65e5, 0x0448, 0xdc07, 0x65e5, 0x0448}};
    std::u32string final2{{0x000065e5, 0x00000448, 0x0000fffd, 0x000065e5, 0x00000448}};
    EXPECT_EQ(utf16_to_32(invalid2), final2);
}

TEST(UtilityTests, test_utf32_to_8)
{
    using namespace utfcpp;
    EXPECT_EQ(utf32_to_8(U""), u8"");
    EXPECT_EQ(utf32_to_8(U"л"), u8"л");
    EXPECT_EQ(utf32_to_8(U"aл"), u8"aл");
    EXPECT_EQ(utf32_to_8(U"abcdxyzшницла水手𐌀"), u8"abcdxyzшницла水手𐌀");

    // 0x0011ffff is invalid; beyond last possible code point
    std::u32string invalid{{0x000065e5, 0x00000448, 0x0011ffff}};
    std::u8string final{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd}};
    EXPECT_EQ(utf32_to_8(invalid), final);

    std::u32string invalid2{{0x000065e5, 0x00000448, 0x0011ffff, 0x000065e5, 0x00000448}};
    std::u8string final2{{0xe6, 0x97, 0xa5, 0xd1, 0x88, 0xef, 0xbf, 0xbd, 0xe6, 0x97, 0xa5, 0xd1, 0x88}};
    EXPECT_EQ(utf32_to_8(invalid2), final2);
}

TEST(UtilityTests, test_utf32_to_16)
{
    using namespace utfcpp;
    EXPECT_EQ(utf32_to_16(U""), u"");
    EXPECT_EQ(utf32_to_16(U"л"), u"л");
    EXPECT_EQ(utf32_to_16(U"aл"), u"aл");
    EXPECT_EQ(utf32_to_16(U"abcdxyzшницла水手𐌀"), u"abcdxyzшницла水手𐌀");

    // 0x0011ffff is invalid; beyond last possible code point
    std::u32string invalid{{0x000065e5, 0x00000448, 0x0011ffff}};
    std::u16string final{{0x65e5, 0x0448, 0xfffd}};
    EXPECT_EQ(utf32_to_16(invalid), final);

    std::u32string invalid2{{0x000065e5, 0x00000448, 0x0011ffff, 0x000065e5, 0x00000448}};
    std::u16string final2{{0x65e5, 0x0448, 0xfffd, 0x65e5, 0x0448}};
    EXPECT_EQ(utf32_to_16(invalid2), final2);
}

TEST(UtilityTests, test_utf32_to_32)
{
    using namespace utfcpp;
    EXPECT_EQ(utf32_to_32(U""), U"");
    EXPECT_EQ(utf32_to_32(U"л"), U"л");
    EXPECT_EQ(utf32_to_32(U"aл"), U"aл");
    EXPECT_EQ(utf32_to_32(U"abcdxyzшницла水手𐌀"), U"abcdxyzшницла水手𐌀");

    // 0x0011ffff is invalid; beyond last possible code point
    std::u32string invalid{{0x000065e5, 0x00000448, 0x0011ffff}};
    std::u32string final{{0x000065e5, 0x00000448, 0x0000fffd}};
    EXPECT_EQ(utf32_to_32(invalid), final);

    std::u32string invalid2{{0x000065e5, 0x00000448, 0x0011ffff, 0x000065e5, 0x00000448}};
    std::u32string final2{{0x000065e5, 0x00000448, 0x0000fffd, 0x000065e5, 0x00000448}};
    EXPECT_EQ(utf32_to_32(invalid2), final2);
}
