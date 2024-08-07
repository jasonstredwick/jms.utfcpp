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

TEST(ViewTests, test_u8_view_construct)
{
    using namespace utfcpp;
    UTFView<char8_t> view_templated{u8"hello"};
    UTFView          view_deduced  {std::u8string_view{u8"hello"}};
    UTFView<char8_t, UTFInputIterator> view_templated2{u8"hello"};
    UTFView<char8_t, UTFInputIterator> view_templated3{std::u8string_view{u8"hello"}};

    utf8_view view_deduced2{u8"hello"};
    utf8_view view_deduced3{std::u8string_view{u8"hello"}};
    utf8_view<UTFInputIterator> view_templated4{u8"hello"};
    utf8_view<UTFInputIterator> view_templated5{std::u8string_view{u8"hello"}};

    utf16_view view_deduced4{u"hello"};
    utf16_view view_deduced5{std::u16string_view{u"hello"}};
    utf16_view<UTFInputIterator> view_templated6{u"hello"};
    utf16_view<UTFInputIterator> view_templated7{std::u16string_view{u"hello"}};

    utf32_view view_deduced6{U"hello"};
    utf32_view view_deduced7{std::u32string_view{U"hello"}};
    utf32_view<UTFInputIterator> view_templated8{U"hello"};
    utf32_view<UTFInputIterator> view_templated9{std::u32string_view{U"hello"}};

    std::u8string_view sv8{u8"abcdxyzшницла水手𐌀"};
    UTFView v8{sv8};
    EXPECT_EQ(sv8, v8.data());
    std::u16string_view sv16{u"abcdxyzшницла水手𐌀"};
    UTFView v16{sv16};
    EXPECT_EQ(sv16, v16.data());
    std::u32string_view sv32{U"abcdxyzшницла水手𐌀"};
    UTFView v32{sv32};
    EXPECT_EQ(sv32, v32.data());
}

TEST(ViewTests, test_u8_view_begin_end)
{
    using namespace utfcpp;
    std::u8string_view sv8{u8"a"};
    UTFView v8{sv8};
    EXPECT_EQ(*v8.begin(), U'a');
    EXPECT_FALSE(v8.begin() == v8.end());
    EXPECT_TRUE(++(v8.begin()) == v8.end());

    std::u16string_view sv16{u"a"};
    UTFView v16{sv16};
    EXPECT_EQ(*v16.begin(), U'a');
    EXPECT_FALSE(v16.begin() == v16.end());
    EXPECT_TRUE(++(v16.begin()) == v16.end());

    std::u32string_view sv32{U"a"};
    UTFView v32{sv32};
    EXPECT_EQ(*v32.begin(), U'a');
    EXPECT_FALSE(v32.begin() == v32.end());
    EXPECT_TRUE(++(v32.begin()) == v32.end());

    UTFView e1{std::u8string_view{}};
    EXPECT_TRUE(e1.begin() == e1.end());
    UTFView e2{std::u16string_view{}};
    EXPECT_TRUE(e2.begin() == e2.end());
    UTFView e3{std::u32string_view{}};
    EXPECT_TRUE(e3.begin() == e3.end());
}

TEST(ViewTests, test_u8_view_dims)
{
    using namespace utfcpp;
    UTFView e1{std::u8string_view{}};
    EXPECT_TRUE(e1.empty());
    EXPECT_FALSE(e1);
    EXPECT_EQ(e1.size(), 0);
    UTFView e2{std::u16string_view{}};
    EXPECT_TRUE(e2.empty());
    EXPECT_TRUE(!e2);
    EXPECT_EQ(e2.size(), 0);
    UTFView e3{std::u32string_view{}};
    EXPECT_TRUE(e3.empty());
    EXPECT_TRUE(!e3);
    EXPECT_EQ(e3.size(), 0);

    std::u8string_view sv1{u8"a"};
    UTFView ev1{sv1};
    EXPECT_FALSE(ev1.empty());
    EXPECT_TRUE(ev1);
    EXPECT_EQ(ev1.size(), sv1.size());

    std::u8string_view sv2{u8"abcdxyzшницла水手𐌀"};
    UTFView ev2{sv2};
    EXPECT_FALSE(ev2.empty());
    EXPECT_TRUE(ev2);
    EXPECT_EQ(ev2.size(), sv2.size());
}
