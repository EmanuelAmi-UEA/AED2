#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <string> 

using uint = unsigned int;

// -------------------- Estrutura de Posicao --------------------
// Representa uma posicao no tabuleiro
// Pode ser convertida para notacao de xadrez (ex: a1, b2)
// ou convertida de notacao de xadrez para coordenadas
struct Position {
    int row, col;
    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
    std::string toChess() const { return std::string(1, 'a' + col) + std::to_string(row + 1); }

    static Position fromChess(const std::string& s) {
        if(s.size() < 2) return {0, 0}; // conversao simples de seguranca
        int col = s[0] - 'a';
        int row = std::stoi(s.substr(1)) - 1;
        return {row, col};
    }

    // Converte indice linear para linha e coluna
    static Position fromIndex(int i, int size) { return {i / size, i % size}; }
};

// -------------------- Estrutura de Exercito --------------------
// Cada exercito tem uma cor, posicao inicial e inimigos declarados
// Possui contadores de movimentos, peso do caminho e estado de atividade
class Army {
public:
    std::string color;
    Position pos, originalPos;
    std::vector<std::string> enemies;
    int delay = 0, moves = 0, weight = 0;
    bool active = true, reached = false;

    Army(const std::string& c, Position p, const std::vector<std::string>& e) :
        color(c), pos(p), originalPos(p), enemies(e) {}

    // Verifica se uma cor eh inimiga
    bool isEnemy(const std::string& c) const {
        for(const auto& e : enemies) if(e == c) return true;
        return false;
    }
};

// -------------------- Estrutura de Heap Minimo --------------------
// Usada pelo algoritmo de Dijkstra para escolher proximo vertice
class MinHeap {
private:
    std::vector<std::pair<int, uint>> heap; // (distancia, vertice)

    int parent(int i) const { return (i-1)/2; }
    int left(int i) const { return 2*i+1; }
    int right(int i) const { return 2*i+2; }

