#pragma once

#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace StrUtil {
    constexpr unsigned long Joaat(const char* s) {
        unsigned long hash = 0;
        for (; *s != '\0'; ++s) {
            auto c = *s;
            if (c >= 0x41 && c <= 0x5a) {
                c += 0x20;
            }
            hash += c;
            hash += hash << 10;
            hash ^= hash >> 6;
        }
        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;
        return hash;
    }

    constexpr unsigned long Joaat(const std::string& s) {
        return Joaat(s.c_str());
    }

    template<typename Out>
    void Split(const std::string& s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }

    std::vector<std::string> Split(const std::string& s, char delim);

    // Naive "fmt::join"-esque something for vectors until C++23 range formatting arrives
    // fmt: something like "{:.3f}", or just "{}"
    template<typename T>
    std::string Join(const std::vector<T>& container, const std::string& delimiter, std::string_view fmt) {
        if (container.size() == 0) {
            return {};
        }
        if (container.size() == 1) {
            return std::vformat(fmt, std::make_format_args(container[0]));
        }

        std::string output;
        for (auto it = container.begin(); it != container.end(); ++it) {
            std::string fmtVal = std::vformat(fmt, std::make_format_args(*it));
            if (std::next(it) != container.end()) {
                output = std::format("{}{}{}", output, fmtVal, delimiter);
            }
            else {
                output = std::format("{}{}", output, fmtVal);
            }
        }

        return output;
    }

    std::string ToLower(std::string s);

    //https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
    // Convert a wide Unicode string to an UTF8 string
    std::string Utf8Encode(const std::wstring& wstr);

    // Convert an UTF8 string to a wide Unicode String
    std::wstring Utf8Decode(const std::string& str);

    // trim from start (in place)
    void Ltrim(std::string& s);

    // trim from end (in place)
    void Rtrim(std::string& s);

    // trim from both ends (in place)
    void Trim(std::string& s);

    // trim from start (copying)
    std::string LtrimCopy(std::string s);

    // trim from end (copying)
    std::string RtrimCopy(std::string s);

    // trim from both ends (copying)
    std::string TrimCopy(std::string s);

    bool Strcmpwi(std::string a, std::string b);
}
