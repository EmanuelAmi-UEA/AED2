#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>


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
      std::vector<Color> color; // Cor (w (white)= undiscovered, g (grey)= discovered but not explored, b (black) = explored)
      std::vector<int> pred; // Lista de predecessores 
      std::vector<int> dist; // Distancias


  public:
      BFS(const GraphAL& g):g(g),color(g.get_num_vertices(),'w'),pred(g.get_num_vertices(),-1),dist(g.get_num_vertices(),-1) {}

    ~BFS(){
      // Destrutor da Classe BFS vazio
    }

    void traverse (Vertex start){
      if (start >= g.get_num_vertices()) {
        throw std::invalid_argument("Vertice invalido");
      }

      std::list<Vertex> q; // std::list como fila
      color[start]='g';
      dist[start] = 0;
      q.push_back(start); // Adiciona ao fim da fila
      
      while (!q.empty()){
        Vertex u = q.front(); // Pega o elemento 0 
        q.pop_front(); // Remove o elemento 0

        for(const Vertex& v : g.get_adj(u)){
          if(color[v]=='w'){
            color[v]='g';
            pred[v] = u;
            dist[v] = dist[u] + 1;
            q.push_back(v); // Adiciona no final da fila
          }
        }
        color[u] ='b'; // Marca como explorado (black)


      }

    }

    const std::vector<int>& get_pred() const{return pred;}
    const std::vector<int>& get_dist() const{return dist;}
};
// Classe cavaleiro;
class Knight{
  private:
    std::vector<int> s;  // Posicao inicial
    std::vector<int> goal; // Posicao do rei
    int d; // Menor Distancia;
  public:

    Knight(std::vector<int> s, std::vector<int> goal): s(s),goal(goal),d(-1){};

    ~Knight(){} // Sem necessidade de desalocar manualmente.

    int get_distance () {return d;}

    void calculate_distance(const GraphAL& g){
      int start = s[0]*8 + s[1];
      int target = goal[0]*8 + goal[1];

      BFS bfs(g);
      bfs.traverse(start);

      d = bfs.get_dist()[target];
    }

};

std::vector<std::string> treat_input(const std::string& input){
  // Recebe o input e devolve um vetor com posicoes
  std::vector<std::string>result;
  std::istringstream iss(input);
  std::string word;

  while (iss >> word){
    result.push_back(word);
  }

  return result;

};
GraphAL build_chess_graph(){
  GraphAL c(64); // Grafo 8x8
  int mov_x[8] = {-2,-2,-1,-1,1,1,2,2};  // Exemplo: (-2  -> 2 para cima, -1 -> 1 para esquerda) (linha,coluna) sendo que a linha (0,0) está em A1
  int mov_y[8] = {-1,1,-2,2,-2,2,-1,1}; //  Pares de movimentos possíveis para um cavaleiro.

  for(int i =0; i< 8;i++){
    for(int j = 0; j<8 ; j++){
      int u = i*8 +j;
      for(int k =0;k<8; k++){
        int ni = i + mov_x[k]; // Usa indice k para selecionar movimentos válidos
        int nj = j + mov_y[k]; 
        if( ni >=0 && ni <8 && nj >=0 && nj <8){
          int v = ni*8 + nj;
          if(u<v) {
            c.add_edge(u,v);
          }
        }
      }
    }

  }
  return c;
};
std::vector<int> chess_to_matrix_notation(const std::string& position){
  std::string lines = "abcdefgh";
  std::string cols = "12345678";

  int i = lines.find(position[0]);
  int j = cols.find(position[1]);

  return {i,j};

};

int main() {
  std::string input;
  std::getline(std::cin,input); // Input
  std::vector<std::string> result = treat_input(input); // Transforma o Input

  GraphAL g = build_chess_graph(); // Nosso tabuleiro com os movimentos válidos para um cavaleiro.

  std::vector<Knight> knights; // Vetor de cavaleiros
  for(int i =0;i< 4;i++){
    Knight k(chess_to_matrix_notation(result[i]),chess_to_matrix_notation(result[4]));  
    k.calculate_distance(g);
    knights.push_back(k);
  }

  for(int i =0;i<knights.size();i++){
    std::cout <<"Knight " << i+1 << " -> distância até o rei: " << knights[i].get_distance() <<std::endl;
  
  }
  return 0;
};