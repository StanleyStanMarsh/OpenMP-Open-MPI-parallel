#include "RLECoding.h"

bool RLECoding::Encode(const std::string &file_name) {
    std::wifstream in;
    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    in >> std::noskipws;
    in.open(file_name);

    if (!in) {
        in.close();
        return false;
    }

    wchar_t sym;
    std::wstring file_code;
    while (in >> sym) {
        file_code += sym;
    }

    int count;
    for (int i = 0; i < file_code.length(); i++)
    {
        // count occurrences of character at index `i`
        count = 1;
        while (file_code[i] == file_code[i + 1] && i < file_code.length() - 1) {
            count++, i++;
        }

        // append current character and its count to the result
        code += std::to_wstring(count) + file_code[i];
        code_vector.emplace_back(count, file_code[i]);
    }
    in.close();
    return true;
}

std::wstring RLECoding::Encode(std::wstring file_code) {
    int count;
    for (int i = 0; i < file_code.length(); i++)
    {
        // count occurrences of character at index `i`
        count = 1;
        while (file_code[i] == file_code[i + 1] && i < file_code.length() - 1) {
            count++, i++;
        }

        // append current character and its count to the result
        code += std::to_wstring(count) + file_code[i] + L'#';
        code_vector.emplace_back(count, file_code[i]);
    }
    code.erase(code.end() - 1);
    return code;
}

std::wstring RLECoding::get_code() {
    return code;
}

double RLECoding::CompressionRatio(const std::string &file_name) {
    std::wifstream in;
    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    in >> std::noskipws;
    in.open(file_name);

    if (!in) {
        in.close();
        return -1;
    }

    wchar_t sym;
    std::wstring file_code;
    while (in >> sym) {
        file_code += sym;
    }
    in.close();
//    std::wcout << file_code.size() << ' ' << code.size() <<  '\n';
    return (double)(file_code.size()) / (double)(code.size());
}

void RLECoding::WriteToFile(const std::string &file_name) {
    std::wofstream out;
    out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    out.open(file_name);

    out << code;
    out.close();
}

std::wstring RLECoding::Decode(const std::wstring &code) {
    auto splitted_msg = split(code, L'#');
    std::wstring decoded;
    for (auto c: splitted_msg) {
        int k = 0;
        switch (c[0])
        {
        case L'0':
            k = 0;
            break;
        case L'1':
            k = 1;
            break;
        case L'2':
            k = 2;
            break;
        case L'3':
            k = 3;
            break;
        case L'4':
            k = 4;
            break;
        case L'5':
            k = 5;
            break;
        case L'6':
            k = 6;
            break;
        case L'7':
            k = 7;
            break;
        case L'8':
            k = 8;
            break;
        case L'9':
            k = 9;
            break;
        default:
            break;
        }
        decoded += std::wstring(k, c[1]);
    }
    return decoded;
}

bool RLECoding::EncodeParallel(const std::string &file_name) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,  &size);

    std::wstring p;

    if (rank == 0) {
        // 0 процесс разделяет файл по частям
        std::vector<std::wstring> parts = splitFileVec(file_name, size);

        for (int i = 1; i < size; i++)
            // рассылаем от 0 процесса на остальные
            MPI_Send(
                    parts[i].data(),
                    parts[i].size(),
                    MPI_WCHAR,
                    i,
                    0,
                    MPI_COMM_WORLD
                    );
        p = parts[0];
    }
    else {
        MPI_Status status;
        // MPI_Probe используется для просмотра входящего сообщения без
        // его удаления из буфера сообщений. Функция проверяет, есть ли
        // сообщение, отправленное процессом с рангом 0
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        int count;
        // MPI_Get_count определяет количество элементов, которые можно
        // получить из сообщения, информация о котором содержится в status.
        MPI_Get_count(&status, MPI_WCHAR, &count);
        p.resize(count);
        // принимаем от 0 процесса кусок для кодирования
        MPI_Recv(
                &p[0],
                count,
                MPI_WCHAR,
                0,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
                );
    }

    std::wstring enc_p = Encode(p);

    if (rank == 0) {
        std::vector<std::wstring> enc_parts(size);
        enc_parts[0] = std::move(enc_p);
        for (int i = 1; i < size; i++) {
            MPI_Status status;
            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_WCHAR, &count);
            enc_parts[i].resize(count);
            // собираем на 0 процессе разосланные закодированные части
            MPI_Recv(
                    &enc_parts[i][0],
                     count,
                     MPI_WCHAR,
                     i,
                     0,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE
                     );
        }

        std::wofstream out;
        out.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        out.open(file_name + ".encodedRLE");

        for (int i = 0; i < size; i++) {
            if (i != size - 1)
                out << enc_parts[i] << L"|";
            else
                out << enc_parts[i];
        }
        out.close();

    }
    else {
        // Отправка закодированной части самому 0 процессу
        MPI_Send(
                enc_p.data(),
                enc_p.size(),
                MPI_WCHAR,
                0,
                0,
                MPI_COMM_WORLD
                );
    }

    return true;
}

bool RLECoding::DecodeParallel(const std::string &file_name) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,  &size);

    std::wstring p;

    if (rank == 0) {
        // 0 процесс разделяет файл по частям
        std::vector<std::wstring> parts = splitFileBySym(file_name, L'|');
        for (int i = 1; i < size; i++)
            // рассылаем от 0 процесса на остальные
            MPI_Send(
                    parts[i].data(),
                    parts[i].size(),
                    MPI_WCHAR,
                    i,
                    0,
                    MPI_COMM_WORLD
            );
        p = parts[0];
    }
    else {
        MPI_Status status;
        // MPI_Probe используется для просмотра входящего сообщения без
        // его удаления из буфера сообщений. Функция проверяет, есть ли
        // сообщение, отправленное процессом с рангом 0
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        int count;
        // MPI_Get_count определяет количество элементов, которые можно
        // получить из сообщения, информация о котором содержится в status.
        MPI_Get_count(&status, MPI_WCHAR, &count);
        p.resize(count);
        // принимаем от 0 процесса кусок для кодирования
        MPI_Recv(
                &p[0],
                count,
                MPI_WCHAR,
                0,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
        );
    }

    std::wstring dec_p = Decode(p);

    if (rank == 0) {
        std::vector<std::wstring> dec_parts(size);
        dec_parts[0] = std::move(dec_p);
        for (int i = 1; i < size; i++) {
            MPI_Status status;
            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_WCHAR, &count);
            dec_parts[i].resize(count);
            // собираем на 0 процессе разосланные закодированные части
            MPI_Recv(
                    &dec_parts[i][0],
                    count,
                    MPI_WCHAR,
                    i,
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
            );
        }

        try {
            std::wofstream out;
            out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
            out.open(file_name + "-decoded.txt", std::ios::binary);

            for (auto &part: dec_parts) {
                out << part;
            }
            out.close();
        } catch (std::exception& e) {
            std::wcerr << "ERROR IN " << rank << " PROCESS" << std::endl;
            std::wcerr << e.what() << std::endl;
        }

    }
    else {
        // Отправка закодированной части самому 0 процессу
        MPI_Send(
                dec_p.data(),
                dec_p.size(),
                MPI_WCHAR,
                0,
                0,
                MPI_COMM_WORLD
        );
    }
    return true;
}
