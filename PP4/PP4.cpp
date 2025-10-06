#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <cctype>
#include <cstdlib>

const int MAX_LINE = 200;
using uint = unsigned int;

// -------------------- Struct de Aresta --------------------
// Usada para armazenar as informacoes de arestas
struct Edge {
    uint u, v;
    int weight;
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// -------------------- Union-Find --------------------
// TAD para criacao de Conjuntos(Sets)
class UnionFind {
    // IMPORTANTE : REFATORAR PARA NAO USAR STD::UNORDERED_MAP
private:
    std::unordered_map<int, int> parent; // mapeia cada elemento para seu pai
    std::unordered_map<int, int> rank;   // aproximacao da altura da arvore

public:
    // MAKE-SET(x): cria um conjunto com x como unico representante
    void makeSet(int x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            rank[x] = 0;
        }
    }

    // FIND-SET(x): retorna o representante do conjunto de x 
    int findSet(int x) {
        if (parent[x] != x) {
            parent[x] = findSet(parent[x]); 
        }
        return parent[x];
    }

    // UNION(x, y): une os conjuntos de x e y (separados) com union 
    // union e palavra reservada.
    void UNION(int x, int y) {
        int repX = findSet(x);
        int repY = findSet(y);
        if (repX == repY) return; // ja estao no mesmo conjunto

        // uniao por rank
        if (rank[repX] < rank[repY]) {
            parent[repX] = repY;
        } else if (rank[repX] > rank[repY]) {
            parent[repY] = repX;
        } else {
            parent[repY] = repX;
            rank[repX]++;
        }
    }
};

// -------------------- Estrutura de Heap Minimo --------------------
// Usada pelo algoritmo de Dijkstra para escolher proximo vertice
class MinHeap {
private:
    std::vector<std::pair<int, uint>> heap; // (distancia, vertice)

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i) const { return 2 * i + 1; }
    int right(int i) const { return 2 * i + 2; }

    void up(int i) {
        while (i > 0 && heap[parent(i)].first > heap[i].first) {
            std::swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    void down(int i) {
        int smallest = i, l = left(i), r = right(i), n = heap.size();
        if (l < n && heap[l].first < heap[smallest].first) smallest = l;
        if (r < n && heap[r].first < heap[smallest].first) smallest = r;
        if (smallest != i) {
            std::swap(heap[i], heap[smallest]);
            down(smallest);
        }
    }

public:
    void insert(int dist, uint v) {
        heap.emplace_back(dist, v);
        up(heap.size() - 1);
    }

    std::pair<int, uint> extractMin() {
        if (heap.empty()) return { -1, 0 };
        auto min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) down(0);
        return min;
    }

    bool empty() const { return heap.empty(); }
};

// -------------------- Grafo Ponderado --------------------
class WeightedGraph {
private:
    uint vertices;
    std::vector<std::list<std::pair<uint, int>>> adj;
    std::vector<Edge> edges;
    uint size;

public:
    WeightedGraph(uint n) : vertices(n), adj(n) {}

    void setSize(uint s){
        size =s;
    }
    uint getSize()const{
        return size;
    }

    void addEdge(uint u, uint v, int w) {
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w); // grafo nao direcionado
        edges.push_back({u, v, w});
    }

    const std::list<std::pair<uint, int>>& getNeighbors(uint u) const {
        return adj[u];
    }

    uint vertexCount() const {
        return vertices;
    }

    int weight(uint u, uint v) const {
        for (const auto& [dest, w] : adj[u]) {
            if (dest == v) return w;
        }
        return std::numeric_limits<int>::max(); // se nao houver aresta
    }

    // -------------------Kruskal---------------------
    std::vector<Edge> kruskalMST() const {
    std::vector<Edge> result;
    UnionFind uf;

    // Inicializa MAKE-SET para cada vertice
    for (uint i = 0; i < vertices; ++i) {
        uf.makeSet(i);
    }

    // 2. Ordena arestas por peso
    std::vector<Edge> sortedEdges = edges;
    std::sort(sortedEdges.begin(), sortedEdges.end());

    // 3. Itera pelas arestas
    for (const auto& edge : sortedEdges) {
        if (uf.findSet(edge.u) != uf.findSet(edge.v)) {
            result.push_back(edge);
            uf.UNION(edge.u, edge.v);
        }
    }

    return result;
}
};

// -------------------- Algoritmo de Dijkstra --------------------
// Calcula menor caminho entre duas posicoes no grafo
class DijkstraSolver {
public:
    std::vector<uint> findPath(const WeightedGraph& g, uint start, uint target) {
        uint n = g.vertexCount();
        std::vector<int> dist(n, std::numeric_limits<int>::max());
        std::vector<int> pred(n, -1);
        MinHeap pq;

        dist[start] = 0;
        pq.insert(0, start);

        while (!pq.empty()) {
            auto [d, u] = pq.extractMin();
            if (d == -1) break;
            if (d > dist[u]) continue;
            if (u == target) break;

            for (const auto& [v, w] : g.getNeighbors(u)) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.insert(dist[v], v);
                }
            }
        }

        if (pred[target] == -1) return {}; // sem caminho

        std::vector<uint> path;
        for (int v = target; v != -1; v = pred[v]) {
            path.push_back(v);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    int pathWeight(const std::vector<uint>& path, const WeightedGraph& g) const {
        if (path.size() < 2) return 0;
        int total = 0;
        for (uint i = 0; i < path.size() - 1; i++) {
            total += g.weight(path[i], path[i + 1]);
        }
        return total;
    }
};

