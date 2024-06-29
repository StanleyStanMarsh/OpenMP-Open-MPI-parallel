#pragma once
#include <iostream>
#include <algorithm>
#include <codecvt>
#include <string>
#include <vector>
#include <fstream>

class RLECoding {
    std::wstring code;
    std::vector<std::pair<int, wchar_t>> code_vector;
public:
    RLECoding() { code = L""; }
    bool Encode(const std::string &file_name);
    bool Encode(std::wstring file_code);
    void WriteToFile(const std::string &file_name);
    std::wstring Decode(const std::string &file_name);
    bool CompareFiles(const std::string &file_name1, const std::string &file_name2);
    double CompressionRatio(const std::string &file_name);
    std::wstring get_code();
};
