#include <iostream>
#include <string>
#include <windows.h>
#include <vector>

void setConsoleColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

class Terminal {
public:
    void run() {
        printWelcomeMessage();

        std::string command;
        while (true) {
            setConsoleColor(10, 0); 
            std::cout << "Aether > ";
            setConsoleColor(7, 0); 
            std::getline(std::cin, command);

            if (command.empty()) {
                continue;
            }

            if (command == "exit") {
                break;
            }

            executeCommand(command);
        }
    }

private:
    void printWelcomeMessage() {
        setConsoleColor(9, 0); 
        std::cout << R"(
         ___           _       _        
        / _ \         | |     | |       
       / /_\ \_ __ __| |_   _| | __ ___ 
       |  _  | '_ \ / _` | | | | |/ _` |
       | | | | | | | (_| | |_| | | (_| |
       \_| |_/_| |_|\__,_|\__,_|_|\__,_|
                                         
        )" << std::endl;
        setConsoleColor(7, 0); 
        std::cout << "Welcome to Aether Terminal!\n";
        std::cout << "Type 'help' for a list of commands.\n" << std::endl;
    }

    void executeCommand(const std::string& command) {
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::wstring wcommand = L"cmd.exe /C " + std::wstring(command.begin(), command.end());

        wchar_t wcmd[512];
        wcsncpy(wcmd, wcommand.c_str(), sizeof(wcmd) / sizeof(wchar_t));

        if (CreateProcessW(NULL, wcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            setConsoleColor(12, 0); 
            DWORD errorCode = GetLastError();
            std::cerr << "⚠️  Error: Command execution failed! Error Code: " << errorCode << std::endl;
            setConsoleColor(7, 0); 
        }
    }
};

int main() {
    Terminal terminal;
    terminal.run();
    return 0;
}