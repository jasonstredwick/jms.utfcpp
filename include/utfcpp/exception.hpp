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


#include <exception>
#include <stdexcept>

#include "utfcpp/core.hpp"


namespace utfcpp {


class exception : public ::std::exception {};
class not_enough_room : public utfcpp::exception {};
class invalid_lead : public utfcpp::exception {};
class incomplete_sequence : public utfcpp::exception {};
class overlong_sequence : public utfcpp::exception {};
class invalid_code_point : public utfcpp::exception {};


class encoding_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


class decoding_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


auto on_decode_error_throw_f = [](UTF_ERROR e) { throw decoding_error{utfcpp::to_string(e)}; };
auto on_encode_error_throw_f = [](UTF_ERROR e) { throw encoding_error{utfcpp::to_string(e)}; };


} // namespace utfcpp
