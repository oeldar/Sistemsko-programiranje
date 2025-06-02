#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>

void prompt() {
    std::cout << "mini-shell> ";
}

void executeCommand(const std::vector<std::string>& args) {
    pid_t pid = fork();  // Fork novo dijete

    if (pid == 0) {
        // Dijete proces
        // Pretvaramo stringove u format koji exec funkcije mogu da prepoznaju
        std::vector<const char*> execArgs;
        for (const auto& arg : args) {
            execArgs.push_back(arg.c_str());
        }
        execArgs.push_back(nullptr);  // Poslednji element mora biti nullptr

        // Koristimo execvp da izvršimo komandu
        if (execvp(execArgs[0], const_cast<char* const*>(execArgs.data())) == -1) {
            perror("execvp failed");
            exit(1);  // Ako exec ne uspije, izlazi sa greškom
        }
    } else if (pid > 0) {
        // Roditeljski proces
        wait(nullptr);  // Čekaj da dijete završi
    } else {
        std::cerr << "Fork failed!" << std::endl;
    }
}

int main() {
    std::string input;

    while (true) {
        prompt();  // Ispisivanje shell prompta
        std::getline(std::cin, input);  // Unos komande

        // Ako korisnik unese "exit", izlazimo iz shell-a
        if (input == "exit") {
            break;
        }

        // Razdvajamo unos na argumente komande (delimo po razmacima)
        std::vector<std::string> args;
        size_t pos = 0;
        while ((pos = input.find(' ')) != std::string::npos) {
            args.push_back(input.substr(0, pos));
            input.erase(0, pos + 1);
        }
        args.push_back(input);  // Poslednji argument

        // Izvršavamo komandu
        executeCommand(args);
    }

    return 0;
}

