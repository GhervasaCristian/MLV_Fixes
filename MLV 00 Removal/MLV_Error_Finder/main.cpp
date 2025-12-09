#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <cctype>
#include <stdarg.h>
#include <algorithm>
#include <sys/stat.h>

// Global log setup
std::string g_log_filename = "mlv_span_log.txt";
std::ofstream log_file;

#pragma pack(push, 1)
typedef struct {
    char      blockType[4];
    uint32_t blockSize;
    uint64_t timestamp;
} mlv_hdr_t;
#pragma pack(pop)

// --- Helper Functions ---

void log_and_print(const char* format, ...) {
    va_list args_console;
    va_start(args_console, format);
    std::vprintf(format, args_console);
    va_end(args_console);

    if (log_file.is_open()) {
        va_list args_file;
        va_start(args_file, format);
        char buffer[4096];
        int len = std::vsnprintf(buffer, sizeof(buffer), format, args_file);
        if (len > 0) log_file.write(buffer, len);
        va_end(args_file);
    }
}

// Check if a file exists
bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

// Generate the next filename (e.g., test.MLV -> test.M00 -> test.M01)
std::string get_next_filename(std::string current_path, int index) {
    std::string next_path = current_path;
    size_t dot_pos = next_path.find_last_of(".");
    if (dot_pos == std::string::npos) return "";

    char ext[5];
    std::snprintf(ext, 5, ".M%02d", index);
    next_path.replace(dot_pos, 4, ext);
    return next_path;
}

bool is_valid_type(const char* type) {
    const char* valid_tags[] = {
        "MLVI", "RAWI", "VIDF", "AUDF", "RAWC", "IDNT",
        "EXPO", "LENS", "RTCI", "WBAL", "VERS", "DISO",
        "WAVI", "NULL", "MARK", "STYLES", "ELVL"
    };
    for (const char* tag : valid_tags) {
        if (std::strncmp(type, tag, 4) == 0) return true;
    }
    if (isupper(type[0]) && isupper(type[1]) && isupper(type[2]) && isupper(type[3])) return true;
    return false;
}

void dump_hex(std::ifstream& f, long long offset, int size) {
    long long old_pos = f.tellg();
    f.seekg(offset, std::ios::beg);
    std::vector<uint8_t> buf(size);
    f.read(reinterpret_cast<char*>(buf.data()), size);

    log_and_print("\n    HEX DUMP (Offset 0x%llX): ", offset);
    for (int i = 0; i < size; i++) log_and_print("%02X ", buf[i]);
    log_and_print("\n    ASCII:                    ");
    for (int i = 0; i < size; i++) {
        char c = static_cast<char>(buf[i]);
        log_and_print("%c  ", (isprint(c) ? c : '.'));
    }
    log_and_print("\n");
    f.seekg(old_pos, std::ios::beg);
}

// --- Main Logic ---

int main() {
    char path_buffer[1024];
    std::cout << "--- MLV Multi-File (Spanned) Diagnostic Tool ---\n";
    std::cout << "Enter path to the main .MLV file: ";
    if (!(std::cin >> path_buffer)) return 1;

    std::string base_path = path_buffer;

    // Setup Log
    std::string filename_only = base_path.substr(base_path.find_last_of("/\\") + 1);
    g_log_filename = filename_only + "_scan_log.txt";
    log_file.open(g_log_filename, std::ios::out | std::ios::trunc);
    log_and_print("✅ Logging to: %s\n", g_log_filename.c_str());

    // Prepare list of files to scan
    std::vector<std::string> files_to_scan;
    files_to_scan.push_back(base_path);

    // Auto-detect .M00, .M01, etc.
    int ext_index = 0;
    while (true) {
        std::string next_file = get_next_filename(base_path, ext_index);
        if (file_exists(next_file)) {
            files_to_scan.push_back(next_file);
            ext_index++;
        } else {
            break;
        }
    }

    log_and_print("Found %d file(s) in sequence.\n", files_to_scan.size());

    // Iterate through files strictly
    int total_blocks = 0;

    for (size_t i = 0; i < files_to_scan.size(); i++) {
        std::string current_file = files_to_scan[i];
        log_and_print("\n========================================\n");
        log_and_print("📂 Opening File %d/%d: %s\n", i + 1, files_to_scan.size(), current_file.c_str());
        log_and_print("========================================\n");

        std::ifstream f(current_file, std::ios::binary);
        if (!f.is_open()) {
            log_and_print("❌ Failed to open file: %s\n", current_file.c_str());
            break;
        }

        f.seekg(0, std::ios::end);
        long long file_size = f.tellg();
        f.seekg(0, std::ios::beg);

        long long pos = 0;

        while (pos < file_size) {
            // Check if we are near the end of the file
            if (pos + sizeof(mlv_hdr_t) > file_size) {
                long long remaining = file_size - pos;
                log_and_print("\n⚠️  End of file reached with %lld bytes remaining at offset 0x%llX.\n", remaining, pos);

                if (remaining > 0) {
                     log_and_print("   Dumping trailing bytes (potential garbage):\n");
                     dump_hex(f, pos, remaining);
                }

                // If this is NOT the last file, this is likely the error!
                // MLV App might crash if the first file doesn't end cleanly on a block boundary.
                if (i < files_to_scan.size() - 1) {
                     log_and_print("❗ WARNING: This file ends with partial data. This might crash the transition to the next file.\n");
                }
                break;
            }

            mlv_hdr_t hdr{};
            f.seekg(pos, std::ios::beg);
            f.read(reinterpret_cast<char*>(&hdr), sizeof(mlv_hdr_t));

            // CRITICAL CHECK FOR ZERO BLOCKSIZE
            if (hdr.blockSize == 0) {
                log_and_print("\n🛑 FATAL ERROR: blockSize == 0 at File Offset 0x%llX (Block Index %d)\n", pos, total_blocks);
                log_and_print("   This is the crash point.\n");
                dump_hex(f, pos, 64);
                return 1; // Stop immediately
            }

            if (!is_valid_type(hdr.blockType) || hdr.blockSize < sizeof(mlv_hdr_t)) {
                log_and_print("\n🛑 ERROR: Invalid Header at 0x%llX. Type: %.4s, Size: %u\n", pos, hdr.blockType, hdr.blockSize);
                dump_hex(f, pos, 64);
                return 1; // Stop immediately
            }

            // Move to next block
            long long next_pos = pos + hdr.blockSize;

            // Check if block exceeds file boundaries
            if (next_pos > file_size) {
                 log_and_print("\n🛑 ERROR: Block at 0x%llX (Size %u) extends BEYOND the file end (File Size %lld).\n", pos, hdr.blockSize, file_size);
                 log_and_print("   It expects data that isn't in this file.\n");
                 return 1;
            }

            pos = next_pos;
            total_blocks++;

            // Limit console output, print every 5000th block or if it's the very first block of a file
            if (pos == hdr.blockSize || total_blocks % 5000 == 0) {
                 std::cout << "\r   Scanned " << total_blocks << " blocks..." << std::flush;
            }
        }

        log_and_print("\n✅ Finished File %d cleanly at offset 0x%llX\n", i + 1, pos);
        f.close();
    }

    log_and_print("\n🎉 ALL FILES SCANNED SUCCESSFULLY. Total Blocks: %d\n", total_blocks);
    if (log_file.is_open()) log_file.close();
    return 0;
}
