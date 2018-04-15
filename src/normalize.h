#pragma once
#include <algorithm>
#include <cctype>
#include <string>

namespace norm {

inline std::string to_lower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

inline std::string trim(const std::string& s) {
    size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e-1]))) --e;
    return s.substr(b, e - b);
}

inline std::string normalize_email(const std::string& s) {
    return to_lower(trim(s));
}

inline std::string digits_only(const std::string& s) {
    std::string out; out.reserve(s.size());
    for (unsigned char c : s) if (std::isdigit(c)) out.push_back(static_cast<char>(c));
    return out;
}

inline std::string normalize_phone(const std::string& s) {
    return digits_only(trim(s)); // digits only; no country inference
}

} // namespace norm
