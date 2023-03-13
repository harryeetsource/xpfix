#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <windows.h>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    // Perform full cleanup and system file check
    std::cout << "Performing full cleanup and system file check." << std::endl;

    system("cleanmgr /sagerun:1");
    system("sfc /scannow");

    // Perform additional cleanup steps
    std::cout << "Performing additional cleanup steps." << std::endl;

    system("del /q /s %temp%\\*");
    system("rmdir /q /s C:\\Windows\\SoftwareDistribution\\Download");
    std::cout << "Enter the name of an application to uninstall or press Enter to skip: ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        std::string command = "wmic product where \"Name='" + input + "'\" get IdentifyingNumber /value | findstr IdentifyingNumber";
        std::string productCode = exec(command.c_str());
        productCode.erase(0, 19);
        command = "msiexec /x {" + productCode + "}";
        system(command.c_str());
    }
    system("forfiles /p \"C:\\Windows\\Logs\" /s /m *.log /d -7 /c \"cmd /c del @path\"");

    // Wait for user input before exiting
    std::cout << "Cleanup complete. Press Enter to exit." << std::endl;
    std::cin.ignore();

    return 0;
}
