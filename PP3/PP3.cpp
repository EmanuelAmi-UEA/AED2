#include <iostream>
#include <vector>
#include <stdexcept>

class PriorityQueue{
  private:
    std::vector<int> heap;

    int get_parent(int i){return (i-1)/2; } // int e um tipo leve e primitivo nao necessita o uso de referencias constantes (const)
    int get_left_child(int i) {return 2*i + 1;}
    int get_right_child (int i ){return 2*i +2;}
    
    void heapify_up(int index){ // Move um elemento de baixo para cima ate que ele esteja na posicao correta
      while (index >0 && heap[get_parent(index)] < heap[index]){
        std::swap(heap[get_parent(index)],heap[index]);
        index = get_parent(index);
      }
    }
    void max_heapify(int index, int size){ // Move um elemento para baixo no heap ate que esteja na posicao correta
      int largest = index;
      int left = get_left_child(index);
      int right =get_right_child(index);

      if (left< size && heap[left]>heap[largest]) {
        largest = left;
      }
      if (right < size && heap[right]> heap[largest]){
        largest = right;
      }
      if (largest != index){
        std::swap(heap[index],heap[largest]);
        max_heapify(largest,size);
      }
    }
    void build_max_heap(){ // Ordena todos os elementos do heap
      int size = heap.size();
      for (int i = (size/2)-1;i>=0;i--){
        max_heapify(i,size);
      }
    }
  public:

    PriorityQueue(); // Construtor default

    PriorityQueue(const std::vector<int>& arr) :heap(arr){
      build_max_heap(); // Inicializa a FP e ordena em heap instantaneamente.
    }

    void insert(int key){
      heap.push_back(key);
      heapify_up(heap.size()-1);
    }

    int maximum() const{
      if (heap.empty()){
        throw std::runtime_error("A Fila de Prioridade esta vazia!");
      }
      return heap[0];
    }
    
    int extract_max(){
      if(heap.empty()){
        throw std::runtime_error("A Fila de Prioridade esta vazia!");
      }
      int max_value = heap[0];
      heap[0] = heap.back();
      heap.pop_back();
      if (!heap.empty()){
        max_heapify(0,heap.size());
      }
      return max_value;
    }
  void increase_key(int i, int new_key){
    if (i<0 || i>= (int)heap.size()){
      throw std::out_of_range("Indice fora de alcance!");

    }
    if (new_key < heap[i]){
      throw std::invalid_argument("Nova chave e menor que chave atual!");

    }
    heap[i] = new_key;
    heapify_up(i);

  }

  bool empty () const { return heap.empty();}

  int size() const {return heap.size();}

  // TO DO: implementar Heap binario minimo

  
};

int main() {
  std::cout << "Hello world!";

  return 0;
}