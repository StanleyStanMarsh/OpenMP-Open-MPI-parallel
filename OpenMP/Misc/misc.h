#pragma  once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <codecvt>
#include <cstdio>

void splitFile(const std::string& filename, int numberOfFiles);

void cleanDir(const std::string& filename, int numberOfFiles);

void joinFiles(const std::string& filename, int numberOfFiles);