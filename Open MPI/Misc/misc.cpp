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

    size_t size = count_characters(filename);

    size_t partSize = size / numberOfFiles;
    size_t remainingSize = size % numberOfFiles;

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

std::vector<std::wstring> splitFileVec(const std::string& filename, int numberOfFiles) {
    std::wifstream infile;
    infile.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    infile >> std::noskipws;
    infile.open(filename);
    if (!infile.is_open()) {
        std::wcerr << L"Не удается открыть файл!\n";
        return {};
    }

    size_t size = count_characters(filename);

    size_t partSize = size / numberOfFiles;
    size_t remainingSize = size % numberOfFiles;

    std::vector<wchar_t> buffer(partSize);

    // Чтение и запись каждой части файла
    std::vector<std::wstring> splitted(numberOfFiles);
    wchar_t sym;
    int i = 0, k = 0;
    std::wstring buf;
    while(infile >> sym) {
        if (i == partSize) {
            splitted[k++] = buf;
            buf.clear();
            i = 0;
        }
        buf += sym;
        i++;
    }

    splitted[splitted.size() - 1] += buf;

    infile.close();

    // for (auto &part: splitted)
    //     std::wcout << L"HERE " << part.size() << std::endl;

    return splitted;
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

std::vector<std::wstring> splitFileBySym(const std::string& filename, wchar_t c) {
    std::wifstream infile;
    infile.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    infile >> std::noskipws;
    infile.open(filename);
    if (!infile.is_open()) {
        std::wcerr << L"Не удается открыть файл!\n";
        return {};
    }

    std::vector<std::wstring> splitted;

    std::wstring buf;
    wchar_t sym;
    while(infile >> sym) {
        if (sym == c) {
            splitted.push_back(buf);
            buf.clear();
        } else {
            buf += sym;
        }
    }
    splitted.push_back(buf);

    return splitted;
}

bool compare_files(const std::string& filePath1, const std::string& filePath2) {

    // Открываем первый файл
    std::wifstream file1(filePath1);
    if (!file1.is_open()) {
        return false;
    }

    // Открываем второй файл
    std::wifstream file2(filePath2);
    if (!file2.is_open()) {
        file1.close();
        return false;
    }

    file1.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    file2.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );

    file1 >> std::noskipws;
    file2 >> std::noskipws;

    // Сравниваем содержимое файлов
    wchar_t ch1, ch2;
    while (file1.get(ch1) && file2.get(ch2)) {
        if (ch1 != ch2) {
            // Закрываем файлы перед выходом
            file1.close();
            file2.close();
            return false;
        }
    }

    // Закрываем файлы
    file1.close();
    file2.close();

    return true;
}

std::vector<std::wstring> split(const std::wstring &str, wchar_t c) {
    std::vector<std::wstring> tokens;
    std::wstring currentToken;
    for (wchar_t ch: str) {
        if (ch == c) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += ch;
        }
    }
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    return tokens;
}

std::size_t count_characters(const std::string& file_name) {
    std::wifstream file(file_name);
    if (!file.is_open()) {
        std::wcerr << "Failed to open file." << std::endl;
        return 0;
    }

    file.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );

    std::size_t count = 0;
    wchar_t ch;
    while (file.get(ch)) {
        ++count;
    }

    file.close();
    return count;
}