#include <iostream>
#include <vector>
#include <list>
#include <stdexcept>


using uint = unsigned int;
using Vertex = unsigned int;
using Weight = unsigned int;

class GraphAM {
private:
    uint num_vertices;
    uint num_edges;
    std::vector<std::vector<Weight>> adj;

public:

    GraphAM(uint num_vertices) : num_vertices(num_vertices), num_edges(0), adj(num_vertices, std::vector<Weight>(num_vertices, 0)) {}


    ~GraphAM() {} // Não é necessário desalocar, Container STL libera automaticamente.


    void GraphAM::add_edge(const Vertex& u,const Vertex& v) {
        if (u >= num_vertices || v >= num_vertices || u == v) {
            throw std::invalid_argument("Argumentos de vértice inválidos.");
            return;
        }
        adj[u][v] = 1;
        adj[v][u] = 1; 
        num_edges++;
        /*
        if (adj[u][v] != 0) {
            std::cout << "Aresta já existe!\n";
            return;
        }
        */
    }

    uint GraphAM::get_num_vertices ()const {return num_vertices;};

    std::list<Vertex> GraphAM::get_adj(Vertex u) const {
      if (u >= num_vertices) throw std::invalid_argument("Vértice inválido");
      std::list<Vertex> l;
        for (Vertex v = 0; v < num_vertices; ++v) {
            if (adj[u][v] != 0) {
                l.push_back(v);
            }
        }
        return l;
    }

    std::vector<std::vector<uint>> GraphAM::get_adj_matrix() const{
      return adj;
    }
    
    void GraphAM::print_adjacency_matrix(const GraphAM& g) const{
        auto adj_matrix = g.get_adj_matrix();
        auto n = g.get_num_vertices();

        for(int i =0 ;i < n ;i++){
            for(int j =0 ;j<n;j++){
                std::cout << adj_matrix[i][j] << " ";
            }
            std::cout << "\n";
        }


    }
    bool GraphAM::edge_exists(const Vertex& u , const Vertex& v){
        std::cout << "Vétice já existe!";
        if (adj[v][u] !=0){ return true;}
    }
    
};

int main () {

    uint n = 0;
    uint m =0;

    std::cout << "Entre com a ordem do grafo: " << std::endl;
    std::cin >> n;
    std::cout <<"Entre com o número de arestas: " << std::endl;
    std::cin >> m;

    GraphAM graph = GraphAM(n);

    uint u =0;
    uint v =0;

    for (int i =0;i<n;i++){
        std::cout<< "Vertice " << i+1 << std::endl;
        do{
            std::cout << "Valor u:" << std::endl;
            std::cin >> u;
            std::cout << "Valor v:" << std::endl;
            std::cin >> v;
        }while(graph.edge_exists(u,v));
    
    }
    graph.print_adjacency_matrix(graph);

    return 0;
}
