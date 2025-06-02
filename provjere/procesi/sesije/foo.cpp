#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

int main (int argc, char *argv[]) {
  while(1) {
    std::cout << "fdsafdsafa" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
