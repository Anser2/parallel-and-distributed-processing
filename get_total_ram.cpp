#include <iostream>
#include <sys/sysinfo.h>

void getRAMInfo() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        std::cout << "Total RAM: " << info.totalram / (1024 * 1024) << " MB\n";
        std::cout << "Available RAM: " << info.freeram / (1024 * 1024) << " MB\n";
    } else {
        std::cerr << "Failed to get RAM info.\n";
    }
}

int main() {
    getRAMInfo();
    return 0;
}