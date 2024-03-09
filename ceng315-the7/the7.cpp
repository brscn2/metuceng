#include "the7.h"

// do not add extra libraries here

void get_infection_scores(const std::vector<std::vector<std::pair<int, int>>> &network,
                          std::vector<float> &infection_scores)
{
    int MAX_DIST = INT_MIN;

    std::vector<std::vector<int>> distanceMatrix = std::vector<std::vector<int>>(network.size(), std::vector<int>(network.size(), INT_MAX));

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
                if ((ij_weight > ik_weight + kj_weight) && kj_weight != INT_MAX && ik_weight != INT_MAX)
                {
                    distanceMatrix[i][j] = ik_weight + kj_weight;
                    if (ik_weight + kj_weight > MAX_DIST)
                        MAX_DIST = ik_weight + kj_weight;
                }
            }
        }
    }

    // std::cout << "MaxDist is: " << MAX_DIST << "\n";
    // for (int i = 0; i < distanceMatrix->size(); i++)
    // {
    //     for (int j = 0; j < distanceMatrix->size(); j++)
    //     {
    //         std::cout << distanceMatrix->at(i)[j] << "\t";
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
                int sp = distanceMatrix[i][j] == INT_MAX ? MAX_DIST + 1 : distanceMatrix[i][j];
                ais += sp;
            }
        }

        ais /= (network.size() - 1);
        infection_scores.push_back(1.0 / ais);
    }
}