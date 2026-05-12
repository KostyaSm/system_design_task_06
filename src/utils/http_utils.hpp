#pragma once

#include <string>
#include <optional>

namespace utils {

std::optional<int> ParseOptionalInt(const std::string& str);
bool IsValidEmail(const std::string& email);

}