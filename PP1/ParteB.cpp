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
    GraphAM(uint num_vertices)
        : num_vertices(num_vertices), num_edges(0), adj(num_vertices, std::vector<Weight>(num_vertices, 0)) {}

    ~GraphAM() {}

    void add_edge(const Vertex& u, const Vertex& v) {
        if (u >= num_vertices || v >= num_vertices || u == v) {
            throw std::invalid_argument("Argumentos de vértice inválidos.");
        }
        if (!edge_exists(u, v)) {
            adj[u][v] = 1;
            adj[v][u] = 1;
            num_edges++;
        }
    }
    uint get_num_edges () const {return num_edges;}
    uint get_num_vertices() const { return num_vertices; }

    std::vector<std::vector<uint>> get_adj_matrix() const {
        return adj;
    }

    bool edge_exists(const Vertex& u, const Vertex& v) const {
        return adj[u][v] != 0;
    }
};
 void print_adjacency_matrix(GraphAM g){
    uint num_vertices = g.get_num_vertices();
    std::cout << "num vertices: " << num_vertices;
    uint num_edges = g.get_num_edges();
    std::vector<std::vector<Weight>> adj = g.get_adj_matrix();
        for (uint i = 0; i < num_vertices; i++) {
            for (uint j = 0; j < num_vertices; j++) {
                std::cout << adj[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

   
int main() {
    uint n = 0, m = 0;
    std::cout << "Entre com n e m: ";
    std::cin >> n >> m;

    GraphAM graph(n);

    for (uint i = 0; i < m; i++) {
        uint u = 0, v = 0;
        do {
            std::cout << "Aresta " << i + 1 << "\n";
            std::cout << "Valor u: ";
            std::cin >> u;
            std::cout << "Valor v: ";
            std::cin >> v;

            if (graph.edge_exists(u, v)) {
                std::cout << "Aresta já existe!\n";
            }
        } while (graph.edge_exists(u, v));

        try {
            graph.add_edge(u, v);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro ao adicionar aresta: " << e.what() << "\n";
            i--; 
        }
    }
    print_adjacency_matrix(graph);

    return 0;
}
