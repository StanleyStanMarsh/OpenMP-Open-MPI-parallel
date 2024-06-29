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

std::wstring FanoCoding::EncodeToWstring(const std::string &file_name) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wifstream in;
    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    in >> std::noskipws;
    in.open(file_name);

    if (!in) {
        in.close();
        return L"err";
    }

    FindProbabilities(in);
    MakeFanoCodes(0, alphabet->size(), L"");
    in.close();

    in.open(file_name);
    in >> std::noskipws;

    wchar_t sym;
    std::wstring code;
    while(in >> sym) {
        code += fano_codes[sym];
    }
    in.close();
    return code;
}

std::wstring FanoCoding::DecodeFromWstring(std::wstring code_string) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wstring decoded_msg;
    bool found = true;
    while (found) {
        for (auto code: fano_codes) {
            found = false;
            if (code_string.starts_with(code.second)) {
                code_string = code_string.substr(code.second.size());
                decoded_msg += code.first;
                found = true;
                break;
            }
        }
    }
    return decoded_msg;
}

bool FanoCoding::IsDecodeCorrect(const std::string &file_name) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wifstream in;
    in.imbue( std::locale(std::locale(), new std::codecvt_utf8<wchar_t>) );
    in >> std::noskipws;
    in.open(file_name);

    std::wstring original;
    wchar_t sym;
    while (in >> sym) {
        original += sym;
    }
    std::wstring decoded = DecodeFromWstring(EncodeToWstring(file_name));
    for (int i = 0; i < original.size(); i++) {
        if (original[i] != decoded[i]) return false;
    }
    in.close();
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
