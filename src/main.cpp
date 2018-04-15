#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>

#include "csv.h"
#include "normalize.h"

struct Args {
    std::string mode;           // "normalize" | "dedupe"
    std::string in_path;
    std::string out_path;
    std::string email_col = "email";
    std::string phone_col = "phone";
    std::string keep = "first"; // for dedupe: first|last
};

static void usage() {
    std::cerr <<
R"(csvutil (C++17)

Usage:
  csvutil normalize --in <input.csv> --out <output.csv> --email-col <name> --phone-col <name>
  csvutil dedupe    --in <input.csv> --out <output.csv> --email-col <name> --phone-col <name> [--keep first|last]
)";
}

static Args parse_args(int argc, char** argv) {
    Args a;
    if (argc < 2) { usage(); std::exit(1); }
    a.mode = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string k = argv[i];
        auto need = [&](const char* opt){
            if (i + 1 >= argc) { std::cerr << "Missing value for " << opt << "\n"; std::exit(1); }
            return std::string(argv[++i]);
        };
        if      (k == std::string("--in"))        a.in_path   = need("--in");
        else if (k == std::string("--out"))       a.out_path  = need("--out");
        else if (k == std::string("--email-col")) a.email_col = need("--email-col");
        else if (k == std::string("--phone-col")) a.phone_col = need("--phone-col");
        else if (k == std::string("--keep"))      a.keep      = need("--keep");
        else { std::cerr << "Unknown arg: " << k << "\n"; usage(); std::exit(1); }
    }

    if (a.mode != "normalize" && a.mode != "dedupe") {
        std::cerr << "Mode must be 'normalize' or 'dedupe'\n"; usage(); std::exit(1);
    }
    if (a.in_path.empty() || a.out_path.empty()) {
        std::cerr << "--in and --out are required\n"; usage(); std::exit(1);
    }
    if (a.keep != "first" && a.keep != "last") {
        std::cerr << "--keep must be first|last\n"; std::exit(1);
    }
    return a;
}

static int find_or_throw(const std::vector<std::string>& header, const std::string& name) {
    for (size_t i = 0; i < header.size(); ++i) if (header[i] == name) return static_cast<int>(i);
    std::cerr << "Column not found: " << name << "\n";
    std::exit(1);
}

static void cmd_normalize(const Args& a) {
    std::ifstream is(a.in_path);
    if (!is) { std::cerr << "Cannot open input: " << a.in_path << "\n"; std::exit(1); }
    std::ofstream os(a.out_path);
    if (!os) { std::cerr << "Cannot open output: " << a.out_path << "\n"; std::exit(1); }

    auto header = tinycsv::read_row(is);
    if (header.empty()) { std::cerr << "Empty input\n"; std::exit(1); }

    int i_email = find_or_throw(header, a.email_col);
    int i_phone = find_or_throw(header, a.phone_col);

    std::vector<std::string> out_header = header;
    out_header.push_back("email_norm");
    out_header.push_back("phone_norm");
    tinycsv::write_row(os, out_header);

    for (;;) {
        auto row = tinycsv::read_row(is);
        if (row.empty()) break;
        if (row.size() < header.size()) row.resize(header.size(), "");

        std::string email_norm = norm::normalize_email(row[i_email]);
        std::string phone_norm = norm::normalize_phone(row[i_phone]);

        auto out_row = row;
        out_row.push_back(email_norm);
        out_row.push_back(phone_norm);
        tinycsv::write_row(os, out_row);
    }
}

static void cmd_dedupe(const Args& a) {
    std::ifstream is(a.in_path);
    if (!is) { std::cerr << "Cannot open input: " << a.in_path << "\n"; std::exit(1); }
    std::ofstream os(a.out_path);
    if (!os) { std::cerr << "Cannot open output: " << a.out_path << "\n"; std::exit(1); }

    auto header = tinycsv::read_row(is);
    if (header.empty()) { std::cerr << "Empty input\n"; std::exit(1); }

    int i_email = find_or_throw(header, a.email_col);
    int i_phone = find_or_throw(header, a.phone_col);

    std::vector<std::string> out_header = header;
    out_header.push_back("email_norm");
    out_header.push_back("phone_norm");
    tinycsv::write_row(os, out_header);

    struct RowStore {
        std::vector<std::string> row;
        std::string email_norm;
        std::string phone_norm;
        size_t seq;
    };

    auto key_of = [](const std::string& e, const std::string& p){
        return e + "\x1f" + p; // unlikely delimiter
    };

    std::unordered_map<std::string, RowStore> best;
    size_t seq = 0;

    for (;;) {
        auto row = tinycsv::read_row(is);
        if (row.empty()) break;
        if (row.size() < header.size()) row.resize(header.size(), "");

        std::string email_norm = norm::normalize_email(row[i_email]);
        std::string phone_norm = norm::normalize_phone(row[i_phone]);
        std::string key = key_of(email_norm, phone_norm);

        RowStore cur{row, email_norm, phone_norm, seq++};

        auto it = best.find(key);
        if (it == best.end()) {
            best.emplace(key, std::move(cur));
        } else {
            if (a.keep == "last") it->second = std::move(cur);
            // if keep == first, keep existing
        }
    }

    // Dump (unordered_map order is undefined; content is correct)
    for (const auto& kv : best) {
        auto out_row = kv.second.row;
        out_row.push_back(kv.second.email_norm);
        out_row.push_back(kv.second.phone_norm);
        tinycsv::write_row(os, out_row);
    }
}

int main(int argc, char** argv) {
    Args a = parse_args(argc, argv);
    if (a.mode == "normalize") cmd_normalize(a);
    else                       cmd_dedupe(a);
    return 0;
}
