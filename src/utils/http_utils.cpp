#include "http_utils.hpp"
#include <optional>
#include <regex>

namespace utils {

std::optional<int> ParseOptionalInt(const std::string& str) {
    if (str.empty()) return std::nullopt;
    try { return std::stoi(str); }
    catch (...) { return std::nullopt; }
}

bool IsValidEmail(const std::string& email) {
    static const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

}