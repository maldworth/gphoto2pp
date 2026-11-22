#ifndef GPHOTO2PP_WIDGET_STRING_HELPER_HPP
#define GPHOTO2PP_WIDGET_STRING_HELPER_HPP

#include <gphoto2pp/exceptions.hpp>

#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace gphoto2pp {
namespace detail {

inline std::string copyWidgetString(
    const char *value, std::size_t maxLength, const char *field) {
    if (value == nullptr) {
        throw exceptions::NullWidget(std::string("Camera widget ") + field + " is null");
    }
    if (maxLength == std::numeric_limits<std::size_t>::max()) {
        throw exceptions::ArgumentException("Maximum widget string length is too large");
    }
    const auto length = ::strnlen(value, maxLength + 1);
    if (length > maxLength) {
        throw std::length_error(std::string("Camera widget ") + field + " is too long");
    }
    return std::string(value, length);
}

}  // namespace detail
}  // namespace gphoto2pp

#endif