    void up(int i) {
        while(i > 0 && heap[parent(i)].first > heap[i].first) {
            std::swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    void down(int i) {
        int smallest = i, l = left(i), r = right(i), n = heap.size();
        if(l < n && heap[l].first < heap[smallest].first) smallest = l;
        if(r < n && heap[r].first < heap[smallest].first) smallest = r;
        if(smallest != i) {
            std::swap(heap[i], heap[smallest]);
            down(smallest);
        }
    }

public:
    void insert(int p, uint e) {
        heap.push_back({p, e});
        up(heap.size()-1);
    }

    std::pair<int, uint> extractMin() {
        if(heap.empty()) return {-1, 0};
        auto min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if(!heap.empty()) down(0);
        return min;
    }

    bool empty() const { return heap.empty(); }
};

// -------------------- Estrutura de Grafo --------------------
// Representa o tabuleiro como grafo com pesos nas arestas
// Cada celula eh um vertice
// Movimentos possiveis sao os do cavalo do xadrez
class Graph {
private:
    int size;
    std::vector<std::list<std::pair<int, int>>> adj;

    int toIdx(Position p) const { return p.row * size + p.col; }
    bool valid(int r, int c) const { return r >= 0 && r < size && c >= 0 && c < size; }

public:
    Graph(int s) : size(s), adj(s*s) {}

    void addEdge(Position a, Position b, int w) {
        int u = toIdx(a), v = toIdx(b);
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    const std::list<std::pair<int, int>>& getNeighbors(uint i) const {
        return adj[i];
    }

    uint vertexCount() const { return size * size; }
    int getSize() const { return size; }

    // Funcao para calcular peso da aresta entre duas posicoes
    int weight(Position a, Position b) const {
        int alpha_u = a.col + 'a';
        int beta_u  = a.row + 1;
        int alpha_v = b.col + 'a';
        int beta_v  = b.row + 1;
        return (alpha_u * beta_u + alpha_v * beta_v) % 19;
    }

    // Gera todos os movimentos de cavalo no tabuleiro
    void genMoves() {
        int knightMoves[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
        for(int r = 0; r < size; r++) {
            for(int c = 0; c < size; c++) {
                Position from = {r, c};
                for(int i = 0; i < 8; i++) {
                    int nr = r + knightMoves[i][0], nc = c + knightMoves[i][1];
                    if(valid(nr, nc)) {
                        Position to = {nr, nc};
                        addEdge(from, to, weight(from, to));
                    }
                }
            }
        }
    }
};

// -------------------- Algoritmo de Dijkstra --------------------
// Calcula menor caminho entre duas posicoes no grafo
class DijkstraSolver {
public:
    std::vector<Position> findPath(const Graph& g, Position start, Position target) {
        uint n = g.vertexCount();
        int boardSize = g.getSize();
        std::vector<int> dist(n, std::numeric_limits<int>::max());
        std::vector<int> pred(n, -1);
        MinHeap pq;

        auto toIdx = [&](Position p) { return p.row * boardSize + p.col; };
        uint startIdx = toIdx(start);
        uint targetIdx = toIdx(target);

        dist[startIdx] = 0;
        pq.insert(0, startIdx);

        if(startIdx == targetIdx) {
            return {start};
        }

        while(!pq.empty()) {
            auto [d, u] = pq.extractMin();
            if(d == -1) break;
            if(d > dist[u]) continue;
            if(u == targetIdx) break;

            for(const auto& [v, w] : g.getNeighbors(u)) {
                if(dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.insert(dist[v], v);
                }
            }
        }

        if(pred[targetIdx] == -1) return {};

        std::vector<Position> path;
        for(int i = targetIdx; i != -1; i = pred[i]) {
            path.insert(path.begin(), Position::fromIndex(i, boardSize));
        }
        return path;
    }

    // Calcula peso total de um caminho
    int pathWeight(const std::vector<Position>& path, const Graph& g) const {
        if(path.size() < 2) return 0;
        int total = 0;
        for(uint i = 0; i < path.size()-1; i++) {
            total += g.weight(path[i], path[i+1]);
        }
        return total;
    }
};

// -------------------- Simulacao da Guerra --------------------
// Gerencia exercitos, castelo e tempestades
// Executa turnos de movimento aplicando as regras
class WarSimulation {
private:
    Graph graph;
    std::vector<Army> armies;
    Position castle;
    std::vector<Position> storms;
    DijkstraSolver dijkstra;

    Army* findArmyAt(Position p) {
        for(auto& a : armies) {
            if(a.pos == p && a.active) return &a;
        }
        return nullptr;
    }

    bool hasStormAt(Position p) const {
        for(const auto& s : storms) {
            if(s == p) return true;
        }
        return false;
    }

    void removeStorm(Position p) {
        for(uint i = 0; i < storms.size(); i++) {
            if(storms[i] == p) {
                storms.erase(storms.begin() + i);
                return;
            }
        }
    }

    int countAlliesAt(Position p) const {
        int count = 0;
        for(const auto& a : armies) {
            if(a.pos == p && a.active) count++;
        }
        return count;
    }

public:
    WarSimulation(int s) : graph(s) {
        graph.genMoves();
    }

    void addArmy(const std::string& c, Position p, const std::vector<std::string>& e) {
        armies.emplace_back(c, p, e);
    }

    void setCastle(Position p) { castle = p; }
    void addStorm(Position p) { storms.push_back(p); }

    // Executa simulacao de varios turnos ate achar vencedores
    void simulate() {
        // Calcula movimentos e pesos iniciais de cada exercito
        for(auto& a : armies) {
            auto path = dijkstra.findPath(graph, a.originalPos, castle);
            if(!path.empty()) {
                a.moves = path.size() - 1;
                a.weight = dijkstra.pathWeight(path, graph);
            }
        }

        std::vector<Army*> winners;
        int maxRounds = 100;

        // Executa turnos de movimento
        for(int round = 1; round <= maxRounds && winners.empty(); round++) {
            for(auto& army : armies) {
                if(!army.active || army.reached || army.delay > 0) {
                    if(army.delay > 0) army.delay--;
                    continue;
                }

                auto path = dijkstra.findPath(graph, army.pos, castle);
                if(path.size() < 2) continue;

                Position nextMove = path[1];
                Army* otherArmy = findArmyAt(nextMove);
                bool stormHere = hasStormAt(nextMove);

                if(otherArmy != nullptr) {
                    // Caso de inimigo bloqueando
                    if(army.isEnemy(otherArmy->color)) continue;
                    else army.pos = nextMove;
                } else if(stormHere) {
                    // Caso de tempestade
                    int allyCount = countAlliesAt(army.pos);
                    if(allyCount >= 2) {
                        army.pos = nextMove;
                        removeStorm(nextMove);
                    } else {
                        army.delay = 1;
                        removeStorm(nextMove);
                        continue;
                    }
                } else {
                    army.pos = nextMove;
                }

                if(army.pos == castle) {
                    army.reached = true;
                    army.active = false;
                    winners.push_back(&army);
                }
            }
        }

        // Ordena vencedores por cor
        for(uint i = 0; i < winners.size(); i++) {
            for(uint j = i+1; j < winners.size(); j++) {
                if(winners[i]->color > winners[j]->color) {
                    std::swap(winners[i], winners[j]);
                }
            }
        }

        // Imprime resultado final
        for(uint i = 0; i < winners.size(); i++) {
            std::cout << winners[i]->color << " " << winners[i]->moves << " " << winners[i]->weight;
            if(i < winners.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
};

// -------------------- Programa Principal --------------------
// Le entrada, configura simulacao e executa
int main() {
    int boardSize;
    std::cin >> boardSize;

    WarSimulation sim(boardSize);

    int numArmies;
    std::cin >> numArmies;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for(int i = 0; i < numArmies; i++) {
        std::string line;
        std::getline(std::cin, line);

        size_t first = line.find(' ');
        size_t second = line.find(' ', first + 1);

        if(first == std::string::npos || second == std::string::npos) continue;

        std::string color = line.substr(0, first);
        std::string posStr = line.substr(first + 1, second - first - 1);
        std::vector<std::string> enemies;

        size_t start = second + 1;
        while(start < line.size()) {
            size_t end = line.find(' ', start);
            if(end == std::string::npos) end = line.size();

            std::string enemy = line.substr(start, end - start);
            if(!enemy.empty()) enemies.push_back(enemy);

            if(end == line.size()) break;
            start = end + 1;
        }

        sim.addArmy(color, Position::fromChess(posStr), enemies);
    }

    std::string castlePos;
    std::cin >> castlePos;
    sim.setCastle(Position::fromChess(castlePos));

    int numStorms;
    std::cin >> numStorms;

    for(int i = 0; i < numStorms; i++) {
        std::string stormPos;
        std::cin >> stormPos;
        sim.addStorm(Position::fromChess(stormPos));
    }

    sim.simulate();
    return 0;
}
