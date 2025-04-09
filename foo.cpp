#include <iostream>
#include <sstream>
#include <string>

int main (int argc, char *argv[]) {
  std::string input;
  getline(std::cin, input);
  std::cout << input << std::endl;

  std::string drugi;

  std::istringstream iss(input);
  iss >> input;
  iss >> drugi; 
  std::cout << input << std::endl;
  std::cout << drugi << std::endl;
  return 0;
}
