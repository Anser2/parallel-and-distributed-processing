#include <iostream>
#include <fstream>
#include <string>

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>


void printMemoryMap() {
    std::ifstream mapsFile("/proc/self/maps");
    if (!mapsFile) {
        std::cerr << "Failed to open /proc/self/maps\n";
        return;
    }

    std::cout << "\n=== Memory Map ===\n";
    std::string line;
    while (std::getline(mapsFile, line)) {
        std::cout << line << "\n";
    }
    std::cout << "===================\n";
}

void allocateMemoryAndPrintMap(const std::string &name, double percentage) {
    struct sysinfo info;
    sysinfo(&info);
    size_t totalRAM = info.totalram;
    size_t allocateSize = static_cast<size_t>(totalRAM * percentage);

    void* allocatedMemory = mmap(NULL, allocateSize, PROT_READ | PROT_WRITE,
                                 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    if (allocatedMemory == MAP_FAILED) {
        std::cerr << "Memory allocation failed for " << name << "\n";
        return;
    }

    std::cout << name << " allocated " << allocateSize / (1024 * 1024) << " MB at " 
              << allocatedMemory << "\n";
}

int main() {
    pid_t pidA, pidB, pidC;

    if ((pidA = fork()) == 0) {
        allocateMemoryAndPrintMap("A", 0.5);
        pause();
    }
    if ((pidB = fork()) == 0) {
        allocateMemoryAndPrintMap("B", 0.4);
        pause();
    }
    if ((pidC = fork()) == 0) {
        allocateMemoryAndPrintMap("C", 0.3);
        pause();
    }
    printMemoryMap();
    sleep(2); // Allow child processes to print before parent exits
    return 0;
}
