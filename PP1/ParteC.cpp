#include <iostream>
#include <list>
#include <utility>
#include <stdexcept>

using uint = unsigned int;
using Vertex = uint;
using Weight = float;
using VertexWeightPair = std::pair<Vertex, Weight>;

class WeightedGraphAL {
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

int main() {
    uint n, m;
    std::cin >> n >> m;

    WeightedGraphAL graph(n);

    for (uint i = 0; i < m; ++i) {
        Vertex u, v;
        Weight w;
        std::cin >> u >> v >> w;
        try {
            graph.add_edge(u, v, w);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro ao adicionar aresta: " << e.what() << std::endl;
        }
    }

    PrintAdjacencyList(graph);

    return 0;
}
