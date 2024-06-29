#pragma  once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>
#include <cstdio>

void splitFile(const std::string& filename, int numberOfFiles);

std::vector<std::wstring> splitFileVec(const std::string& filename, int numberOfFiles);

std::vector<std::wstring> splitFileBySym(const std::string& filename, wchar_t c);

void cleanDir(const std::string& filename, int numberOfFiles);

void joinFiles(const std::string& filename, int numberOfFiles);

bool compare_files(const std::string& filePath1, const std::string& filePath2);

std::vector<std::wstring> split(const std::wstring &str, wchar_t c);

std::size_t count_characters(const std::string& file_name);
