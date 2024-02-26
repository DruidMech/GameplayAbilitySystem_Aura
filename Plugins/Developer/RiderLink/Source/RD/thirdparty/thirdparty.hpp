#ifndef RD_CPP_THIRDPARTY_HPP
#define RD_CPP_THIRDPARTY_HPP

#include "tsl/ordered_set.h"
#include "tsl/ordered_map.h"

#if __cplusplus >= 201703L

#include <optional>
#include <variant>
#include <string_view>

namespace rd
{
using std::get;
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;
using std::variant;
using std::visit;
}	 // namespace rd

#else

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4583)
#pragma warning(disable : 4582)
#endif
#include "optional.hpp"
#include "mpark/variant.hpp"
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace rd
{
using mpark::get;
using mpark::variant;
using mpark::visit;
using tl::make_optional;
using tl::nullopt;
using tl::nullopt_t;
using tl::optional;
}	 // namespace rd

#endif

#include "nonstd/string_view.hpp"
namespace rd
{
using nonstd::string_view;
using nonstd::wstring_view;
using namespace std::literals;
using namespace nonstd::literals;
}	 // namespace rd

namespace rd
{
using tsl::ordered_map;
using tsl::ordered_set;
}	 // namespace rd

#endif	  // RD_CPP_THIRDPARTY_H
