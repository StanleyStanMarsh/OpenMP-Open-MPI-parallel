#pragma once
#include <iostream>
#include <algorithm>
#include <codecvt>
#include <string>
#include <vector>
#include <fstream>
#include <codecvt>
#include <locale>
#include <mpi.h>
#include "../Misc/misc.h"

class RLECoding {
    std::wstring code;
    std::vector<std::pair<int, wchar_t>> code_vector;
public:
    RLECoding() { code = L""; }
    bool Encode(const std::string &file_name);
    std::wstring Encode(std::wstring file_code);
    bool EncodeParallel(const std::string &file_name);
    void WriteToFile(const std::string &file_name);
    std::wstring Decode(const std::wstring &code);
    bool DecodeParallel(const std::string &file_name);
    double CompressionRatio(const std::string &file_name);
    std::wstring get_code();
};
