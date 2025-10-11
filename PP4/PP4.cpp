#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <cctype>
#include <cstdlib>

const int MAX_LINE = 200;
using uint = unsigned int;

// Representa uma aresta em um grafo com vertices u e v e peso weight
// Usada tanto para o grafo cerebral quanto para os grafos dos blocos
struct Edge {
    uint u, v;
    double weight;
    // Operador de comparacao para ordenacao por peso
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Implementacao da estrutura Union-Find (Disjoint Set Union)
// Utilizada pelo algoritmo de Kruskal para gerenciar conjuntos de vertices
class UnionFind {
private:
    std::vector<int> parent;  // Armazena o representante de cada conjunto
    std::vector<int> rank;    // Otimizacao para uniao por rank

public:
    // Inicializa n conjuntos disjuntos, cada um com um elemento
    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    // Encontra o representante do conjunto que contem x
    // Comprime o caminho para otimizacao futura
    int findSet(int x) {
        if (parent[x] != x) {
            parent[x] = findSet(parent[x]);
        }
        return parent[x];
    }

    // Une os conjuntos que contem x e y
    // Usa uniao por rank para manter a arvore balanceada
    void UNION(int x, int y) {
        int repX = findSet(x);
        int repY = findSet(y);
        if (repX == repY) return;

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

// Implementacao manual do algoritmo de ordenacao bubble sort
// Necessaria pois std::sort nao e permitido pelos requisitos do projeto
void manualSort(std::vector<Edge>& edges) {
    for (size_t i = 0; i < edges.size(); i++) {
        for (size_t j = i + 1; j < edges.size(); j++) {
            if (edges[j].weight < edges[i].weight) {
                std::swap(edges[i], edges[j]);
            }
        }
    }
}

// Implementacao manual da reversao de um vetor
// Substitui std::reverse que nao e permitido
void manualReverse(std::vector<uint>& path) {
    size_t i = 0, j = path.size() - 1;
    while (i < j) {
        std::swap(path[i], path[j]);
        i++;
        j--;
    }
}

// Implementacao de um heap binario minimo
// Utilizado pelo algoritmo de Dijkstra para selecionar o proximo vertice
class MinHeap {
private:
    // Armazena pares (distancia, vertice)
    std::vector<std::pair<double, uint>> heap;

    // Funcoes auxiliares para navegacao no heap
    int parent(int i) const { return (i - 1) / 2; }
    int left(int i) const { return 2 * i + 1; }
    int right(int i) const { return 2 * i + 2; }

    // Move um elemento para cima no heap para manter a propriedade de heap
    void up(int i) {
        while (i > 0 && heap[parent(i)].first > heap[i].first) {
            std::swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    // Move um elemento para baixo no heap para manter a propriedade de heap
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
    // Insere um novo elemento no heap
    void insert(double dist, uint v) {
        heap.emplace_back(dist, v);
        up(heap.size() - 1);
    }

    // Remove e retorna o elemento com menor distancia
    std::pair<double, uint> extractMin() {
        if (heap.empty()) return { -1.0, 0 };
        auto min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) down(0);
        return min;
    }

    bool empty() const { return heap.empty(); }
};

// Classe que representa um grafo ponderado nao direcionado
// Pode representar tanto o grafo cerebral quanto os grafos dos blocos internos
class WeightedGraph {
private:
    uint vertices;  // Numero de vertices no grafo
    // Lista de adjacencia: para cada vertice, lista de (vizinho, peso)
    std::vector<std::list<std::pair<uint, double>>> adj;
    std::vector<Edge> edges;  // Lista de todas as arestas para Kruskal
    bool isSick;    // Indica se este bloco contem neuronios doentes

public:
    // Construtor padrao necessario para compatibilidade com std::vector
    WeightedGraph() : vertices(0), isSick(false) {}
    
    // Construtor que inicializa grafo com n vertices
    WeightedGraph(uint n) : vertices(n), adj(n), isSick(false) {}

    void setSick(bool sick) {
        isSick = sick;
    }

    bool getSick() const {
        return isSick;
    }

    // Adiciona uma aresta nao direcionada entre u e v com peso w
    // Converte indices de 1-based (entrada) para 0-based (interno)
    void addEdge(uint u, uint v, double w) {
        // Ajustar indices para 0-based internamente
        u--; v--;
        
        if (u >= vertices || v >= vertices) {
            return; // Ignora vertices fora do range
        }

        if (u == v) {
            return; // Ignora arestas de loop
        }
        
        // Adiciona aresta em ambas as direcoes (grafo nao direcionado)
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
        edges.push_back({u, v, w});
    }

    // Retorna a lista de vizinhos de um vertice
    const std::list<std::pair<uint, double>>& getNeighbors(uint u) const {
        if (u >= vertices) {
            static std::list<std::pair<uint, double>> empty;
            return empty;
        }
        return adj[u];
    }

    uint vertexCount() const {
        return vertices;
    }

    // Retorna o peso da aresta entre u e v, ou infinito se nao existir
    double weight(uint u, uint v) const {
        if (u >= vertices) return std::numeric_limits<double>::max();
        
        for (const auto& [dest, w] : adj[u]) {
            if (dest == v) return w;
        }
        return std::numeric_limits<double>::max();
    }

    // Implementacao do algoritmo de Kruskal para encontrar MST
    // Retorna a arvore geradora minima do grafo
    std::vector<Edge> kruskalMST() const {
        std::vector<Edge> result;
        UnionFind uf(vertices);

        // Ordena arestas por peso usando sort manual
        std::vector<Edge> sortedEdges = edges;
        manualSort(sortedEdges);

        // Processa arestas em ordem crescente de peso
        for (const auto& edge : sortedEdges) {
            if (uf.findSet(edge.u) != uf.findSet(edge.v)) {
                result.push_back(edge);
                uf.UNION(edge.u, edge.v);
            }
        }

        return result;
    }
};

// Funcao auxiliar para ler um valor double de uma string
double readDouble(char*& ptr) {
    while (*ptr != '\0' && std::isspace(*ptr)) ++ptr;
    if (*ptr == '\0') throw std::runtime_error("Fim inesperado da linha");

    char* endptr;
    double num = std::strtod(ptr, &endptr);
    
    if (endptr == ptr) {
        throw std::runtime_error("Valor double invalido");
    }
    
    ptr = endptr;
    return num;
}

// Funcao auxiliar para ler um valor uint de uma string
uint readUInt(char*& ptr) {
    while (*ptr != '\0' && std::isspace(*ptr)) ++ptr;
    if (*ptr == '\0') throw std::runtime_error("Fim inesperado da linha");

    char* endptr;
    long num = std::strtol(ptr, &endptr, 10);
    
    if (endptr == ptr || num < 0) {
        throw std::runtime_error("Numero uint invalido");
    }
    
    ptr = endptr;
    return static_cast<uint>(num);
}

// Constroi o grafo cerebral a partir da entrada
// O grafo cerebral representa as conexoes entre os blocos de neuronios
WeightedGraph buildBrainGraph(uint order, uint size){
    try {
        // Grafo cerebral: vertices de 1 a 'order'
        WeightedGraph g(order);

        for(uint i = 0; i < size; i++){
            char line[MAX_LINE];
            if (!std::cin.getline(line, MAX_LINE)) {
                throw std::runtime_error("Erro ao ler linha da aresta cerebral");
            }
            
            char* ptr = line;
            uint u = readUInt(ptr);
            uint v = readUInt(ptr);
            double w = readDouble(ptr);

            g.addEdge(u, v, w);
        }
        
        return g;

    } catch(const std::runtime_error& e) {
        std::cerr << "Erro ao construir grafo cerebral: " << e.what() << "\n";
        throw;
    }  
}

// Constroi um grafo de bloco individual a partir da entrada
// Cada bloco representa a rede interna de um grupo de neuronios
WeightedGraph buildBlockGraph(uint order, uint size, bool& isSick) {
    try {
        WeightedGraph block(order);
        isSick = false;

        // Ler numero de neuronios doentes
        char line[MAX_LINE];
        if (!std::cin.getline(line, MAX_LINE)) {
            throw std::runtime_error("Erro ao ler numero de neuronios doentes");
        }
        
        char* ptr = line;
        uint numIll = readUInt(ptr);
        
        if (numIll > 0) {
            isSick = true;
            
            // Ler os neuronios doentes (lista de indices)
            if (!std::cin.getline(line, MAX_LINE)) {
                throw std::runtime_error("Erro ao ler neuronios doentes");
            }
            
            ptr = line;
            // Apenas lemos os neuronios doentes, mas nao usamos para nada
            // pois o grafo ja sera marcado como doente
            for (uint i = 0; i < numIll; i++) {
                readUInt(ptr);
            }
        }

        // Ler as arestas do grafo interno do bloco
        for(uint i = 0; i < size; i++){
            if (!std::cin.getline(line, MAX_LINE)) {
                throw std::runtime_error("Erro ao ler aresta do bloco");
            }
            
            ptr = line;
            uint u = readUInt(ptr);
            uint v = readUInt(ptr);
            double w = readDouble(ptr);

            block.addEdge(u, v, w);
        }
        
        block.setSick(isSick);
        return block;

    } catch(const std::runtime_error& e) {
        std::cerr << "Erro ao construir bloco: " << e.what() << "\n";
        throw;
    }
}

// Le os vertices de entrada e saida do grafo cerebral
std::vector<uint> getEntranceExit(){
    char line[MAX_LINE];
    if (!std::cin.getline(line, MAX_LINE)) {
        throw std::runtime_error("Erro ao ler entrada/saida");
    }
    
    char* ptr = line;
    std::vector<uint> entrance_exit;
    
    try {
        while (*ptr != '\0') {
            entrance_exit.push_back(readUInt(ptr));
        }
        
        if (entrance_exit.size() != 2) {
            throw std::runtime_error("Esperado dois valores (entrada e saida).");
        }

        return entrance_exit;

    } catch(const std::runtime_error& e) {
        std::cerr << "Erro ao ler entrada/saida: " << e.what() << "\n";
        throw;
    }
}

// Le a ordem (numero de vertices) e tamanho (numero de arestas) de um grafo
std::vector<uint> getOrderSize(){
    char line[MAX_LINE];
    if (!std::cin.getline(line, MAX_LINE)) {
        throw std::runtime_error("Erro ao ler ordem/tamanho");
    }
    
    char* ptr = line;
    std::vector<uint> order_size;
    
    try {
        while(*ptr != '\0') {
            order_size.push_back(readUInt(ptr));
        }

        if(order_size.size() < 2){
            throw std::runtime_error("Entrada insuficiente para ordem e tamanho do grafo.");
        }

        return order_size;

    } catch(const std::runtime_error& e) {
        std::cerr << "Erro ao ler ordem/tamanho: " << e.what() << "\n";
        throw;
    }
}

// Implementacao do algoritmo de Dijkstra para encontrar o caminho minimo
// entre dois vertices em um grafo ponderado
class DijkstraSolver {
public:
    // Encontra o caminho minimo de start para target no grafo g
    // Retorna uma lista de vertices representando o caminho
    std::vector<uint> findPath(const WeightedGraph& g, uint start, uint target) {
        // Ajustar indices para 0-based
        start--; target--;
        
        uint n = g.vertexCount();
        std::vector<double> dist(n, std::numeric_limits<double>::max());
        std::vector<int> pred(n, -1);
        MinHeap pq;

        dist[start] = 0;
        pq.insert(0, start);

        // Processa vertices em ordem de distancia crescente
        while (!pq.empty()) {
            auto [d, u] = pq.extractMin();
            if (d == -1.0) break;
            if (d > dist[u]) continue;
            if (u == target) break;

            // Relaxa todas as arestas do vertice atual
            for (const auto& [v, w] : g.getNeighbors(u)) {
                if (v < n && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.insert(dist[v], v);
                }
            }
        }

        if (pred[target] == -1) return {};

        // Reconstroi o caminho a partir dos predecessores
        std::vector<uint> path;
        for (int v = target; v != -1; v = pred[v]) {
            path.push_back(v);
        }
        manualReverse(path);
        
        // Converte de volta para indices 1-based
        for (auto& vertex : path) {
            vertex++;
        }
        
        return path;
    }
};

// Funcao principal que orquestra todo o processamento
int main() {
    try {
        // 1. Ler grafo cerebral (conexoes entre blocos)
        std::vector<uint> brainOrderSize = getOrderSize();
        uint brainOrder = brainOrderSize[0];
        uint brainSize  = brainOrderSize[1];

        WeightedGraph brain = buildBrainGraph(brainOrder, brainSize);

        // 2. Ler entrada e saida do robo
        std::vector<uint> entrance_exit = getEntranceExit();
        uint start = entrance_exit[0];
        uint end = entrance_exit[1];

        // 3. Ler todos os blocos internos (grafos dos neuronios)
        // Um bloco para cada vertice do grafo cerebral
        std::vector<WeightedGraph> blocks;
        
        for (uint i = 0; i < brainOrder; ++i) {
            std::vector<uint> blockOrderSize = getOrderSize();
            uint blockOrder = blockOrderSize[0];
            uint blockSize = blockOrderSize[1];
            
            bool isSick;
            WeightedGraph block = buildBlockGraph(blockOrder, blockSize, isSick);
            blocks.push_back(block);
        }

        // 4. Encontrar caminho minimo do inicio ao fim no grafo cerebral
        DijkstraSolver solver;
        std::vector<uint> path = solver.findPath(brain, start, end);
        
        if (path.empty()){
            std::cerr << "Nao existe caminho entre os neuronios de entrada e saida.";
            return 1;
        }

        // 5. Calcular soma dos pesos das MSTs dos blocos doentes no caminho
        // O robo so calcula MSTs para blocos que contem neuronios doentes
        double totalMSTWeight = 0;
        for (uint vertex : path) {
            // vertex esta em 1-based, converter para 0-based
            uint blockIndex = vertex - 1;
            
            if (blockIndex < blocks.size()) {
                const WeightedGraph& block = blocks[blockIndex];
                if (block.getSick()) {
                    std::vector<Edge> mst = block.kruskalMST();
                    double sum = 0;
                    for (const auto& e : mst) {
                        sum += e.weight;
                    }
                    totalMSTWeight += sum;
                }
            }
        }

        // 6. Output: soma dos pesos das MSTs calculadas
        std::cout << totalMSTWeight << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Erro durante o processamento: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
