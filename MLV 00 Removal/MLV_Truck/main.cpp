#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::string path;
    std::cout << "Enter MLV path: ";
    std::cin >> path;

    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    std::streampos file_size = in.tellg();
    if (file_size <= 0) {
        std::cerr << "File is empty or error reading size.\n";
        return 1;
    }

    // Scan from EOF backward
    std::streampos pos = file_size - 1;
    char byte;
    std::streampos last_nonzero = pos;

    while (pos >= 0) {
        in.seekg(pos);
        in.read(&byte, 1);
        if (!in) break;

        if (byte != 0) {
            last_nonzero = pos;
            break; // first non-zero byte from EOF
        }

        pos=pos-1;
    }

    std::streamsize trailingZeros = file_size - last_nonzero - 1;

    if (trailingZeros > 0) {
        std::ofstream out(path + ".tmp", std::ios::binary);
        if (!out) {
            std::cerr << "Cannot create temporary file.\n";
            return 1;
        }

        // Copy only up to last_nonzero + 1
        in.clear();
        in.seekg(0);
        std::vector<char> chunk(1024 * 1024); // 1 MB buffer
        std::streampos copied = 0;
        while (copied <= last_nonzero) {
            std::streamsize toRead = std::min<std::streamsize>(chunk.size(), last_nonzero + 1 - copied);
            in.read(chunk.data(), toRead);
            out.write(chunk.data(), toRead);
            copied += toRead;
        }

        out.close();
        in.close();

        // Replace original file
        std::remove(path.c_str());
        std::rename((path + ".tmp").c_str(), path.c_str());
    }

    in.close();
    return 0;
}
