#include <stdio.h>
#include <list>
#include <stdexcept>
#include <iostream>

using Vertex = unsigned int;
using uint = unsigned int;

class GraphAL {
private:
    uint num_vertices;
    uint num_edges;
    std::list<Vertex>* adj;

public:
    GraphAL(uint num_vertices) : num_vertices(num_vertices), num_edges(0) {
        adj = new std::list<Vertex>[num_vertices];
    }
    ~GraphAL() {
        delete[] adj;
        adj = nullptr;
    }
    void add_edge(Vertex u, Vertex v);
    void remove_edge(Vertex u, Vertex v);
    std::list<Vertex> get_adj(Vertex u) const;
    uint get_num_vertices() const { return num_vertices; }
    uint get_num_edges() const { return num_edges; }
};

void GraphAL::add_edge(const Vertex u, const Vertex v) {
    if (u >= num_vertices || v >= num_vertices || u == v) {
        throw std::invalid_argument("Invalid parameters");
    }
    adj[u].push_back(v);
    adj[v].push_back(u);
    num_edges++;
}

std::list<Vertex> GraphAL::get_adj(Vertex u) const {
    if (u >= num_vertices) {
        throw std::invalid_argument("Invalid parameter");
    }
    return adj[u];
}

void Print_Adjacency_List(const GraphAL& g) {
    printf("num_vertices: %u\n", g.get_num_vertices());
    printf("num_edges: %u\n", g.get_num_edges());
    for (uint u = 0; u < g.get_num_vertices(); ++u) {
        printf("%u:", u);
        std::list<Vertex> l = g.get_adj(u);
        for (Vertex v : l) {
            printf(" %u,", v);
        }
        printf("\n");
    }
}

int main() {
    uint n = 0;
    uint m = 0;
    std::cin >> n >> m;
    GraphAL graph(n);
    uint u = 0, v = 0;
    for (uint i = 0; i < m; ++i) {
        std::cin >> u >> v;
        try {
            graph.add_edge(u, v);
        } catch (const std::exception& e) {
            std::cerr << "Error adding edge {" << u << ", " << v << "}: " << e.what() << std::endl;
        }
    }
    Print_Adjacency_List(graph);
    return 0;
}