WeightedGraph buildGraph(uint order,uint size){
    // Constroi o cerebro (Grafo grande)

    try{

        WeightedGraph g (order); // Constroi grafo de ordem que esta na entrada

        // Agora pra ler cada aresta
        for(uint i = 0; i < size;i++){
            char line[MAX_LINE];
            std::cin.getline(line,MAX_LINE);
            char* ptr = line;
            std::vector<uint>edge;

            while (*ptr !='\0'){
                while (*ptr !='\0' && std::isspace(*ptr)) ++ptr;
                if (*ptr == '\0') break;

                char* endptr;
                long num = std::strtol(ptr,&endptr,10);

                if (endptr != ptr){
                    if (num <0 || num >std::numeric_limits<uint>::max()){
                        throw std::runtime_error("Numero fora dos limites uint.");

                    }
                    edge.push_back(static_cast<uint>(num));

                }else{
                    throw std::runtime_error("Valor invalido de aresta");
                }

                ptr = endptr;
            }
            if (edge.size() <3){
                throw std::runtime_error("Aresta esta incompleta (sem peso)");
            }

            uint u = edge[0];
            uint v = edge[1];
            uint w =edge[2];

            g.addEdge(u,v,w);
        }
        
        return g;

    }catch(const std::runtime_error& e){
        std::cerr << "Erro: " << e.what() << "\n";
    }  
};
std::vector<uint> getEntranceExit(){
    // Function apenas pra pegar o valor de entrada e saida de um grafo Brain
    char line[MAX_LINE];
    std::cin.getline(line,MAX_LINE);
    char* ptr = line;
    std::vector<uint> entrance_exit;
    while (*ptr != '\0'){
        while (*ptr !='\0' && std::isspace(*ptr)) ++ptr;

        if (*ptr == '\0') break;
        char *endptr;

        long num = std::strtol(ptr,&endptr,10);

        if (endptr != ptr){
            if (num <0 || num >std::numeric_limits<uint>::max()){
                throw std::runtime_error("Numero fora dos limites uint.");

            }
            entrance_exit.push_back(static_cast<uint>(num));

        }else{
            throw std::runtime_error("Valor invalido de entrada e saida do grafo.");
        }

        ptr = endptr;
    }
    return entrance_exit;
   
}
std::vector<uint> getOrderSize(){
    // Function apenas para pegar Ordem e Tamanho de um grafo qualquer.
    char line[MAX_LINE];
    std::cin.getline(line,MAX_LINE);
    char* ptr = line;
    std::vector<uint> order_size;
    try{
        while(*ptr !='\0'){
            while(*ptr !='\0' && std::isspace(*ptr)) ++ptr;

            if(*ptr =='\0') break;

            char* endptr;
            long num = std::strtol(ptr,&endptr,10);

            if(endptr !=ptr){
                if(num< 0 || num > std::numeric_limits<uint>::max()){
                    throw std::runtime_error("Fora do limite uint");
                
                }
                order_size.push_back(static_cast<uint>(num)); // Converte para UINT
            } else{
                throw std::runtime_error("Entrada invalida");
            }

            ptr = endptr;
        }

        if(order_size.size() <2){
            throw std::runtime_error("Entrada insuficiente para ordem e tamanho do grafo.");

        }

        return order_size;

    }catch(const std::runtime_error& e){
        std::cerr << "Erro: " << e.what() <<"\n";
    }
    return {};
    
}
uint getIllNeurons(){
    // Function que pega o numero de neuronios doentes.
    uint illNeurons;
    char line[MAX_LINE];
    std::cin.getline(line,MAX_LINE);
    char* ptr = line;
    while (*ptr !='\0'){
        while(*ptr !='\0' && std::isspace(*ptr)) ++ptr;
        if (*ptr == '\0') break;
        char* endptr;
        long num = std::strtol(ptr,&endptr,10);
        if(endptr !=ptr){
            if(num< 0 || num > std::numeric_limits<uint>::max()){
                throw std::runtime_error("Fora do limite uint");
            }
            illNeurons = num;
        
        }else{
            throw std::runtime_error("Entrada invalida.");
        }
        ptr = endptr;
    }
    return illNeurons;
}

WeightedGraph buildBock(){
    // Aqui se constroi cada minigrafo.
    std::vector<uint> order_size = getOrderSize();
    uint order,size;
    order = order_size[0];
    size = order_size[1];
    // Pegar o numero de vertices doentes.
    uint NumberIllNeurons = getIllNeurons();
    if (NumberIllNeurons != 0 ){
        std::vector<uint> illNeurons;
        char line[MAX_LINE];
        std::cin.getline(line,MAX_LINE);
        char* ptr = line;
        while (*ptr !='\0'){
            while(*ptr !='\0' && std::isspace(*ptr)) ++ptr;
            if (*ptr == '\0') break;
            char* endptr;
            long num = std::strtol(ptr,&endptr,10);
            if(endptr !=ptr){
                if(num< 0 || num > std::numeric_limits<uint>::max()){
                    throw std::runtime_error("Fora do limite uint");
                }
                illNeurons.push_back(num);
            
            }else{
                throw std::runtime_error("Entrada invalida.");
            }
            ptr = endptr;
        }
    }
    WeightedGraph block = buildGraph(order,size);

    return block;

    


};
int main() {
   
    // TO DO: Usar <cstdlib> para converter String para inteiro com std::strtol()
    // USAR CCTYPE E CSTDLIB

return 0;
}