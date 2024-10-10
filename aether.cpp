#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

namespace fs = std::filesystem;

void setConsoleColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

class Terminal {
public:
    void run() {
        printWelcomeMessage();
        loadCommandHistory();
        loadAliases();

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
                saveAliases();
                break;
            }

            addCommandToHistory(command);
            executeCommand(command);
        }
    }

private:
    std::vector<std::string> commandHistory;
    std::map<std::string, std::string> aliases;

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
        std::cout << "Welcome to the Aether Terminal.\n";
        std::cout << "(C) 2024 CaydenDev. Licensed under the MIT license.\n";
        std::cout << "Type 'help' for a list of commands.\n" << std::endl;
    }

    void loadCommandHistory() {
        commandHistory = {"help", "cls", "exit", "time", "calc"};
    }

    void loadAliases() {
        aliases = {
            {"ll", "dir /w"},
            {"open", "start"},
            {"copy", "copy"},
            {"move", "move"}
        };
    }

    void saveAliases() {
        std::ofstream aliasFile("aliases.txt");
        for (const auto& pair : aliases) {
            aliasFile << pair.first << "=" << pair.second << std::endl;
        }
    }

    void addCommandToHistory(const std::string& command) {
        commandHistory.push_back(command);
    }

    void printHelp() {
        setConsoleColor(7, 0); 
        std::cout << "Available commands:\n";
        std::cout << "  help        - Shows this help\n";
        std::cout << "  cls         - Clears the screen\n";
        std::cout << "  exit        - Exits the terminal\n";
        std::cout << "  echo [text] - Outputs the specified text\n";
        std::cout << "  time        - Shows the current time\n";
        std::cout << "  calc        - Starts the Windows calculator\n";
        std::cout << "  history     - Shows command history\n";
        std::cout << "  alias       - Shows defined aliases\n";
        std::cout << "  setalias [alias] [command] - Sets an alias for a command\n";
        std::cout << "  create [filename] - Creates a file\n";
        std::cout << "  delete [filename] - Deletes a file\n";
        std::cout << "  list        - Lists files in the current directory\n";
        std::cout << "  ping [host] - Executes a ping command\n";
        std::cout << "  random      - Generates a random number\n";
        std::cout << "  sleep [seconds] - Pauses execution for a specified number of seconds\n";
        std::cout << std::endl;
    }

    void executeCommand(const std::string& command) {
        if (command == "help") {
            printHelp();
            return;
        } else if (command == "cls") {
            system("cls");
            return;
        } else if (command.substr(0, 5) == "echo ") {
            std::cout << command.substr(5) << std::endl;
            return;
        } else if (command == "time") {
            SYSTEMTIME st;
            GetLocalTime(&st);
            std::cout << "Current time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;
            return;
        } else if (command == "calc") {
            system("start calc");
            return;
        } else if (command == "history") {
            printHistory();
            return;
        } else if (command == "alias") {
            printAliases();
            return;
        } else if (command.substr(0, 8) == "setalias ") {
            setAlias(command.substr(8));
            return;
        } else if (command.substr(0, 6) == "create ") {
            createFile(command.substr(7));
            return;
        } else if (command.substr(0, 7) == "delete ") {
            deleteFile(command.substr(7));
            return;
        } else if (command == "list") {
            listFiles();
            return;
        } else if (command.substr(0, 5) == "ping ") {
            pingHost(command.substr(5));
            return;
        } else if (command == "random") {
            generateRandomNumber();
            return;
        } else if (command.substr(0, 6) == "sleep ") {
            sleepForSeconds(command.substr(6));
            return;
        }

        
        if (aliases.find(command) != aliases.end()) {
            executeCommand(aliases[command]);
            return;
        }

        
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
            std::cerr << "⚠️  Error: Command could not be executed! Error code: " << errorCode << std::endl;
            setConsoleColor(7, 0); 
        }
    }

    void setAlias(const std::string& aliasCommand) {
        std::istringstream iss(aliasCommand);
        std::string alias, command;
        if (iss >> alias) {
            std::getline(iss, command);
            command.erase(0, command.find_first_not_of(" ")); 
            aliases[alias] = command;
            std::cout << "Alias '" << alias << "' set for: " << command << std::endl;
        } else {
            std::cout << "Invalid alias command. Use: setalias [alias] [command]" << std::endl;
        }
    }

    void printHistory() {
        setConsoleColor(7, 0);
        std::cout << "Command history:\n";
        for (const auto& cmd : commandHistory) {
            std::cout << "  " << cmd << std::endl;
        }
        std::cout << std::endl;
    }

    void printAliases() {
        setConsoleColor(7, 0);
        std::cout << "Defined aliases:\n";
        for (const auto& pair : aliases) {
            std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
        }
        std::cout << std::endl;
    }

    void createFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file) {
            std::cout << "File '" << filename << "' created." << std::endl;
        } else {
            std::cerr << "Error creating file '" << filename << "'." << std::endl;
        }
    }

    void deleteFile(const std::string& filename) {
        if (fs::remove(filename)) {
            std::cout << "File '" << filename << "' deleted." << std::endl;
        } else {
            std::cerr << "Error deleting file '" << filename << "'." << std::endl;
        }
    }

        void listFiles() {
        std::cout << "Files in the current directory:\n";
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            std::cout << "  " << entry.path().filename().string() << std::endl;
        }
        std::cout << std::endl;
    }

    void pingHost(const std::string& host) {
        std::string command = "ping " + host;
        system(command.c_str());
    }

    void generateRandomNumber() {
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_int_distribution<> dis(1, 100); 
        std::cout << "Random number: " << dis(gen) << std::endl;
    }

    void sleepForSeconds(const std::string& secondsStr) {
        int seconds = std::stoi(secondsStr);
        std::cout << "Sleeping for " << seconds << " seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        std::cout << "Awake!" << std::endl;
    }
};

int main() {
    Terminal terminal;
    terminal.run();
    return 0;
}