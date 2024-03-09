#include "the5.h"
#include <iostream>

// do not add extra libraries here

enum colors
{
    WHITE,
    GRAY,
    BLACK
};

int currentTime = 0;

typedef struct
{
    int vertex;
    colors color; /* white: 0, gray: 1 or black: 2 */
    int parent;
    int discovery;
    int finish;
} Vertex;

void sort_vertices_on_finish_time(std::vector<Vertex *> &vertices)
{
    int key;
    for (int i = 0; i < vertices.size(); i++)
    {
        key = vertices[i]->finish;
        Vertex *key_vertex = vertices[i];
        int j = i - 1;

        while (j >= 0 && vertices[j]->finish > key)
        {
            vertices[j + 1] = vertices[j];
            j = j - 1;
        }
        vertices[j + 1] = key_vertex;
    }
}

int find_vertex(std::vector<Vertex *> &vertices, int vertex)
{
    for (int i = 0; i < vertices.size(); i++)
    {
        if (vertices.at(i)->vertex == vertex)
        {
            return i;
        }
    }
}

void dfs_visit(const std::vector<std::vector<int>> &dependencyMatrix, bool isTranspose, std::vector<Vertex *> *vertices, Vertex *u, bool &isCompilable, std::vector<int> &compileOrder, std::vector<int> *cyclicDependency, std::vector<std::vector<int>> &cyclicDependencies)
{
    currentTime = currentTime + 1;
    u->discovery = currentTime;
    u->color = GRAY;
    // std::cout << "Visiting: " << u->vertex << "\n";

    for (int j = dependencyMatrix.at(0).size() - 1; j >= 0; j--)
    {
        if (dependencyMatrix.at(u->vertex).at(j) == 1)
        {
            int index = isTranspose ? find_vertex(*vertices, j) : j;
            // std::cout << "J is: " << j << " Index is: " << index << " Color: " << vertices->at(index)->color << "\n";
            if (vertices->at(index)->color == GRAY)
            {
                // std::cout << "Gray: " << j << "\n";
                isCompilable = false;
            }
            else if (vertices->at(index)->color == WHITE)
            {
                // std::cout << "White: " << j << "\n";
                vertices->at(index)->parent = u->vertex;
                dfs_visit(dependencyMatrix, isTranspose, vertices, vertices->at(index), isCompilable, compileOrder, cyclicDependency, cyclicDependencies);
            }
        }
    }
    u->color = BLACK;
    currentTime = currentTime + 1;
    if (isCompilable)
        compileOrder.insert(compileOrder.begin(), u->vertex);
    if (isTranspose)
        cyclicDependency->push_back(u->vertex);
    u->finish = currentTime;
}

void dfs(const std::vector<std::vector<int>> &dependencyMatrix, bool isTranspose, std::vector<Vertex *> *vertices, bool &isCompilable, std::vector<int> &compileOrder, std::vector<std::vector<int>> &cyclicDependencies)
{
    for (int i = 0; i < vertices->size(); i++)
    {
        vertices->at(i)->color = WHITE;
        vertices->at(i)->parent = -1;
    }

    currentTime = 0;

    if (!isTranspose)
        for (int i = vertices->size() - 1; i >= 0; i--)
        {
            if (vertices->at(i)->color == WHITE)
            {
                dfs_visit(dependencyMatrix, isTranspose, vertices, vertices->at(i), isCompilable, compileOrder, nullptr, cyclicDependencies);
            }
        }
    else
    {
        sort_vertices_on_finish_time(*vertices);
        // for (int i = 0; i < vertices->size(); i++)
        // {
        //     std::cout << "Finishing time: " << vertices->at(i)->finish << " Vertex: " << vertices->at(i)->vertex << "\n";
        // }

        for (int i = vertices->size() - 1; i >= 0; i--)
        {
            if (vertices->at(i)->color == WHITE)
            {
                // std::cout << "Source is: " << vertices->at(i)->vertex << "\n";
                std::vector<int> *cyclicDependency = new std::vector<int>;
                dfs_visit(dependencyMatrix, isTranspose, vertices, vertices->at(i), isCompilable, compileOrder, cyclicDependency, cyclicDependencies);
                if (cyclicDependency->size() > 1)
                    cyclicDependencies.push_back(*cyclicDependency);
            }
        }
    }
}

std::vector<std::vector<int>> *transpose_graph(const std::vector<std::vector<int>> &dependencyMatrix)
{
    std::vector<std::vector<int>> *transposeDependencyMatrix = new std::vector<std::vector<int>>(dependencyMatrix.size());
    for (int i = 0; i < transposeDependencyMatrix->size(); i++)
    {
        std::vector<int> *transposeRow = new std::vector<int>(dependencyMatrix.size());
        for (int j = 0; j < transposeDependencyMatrix->size(); j++)
        {
            transposeRow->at(j) = 0;
        }
        transposeDependencyMatrix->at(i) = *transposeRow;
    }

    for (int i = 0; i < dependencyMatrix.size(); i++)
    {

        for (int j = 0; j < dependencyMatrix.at(i).size(); j++)
        {
            transposeDependencyMatrix->at(j).at(i) = dependencyMatrix[i][j];
        }
    }

    return transposeDependencyMatrix;
}
void run(const std::vector<std::vector<int>> &dependencyMatrix,
         bool &isCompilable,
         std::vector<int> &compileOrder,
         std::vector<std::vector<int>> &cyclicDependencies)
{
    std::vector<Vertex *> *vertices = new std::vector<Vertex *>();
    for (int i = 0; i < dependencyMatrix.size(); i++)
    {
        Vertex *v = new Vertex;
        v->vertex = i;
        vertices->push_back(v);
    }

    isCompilable = true;
    dfs(dependencyMatrix, false, vertices, isCompilable, compileOrder, cyclicDependencies);

    if (!isCompilable)
    {
        std::vector<std::vector<int>> *transposeDependencyMatrix;
        transposeDependencyMatrix = transpose_graph(dependencyMatrix);

        // for (int i = 0; i < transposeDependencyMatrix->size(); i++)
        // {
        //     for (int j = 0; j < transposeDependencyMatrix->size(); j++)
        //     {
        //         std::cout << transposeDependencyMatrix->at(i).at(j) << " ";
        //     }
        //     std::cout << "\n";
        // }

        dfs(*transposeDependencyMatrix, true, vertices, isCompilable, compileOrder, cyclicDependencies);
    }
}