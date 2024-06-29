#pragma once
#include <fstream>
#include <codecvt>
#include <string>
#include <vector>
#include <random>


class FileGenerator {
    std::wofstream out;
    int num_of_symbols;
public:
    FileGenerator(const std::string& file_name, int n);
    void Generate(std::vector<wchar_t> &alpha);
    ~FileGenerator();
};
