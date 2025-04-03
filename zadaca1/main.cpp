#include "mBash.hpp"
#include <iostream>
#include <sstream>
#include <string>

void mBashInfo() {
  std::cout << "==================================\n";
  std::cout << "        Welcome to mBash!         \n";
  std::cout << "==================================\n";
  std::cout << "Available commands:\n";
  std::cout << "  mls    - List directory contents\n";
  std::cout << "  mcat   - Display file contents\n";
  std::cout << "  mtouch - Create an empty file\n";
  std::cout << "  mecho  - Print text to the console\n";
  std::cout << "----------------------------------\n";
}

int main(int argc, char *argv[]) {
  mBashInfo();
  std::string input, command, argument;

  while (1) {
    std::cout << ">> ";
    std::getline(std::cin, input);

    std::istringstream iss(input);
    iss >> command;
    std::getline(iss, argument);

    if (!argument.empty() && argument[0] == ' ') {
      argument = argument.substr(1);
    } // zbog viska razmaka
    if (argument[0] == ' ' || argument[0] == '\t')
      argument = "";

    if (command == "mls") {
      if (argument == "") argument = ".";
      mls(argument);

    } else if (command == "mcat") {
      mcat(argument);

    } else if (command == "mtouch") {
      mtouch(argument);

    } else if (command == "mecho") {
      mecho(argument);

    } else if (command == "exit" || input == "quit") {
      return 0;

    } else
      std::cout << "Command " << command << " not found." << std::endl;


    command.clear();
    argument.clear();
  }

  return 0;
}
