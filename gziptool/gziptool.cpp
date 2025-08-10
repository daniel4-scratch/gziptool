#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <zlib.h>

namespace fs = std::filesystem;

const std::string VERSION = "1.0.0";
const std::string URL = "https://github.com/Daniel4-Scratch/gziptool";

bool isListFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        if (!fs::is_regular_file(file))
            return false;
    }
    return true;
}

bool isFileGz(const std::string& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) return false;
    unsigned char bytes[2];
    in.read(reinterpret_cast<char*>(bytes), 2);
    return in.gcount() == 2 && bytes[0] == 0x1f && bytes[1] == 0x8b;
}

bool gzWriteAll(gzFile out, const void* data, size_t size) {
    return gzwrite(out, data, static_cast<unsigned int>(size)) == static_cast<int>(size);
}

bool gzReadAll(gzFile in, void* data, size_t size) {
    return gzread(in, data, static_cast<unsigned int>(size)) == static_cast<int>(size);
}

void create_custom_gzip_archive(const std::string& output_file, const std::vector<std::string>& input_files) {
    gzFile out = gzopen(output_file.c_str(), "wb");
    if (!out) throw std::runtime_error("Failed to open output gzip file");

    for (const auto& file : input_files) {
        std::string name = fs::path(file).filename().string();
        std::ifstream in(file, std::ios::binary);
        if (!in) throw std::runtime_error("Failed to open input file: " + file);

        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);

        // Write file name and size
        std::string name_line = name + "\n";
        std::string size_line = std::to_string(size) + "\n";
        gzWriteAll(out, name_line.data(), name_line.size());
        gzWriteAll(out, size_line.data(), size_line.size());

        // Write file contents
        std::vector<char> buffer(size);
        in.read(buffer.data(), size);
        gzWriteAll(out, buffer.data(), size);
    }

    gzclose(out);
}

void extract_custom_gzip_archive(const std::string& input_file, const std::string& output_dir) {
    fs::create_directories(output_dir);
    gzFile in = gzopen(input_file.c_str(), "rb");
    if (!in) throw std::runtime_error("Failed to open input gzip file");

    char line[4096];
    while (gzgets(in, line, sizeof(line))) {
        std::string file_name = std::string(line);
        file_name.erase(file_name.find_last_not_of("\r\n") + 1);
        if (file_name.empty()) break;

        gzgets(in, line, sizeof(line));
        size_t file_size = std::stoull(line);

        std::vector<char> buffer(file_size);
        gzReadAll(in, buffer.data(), file_size);

        std::ofstream out(fs::path(output_dir) / file_name, std::ios::binary);
        out.write(buffer.data(), file_size);
    }

    gzclose(in);
}

std::map<std::string, std::vector<char>> extract_custom_gzip_archive_to_memory(const std::string& input_file) {
    std::map<std::string, std::vector<char>> files;
    gzFile in = gzopen(input_file.c_str(), "rb");
    if (!in) throw std::runtime_error("Failed to open input gzip file");

    char line[4096];
    while (gzgets(in, line, sizeof(line))) {
        std::string file_name = std::string(line);
        file_name.erase(file_name.find_last_not_of("\r\n") + 1);
        if (file_name.empty()) break;

        gzgets(in, line, sizeof(line));
        size_t file_size = std::stoull(line);

        std::vector<char> buffer(file_size);
        gzReadAll(in, buffer.data(), file_size);
        files[file_name] = std::move(buffer);
    }

    gzclose(in);
    return files;
}

std::string current_time_str() {
    auto t = std::time(nullptr);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
    return oss.str();
}

int main(int argc, char* argv[]) {
    try {
        if (argc > 3 && std::string(argv[1]) == "archive") {
            std::vector<std::string> files(argv + 3, argv + argc);
            create_custom_gzip_archive(argv[2], files);
        }
        else if (argc > 3 && std::string(argv[1]) == "unarchive") {
            extract_custom_gzip_archive(argv[2], argv[3]);
        }
        else if (argc > 2 && isListFiles(std::vector<std::string>(argv + 1, argv + argc))) {
            std::string out_name = "archive_" + current_time_str();
            create_custom_gzip_archive(out_name, std::vector<std::string>(argv + 1, argv + argc));
        }
        else if (argc == 2 && isFileGz(argv[1])) {
            std::string out_dir = "unarchive_" + fs::path(argv[1]).filename().string();
            extract_custom_gzip_archive(argv[1], out_dir);
        }
        else if (argc == 2 && std::string(argv[1]) == "info") {
            std::cout << VERSION << "\n" << URL << "\n";
        }
        else {
            std::cout << "Invalid arguments\n";
            std::cout << "Archive: gziptool archive <output_file> <input_files> ...\n";
            std::cout << "Unarchive: gziptool unarchive <input_file> <output_dir>\n";
        }
    }
    catch (const std::exception& e) {
        std::ofstream log("error.log");
        log << e.what();
#ifdef _WIN32
        system("notepad.exe error.log");
#else
        std::cerr << "An error occurred. Check error.log in the cwd for details.\n";
#endif
    }
    return 0;
}
