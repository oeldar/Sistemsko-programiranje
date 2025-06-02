#include <iostream>
#include <thread>

void foo(){
  while(1);
}

int main (int argc, char *argv[]) {
  std::thread t1{foo};
  return 0;
}
