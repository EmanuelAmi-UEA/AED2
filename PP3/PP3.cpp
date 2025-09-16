#include <iostream>
#include <vector>
#include <stdexcept>

class PriorityQueue{
  private:
  std::vector<int> heap;

  int get_parent(int i){return (i-1)/2; } // int é um tipo leve e primitivo não necessita o uso de referências constantes (const)
  int get_left_child(int i) {return 2*i + 1;}
  int get_right_child (int i ){return 2*i +2;}
  // TO DO: Implementar PQ max com  heap binario de acordo com os slides
};

int main() {
  std::cout << "Hello world!";

  return 0;
}