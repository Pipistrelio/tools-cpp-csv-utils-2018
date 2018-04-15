#pragma once
#include <string>
#include <vector>
#include <istream>
#include <ostream>

namespace tinycsv {

// Trim trailing CR for \r\n lines
inline std::string trim_cr(const std::string& s) {
    if (!s.empty() && s.back() == '\r') return s.substr(0, s.size()-1);
    return s;
}

// Parse RFC4180-ish CSV line with quotes and escaped quotes
inline std::vector<std::string> parse_line(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (in_quotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    cur.push_back('"'); ++i; // escaped quote
                } else {
                    in_quotes = false;
                }
            } else {
                cur.push_back(c);
            }
        } else {
            if (c == ',') { out.emplace_back(std::move(cur)); cur.clear(); }
            else if (c == '"') { in_quotes = true; }
            else { cur.push_back(c); }
        }
    }
    out.emplace_back(std::move(cur));
    return out;
}

inline std::string escape_field(const std::string& s) {
    bool need_quotes = false;
    std::string out; out.reserve(s.size()+2);
    for (char c : s) {
        if (c == '"' || c == ',' || c == '\n' || c == '\r') need_quotes = true;
        if (c == '"') out += "\"\""; else out.push_back(c);
    }
    return need_quotes ? ("\"" + out + "\"") : out;
}

inline void write_row(std::ostream& os, const std::vector<std::string>& row) {
    for (size_t i = 0; i < row.size(); ++i) {
        if (i) os << ',';
        os << escape_field(row[i]);
    }
    os << '\n';
}

inline std::vector<std::string> read_row(std::istream& is) {
    std::string line;
    if (!std::getline(is, line)) return {};
    return parse_line(trim_cr(line));
}

} // namespace tinycsv
