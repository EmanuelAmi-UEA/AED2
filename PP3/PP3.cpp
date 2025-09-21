#include <iostream>
#include <vector>
#include <stdexcept>
#include <list>
#include <utility>
#include <unordered_map>

using uint = unsigned int;
using Vertex = uint;
using Weight = float;
using VertexWeightPair = std::pair<Vertex, Weight>;

class PriorityQueue{
  private:
    std::vector<std::pair<Weight,Vertex>>heap; // Lista que guarda o par Peso, vertice
    std::unordered_map<Vertex,int>pos; //mapeiamento de vertice -> indice

    int get_parent(int i){return (i-1)/2; } // int e um tipo leve e primitivo nao necessita o uso de referencias constantes (const)
    int get_left_child(int i) {return 2*i + 1;}
    int get_right_child (int i ){return 2*i +2;}
    
    void heapify_up(int index){ // Move um elemento de baixo para cima ate que ele esteja na posicao correta
      while (index >0 && heap[get_parent(index)] > heap[index]){
        std::swap(heap[get_parent(index)],heap[index]);
        index = get_parent(index);
      }
    }
    void min_heapify(int index, int size){ // Move um elemento para baixo no heap ate que esteja na posicao correta
      int smallest = index;
      int left = get_left_child(index);
      int right =get_right_child(index);

      if (left< size && heap[left]<heap[smallest]) {
        smallest = left;
      }
      if (right < size && heap[right]< heap[smallest]){
        smallest = right;
      }
      if (smallest != index){
        std::swap(heap[index],heap[smallest]);
        min_heapify(smallest,size);
      }
    }
    void build_min_heap(){ // Ordena todos os elementos do heap
      int size = heap.size();
      for (int i = (size/2)-1;i>=0;i--){
        min_heapify(i,size);
      }
    }
  public:

    PriorityQueue(); // Construtor default

    PriorityQueue(const std::vector<int>& arr) :heap(arr){
      build_min_heap(); // Inicializa a FP e ordena em heap instantaneamente.
    }

    void insert(const std::pair<Weight,Vertex>& key){
      heap.push_back(key);
      heapify_up(heap.size()-1);
    }

    std::pair<Weight,Vertex> minimum() const{
      if (heap.empty()){
        throw std::runtime_error("A Fila de Prioridade esta vazia!");
      }
      return heap[0];
    }
    
    std::pair<Weight,Vertex> extract_min(){
      if(heap.empty()){
        throw std::runtime_error("A Fila de Prioridade esta vazia!");
      }
      auto min_value = heap[0];
      heap[0] = heap.back();
      heap.pop_back();
      if (!heap.empty()){
        min_heapify(0,heap.size());
      }
      return min_value;
    }
    void decrease_key(Vertex v, Weight new_dist) {
      if (pos.find(v) == pos.end()) return; // vertice não está mais no heap
      int i = pos[v];
      if (new_dist > heap[i].first) {
          throw std::invalid_argument("Nova chave maior que a atual!");
      }
      heap[i].first = new_dist;
      heapify_up(i);
  }

  bool empty () const { return heap.empty();}

  int size() const {return heap.size();}



  
};

// Grafo bidirecionado com pesos
class WeightedGraphAL{
  private:
    uint num_vertices;
    uint num_edges;
    std::list<VertexWeightPair>* adj;

    bool is_valid_vertex(Vertex v) const {
        return (v < num_vertices);
    }

  public:
    WeightedGraphAL(uint num_vertices) {
        this->num_vertices = num_vertices;
        this->num_edges = 0;
        this->adj = new std::list<VertexWeightPair>[num_vertices];
    }

    ~WeightedGraphAL() {
        delete[] adj;
        adj = nullptr;
    }

    void add_edge(Vertex u, Vertex v, Weight w) {
        if (!is_valid_vertex(u) || !is_valid_vertex(v) || u == v) {
            throw std::invalid_argument("Vertices invalidos ou laco.");
        }

        adj[u].push_back(std::make_pair(v, w));
        adj[v].push_back(std::make_pair(u, w));

        num_edges++;
    }

    void remove_edge(Vertex u, Vertex v) {
        if (!is_valid_vertex(u) || !is_valid_vertex(v) || u == v) {
            throw std::invalid_argument("Vertices invalidos para remocao.");
        }
        
        auto original_size = adj[u].size();
        adj[u].remove_if([v](const VertexWeightPair& pair) {
            return pair.first == v;
        });

        if (adj[u].size() < original_size) {
            adj[v].remove_if([u](const VertexWeightPair& pair) {
                return pair.first == u;
            });
            num_edges--;
        }
    }

    const std::list<VertexWeightPair>& get_adj(Vertex u) const {
        if (!is_valid_vertex(u)) {
            throw std::invalid_argument("Vertice invalido.");
        }
        return adj[u];
    }

    uint get_num_vertices() const {
        return num_vertices;
    }
    
    uint get_num_edges() const {
        return num_edges;
    }
    bool has_edge(Vertex u, Vertex v) const{
      if(!is_valid_vertex(u)|| !is_valid_vertex(v)){ return false;}

      for(const auto&pair:adj[u]){
        if(pair.first==v){
          return true;
        }
      }
      return false;
    }
    Weight get_weight(Vertex u, Vertex v) const{
      if(!is_valid_vertex(u) || !is_valid_vertex(v)){
        throw std::invalid_argument("Vertice Invalido.");
      }
      for (const auto& pair:adj[u]){
        if(pair.first==v){
          return pair.second;
        }
      };
      throw std::runtime_error("Aesta nao existe.");
    };
};

void PrintAdjacencyList(const WeightedGraphAL& g) {
    std::cout << "num_vertices: " << g.get_num_vertices() << std::endl;
    std::cout << "num_edges: " << g.get_num_edges() << std::endl;
    for (Vertex u = 0; u < g.get_num_vertices(); ++u) {
        std::cout << u << ": ";
        const auto& adj_list = g.get_adj(u);
        for (const auto& pair : adj_list) {
            std::cout << "(" << pair.first << ", " << pair.second << "), ";
        }
        std::cout << std::endl;
    }
}

bool relax(Vertex u, Vertex v, Weight w, std::vector<Weight>& dist, std::vector<Vertex>& pred) {
  // Relaxamento   
  if (dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pred[v] = u;
        return true;
    }
    return false;
}
std::vector<Weight> dijkstra(const WeightedGraphAL& g, Vertex source) {
    uint n = g.get_num_vertices();
    std::vector<Weight> dist(n, std::numeric_limits<Weight>::infinity());
    std::vector<Vertex> pred(n, -1);

    PriorityQueue pq;

    // inicializa todos os vertices na fila
    for (Vertex u = 0; u < n; u++) {
        if (u == source) dist[u] = 0.0;
        pq.insert({dist[u], u});
    }

    while (!pq.empty()) {
        auto [d, u] = pq.extract_min();

        for (const auto& [v, w] : g.get_adj(u)) {
            if (relax(u, v, w, dist, pred)) {
                pq.decrease_key(v, dist[v]);
            }
        }
    }
    return dist;
}

int main() {
  std::cout << "Hello world!";
  // TO DO : Formatar a entrada para formar o Grafo 
  return 0;
}