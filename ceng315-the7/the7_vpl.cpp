#include "the7.h"
#include <iostream>
// do not add extra libraries here

// int get_sp(int i, int j, std::vector<std::vector<int>> &distanceMatrix, int MAX_DIST)
// {
//     if (distanceMatrix[i][j] == 51)
//     {
//         // std::cout << "No path between (" << i << "," << j << ").\n";
//         return MAX_DIST + 1;
//     }
//     else
//     {
//         // std::cout << "Shortest distance between (" << i << "," << j << ") is: " << distanceMatrix[i][j] << "\n";
//         return distanceMatrix[i][j];
//     }
// }

// float get_ais(int i, int node_count, std::vector<std::vector<int>> &distanceMatrix, int MAX_DIST)
// {
//     float sum = 0;
//     for (int j = 0; j < node_count; j++)
//     {
//         if (j != i)
//             sum += get_sp(i, j, distanceMatrix, MAX_DIST);
//     }

//     return sum / (node_count - 1);
// }

// float get_is(int i, int node_count, std::vector<std::vector<int>> &distanceMatrix, int MAX_DIST)
// {
//     float ais = 0;
//     for (int j = 0; j < node_count; j++)
//     {
//         if (j != i)
//         {
//             int sp = distanceMatrix[i][j] == 51 ? MAX_DIST + 1 : distanceMatrix[i][j];
//             ais += sp;
//         }
//     }

//     ais /= (node_count - 1);

//     return 1.0 / ais;
// }

void get_infection_scores(const std::vector<std::vector<std::pair<int, int>>> &network,
                          std::vector<float> &infection_scores)
{
    int MAX_DIST = INT_MIN;

    std::vector<std::vector<int>> distanceMatrix = std::vector<std::vector<int>>(network.size(), std::vector<int>(network.size(), 51));

    for (int i = 0; i < network.size(); i++)
        distanceMatrix[i][i] = 0;

    for (int i = 0; i < network.size(); i++)
    {
        for (int j = 0; j < network[i].size(); j++)
        {
            int index = network[i][j].first;
            int weight = network[i][j].second;
            distanceMatrix[i][index] = weight;
            if (weight > MAX_DIST)
                MAX_DIST = weight;
        }
    }

    for (int k = 0; k < network.size(); k++)
    {
        for (int i = 0; i < network.size(); i++)
        {
            for (int j = 0; j < network.size(); j++)
            {
                int ij_weight = distanceMatrix[i][j], ik_weight = distanceMatrix[i][k], kj_weight = distanceMatrix[k][j];
                if (kj_weight != 51 && ik_weight != 51 && (ij_weight > ik_weight + kj_weight))
                {
                    distanceMatrix[i][j] = ik_weight + kj_weight;
                    if (ik_weight + kj_weight > MAX_DIST)
                        MAX_DIST = ik_weight + kj_weight;
                }
            }
        }
    }

    // std::cout << "MaxDist is: " << MAX_DIST << "\n";
    // for (int i = 0; i < distanceMatrix.size(); i++)
    // {
    //     for (int j = 0; j < distanceMatrix.size(); j++)
    //     {
    //         std::cout << distanceMatrix[i][j] << "\t";
    //     }

    //     std::cout << "\n";
    // }

    for (int i = 0; i < network.size(); i++)
    {
        float ais = 0;
        for (int j = 0; j < network.size(); j++)
        {
            if (j != i)
            {
                int sp = distanceMatrix[i][j] == 51 ? MAX_DIST + 1 : distanceMatrix[i][j];
                ais += sp;
            }
        }

        ais /= (network.size() - 1);
        infection_scores.push_back(1.0 / ais);
    }
}