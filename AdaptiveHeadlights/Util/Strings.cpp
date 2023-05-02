#include "Strings.hpp"

#include <Windows.h>
#include <algorithm>

std::vector<std::string> StrUtil::Split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    Split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string StrUtil::ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string StrUtil::Utf8Encode(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring StrUtil::Utf8Decode(const std::string& str) {
    if (str.empty())
        return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// trim from start (in place)
void StrUtil::Ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
void StrUtil::Rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
void StrUtil::Trim(std::string& s) {
    Ltrim(s);
    Rtrim(s);
}

// trim from start (copying)
std::string StrUtil::LtrimCopy(std::string s) {
    Ltrim(s);
    return s;
}

// trim from end (copying)
std::string StrUtil::RtrimCopy(std::string s) {
    Rtrim(s);
    return s;
}

// trim from both ends (copying)
std::string StrUtil::TrimCopy(std::string s) {
    Trim(s);
    return s;
}

bool StrUtil::Strcmpwi(std::string a, std::string b) {
    Trim(a);
    Trim(b);
    return ToLower(a) == ToLower(b);
}
