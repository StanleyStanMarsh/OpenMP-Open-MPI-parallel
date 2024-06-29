#include "misc.h"

void splitFile(const std::string& filename, int numberOfFiles) {
    std::wifstream infile;
    infile.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    infile >> std::noskipws;
    infile.open(filename);
    if (!infile.is_open()) {
        std::wcerr << L"Не удается открыть файл!\n";
        return;
    }

    size_t size = 10000;

    size_t partSize = size / numberOfFiles;
    size_t remainingSize = size % numberOfFiles;

    std::wcout << size << std::endl;

    std::vector<wchar_t> buffer(partSize);

    // Чтение и запись каждой части файла
    wchar_t sym;
    int i = 0, k = 0;
    std::wstring buf;
    while(infile >> sym) {
        if (i == partSize) {
            std::wofstream out;
            out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>)  );
            std::string partFilename = filename.substr(0, filename.find_last_of('.')) + "_" + std::to_string(k + 1) + filename.substr(filename.find_last_of('.'));
            out.open(partFilename);
            out << buf;
            out.close();
            buf.clear();
            i = 0;
            k++;
        }
        buf += sym;
        i++;
    }

    std::wofstream out;
    out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>)  );
    std::string partFilename = filename.substr(0, filename.find_last_of('.')) + "_" + std::to_string(k) + filename.substr(filename.find_last_of('.'));
    out.open(partFilename, std::ios_base::app);
    out << buf;
    out.close();

    infile.close();
}

void cleanDir(const std::string& filename, int numberOfFiles) {
    for (int i = 0; i < numberOfFiles; i++) {
        std::string partFilename = filename.substr(0, filename.find_last_of('.')) + "_" + std::to_string(i+1) + filename.substr(filename.find_last_of('.'));
        remove(partFilename.c_str());
    }
}

void joinFiles(const std::string& filename, int numberOfFiles) {
    for  (int i = 0; i < numberOfFiles; i++) {
        std::string partFilename = filename.substr(0, filename.find_last_of('.')) + "_" +
                                   std::to_string(i + 1) + filename.substr(filename.find_last_of('.'));
        std::wifstream infile;
        infile.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
        infile >> std::noskipws;
        infile.open(partFilename);
        std::wofstream out;
        out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
        out.open(filename, std::ios_base::app);
        wchar_t sym;
        while(infile >> sym)  {
            out << sym;
        }
        out.close();
        infile.close();
    }
}
