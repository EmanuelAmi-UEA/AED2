#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>
/*

class Graph {...}; // Deixa o Grafo do jeito que está.

class BFS {
  Graph g;
  std::vector<int> pred;
  std::vector<Color> color;
  std::vector<uint> dist;
  // Orientada a Objetos.
  processBFS(){

  }
};
*/
// Grafo com lista de adjacência implementado!
using uint =unsigned int;
using Vertex = unsigned int;
using Color = char;
class GraphAL {
  private:
    uint num_vertices;
    uint num_edges;
    std::list<Vertex>* adj;
  public: 
    GraphAL(uint num_vertices): num_vertices(num_vertices), num_edges(0) {
      adj = new std::list<Vertex>[num_vertices];
    }
    ~GraphAL(){ 
      delete[]adj;
      adj = nullptr;
    }
    void add_edge(const Vertex& u, const Vertex& v){
      if (u >=num_vertices || v >=num_vertices|| u==v){
        throw std::invalid_argument("Parametros invalidos para vertices.");
      }
      adj[u].push_back(v);
      adj[v].push_back(u);
      num_edges++;
    }
    uint get_num_vertices()const{
      return num_vertices;
    }
    uint get_num_edges()const{
      return num_edges;
    }
    const std::list<Vertex>& get_adj(const Vertex& u)const{
      if(u>= num_vertices){
        throw std::invalid_argument("Parametro invalido para get adj.");

      }
      return adj[u];
    }
};
// Classe BFS Para Orientacao a objetos (MANDATORIO).
class BFS{
  private:
      const GraphAL& g; // Recebe o Grafo de Lista de Adjacência
      std::vector<Color>* color; // Cor (w (white)= undiscovered, g (grey)= discovered but not explored, b (black) = explored)
      std::vector<Vertex>* pred; // Lista de predecessores 


  public:
      BFS(const GraphAL& g):g(g) {
        this -> color = new std::vector<Color>(g.get_num_vertices(),'w'); 
        this -> pred = new std::vector<Vertex>(g.get_num_vertices(),-1); 
      }

    ~BFS(){
      delete color;
      delete pred;  // Destrutor da Classe BFS desaloca Color e Predecessor.
    }

    void traverse (Vertex start){
      if (start >= g.get_num_vertices()) {
        throw std::invalid_argument("Vertice invalido");
      }

      std::list<Vertex> q; // std::list como fila
      (*color)[start]='g';
      q.push_back(start); // Adiciona ao fim da fila
      
      while (!q.empty()){
        Vertex u = q.front(); // Pega o elemento 0 
        q.pop_front(); // Remove o elemento 0
        std::cout << "Visitado: " << u << std::endl;

        for(const Vertex& v : g.get_adj(u)){
          if((*color)[v]=='w'){
            (*color)[v]='g';
            (*pred)[v] = u;
            q.push_back(v); // Adiciona no final da fila
          }
        }
        (*color)[u] ='b'; // Marca como explorado (black)


      }

    }

    const std::vector<Vertex>& get_pred() const{return *pred;}
};
std::vector<std::string> parse_input(const std::string& input){
  // Recebe o input e devolve um vetor com as posições parseado
  std::vector<std::string>result;
  std::istringstream iss(input);
  std::string word;

  while (iss >> word){
    result.push_back(word);
  }

  return result;

}

int main() {
  std::string input;
  std::getline(std::cin,input);
  std::vector<std::string> result = parse_input(input);
  std::cout <<"Dentro do vector: "<< std::endl;
  for(int i =0; i<result.size();i++){
    std::cout<<result[i]<<std::endl;
  }
  return 0;
};
