#include <iostream>
#include <fstream>
#include "the7.h"

void print_network(std::vector<std::vector<std::pair<int, int>>> &network)
{
    int node_number = (int)network.size();
    if (node_number == 0)
    {
        std::cout << "There is no node in the network" << std::endl;
        return;
    }

    for (int idx = 0; idx < node_number; idx++)
    {
        std::cout << idx << ":\t{";
        for (const auto &edge : network[idx])
        {
            std::cout << " (" << edge.first << ", " << edge.second << ") ";
        }
        std::cout << "} " << std::endl;
    }
}

void read_from_file(std::vector<std::vector<std::pair<int, int>>> &network)
{
    int node_number, edge_number;
    char addr[] = "./the7_sampleio/inp05.txt"; // 01-10 are available
    std::ifstream infile(addr);
    if (!infile.is_open())
    {
        std::cout << "File \'" << addr
                  << "\' can not be opened. Make sure that this file exists." << std::endl;
        return;
    }
    infile >> node_number >> edge_number;
    network.resize(node_number);
    for (int idy = 0; idy < edge_number; idy++)
    {
        int source, dest, weight;
        infile >> source >> dest >> weight;
        network[source].push_back(std::make_pair(dest, weight));
    }
    infile.close();
}

int main()
{
    std::vector<std::vector<std::pair<int, int>>> network;
    std::vector<float> infection_scores;
    read_from_file(network);
    print_network(network);
    get_infection_scores(network, infection_scores);
    std::cout << "Infection scores: ";
    for (const auto &score : infection_scores)
        std::cout << score << " ";
    std::cout << std::endl
              << "------------------------------------------" << std::endl;
    return 0;
}