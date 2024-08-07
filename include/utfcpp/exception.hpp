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


class Exception : public std::exception {};
class NotEnoughRoom : public Exception {};
class InvalidLead : public Exception {};
class IncompleteSequence : public Exception {};
class OverlongSequence : public Exception {};
class InvalidCodePoint : public Exception {};


class EncodingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


class DecodingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


auto OnDecodeErrorThrow_f = [](UTF_ERROR e) { throw DecodingError{ToString(e)}; };
auto OnEncodeErrorThrow_f = [](UTF_ERROR e) { throw EncodingError{ToString(e)}; };


} // namespace utfcpp
