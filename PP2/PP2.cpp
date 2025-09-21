#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using uint = unsigned int;
using Vertex = unsigned int;
using Color = char;

// Classe que representa um grafo usando lista de adjacencia
// Contem metodos para construir o grafo e acessar vertices adjacentes
class GraphAL {
  private:
    uint num_vertices;       // Numero total de vertices
    uint num_edges;          // Numero total de arestas
    std::list<Vertex>* adj;  // Lista de adjacencia
    
    // Ordena as listas de adjacencia em ordem nao decrescente
    void sort_adjacency_list() {
        for (uint i = 0; i < num_vertices; i++) {
            adj[i].sort();
        }
    }
    
  public: 
    // Construtor: inicializa o grafo com numero de vertices
    GraphAL(uint num_vertices): num_vertices(num_vertices), num_edges(0) {
      adj = new std::list<Vertex>[num_vertices];
    }
    
    // Destrutor: libera memoria alocada
    ~GraphAL(){ 
      delete[] adj;
      adj = nullptr;
    }
    
    // Adiciona uma aresta entre os vertices u e v
    // Tambem ordena a lista de adjacencia apos adicionar
    void add_edge(const Vertex& u, const Vertex& v){
      if (u >= num_vertices || v >= num_vertices || u == v){
        return;
      }
      adj[u].push_back(v);
      adj[v].push_back(u);
      num_edges++;
      sort_adjacency_list();
    }
    
    // Retorna o numero de vertices do grafo
    uint get_num_vertices() const {
      return num_vertices;
    }
    
    // Retorna o numero de arestas do grafo
    uint get_num_edges() const {
      return num_edges;
    }
    
    // Retorna a lista de vertices adjacentes ao vertice u
    const std::list<Vertex>& get_adj(const Vertex& u) const {
      return adj[u];
    }
};

// Classe que implementa o algoritmo BFS (Busca em Largura)
// Usada para encontrar a distancia minima ate posicoes que ameacam o rei
class BFS {
  private:
    const GraphAL& g;        // Grafo de referencia
    std::vector<Color> color; // Cores dos vertices (w: branco, g: cinza, b: preto)
    std::vector<int> dist;    // Distancias dos vertices
    Vertex king_pos;         // Posicao do rei no grafo

  public:
    // Construtor: inicializa estruturas para BFS
    BFS(const GraphAL& g, Vertex king): g(g), color(g.get_num_vertices(), 'w'), 
                                      dist(g.get_num_vertices(), -1), king_pos(king) {}

    // Encontra a distancia minima ate uma posicao que ameace o rei
    // Retorna o numero minimo de movimentos necessarios
    int find_min_threat_distance(Vertex start) {
      if (start >= g.get_num_vertices()) {
        return -1;
      }

      std::list<Vertex> q;  // Fila para BFS usando std::list
      color[start] = 'g';   // Marca vertice inicial como descoberto
      dist[start] = 0;      // Distancia inicial zero
      q.push_back(start);   // Adiciona na fila
      
      while (!q.empty()) {
        Vertex u = q.front();  // Pega proximo vertice da fila
        q.pop_front();         // Remove da fila

        // Verifica se esta posicao ameaca o rei
        // Um vertice ameaca o rei se eh adjacente a posicao do rei
        for (const Vertex& threat_pos : g.get_adj(king_pos)) {
          if (u == threat_pos) {
            return dist[u];  // Retorna distancia quando acha ameaca
          }
        }

        // Percorre vertices adjacentes
        for(const Vertex& v : g.get_adj(u)) {
          if(color[v] == 'w') {          // Se vertice nao foi descoberto
            color[v] = 'g';              // Marca como descoberto
            dist[v] = dist[u] + 1;       // Atualiza distancia
            q.push_back(v);              // Adiciona na fila
          }
        }
        color[u] = 'b';  // Marca vertice como explorado
      }
      
      return -1;  // Retorna -1 se nao encontrou (nao deveria acontecer)
    }
};

