#include <iostream>
#include <fcntl.h>
#include <sstream>
#include <locale>
#include <codecvt>
#include "Misc/alphabet.h"
#include "Misc/misc.h"
#include "Coding/FanoCoding.h"
#include "Coding/RLECoding.h"
#include "File Generation/FileGenerator.h"

int main() {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    #ifdef _WIN32
    _setmode(_fileno(stdout), _O_U8TEXT);
    #endif

    double start;
    double end;

    std::string file_name = "test1.txt";


    FileGenerator gener(file_name, 10000);
    gener.Generate(alphabet);

    splitFile(file_name, omp_get_max_threads());


    bool FanoEncOK = true;
    bool FanoDecOK = true;
    std::wcout << L"THREADS: " << omp_get_max_threads() << std::endl;
    std::wcout << L"____________Fano coding:____________\n";
    #pragma omp parallel
    {
        FanoCoding coder_part(&alphabet);
        bool EncOK = false, DecOK  = false;
        std::string partFilename = file_name.substr(0, file_name.find_last_of('.')) + "_" +
                std::to_string(omp_get_thread_num()+1) + file_name.substr(file_name.find_last_of('.'));
        if (coder_part.Encode(partFilename)) EncOK = true;

        if (coder_part.Decode(partFilename + ".compressed.txt")) DecOK = true;
        #pragma omp critical
        {
            FanoEncOK |= EncOK;
            FanoDecOK |= DecOK;
        }
    }

    std::wcout << L"Is encoding OK: " << FanoEncOK << L'\n';
    std::wcout << L"Is decoding OK: " << FanoDecOK << L'\n';

    joinFiles("decompressed_test1.txt", omp_get_max_threads());
    cleanDir("decompressed_test1.txt", omp_get_max_threads());


    bool RLEEncOK = true;
    bool RLEDecOK = true;
    std::wcout << L"____________RLE coding:____________\n";
    #pragma omp parallel
    {
        RLECoding coder_part;
        bool EncOK = false, DecOK  = false;

        std::string partFilename = file_name.substr(0, file_name.find_last_of('.')) + "_" +
                                   std::to_string(omp_get_thread_num()+1) + file_name.substr(file_name.find_last_of('.'));
        std::string rle_file_name(partFilename.begin(), partFilename.end() - 4);
        rle_file_name = rle_file_name + "_RLE_encoded.txt";
        if (coder_part.Encode(partFilename)) EncOK = true;
        coder_part.WriteToFile(rle_file_name);
        if (!coder_part.Decode(partFilename).empty()) DecOK = true;
        #pragma omp critical
        {
            RLEEncOK |= EncOK;
            RLEDecOK |= DecOK;
        }
    }
    std::wcout << L"Is encoding OK: " << RLEEncOK << L'\n';
    std::wcout << L"Is decoding OK: " << RLEDecOK << L'\n';
    joinFiles("decoded_with_RLE_test1.txt", omp_get_max_threads());
    cleanDir("decoded_with_RLE_test1.txt", omp_get_max_threads());


    std::string rle_file_name(file_name.begin(), file_name.end() - 4);
    rle_file_name = rle_file_name + "_RLE_encoded.txt";
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            std::wcout << L"____________RLE+Fano coding:____________\n";
            RLECoding coder2;
            if (coder2.Encode(file_name)) std::wcout << L"Encoding is OK\n";
            coder2.WriteToFile(rle_file_name);
            FanoCoding multi_coder1(&alphabet);
            if (multi_coder1.Encode(rle_file_name)) std::wcout << L"Encoding is OK\n";
        }
        #pragma omp section
        {
            std::wcout << L"____________Fano+RLE coding:____________\n";
            FanoCoding coder(&alphabet);
            if (coder.Encode(file_name)) std::wcout << L"Encoding is OK\n";
            RLECoding multi_coder2;
            if (multi_coder2.Encode(coder.get_file_code())) std::wcout << "Encoding OK\n";
        }
    }


//    system("pause");
    return 0;
}
