# string_view lite: A single-file header-only version of a C++17-like string_view for C++98, C++11 and later

[![Language](https://img.shields.io/badge/C%2B%2B-98/11/14/17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/badge/license-BSL-blue.svg)](https://opensource.org/licenses/BSL-1.0) [![Build Status](https://travis-ci.org/martinmoene/string-view-lite.svg?branch=master)](https://travis-ci.org/martinmoene/string-view-lite) [![Build status](https://ci.appveyor.com/api/projects/status/1ha3wnxtam547m8p?svg=true)](https://ci.appveyor.com/project/martinmoene/string-view-lite) [![Version](https://badge.fury.io/gh/martinmoene%2Fstring-view-lite.svg)](https://github.com/martinmoene/string-view-lite/releases) [![download](https://img.shields.io/badge/latest-download-blue.svg)](https://github.com/martinmoene/string-view-lite/blob/master/include/nonstd/string_view.hpp) [![Conan](https://img.shields.io/badge/on-conan-blue.svg)](https://bintray.com/agauniyal/nonstd-lite/) [![Try it on wandbox](https://img.shields.io/badge/on-wandbox-blue.svg)](https://wandbox.org/permlink/w2vUu56NVdVNyOiv) [![Try it on godbolt online](https://img.shields.io/badge/on-godbolt-blue.svg)](http://bit.ly/mm-string-view-lite-godbolt)

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Synopsis](#synopsis)
- [Reported to work with](#reported-to-work-with)
- [Building the tests](#building-the-tests)
- [Other implementations of string_view](#other-implementations-of-string_view)
- [Notes and references](#notes-and-references)
- [Appendix](#appendix)


Example usage
-------------

```C++
#include "nonstd/string_view.hpp"
#include <iostream>

using namespace std::literals;
using namespace nonstd::literals;
using namespace nonstd;
    
void write( string_view sv )
{
    std::cout << sv;
}

int main()
{
    write( "hello"     );   // C-string
    write( ", "s       );   // std::string
    write( "world!"_sv );   // nonstd::string_view
}
```

### Compile and run
```
prompt> g++ -Wall -std=c++14 -I../include/ -o 01-basic.exe 01-basic.cpp && 01-basic.exe
hello, world!
```

In a nutshell
---------------
**string-view lite** is a single-file header-only library to provide a non-owning reference to a string. The library provides a [C++17-like string_view](http://en.cppreference.com/w/cpp/string/basic_string_view) for use with C++98 and later. If available, `std::string_view` is used, unless [configured otherwise](#configuration).

**Features and properties of string-view lite** are ease of installation (single header), freedom of dependencies other than the standard library. To mimic C++17-like cooperation with `std::string`, `nonstd::string_view` provides several non-standard conversion functions. These functions may be [omitted via configuration](#configuration).


License
-------
*string-view lite* is distributed under the [Boost Software License](LICENSE.txt).
 

Dependencies
------------
*string-view lite* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------

*string-view lite* is a single-file header-only library. Put `string_view.hpp` in the [include](include) folder directly into the project source tree or somewhere reachable from your project.

Or, if you use the [conan package manager](https://www.conan.io/), follow these steps:

1. Add *nonstd-lite* to the conan remotes:

        conan remote add nonstd-lite https://api.bintray.com/conan/martinmoene/nonstd-lite

2. Add a reference to *string-view-lite* to the *requires* section of your project's `conanfile.txt` file:

        [requires]
        string-view-lite/[~=1]@nonstd-lite/testing

3. Run conan's install command:

        conan install


Synopsis
--------

**Contents**  
[Documentation of `std::string_view`](#stdstring_view)  
[C++20 extensions](#c20-extensions)  
[Non-standard extensions](#non-standard-extensions)  
[Configuration](#configuration)    

Documentation of `std::string_view`
-----------------------------------
Depending on the compiler and C++-standard used, `nonstd::string_view` behaves less or more like `std::string_view`. To get an idea of the capabilities of `nonstd::string_view` with your configuration, look at the output of the [tests](test/string_view.t.cpp), issuing `string-view-lite.t --pass @`. For `std::string_view`, see its [documentation at cppreference](http://en.cppreference.com/w/cpp/string/basic_string_view).  

C++20 extensions
----------------
*string_view-lite* provides the following C++20 *extensions*.

- *[[nodiscard]]* constexpr bool **empty**() const noexcept;
- constexpr bool ***starts_with***( basic_string_view v ) const noexcept;  // (1)
- constexpr bool ***starts_with***( CharT c ) const noexcept;  // (2)
- constexpr bool ***starts_with***( CharT const * s ) const;  // (3)
- constexpr bool ***ends_with***( basic_string_view v ) const noexcept;  // (1)
- constexpr bool ***ends_with***( CharT c ) const noexcept;  // (2)
- constexpr bool ***ends_with***( CharT const * s ) const;  // (3)

Note: [[nodiscard]], constexpr and noexcept if available.
     
Non-standard extensions
-----------------------
### `string_view` literals `sv` and `_sv`
clang compilers do not allow to write `auto sv = "..."sv` with *string-view lite* under C++11. To still provide a literal operator that can be used in that case, *string-view lite* also provides `_sv`. See section [Configuration](#configuration) for how to control the presence of these operators.

The literal operators are declared in the namespace `nonstd::literals::string_view_literals`, where both `literals` and `string_view_literals` are inline namespaces, if supported. Access to these operators can be gained with using namespace `nonstd::literals`, using namespace `nonstd::string_view_literals`, and using namespace `nonstd::literals::string_view_literals`. If inline namespaces are not supported by the compiler, only the latter form is available.

### Cooperation between `std::string` and `nonstd::string_view`
*string-view lite* can provide several methods and free functions to mimic the cooperation between `std::string` and  `nonstd::string_view` that exists in C++17. See the table below. Several macros allow you to control the presence of these functions, see section [Configuration](#configuration).

| Kind                  | Std   | Function or method |                                       
|-----------------------|-------|--------------------|
| **Free functions**    |&nbsp; | macro `nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS`|
| **`std::string_view`**|&nbsp; | &nbsp; |
| to_string()           |>=C++17| template&lt; class CharT, class Traits, class Allocator=std::allocator&lt;CharT> ><br>std::basic_string&lt;CharT, Traits, Allocator&gt;<br>**to_string**( std::basic_string_view&lt;CharT, Traits> v, Allocator const & a=Allocator() );|
| to_string_view()      |>=C++17| template&lt; class CharT, class Traits, class Allocator ><br>std::basic_string_view&lt;CharT, Traits><br>**to_string_view**( std::basic_string&lt;CharT, Traits, Allocator> const & s );|
| **`nonstd::string_view`**|&nbsp; | &nbsp;  |
| to_string()<br>&nbsp;<br>*non-msvc14 (vs2015)* |>=C++11| template&lt; class CharT, class Traits, class Allocator = std::allocator&lt;CharT> ><br>std::basic_string&lt;CharT, Traits, Allocator><br>**to_string**( basic_string_view&lt;CharT, Traits> v, Allocator const & a = Allocator() );|
| to_string()<br>&nbsp;<br>*msvc14 (vs2015)* |<C++11|template&lt; class CharT, class Traits ><br>std::basic_string&lt;CharT, Traits><br>**to_string**( basic_string_view&lt;CharT, Traits> v );|
| to_string()<br>&nbsp;<br>*msvc14 (vs2015)* |<C++11|template&lt; class CharT, class Traits, class Allocator ><br>std::basic_string&lt;CharT, Traits, Allocator><br>**to_string**( basic_string_view&lt;CharT, Traits> v, Allocator const & a );|
| to_string_view()      |>=C++98| template&lt; class CharT, class Traits, class Allocator ><br>basic_string_view&lt;CharT, Traits><br>**to_string_view**( std::basic_string&lt;CharT, Traits, Allocator> const & s );|
| &nbsp;                |&nbsp; | &nbsp; |
| **Class methods**     |&nbsp; | macro `nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS` |
| **`nonstd::basic_string_view`**|&nbsp; |&nbsp; |
| Constructor           |>=C++98| template&lt; class Allocator ><br>**basic_string_view**( std::basic_string&lt;CharT, Traits, Allocator> const & s ) nssv_noexcept;|
| Converting operator   |>=C++11| template&lt; class Allocator ><br>explicit **operator std::basic_string**&lt;CharT, Traits, Allocator>() const;|
| to_string()           |>=C++11| template&lt; class Allocator = std::allocator&lt;CharT> ><br>std::basic_string&lt;CharT, Traits, Allocator><br>**to_string**( Allocator const & a = Allocator() ) const;|
| to_string()           |<C++11 | std::basic_string&lt;CharT, Traits><br>**to_string**() const;|
| to_string()           |<C++11 | template&lt; class Allocator ><br>std::basic_string&lt;CharT, Traits, Allocator><br>**to_string**( Allocator const & a ) const;|
| &nbsp;                |&nbsp; | &nbsp; |
| **Literal operator `sv`**|>=C++11| macro `nssv_CONFIG_STD_SV_OPERATOR` |
| &nbsp;                |&nbsp; | constexpr string_view operator "" **sv**( const char* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr u16string_view operator "" **sv**( const char16_t* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr u32string_view operator "" **sv**( const char32_t* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr wstring_view operator "" **sv**( const wchar_t* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | &nbsp; |
| **Literal operator `_sv`**|>=C++11| macro `nssv_CONFIG_USR_SV_OPERATOR` |
| &nbsp;                |&nbsp; | constexpr string_view operator "" **_sv**( const char* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr u16string_view operator "" **_sv**( const char16_t* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr u32string_view operator "" **_sv**( const char32_t* str, size_t len ) noexcept; |
| &nbsp;                |&nbsp; | constexpr wstring_view operator "" **_sv**( const wchar_t* str, size_t len ) noexcept; |


Configuration
-------------

### Standard selection macro

\-D<b>nssv\_CPLUSPLUS</b>=199711L  
Define this macro to override the auto-detection of the supported C++ standard, if your compiler does not set the `__cpluplus` macro correctly.

### Select `std::string_view` or `nonstd::string_view`
At default, *string-view lite* uses `std::string_view` if it is available and lets you use it via namespace `nonstd`. You can however override this default and explicitly request to use `std::string_view` as `nonstd::string_view` or use string-view lite's `nonstd::string_view` via the following macros.

-D<b>nssv\_CONFIG\_SELECT\_STRING_VIEW</b>=nssv_STRING_VIEW_DEFAULT  
Define this to `nssv_STRING_VIEW_STD` to select `std::string_view` as `nonstd::string_view`. Define this to `nssv_STRING_VIEW_NONSTD` to select `nonstd::string_view` as `nonstd::string_view`. Default is undefined, which has the same effect as defining to `nssv_STRING_VIEW_DEFAULT`.

Note: <b>nssv_CONFIG_SELECT_STD_STRING_VIEW</b> and <b>nssv_CONFIG_SELECT_NONSTD_STRING_VIEW</b> are deprecated and have been removed.

### Disable exceptions
-D<b>nssv\_CONFIG\_NO\_EXCEPTIONS</b>=0
Define this to 1 if you want to compile without exceptions. If not defined, the header tries and detect if exceptions have been disabled (e.g. via `-fno-exceptions`). Default is undefined.

### Add or omit literal operators `sv` and `_sv`

-D<b>nssv_CONFIG_STD_SV_OPERATOR</b>=1  
Define this to 1 to provide literal operator `sv` to create a `string_view` from a literal string. Default is 0. Note that literal operators without leading underscore are reserved for the C++ standard.

-D<b>nssv_CONFIG_USR_SV_OPERATOR</b>=0  
Define this to 0 to omit literal operator `_sv` to create a `string_view` from a literal string. Default is 1.

### Omit cooperation between `std::string`&ndash;`nonstd::string_view`
At default, *string-view lite* provides several methods and free functions to mimic the cooperation between `std::string` and  `nonstd::string_view` (or `std::string_view`) that exists in C++17. See section [Non-standard extensions](#non-standard-extensions). The following macros allow you to control the presence of these functions.
 
-D<b>nssv_CONFIG_CONVERSION_STD_STRING</b>=1  
Define this to 1 to provide `std::string`&ndash; `nonstd::string_view` interoperability via methods of `nonstd::basic_string_view` and free functions, define it to 0 to omit all said methods and functions. Default is undefined.

-D<b>nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS</b>=1  
Define this to 1 to provide `std::string`&ndash; `nonstd::string_view` interoperability via methods of `nonstd::basic_string_view`, define it to 0 to omit all said methods. Default is undefined.

-D<b>nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS</b>=1  
Define this to 1 to provide `std::string`&ndash; `nonstd::string_view` interoperability via free functions, define it to 0 to omit all said functions. This also controls the presence of these function if `std::string_view` is used. Default is undefined.


Reported to work with
---------------------
The table below mentions the compiler versions *string-view lite* is reported to work with.

OS        | Compiler   | Versions |
---------:|:-----------|:---------|
Windows   | Clang/LLVM | ?        |
&nbsp;    | GCC        | 7.2.0    |
&nbsp;    | Visual C++<br>(Visual Studio)| 8 (2005), 9 (2008), 10 (2010), 11 (2012),<br>12 (2013), 14 (2015), 15 (2017) |
GNU/Linux | Clang/LLVM | 3.5 - 6.0  |
&nbsp;    | GCC        | 4.8 - 8 |
OS X      | Clang/LLVM | Xcode 6, Xcode 7, Xcode 8, Xcode 9 |


Building the tests
------------------
To build the tests you need:

- [CMake](http://cmake.org), version 3.0 or later to be installed and in your PATH.
- A [suitable compiler](#reported-to-work-with).

The [*lest* test framework](https://github.com/martinmoene/lest) is included in the [test folder](test).

The following steps assume that the [*string-view lite* source code](https://github.com/martinmoene/string-view-lite) has been cloned into a directory named `c:\string-view-lite`.

1. Create a directory for the build outputs for a particular architecture.
Here we use c:\string-view-lite\build-win-x86-vc10.

        cd c:\string-view-lite
        md build-win-x86-vc10
        cd build-win-x86-vc10

2. Configure CMake to use the compiler of your choice (run `cmake --help` for a list).

        cmake -G "Visual Studio 10 2010" -DSTRINGVIEW_LITE_OPT_BUILD_TESTS=ON ..

3. Build the test suite in the Debug configuration (alternatively use Release).    

        cmake --build . --config Debug

4. Run the test suite.    

        ctest -V -C Debug

All tests should pass, indicating your platform is supported and you are ready to use *string-view lite*.


Other implementations of string_view
------------------------------------
- Marshall Clow. [string_view implementation for libc++](https://github.com/mclow/string_view). GitHub.
- LLVM libc++. [string_view](https://llvm.org/svn/llvm-project/libcxx/trunk/include/string_view). GitHub.
- Matthew Rodusek's, @bitwizeshift. [string_view Standalone](https://github.com/bitwizeshift/string_view-standalone). GitHub.
- @satoren. [string_view for C++03 C++11 C++14](https://github.com/satoren/string_view). GitHub.
- Google Abseil [string_view](https://github.com/abseil/abseil-cpp/tree/master/absl/strings) (non-templated).
- [Search _string view c++_ on GitHub](https://github.com/search?l=C%2B%2B&q=string+view+c%2B%2B&type=Repositories&utf8=%E2%9C%93).


Notes and references
--------------------
*Interface and specification*
- [string_view on cppreference](http://en.cppreference.com/w/cpp/string/basic_string_view).
- [n4659 - C++17 Working Draft: string_view](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf#page=780).

*Presentations*
- Marshall Clow. [string_view - when to use it, and when not](https://cppcon2015.sched.com/event/3vch/stringview-when-to-use-it-and-when-not?iframe=yes&w=700&sidebar=yes&bg=no#). CppCon 2015. [video](https://youtu.be/H9gAaNRoon4), [slides](https://github.com/CppCon/CppCon2015/blob/master/Presentations/string_view/string_view%20-%20Marshall%20Clow%20-%20CppCon%202015.pdf).
- Neil MacIntosh. [A few good types: Evolving array_view and string_view for safe C++ code](https://cppcon2015.sched.com/event/3vbQ/a-few-good-types-evolving-arrayview-and-stringview-for-safe-c-code). CppCon 2015. [video](https://youtu.be/C4Z3c4Sv52U), [slides](https://github.com/CppCon/CppCon2015/blob/master/Presentations/A%20Few%20Good%20Types/A%20Few%20Good%20Types%20-%20Neil%20MacIntosh%20-%20CppCon%202015.pdf).

*Proposals*
- Jeffrey Yasskin. [n3334 - Proposing array_ref<T> and string_ref](https://wg21.link/n3334). 2012.
- Jeffrey Yasskin. [n3921 - string_view: a non-owning reference to a string, revision 7](http://wg21.link/n3921). **Adopted 2014-02**.
- Alisdair Meredith. [n4288 - Strike string_view::clear from Library Fundamentals](http://wg21.link/n4288). **Adopted 2014-11**.
- Neil MacIntosh. [p0123 - Unifying the interfaces of string_view and array_view](http://wg21.link/p0123). 2015.
- Beman Dawes, Alisdair Meredith. [p0220 - Adopt Library Fundamentals V1 TS Components for C++17 (R1)](https://wg21.link/p0220r1). 2016.
- Marshall Clow. [p0254 - Integrating std::string_view and std::string](http://wg21.link/p0254). **Adopted 2016-06**.
- Nicolai Josuttis. [p0392 - Adapting string_view by filesystem paths](https://wg21.link/p0392r0). 2016.
- Marshall Clow. [p0403 - Literal suffixes for basic_string_view](http://wg21.link/p0403). 2016.
- Peter Sommerlad. [p0506 - use string_view for library function parameters instead of const string &/const char *](http://wg21.link/p0506). 2017.
- Daniel Krugler. [wg2946 - LWG 2758's resolution missed further corrections](https://wg21.link/lwg2946). 2017.


Appendix
--------

### A.1 Compile-time information

The version of *string-view lite* is available via tag `[.version]`. The following tags are available for information on the compiler and on the C++ standard library used: `[.compiler]`, `[.stdc++]`, `[.stdlanguage]` and `[.stdlibrary]`.

### A.2 string-view lite test specification

```
string_view: Allows to default construct an empty string_view
string_view: Allows to construct from pointer and size
string_view: Allows to construct from C-string
string_view: Allows to copy-construct from empty string_view
string_view: Allows to copy-construct from non-empty string_view
string_view: Allows to copy-assign from empty string_view
string_view: Allows to copy-assign from non-empty string_view
string_view: Allows forward iteration
string_view: Allows const forward iteration
string_view: Allows reverse iteration
string_view: Allows const reverse iteration
string_view: Allows to obtain the size of the view via size()
string_view: Allows to obtain the size of the view via length()
string_view: Allows to obtain the maximum size a view can be via max_size()
string_view: Allows to check for an empty string_view via empty()
string_view: Allows to observe an element via array indexing
string_view: Allows to observe an element via at()
string_view: Throws at observing an element via at() with an index of size() or larger
string_view: Allows to observe elements via data()
string_view: Yields nullptr (or NULL) with data() for an empty string_view
string_view: Allows to remove a prefix of n elements
string_view: Allows to remove a suffix of n elements
string_view: Allows to swap with other string_view
string_view: Allows to copy a substring of length n, starting at position pos (default: 0) via copy()
string_view: Throws if requested position of copy() exceeds string_view's size()
string_view: Allow to obtain a sub string, starting at position pos (default: 0) and of length n (default full), via substr()
string_view: Throws if requested position of substr() exceeds string_view's size()
string_view: Allows to lexically compare to another string_view via compare(), (1)
string_view: Allows to compare empty string_views as equal via compare(), (1)
string_view: Allows to compare a sub string to another string_view via compare(), (2)
string_view: Allows to compare a sub string to another string_view sub string via compare(), (3)
string_view: Allows to compare to a C-string via compare(), (4)
string_view: Allows to compare a sub string to a C-string via compare(), (5)
string_view: Allows to compare a sub string to a C-string prefix via compare(), (6)
string_view: Allows to check for a prefix string_view via starts_with(), (1)
string_view: Allows to check for a prefix character via starts_with(), (2)
string_view: Allows to check for a prefix C-string via starts_with(), (3)
string_view: Allows to check for a suffix string_view via ends_with(), (1)
string_view: Allows to check for a suffix character via ends_with(), (2)
string_view: Allows to check for a suffix C-string via ends_with(), (3)
string_view: Allows to search for a string_view substring, starting at position pos (default: 0) via find(), (1)
string_view: Allows to search for a character, starting at position pos (default: 0) via find(), (2)
string_view: Allows to search for a C-string substring, starting at position pos and of length n via find(), (3)
string_view: Allows to search for a C-string substring, starting at position pos (default: 0) via find(), (4)
string_view: Allows to search backwards for a string_view substring, starting at position pos (default: npos) via rfind(), (1)
string_view: Allows to search backwards for a character, starting at position pos (default: npos) via rfind(), (2)
string_view: Allows to search backwards for a C-string substring, starting at position pos and of length n via rfind(), (3)
string_view: Allows to search backwards for a C-string substring, starting at position pos (default: 0) via rfind(), (4)
string_view: Allows to search for the first occurrence of any of the characters specified in a string view, starting at position pos (default: 0) via find_first_of(), (1)
string_view: Allows to search for a character, starting at position pos (default: 0) via find_first_of(), (2)
string_view: Allows to search for the first occurrence of any of the characters specified in a C-string, starting at position pos and of length n via find_first_of(), (3)
string_view: Allows to search for the first occurrence of any of the characters specified in a C-string, starting at position pos via find_first_of(), (4)
string_view: Allows to search backwards for the last occurrence of any of the characters specified in a string view, starting at position pos (default: npos) via find_last_of(), (1)
string_view: Allows to search backwards for a character, starting at position pos (default: 0) via find_last_of(), (2)
string_view: Allows to search backwards for the first occurrence of any of the characters specified in a C-string, starting at position pos and of length n via find_last_of(), (3)
string_view: Allows to search backwards for the first occurrence of any of the characters specified in a C-string, starting at position pos via find_last_of(), (4)
string_view: Allows to search for the first character not specified in a string view, starting at position pos (default: 0) via find_first_not_of(), (1)
string_view: Allows to search for the first character not equal to the specified character, starting at position pos (default: 0) via find_first_not_of(), (2)
string_view: Allows to search for  the first character not equal to any of the characters specified in a C-string, starting at position pos and of length n via find_first_not_of(), (3)
string_view: Allows to search for  the first character not equal to any of the characters specified in a C-string, starting at position pos via find_first_not_of(), (4)
string_view: Allows to search backwards for the first character not specified in a string view, starting at position pos (default: npos) via find_last_not_of(), (1)
string_view: Allows to search backwards for the first character not equal to the specified character, starting at position pos (default: npos) via find_last_not_of(), (2)
string_view: Allows to search backwards for  the first character not equal to any of the characters specified in a C-string, starting at position pos and of length n via find_last_not_of(), (3)
string_view: Allows to search backwards for  the first character not equal to any of the characters specified in a C-string, starting at position pos via find_last_not_of(), (4)
string_view: Allows to create a string_view, wstring_view, u16string_view, u32string_view via literal "sv"
string_view: Allows to create a string_view via literal "sv", using namespace nonstd::literals::string_view_literals
string_view: Allows to create a string_view via literal "sv", using namespace nonstd::string_view_literals
string_view: Allows to create a string_view via literal "sv", using namespace nonstd::literals
string_view: Allows to create a string_view, wstring_view, u16string_view, u32string_view via literal "_sv"
string_view: Allows to create a string_view via literal "_sv", using namespace nonstd::literals::string_view_literals
string_view: Allows to create a string_view via literal "_sv", using namespace nonstd::string_view_literals
string_view: Allows to create a string_view via literal "_sv", using namespace nonstd::literals
string_view: Allows to compare a string_view with another string_view
string_view: Allows to compare empty string_view-s as equal
operator<<: Allows printing a string_view to an output stream
std::hash<>: Hash value of string_view equals hash value of corresponding string object
std::hash<>: Hash value of wstring_view equals hash value of corresponding string object
std::hash<>: Hash value of u16string_view equals hash value of corresponding string object
std::hash<>: Hash value of u32string_view equals hash value of corresponding string object
string_view: construct from std::string [extension]
string_view: convert to std::string via explicit operator [extension]
string_view: convert to std::string via to_string() [extension]
to_string(): convert to std::string via to_string() [extension]
to_string_view(): convert from std::string via to_string_view() [extension]
```
