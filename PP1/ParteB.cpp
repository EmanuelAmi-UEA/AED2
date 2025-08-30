        #include <iostream>
        #include <vector>
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
            GraphAM(uint num_vertices): num_vertices(num_vertices), num_edges(0),adj(num_vertices, std::vector<Weight>(num_vertices, 0)) {}

            void add_edge(int u, int v) {
                if (u < 0 || v < 0 || u >= (int)num_vertices || v >= (int)num_vertices)
                    throw std::invalid_argument("Vertice fora do intervalo.");

                if (u == v)
                    throw std::invalid_argument("Nao pode haver autolaco.");

                if (edge_exists(u, v))
                    throw std::invalid_argument("Aresta ja existe.");

                adj[u][v] = 1;
                adj[v][u] = 1;
                num_edges++;
            }

            uint get_num_edges() const { return num_edges; }
            uint get_num_vertices() const { return num_vertices; }
            bool edge_exists(int u, int v) const { return adj[u][v] != 0; }

            const std::vector<std::vector<Weight>>& get_adj_matrix() const { return adj; }
        };

        void print_adjacency_matrix(const GraphAM& g) {
            uint n = g.get_num_vertices();
            std::cout << "num_vertices: " << n << "\n";
            std::cout << "num_edges: " << g.get_num_edges() << "\n";

            const auto& adj = g.get_adj_matrix();
            for (uint i = 0; i < n; i++) {
                for (uint j = 0; j < n; j++)
                    std::cout << adj[i][j] << " "<<std::flush   ;
                std::cout << "\n";
            }
        }

        int main() {
            uint n, m;
            std::cin >> n >> m;

            GraphAM graph(n);

            for (uint i = 0; i < m;) { 
                int u, v;
                std::cin >> u >> v;

                try {
                    graph.add_edge(u, v);
                    i++; 
                } catch (const std::invalid_argument& e) {
                    std::cout << "Erro ao adicionar aresta: " << e.what() << "\n";
                }
            }

            print_adjacency_matrix(graph);
            return 0;
        }
