#include "FanoCoding.h"

FanoCoding::FanoCoding(std::vector<wchar_t> *alpha) {
    this->alphabet = alpha;
}

void FanoCoding::FindProbabilities(std::wifstream &in_stream) {
    in_stream.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );

    wchar_t sym;
    double file_len = 0;
    while(in_stream >> sym) {
        probabilities[sym] += 1;
        file_len += 1;
    }

    for (auto &elem: probabilities) {
        elem.second = elem.second / file_len;
    }
}

void FanoCoding::get_probabilities() {
    for (auto &elem: probabilities) {
        std::wcout << elem.first << L':' << elem.second;
        std::wcout << L'\n';
    }
}

std::vector<wchar_t> FanoCoding::AlphabetSortedByProbabilities() {
    std::vector<wchar_t> sorted_alphabet;
    for (auto &c: probabilities)
        sorted_alphabet.push_back(c.first);
    std::sort(sorted_alphabet.begin(), sorted_alphabet.end(),
              [this](wchar_t &a, wchar_t &b)->bool{
        return probabilities[a] > probabilities[b];
    });
    return sorted_alphabet;
}

void FanoCoding::MakeFanoCodes(int start, int end, std::wstring code) {
    auto sorted_alpha = AlphabetSortedByProbabilities();
    if (start >= end) {
        return;
    }
    if (end - start == 1) {
        fano_codes[sorted_alpha[start]] = code;
        return;
    }

    double total_probability = 0.0;
    for (int i = start; i < end; ++i) {
        total_probability += probabilities[sorted_alpha[i]];
    }

    double current_sum = 0.0;
    int split_point = start;
    for (int i = start; i < end; ++i) {
        current_sum += probabilities[sorted_alpha[i]];
        if (current_sum >= total_probability / 2) {
            split_point = i + 1;
            break;
        }
    }

    MakeFanoCodes(start, split_point, code + L'0');
    MakeFanoCodes(split_point, end, code + L'1');
}

void FanoCoding::print_fano_codes() {
    std::vector<wchar_t> sorted_alpha(AlphabetSortedByProbabilities());
    for (auto sym: sorted_alpha) {
        std::wcout << sym << L": " << fano_codes[sym] << L' ' << probabilities[sym] * 10000 << L'\n';
    }
}

int FanoCoding::FindAlmostEqualSubvectors(int begin, int end) {
    std::vector<wchar_t> sorted_alpha(AlphabetSortedByProbabilities());
    int l = begin;
    int r = end;
    double left_sum = probabilities[sorted_alpha[l]];
    double right_sum = probabilities[sorted_alpha[r]];
    while(l < r) {
        if (std::abs(left_sum - right_sum) == DBL_MIN)
            return l;
        else if (left_sum < right_sum)
            left_sum += probabilities[sorted_alpha[++l]];
        else
            right_sum += probabilities[sorted_alpha[--r]];
    }
    if (l == r) right_sum -= probabilities[sorted_alpha[l]];
    if (begin + 1 != end) {
        double d1 = std::abs(left_sum - right_sum);
        double d2 = std::abs(left_sum - probabilities[sorted_alpha[l]] - (right_sum + probabilities[sorted_alpha[l]]));
        if (d2 < d1)
            return l - 1;
    }
    return l;
}

bool FanoCoding::Encode(const std::string &file_name) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wifstream in;
    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    in >> std::noskipws;
    in.open(file_name);

    if (!in) {
        in.close();
        return false;
    }

    FindProbabilities(in);
    MakeFanoCodes(0, alphabet->size(), L"");
    in.close();

    std::wofstream out;

    out.open(file_name + ".compressed.txt");

    in.open(file_name);
    in >> std::noskipws;


    if (!out) {
        out.close();
        in.close();
        return false;
    }

    wchar_t sym;
    char buffer = 0;
    int bit = 1;
    while(in >> sym) {
        out << fano_codes[sym];
        file_code += fano_codes[sym];
    }

    out.close();
    in.close();
    return true;
}

bool FanoCoding::Decode(const std::string &file_name) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    if (!file_name.ends_with(".compressed.txt")) return false;

    std::wifstream in;
    in.open(file_name);

    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );

    if (!in) {
        in.close();
        return false;
    }

    wchar_t sym;
    std::wstring decoded_msg;
    std::wstring buf;
    while(in >> sym) {
        buf += sym;
        for (auto &elem: fano_codes) {
            if (buf == elem.second) {
                decoded_msg += elem.first;
                buf.clear();
                break;
            }
        }
    }

    std::wofstream out;
    std::string new_file_name = file_name;
    new_file_name.erase(file_name.rfind(".compressed.txt"), 15);
    out.open("decompressed_" + new_file_name);
    out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );

    if (!out) {
        out.close();
        return false;
    }
    out << decoded_msg;

    out.close();
    return true;
}

double FanoCoding::PriceOfEncoding() {
    double price;
    for (int i = 0; i < probabilities.size(); i++) {
        price += probabilities[i] * fano_codes[probabilities[i]].size();
    }
    return price;
}

double FanoCoding::CompressionRatio(const std::string &file_name) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::ifstream in1, in2;

    in1.open(file_name, std::ifstream::ate | std::ifstream::binary);

    in2.open(file_name + ".compressed.txt", std::ifstream::ate | std::ifstream::binary);
    return 8 * ((double)(in1.tellg()) / (double)(in2.tellg()));
}

std::wstring FanoCoding::get_file_code() {
    return file_code;
}

std::wstring FanoCoding::EncodePart(std::wstring &input) {
    FindProbabilities(input);
    MakeFanoCodes(0, alphabet->size(), L"");

    std::wstring code;
    for (auto &sym: input) {
        code += fano_codes[sym];
    }
    return code;
}

void FanoCoding::FindProbabilities(std::wstring &input) {
    probabilities.clear();
    double file_len = 0;
    for (auto &sym: input) {
        probabilities[sym] += 1;
        file_len += 1;
    }

    for (auto &elem: probabilities) {
        elem.second = elem.second / file_len;
    }
}

bool FanoCoding::EncodeParallel(const std::string &file_name) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,  &size);

    std::wstring p;

    if (rank == 0) {
        // 0 процесс разделяет файл на части
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

    std::wstring enc_p = EncodePart(p);

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
        out.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
        out.open(file_name + ".encoded");

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

std::wstring FanoCoding::DecodePart(std::wstring code_string) {
    std::wstring decoded_msg;
    std::wstring buf;
    int counter = 0;
    for (auto &sym: code_string) {
        counter++;
        buf += sym;
        for (auto &elem: fano_codes) {
            if (buf == elem.second && (std::find(alphabet->begin(), alphabet->end(), elem.first) != alphabet->end())) {
                decoded_msg += elem.first;
                buf.clear();
                break;
            }
        }
    }
    return decoded_msg;
}

bool FanoCoding::DecodeParallel(const std::string &file_name) {
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

    std::wstring dec_p = DecodePart(p);

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
