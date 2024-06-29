#include <iostream>
#include <fcntl.h>
#include <sstream>
#include "Misc/alphabet.h"
#include "Misc/misc.h"
#include "Coding/FanoCoding.h"
#include "Coding/RLECoding.h"
#include "File Generation/FileGenerator.h"

#include <mpi.h>

int main(int argc, char **argv) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    #ifdef _WIN32
    _setmode(_fileno(stdout), _O_U8TEXT);

    #endif

    MPI_Init(&argc, &argv);

    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::string filename = "test1.txt";

    if (rank == 0) {
        FileGenerator generator(filename, 10000);
        generator.Generate(alphabet);
    }

    // --------------------- FANO ---------------------

    FanoCoding coder(&alphabet);
    std::wcout << "IS THERE PROBLEM WITH ENCODING FANO: " << !coder.EncodeParallel(filename) << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING FANO: " << !coder.DecodeParallel(filename + ".encoded") << std::endl;

    if (rank == 0) {
        if (compare_files(filename, filename + ".encoded-decoded.txt")) std::wcout << L"Decoding FANO is CORRECT\n\n";
    }

    // --------------------- RLE ---------------------
    
    RLECoding coder2;
    std::wcout << "IS THERE PROBLEM WITH ENCODING RLE: " << !coder2.EncodeParallel(filename) << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING RLE: " << !coder2.DecodeParallel(filename + ".encodedRLE") << std::endl;


    if (rank == 0) {
        if (compare_files(filename, filename + ".encodedRLE-decoded.txt")) std::wcout << L"Decoding RLE is CORRECT\n\n";
    }

    // --------------------- FANO+RLE ---------------------

    RLECoding multi_coder;
    std::wcout << "IS THERE PROBLEM WITH ENCODING RLE AFTER FANO: " << !multi_coder.EncodeParallel(filename + ".encoded") << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING RLE AFTER FANO: " << !multi_coder.DecodeParallel(filename + ".encoded.encodedRLE") << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING FANO: " << !coder.DecodeParallel(filename + ".encoded.encodedRLE") << std::endl;

    if (rank == 0) {
        if (compare_files(filename, filename + ".encoded.encodedRLE-decoded.txt")) std::wcout << L"Decoding FANO+RLE is CORRECT\n\n";
    }

    // --------------------- RLE+FANO ---------------------

    auto extended_alpha = alphabet;
    extended_alpha.push_back(L'|');
    extended_alpha.push_back(L'#');
    FanoCoding multi_coder2(&alphabet);
    std::wcout << "IS THERE PROBLEM WITH ENCODING FANO AFTER RLE: " << !multi_coder2.EncodeParallel(filename + ".encodedRLE") << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING FANO AFTER RLE: " << !multi_coder2.DecodeParallel(filename + ".encodedRLE.encoded") << std::endl;
    std::wcout << "IS THERE PROBLEM WITH DECODING FANO: " << !coder2.DecodeParallel(filename + ".encodedRLE.encoded") << std::endl;

    if (rank == 0) {
        if (compare_files(filename, filename + ".encodedRLE.encoded-decoded.txt")) std::wcout << L"Decoding FANO+RLE is CORRECT\n\n";
    }
    
    MPI_Finalize();
    std::wcout << "END OF PROGRAM\n";
}
