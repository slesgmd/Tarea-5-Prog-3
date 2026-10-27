#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>

class Node {
    int id;
    std::vector<int> next;
public:
    Node() : id(0) {}
    Node(int id) : id(id) {}

    int getId() const { return id; }
    void addNext(int other) { next.push_back(other); }
    std::vector<int> getNext() const { return next; }
};

class Graph {
    // el mapa es privado, nadie de afuera lo toca
    std::map<int, Node> nodes;
public:
    void addEdge(int from, int to) {
        if (nodes.find(from) == nodes.end()) nodes[from] = Node(from);
        if (nodes.find(to) == nodes.end()) nodes[to] = Node(to);
        nodes[from].addNext(to);
    }

    // devuelve una copia de los vecinos, no la estructura
    std::vector<int> neighbors(int id) const {
        auto it = nodes.find(id);
        if (it == nodes.end()) return {};
        return it->second.getNext();
    }

    bool hasNode(int id) const {
        return nodes.find(id) != nodes.end();
    }
};

// interfaz del iterador
class GraphIterator {
public:
    virtual bool hasNext() = 0;
    virtual int next() = 0;
    virtual ~GraphIterator() {}
};

class BFSIterator : public GraphIterator {
    const Graph& g;
    std::queue<int> q;
    std::set<int> visited;
public:
    BFSIterator(const Graph& g, int start) : g(g) {
        if (g.hasNode(start)) {
            q.push(start);
            visited.insert(start);
        }
    }

    bool hasNext() override { return !q.empty(); }

    int next() override {
        int cur = q.front();
        q.pop();
        for (int v : g.neighbors(cur)) {
            // con visited no se queda en un ciclo infinito
            if (visited.count(v) == 0) {
                visited.insert(v);
                q.push(v);
            }
        }
        return cur;
    }
};

class DFSIterator : public GraphIterator {
    const Graph& g;
    std::stack<int> st;
    std::set<int> visited;

    // saca del tope los que ya se visitaron
    void limpiar() {
        while (!st.empty() && visited.count(st.top())) st.pop();
    }
public:
    DFSIterator(const Graph& g, int start) : g(g) {
        if (g.hasNode(start)) st.push(start);
    }

    bool hasNext() override {
        limpiar();
        return !st.empty();
    }

    int next() override {
        limpiar();
        int cur = st.top();
        st.pop();
        visited.insert(cur);
        auto vec = g.neighbors(cur);
        // al reves para que salga primero el primer vecino
        for (int i = vec.size() - 1; i >= 0; --i) {
            if (visited.count(vec[i]) == 0) st.push(vec[i]);
        }
        return cur;
    }
};

int main() {
    Graph g;
    g.addEdge(1,2);
    g.addEdge(1,3);
    g.addEdge(2,4);

    std::unique_ptr<GraphIterator> it =
        std::make_unique<BFSIterator>(g, 1);

    std::cout << "BFS: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    // se cambia el iterador en tiempo de ejecucion
    it = std::make_unique<DFSIterator>(g, 1);

    std::cout << "DFS: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    // prueba con ciclo para ver que no se cuelga
    g.addEdge(4,1);
    g.addEdge(3,1);

    it = std::make_unique<BFSIterator>(g, 1);
    std::cout << "BFS con ciclo: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    it = std::make_unique<DFSIterator>(g, 1);
    std::cout << "DFS con ciclo: ";
    while(it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;
}
