#include "FileGenerator.h"
#include <iostream>

FileGenerator::FileGenerator(const std::string& file_name, int n) {
    this->out.open(file_name);
    this->out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    this->num_of_symbols = n;
}

void FileGenerator::Generate(std::vector<wchar_t> &alpha) {
    std::default_random_engine generator(time(0));
    std::uniform_int_distribution<int> distribution(0,alpha.size()-1);
    std::wstring symbols;
    for (int i = 0; i < num_of_symbols; i++) {
        out << alpha[distribution(generator)];
    }
    out.close();
}

FileGenerator::~FileGenerator() {
    out.close();
}
