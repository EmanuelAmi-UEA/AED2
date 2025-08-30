#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <limits>

using uint = unsigned int;
using Vertex = uint;
using Weight = float;

class WeightedGraphAM{
  private:
    uint num_vertices;
    uint num_edges;
    std::vector<std::vector<Weight>> adj;

  public:

    WeightedGraphAM(uint num_vertices): num_vertices(num_vertices), num_edges(0), 
    adj(num_vertices,std::vector<Weight>(num_vertices,std::numeric_limits<Weight>::infinity())) {}

    ~WeightedGraphAM(){} // Destrutor Vazio, pois adj desaloca automaticamente.

    void add_edge(const Vertex& u, const Vertex& v, const Weight& w){
      if (u >=num_vertices || v >= num_vertices || u==v){
        throw std::invalid_argument("Vertices inválidos!");
      }

      adj[u][v] = w;
      adj[v][u] = w;
      num_edges++;
    }
    uint get_num_vertices( )const {return num_vertices;}
    uint get_num_edges ()const {return num_edges; } 
    std::list<Vertex> get_adj(const Vertex& u)const{
      if (u >=num_vertices){
        throw std::invalid_argument("Vertice inválido para getadj!");
      }
      std::list<Vertex> adjacentes;

      for(Vertex v=0;v<num_vertices;++v){
        if(adj[u][v] !=std::numeric_limits<Weight>::infinity()){
          adjacentes.push_back(v);
        }
      }
      return adjacentes;
    }
    const std::vector<std::vector<Weight>>& get_adj_matrix() const{
      return adj;
    }

    Weight get_weight(const Vertex& u, const Vertex& v){

      if ( u >= num_vertices || v>= num_vertices){
        throw std::invalid_argument("Não é possivel obter peso para este par de vértices!");
      }
      return adj[u][v];

    }
  
};

void print_adjacency_matrix(const WeightedGraphAM& g){
  std::cout <<"num vertices: " << g.get_num_vertices()<<std::endl;
  std::cout <<"num edges: " <<g.get_num_edges()<<std::endl;
  const auto& adj = g.get_adj_matrix();

  for (const auto& linha:adj){
    for(const auto& peso:linha){
      if(peso == std::numeric_limits<Weight>::infinity()){
        std::cout << "inf ";
      }else{
        std::cout<< peso << " ";
      }
    }
    std::cout << "\n";
  }
}

int main(){


  uint n = 0;
  uint m = 0;
  std::cout << "digite a ordem do grafo e o numero de arestas: ";
  std::cin >> n >> m;

  WeightedGraphAM graph(n);

  uint u = 0;
  uint v = 0;
  Weight w = 0;

  for(uint i =0;i<m;++i){
    std::cout << "Entre com u, v e w: ";
    std::cin >> u >> v >> w;
    try{
      graph.add_edge(u,v,w);
    }catch (const std::invalid_argument& e){
      std::cerr << "Não foi possivel adicionar aresta, erro: " << e.what() << "\n";
    }
  }


  print_adjacency_matrix(graph);
  return 0;
}