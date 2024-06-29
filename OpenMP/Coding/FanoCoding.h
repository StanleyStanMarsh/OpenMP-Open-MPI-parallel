#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <float.h>
#include <codecvt>
#include <bitset>
#include <fstream>
#include <climits>

#include <omp.h>

class FanoCoding {
    std::vector<wchar_t> *alphabet;
    std::unordered_map<wchar_t, double> probabilities;
    std::unordered_map<wchar_t, std::wstring> fano_codes;
    std::wstring file_code;
private:
    std::vector<wchar_t> AlphabetSortedByProbabilities();
    void FindProbabilities(std::wifstream &in_stream);
    void MakeFanoCodes(int start, int end, std::wstring code);
public:
    FanoCoding(std::vector<wchar_t> *alpha);

    bool Encode(const std::string &file_name);
    std::wstring EncodeToWstring(const std::string &file_name);
    bool Decode(const std::string &file_name);
    std::wstring DecodeFromWstring(std::wstring code_string);
    bool IsDecodeCorrect(const std::string &file_name);

    double PriceOfEncoding();
    double CompressionRatio(const std::string &file_name);
    void get_probabilities();
    void print_fano_codes();
    std::wstring get_file_code();
};