// Processa string de entrada e retorna vetor de palavras
// Separa a string por espacos em branco
std::vector<std::string> process_input(const std::string& input) {
  std::vector<std::string> result;
  std::istringstream iss(input);
  std::string word;

  while (iss >> word) {
    result.push_back(word);
  }
  return result;
};

// Constroi o grafo do tabuleiro de xadrez para movimentos de cavaleiro
// Cada vertice representa uma casa do tabuleiro (64 vertices)
// As arestas representam movimentos validos em L do cavaleiro
GraphAL build_chess_graph() {
  GraphAL chess_graph(64);  // 8x8 = 64 vertices
  
  // Movimentos possiveis do cavaleiro em L (8 direcoes)
  int moves_x[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
  int moves_y[8] = {-1, 1, -2, 2, -2, 2, -1, 1};

  // Para cada casa do tabuleiro
  for(int row = 0; row < 8; row++) {
    for(int col = 0; col < 8; col++) {
      int current_vertex = row * 8 + col;
      
      // Verifica todos os movimentos possiveis
      for(int move = 0; move < 8; move++) {
        int new_row = row + moves_x[move];
        int new_col = col + moves_y[move];
        
        // Se movimento esta dentro do tabuleiro
        if(new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {
          int new_vertex = new_row * 8 + new_col;
          chess_graph.add_edge(current_vertex, new_vertex);
        }
      }
    }
  }
  return chess_graph;
};

// Converte notacao de xadrez (ex: "e4") para indice do grafo (0-63)
// Letras representam colunas (a-h), numeros representam linhas (1-8)
// A1 = canto inferior esquerdo = indice 63
// H8 = canto superior direito = indice 0
int chess_to_index(const std::string& position) {
  std::string columns = "abcdefgh";  // Colunas de a ate h
  std::string rows = "12345678";     // Linhas de 1 ate 8
  
  int col_index = columns.find(position[0]);  // Encontra indice da coluna
  int row_index = rows.find(position[1]);     // Encontra indice da linha
  
  // Converte para sistema de coordenadas do grafo
  // Inverte as linhas porque no xadrez a1 eh embaixo
  return (7 - row_index) * 8 + col_index;
};

// Funcao principal do programa
int main() {
  std::string input;
  std::getline(std::cin, input);  // Le numero de testes
  int num_tests = std::stoi(input);
  
  // Constroi grafo do tabuleiro (uma vez para todos os testes)
  GraphAL chess_board = build_chess_graph();
  
  // Processa cada caso de teste
  for(int test = 0; test < num_tests; test++) {
    std::getline(std::cin, input);
    std::vector<std::string> positions = process_input(input);
    
    // A quinta posicao eh o rei, as primeiras quatro sao cavaleiros
    int king_index = chess_to_index(positions[4]);
    std::vector<int> knight_distances;  // Armazena distancias dos cavaleiros
    
    // Calcula distancia para cada cavaleiro
    for(int i = 0; i < 4; i++) {
      int knight_index = chess_to_index(positions[i]);
      BFS bfs(chess_board, king_index);
      int distance = bfs.find_min_threat_distance(knight_index);
      knight_distances.push_back(distance);
    }
    
    // Encontra a distancia minima entre todos os cavaleiros
    int min_distance = knight_distances[0];
    for(int i = 1; i < 4; i++) {
      if(knight_distances[i] < min_distance) {
        min_distance = knight_distances[i];
      }
    }
    
    // Primeira linha: mostra distancias minimas na ordem de entrada
    bool first = true;
    for(int i = 0; i < 4; i++) {
      if(knight_distances[i] == min_distance) {
        if(!first) std::cout << " ";
        std::cout << min_distance;
        first = false;
      }
    }
    std::cout << std::endl;
    
    // Segunda linha: mostra 0 se algum cavaleiro ja esta em posicao de ameaca
    if(min_distance == 0) {
      std::cout << "0" << std::endl;
    }
  }
  
  return 0;
}
