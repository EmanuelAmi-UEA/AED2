#include <iostream>
#include <vector>
#include <list>
#include <limits>

using uint = unsigned int;

struct Position {
    int row, col;
    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
    std::string toChess() const { return std::string(1, 'a' + col) + std::to_string(row + 1); }
    static Position fromChess(const std::string& s) {
        return {std::stoi(s.substr(1)) - 1, s[0] - 'a'};
    }
    static Position fromIndex(int i, int size) { return {i / size, i % size}; }
};

class Army {
public:
    std::string color;
    Position pos, originalPos;
    std::vector<std::string> enemies;
    int delay = 0, moves = 0, weight = 0;
    bool active = true, reached = false;

    Army(std::string c, Position p, std::vector<std::string> e) :
        color(c), pos(p), originalPos(p), enemies(e) {}

    bool isEnemy(const std::string& c) const {
        for(auto& e : enemies) if(e == c) return true;
        return false;
    }
};

class MinHeap {
private:
    std::vector<std::pair<int, uint>> heap;

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

    int weight(Position a, Position b) const {
        std::string u = a.toChess(), v = b.toChess();
        int alpha_u = u[0], beta_u = std::stoi(u.substr(1));
        int alpha_v = v[0], beta_v = std::stoi(v.substr(1));
        return (alpha_u * beta_u + alpha_v * beta_v) % 19;
    }

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

class DijkstraSolver {
public:
    std::vector<Position> findPath(Graph& g, Position start, Position target) {
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

        while(!pq.empty()) {
            auto [d, u] = pq.extractMin();
            if(d == -1) break;
            if(d > dist[u]) continue;
            if(u == targetIdx) break;

            for(auto& [v, w] : g.getNeighbors(u)) {
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

    int pathWeight(const std::vector<Position>& path, Graph& g) {
        if(path.size() < 2) return 0;
        int total = 0;
        for(uint i = 0; i < path.size()-1; i++) {
            total += g.weight(path[i], path[i+1]);
        }
        return total;
    }
};

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

    bool hasStormAt(Position p) {
        for(auto& s : storms) {
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

    int countAlliesAt(Position p) {
        int count = 0;
        for(auto& a : armies) {
            if(a.pos == p && a.active) count++;
        }
        return count;
    }

public:
    WarSimulation(int s) : graph(s) {
        graph.genMoves();
    }

    void addArmy(std::string c, Position p, std::vector<std::string> e) {
        armies.push_back(Army(c, p, e));
    }

    void setCastle(Position p) { castle = p; }
    void addStorm(Position p) { storms.push_back(p); }

    void simulate() {
        for(auto& a : armies) {
            auto path = dijkstra.findPath(graph, a.originalPos, castle);
            if(!path.empty()) {
                a.moves = path.size() - 1;
                a.weight = dijkstra.pathWeight(path, graph);
            }
        }

        std::vector<Army*> winners;
        int maxRounds = 100;

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
                bool hasStorm = hasStormAt(nextMove);

                if(otherArmy != nullptr) {
                    if(army.isEnemy(otherArmy->color)) continue;
                    else army.pos = nextMove;
                } else if(hasStorm) {
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

        for(uint i = 0; i < winners.size(); i++) {
            for(uint j = i+1; j < winners.size(); j++) {
                if(winners[i]->color > winners[j]->color) {
                    std::swap(winners[i], winners[j]);
                }
            }
        }

        for(uint i = 0; i < winners.size(); i++) {
            std::cout << winners[i]->color << " " << winners[i]->moves << " " << winners[i]->weight;
            if(i < winners.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    int boardSize;
    std::cin >> boardSize;

    WarSimulation sim(boardSize);

    int numArmies;
    std::cin >> numArmies;
    std::cin.ignore();

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