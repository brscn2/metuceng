#include "the6.h"
// do not add extra libraries here

typedef struct
{
    int vertex;
    int key;
    bool isInQueue;
    int parent;
} Vertex;

struct Compare
{
    bool operator()(const Vertex *a, const Vertex *b)
    {
        return a->key > b->key;
    }
};

int mst_prim(std::vector<std::vector<std::pair<int, int>>> &bond_energies, std::vector<Vertex *> *vertices, int root, std::vector<std::vector<std::pair<int, int>>> &lowest_energy_structure)
{
    int lowest_total_energy = 0;

    std::priority_queue<Vertex *, std::vector<Vertex *>, Compare> *vertexQueue = new std::priority_queue<Vertex *, std::vector<Vertex *>, Compare>(vertices->begin(), vertices->end());
    // for (int i = 0; i < vertices.size(); i++)
    // {
    //     vertexQueue->push(vertices[i]);
    // }

    while (!vertexQueue->empty())
    {
        Vertex *u = vertexQueue->top();
        // std::cout << "Root node: " << u->vertex << "\n";
        u->isInQueue = false;
        vertexQueue->pop();

        for (int i = 0; i < bond_energies[u->vertex].size(); i++)
        {
            Vertex *v = vertices->at(bond_energies[u->vertex][i].first);
            int weight_uv = bond_energies[u->vertex][i].second;

            if (v->isInQueue && weight_uv < v->key)
            {
                // std::cout << "Node: " << u->vertex << " Adj Node: " << v->vertex << " Weight v | uv: " << v->key << " " << weight_uv;
                v->parent = u->vertex;
                v->key = weight_uv;
                // std::cout << " New Weight: " << v->key << "\n";
            }
        }

        if (!vertexQueue->empty())
        {
            /* Pop an element so heap property will be satisfied. Push it again. */
            // Vertex *tempVertex = vertexQueue->top();
            // vertexQueue->pop();
            // vertexQueue->push(tempVertex);

            // std::cout << "Temp Vertex: " << tempVertex->vertex << "\n";

            std::make_heap(const_cast<Vertex **>(&vertexQueue->top()), const_cast<Vertex **>(&vertexQueue->top()) + vertexQueue->size(), Compare());

            std::pair<int, int> *newStructure = new std::pair<int, int>;
            Vertex *newAtom = vertexQueue->top();
            // std::cout << "New Tree Node (vertex | weight): " << newAtom->vertex << " " << newAtom->key << "\n";

            newStructure->first = newAtom->vertex;
            newStructure->second = newAtom->key;
            lowest_energy_structure[newAtom->parent].push_back(*newStructure);

            std::pair<int, int> *newStructureParent = new std::pair<int, int>;
            newStructureParent->first = newAtom->parent;
            newStructureParent->second = newStructure->second;

            lowest_energy_structure[newAtom->vertex].push_back(*newStructureParent);

            lowest_total_energy += newAtom->key;
        }
    }
    return lowest_total_energy;
}

int find_structure(std::vector<std::vector<std::pair<int, int>>> &bond_energies, std::vector<std::vector<std::pair<int, int>>> &lowest_energy_structure)
{
    int lowest_total_energy = 0;

    std::vector<Vertex *> *vertices = new std::vector<Vertex *>();
    for (int i = 0; i < bond_energies.size(); i++)
    {
        Vertex *vertex = new Vertex;
        (*vertex).vertex = i;
        (*vertex).key = INT_MAX;
        (*vertex).isInQueue = true;
        (*vertex).parent = NULL;
        (*vertices).push_back(vertex);
    }
    vertices->at(0)->key = 0; // set root key to 0

    lowest_total_energy = mst_prim(bond_energies, vertices, 0, lowest_energy_structure);

    return lowest_total_energy;
}

/* PART 2 */

enum colors
{
    WHITE,
    GRAY,
    BLACK
};

typedef struct
{
    int vertex;
    colors color;
    int distance;
    int parent;
} Vertex_2;

Vertex_2 *bfs(std::vector<std::vector<std::pair<int, int>>> &molecule_structure, std::vector<Vertex_2 *> &vertices, Vertex_2 *rootVertex, std::vector<int> &chain, bool isSecond)
{
    Vertex_2 *farthestVector;
    if (isSecond)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            Vertex_2 *vertex = vertices[i];
            vertex->color = WHITE;
            vertex->distance = INT_MAX;
            vertex->parent = NULL;
        }
    }
    // Vertex_2 *rootVertex = vertices[root];
    rootVertex->color = GRAY;
    rootVertex->distance = 0;

    std::queue<Vertex_2 *, std::deque<Vertex_2 *>> *queue = new std::queue<Vertex_2 *, std::deque<Vertex_2 *>>();
    queue->push(rootVertex);

    while (!queue->empty())
    {
        Vertex_2 *u = queue->front();
        queue->pop();
        for (int i = 0; i < molecule_structure[u->vertex].size(); i++)
        {
            Vertex_2 *v = vertices.at(molecule_structure[u->vertex][i].first);

            if (v->color == WHITE)
            {
                v->color = GRAY;
                v->distance = u->distance + 1;
                v->parent = u->vertex;
                queue->push(v);
            }
        }

        // if (isSecond && u->parent != last_inserted)
        // {
        //     chain.push_back(u->vertex);
        //     last_inserted = u->vertex;
        // }
        u->color = BLACK;
        if (queue->empty())
            farthestVector = u;
    }

    return farthestVector;
}

int find_longest_chain(std::vector<std::vector<std::pair<int, int>>> &molecule_structure, std::vector<int> &chain)
{
    int longest_chain_size = 0;

    std::vector<Vertex_2 *> *vertices = new std::vector<Vertex_2 *>();
    for (int i = 0; i < molecule_structure.size(); i++)
    {
        Vertex_2 *vertex = new Vertex_2;
        vertex->vertex = i;
        vertex->color = WHITE;
        vertex->distance = INT_MAX;
        vertex->parent = -1;
        vertices->push_back(vertex);
    }

    Vertex_2 *farthestVertex = bfs(molecule_structure, *vertices, vertices->at(0), chain, false);
    // std::cout << "Farthest Vertex: " << farthestVertex->vertex << " Parent: " << farthestVertex->parent << "\n";
    Vertex_2 *rootVertex = bfs(molecule_structure, *vertices, farthestVertex, chain, true);
    // std::cout << "Root Vertex: " << rootVertex->vertex << " Parent: " << rootVertex->parent << "\n";

    // for (int i = 0; i < vertices->size(); i++)
    // {
    //     std::cout << "Vertex: " << vertices->at(i)->vertex << " Parent: " << vertices->at(i)->parent << "\n";
    // }

    while (rootVertex->vertex != farthestVertex->vertex)
    {
        // std::cout << "Vertex: " << rootVertex->vertex << " Parent: " << rootVertex->parent << "\n";
        chain.insert(chain.begin(), rootVertex->vertex);
        rootVertex = vertices->at(rootVertex->parent);
    }

    chain.insert(chain.begin(), farthestVertex->vertex);

    longest_chain_size = chain.size();
    return longest_chain_size;
}
