#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>

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
        std::cout << "Willkommen beim Aether Terminal!\n";
        std::cout << "Tippe 'help' für eine Liste der Befehle.\n" << std::endl;
    }

    void printHelp() {
        setConsoleColor(7, 0); 
        std::cout << "Verfügbare Befehle:\n";
        std::cout << "  help        - Zeigt diese Hilfe an\n";
        std::cout << "  cls         - Löscht den Bildschirm\n";
        std::cout << "  exit        - Beendet das Terminal\n";
        std::cout << "  echo [text] - Gibt den angegebenen Text aus\n";
        std::cout << "  time        - Zeigt die aktuelle Zeit an\n";
        std::cout << "  calc        - Startet den Windows-Rechner\n";
        std::cout << "  dir         - Listet die Dateien im aktuellen Verzeichnis auf\n";
        std::cout << "  cd [dir]    - Wechselt das Verzeichnis\n";
        std::cout << "  mkdir [dir] - Erstellt ein neues Verzeichnis\n";
        std::cout << "  rmdir [dir] - Löscht ein leeres Verzeichnis\n";
        std::cout << "  del [file]  - Löscht eine Datei\n";
        std::cout << "  copy [src] [dest] - Kopiert eine Datei\n";
        std::cout << "  move [src] [dest] - Verschiebt eine Datei\n";
        std::cout << "  ping [addr] - Prüft die Erreichbarkeit einer Adresse\n";
        std::cout << "  ipconfig    - Zeigt die Netzwerkinformationen an\n";
        std::cout << "  tasklist    - Listet alle laufenden Prozesse auf\n";
        std::cout << "  taskkill [pid] - Beendet einen Prozess anhand seiner PID\n";
        std::cout << "  systeminfo   - Zeigt Systeminformationen an\n";
        std::cout << "  notepad      - Öffnet den Windows-Editor\n";
        std::cout << "  shutdown     - Fährt den Computer herunter\n";
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
            std::cout << "Aktuelle Zeit: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;
            return;
        } else if (command == "calc") {
            system("start calc");
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
            std::cerr << "⚠️  Fehler: Befehl konnte nicht ausgeführt werden! Fehlercode: " << errorCode << std::endl;
            setConsoleColor(7, 0); 
        }
    }
};

int main() {
    Terminal terminal;
    terminal.run();
    return 0;
}