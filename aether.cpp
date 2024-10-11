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
#include <conio.h>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>

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
        loadThemes();

        std::string command;
        while (true) {
            setConsoleColor(currentTheme["promptTextColor"], currentTheme["promptBgColor"]);
            std::cout << "Aether > ";
            setConsoleColor(currentTheme["defaultTextColor"], currentTheme["defaultBgColor"]);

            command = getCommandInput();

            if (command.empty()) continue;

            if (command == "exit") {
                saveAliases();
                break;
            }

            addCommandToHistory(command);
            auto start = std::chrono::high_resolution_clock::now();
            executeCommand(command);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "Command executed in " << duration << " ms." << std::endl;
        }
    }

private:
    std::vector<std::string> commandHistory;
    std::map<std::string, std::string> aliases;
    std::map<std::string, int> currentTheme;
    int historyIndex = -1;

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
        commandHistory = {"help", "cls", "exit", "time", "calc", "random", "echo"};
    }

    void loadAliases() {
        std::ifstream aliasFile("aliases.txt");
        std::string line;
        while (std::getline(aliasFile, line)) {
            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                std::string alias = line.substr(0, pos);
                std::string command = line.substr(pos + 1);
                aliases[alias] = command;
            }
        }
    }

    void saveAliases() {
        std::ofstream aliasFile("aliases.txt");
        for (const auto& pair : aliases) {
            aliasFile << pair.first << "=" << pair.second << std::endl;
        }
    }

    void loadThemes() {
        currentTheme = {
            {"promptTextColor", 10},
            {"promptBgColor", 0},
            {"defaultTextColor", 7},
            {"defaultBgColor", 0}
        };
    }

    std::string getCommandInput() {
        std::string command;
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!command.empty()) {
                    command.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch == -32) {
                char arrowKey = _getch();
                if (arrowKey == 72 && historyIndex < (int)commandHistory.size() - 1) {
                    historyIndex++;
                    command = commandHistory[commandHistory.size() - 1 - historyIndex];
                    std::cout << "\rAether > " << command << std::string(50, ' ');
                } else if (arrowKey == 80 && historyIndex >= 0) {
                    historyIndex--;
                    command = (historyIndex == -1) ? "" : commandHistory[commandHistory.size() - 1 - historyIndex];
                    std::cout << "\rAether > " << command << std::string(50, ' ');
                }
            } else {
                command.push_back(ch);
                std::cout << ch;
            }
        }
        std::cout << std::endl;
        return command;
    }

    void addCommandToHistory(const std::string& command) {
        commandHistory.push_back(command);
        historyIndex = -1;
    }

    void executeCommand(const std::string& command) {
        if (command == "help") {
            printHelp();
        } else if (command == "cls") {
            system("cls");
        } else if (command.substr(0, 5) == "echo ") {
            std::cout << command.substr(5) << std::endl;
        } else if (command == "time") {
            displayCurrentTime();
        } else if (command == "calc") {
            system("start calc");
        } else if (command == "history") {
            printHistory();
        } else if (command == "alias") {
            printAliases();
        } else if (command.substr(0, 8) == "setalias ") {
            setAlias(command.substr(8));
        } else if (command.substr(0, 6) == "create ") {
            createFile(command.substr(7));
        } else if (command.substr(0, 7) == "delete ") {
            deleteFile(command.substr(7));
        } else if (command == "list") {
            listFiles();
        } else if (command.substr(0, 5) == "ping ") {
            pingHost(command.substr(5));
        } else if (command == "random") {
            generateRandomNumber();
        } else if (command.substr(0, 6) == "sleep ") {
            sleepForSeconds(command.substr(6));
        } else if (command == "theme") {
            switchTheme();
        } else if (command.substr(0, 8) == "calculator") {
            launchCalculator(command.substr(9));
        } else if (command.substr(0, 10) == "writefile ") {
            writeToFile(command.substr(10));
        } else if (command.substr(0, 9) == "readfile ") {
            readFromFile(command.substr(9));
        } else {
            runCommand(command);
        }
    }

    void displayCurrentTime() {
        SYSTEMTIME st;
        GetLocalTime(&st);
        std::cout << "Current time: " << std::setw(2) << std::setfill('0') << st.wHour << ":" 
                  << std::setw(2) << std::setfill('0') << st.wMinute << ":" 
                  << std::setw(2) << std::setfill('0') << st.wSecond << std::endl;
    }

    void launchCalculator(const std::string& calcType) {
        std::string command = (calcType == "basic") ? "start calc" : "start calc";
        system(command.c_str());
    }

    void writeToFile(const std::string& filename) {
        size_t pos = filename.find(" ");
        if (pos == std::string::npos) {
            std::cout << "Usage: writefile <filename> <text>\n";
            return;
        }
        std::string text = filename.substr(pos + 1);
        std::string file = filename.substr(0, pos);
        std::ofstream outFile(file);
        if (outFile.is_open()) {
            outFile << text;
            std::cout << "Text written to file '" << file << "'\n";
        } else {
            std::cerr << "Error: Unable to write to file '" << file << "'.\n";
        }
    }

    void readFromFile(const std::string& filename) {
        std::ifstream inFile(filename);
        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                std::cout << line << std::endl;
            }
            inFile.close();
        } else {
            std::cerr << "Error: Unable to read from file '" << filename << "'.\n";
        }
    }

    void switchTheme() {
        std::cout << "Choose a theme: (1) Dark, (2) Light\n";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            currentTheme["promptTextColor"] = 10;
            currentTheme["promptBgColor"] = 0;
            currentTheme["defaultTextColor"] = 7;
            currentTheme["defaultBgColor"] = 0;
            std::cout << "Switched to Dark Theme.\n";
        } else if (choice == "2") {
            currentTheme["promptTextColor"] = 0;
            currentTheme["promptBgColor"] = 7;
            currentTheme["defaultTextColor"] = 0;
            currentTheme["defaultBgColor"] = 7;
            std::cout << "Switched to Light Theme.\n";
        }
    }

    void printHelp() {
        std::cout << "Available commands:\n";
        std::cout << "help    - Show this help message\n";
        std::cout << "cls     - Clear the terminal\n";
        std::cout << "exit    - Exit the terminal\n";
        std::cout << "echo    - Echo back the input text\n";
        std::cout << "time    - Show the current time\n";
        std::cout << "calc    - Launch calculator\n";
        std::cout << "history  - Show command history\n";
        std::cout << "alias   - Show aliases\n";
        std::cout << "setalias <alias>=<command> - Create an alias\n";
        std::cout << "create <filename> - Create a new file\n";
        std::cout << "delete <filename> - Delete a file\n";
        std::cout << "list    - List files in the current directory\n";
        std::cout << "ping <host> - Ping a host\n";
        std::cout << "random  - Generate a random number\n";
        std::cout << "sleep <seconds> - Sleep for a specified duration\n";
        std::cout << "theme   - Change terminal theme\n";
        std::cout << "writefile <filename> <text> - Write text to a file\n";
        std::cout << "readfile <filename> - Read text from a file\n";
    }

    void printHistory() {
        std::cout << "Command History:\n";
        for (const auto& cmd : commandHistory) {
            std::cout << " - " << cmd << std::endl;
        }
    }

    void printAliases() {
        std::cout << "Aliases:\n";
        for (const auto& pair : aliases) {
            std::cout << " - " << pair.first << " -> " << pair.second << std::endl;
        }
    }

    void setAlias(const std::string& command) {
        size_t pos = command.find("=");
        if (pos != std::string::npos) {
            std::string alias = command.substr(0, pos);
            std::string cmd = command.substr(pos + 1);
            aliases[alias] = cmd;
            std::cout << "Alias set: " << alias << " -> " << cmd << std::endl;
        } else {
            std::cout << "Usage: setalias <alias>=<command>\n";
        }
    }

    void createFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (outFile) {
            std::cout << "File '" << filename << "' created.\n";
        } else {
            std::cerr << "Error: Unable to create file '" << filename << "'.\n";
        }
    }

    void deleteFile(const std::string& filename) {
        if (fs::remove(filename)) {
            std::cout << "File '" << filename << "' deleted.\n";
        } else {
            std::cerr << "Error: Unable to delete file '" << filename << "'.\n";
        }
    }

    void listFiles() {
        std::cout << "Files in the current directory:\n";
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            std::cout << " - " << entry.path().filename().string() << std::endl;
        }
    }

    void pingHost(const std::string& host) {
        std::string command = "ping " + host;
        system(command.c_str());
    }

    void generateRandomNumber() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, 100);
        std::cout << "Random number: " << distr(gen) << std::endl;
    }

    void sleepForSeconds(const std::string& secondsStr) {
        int seconds = std::stoi(secondsStr);
        std::cout << "Sleeping for " << seconds << " seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        std::cout << "Awake!\n";
    }

    void runCommand(const std::string& command) {
        system(command.c_str());
    }
};

int main() {
    Terminal terminal;
    terminal.run();
    return 0;
}