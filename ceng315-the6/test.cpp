#include <iostream>
#include <fstream>
#include "the6.h"


void print_adj_list(std::vector< std::vector< std::pair<int,int> > >& adj_list) {
    int N = adj_list.size();
    if (N == 0) {
        std::cout << "list is empty!" << std::endl;
        return;
    }
    
    for (int v=0;v<N;v++) {
        std::cout << v << "\t{";
        for (auto p : adj_list[v]) {
            std::cout << " (" << p.first << "," << p.second << ")";
        }
        std::cout << " }\n";
    }
    return;
}

// you can use this if you want to print the adj list as a matrix
void print_adj_list_as_matrix(std::vector< std::vector< std::pair<int,int> > >& adj_list) {
    int N = adj_list.size();
    if (N == 0) {
        std::cout << "list is empty!" << std::endl;
        return;
    }
    int** matrix;
    matrix = new int*[N];
    for(int temp=0; temp < N; temp++) matrix[temp] = new int[N];
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            matrix[i][j] = -1; // no edge
        }
    }
    
    for (int i=0; i<N; i++){
        for (std::pair<int,int> x: adj_list[i]) {
            matrix[i][x.first] = x.second;
        }
    }
    
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            if (matrix[i][j] == -1) std::cout << "- ";
            else std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    for(int i=0; i<N; i++) delete[] matrix[i];
    delete[] matrix;
    return;
}

void read_from_file(std::vector< std::vector<std::pair<int,int> > >& bond_energies, std::vector< std::vector<std::pair<int,int> > >& molecule_structure){
    char addr[]= "inp01.txt"; // 01-05 are available
    std::ifstream infile (addr);
    if (!infile.is_open()){
        std::cout << "File \'"<< addr
                  << "\' can not be opened. Make sure that this file exists." << std::endl;
        return;
    }
    
    int V_p1, E_p1, V_p2, E_p2;
    
    infile >> V_p1 >> E_p1;
    bond_energies.resize(V_p1);
    for (int l=0; l<E_p1; l++) {
        int v1, v2, w;
        infile >> v1 >> v2 >> w;
        bond_energies[v1].push_back(std::make_pair(v2,w));
        bond_energies[v2].push_back(std::make_pair(v1,w));
    }
    
    infile >> V_p2 >> E_p2;
    molecule_structure.resize(V_p2);
    for (int l=0; l<E_p2; l++) {
        int v1, v2, w;
        infile >> v1 >> v2 >> w;
        molecule_structure[v1].push_back(std::make_pair(v2,w));
        molecule_structure[v2].push_back(std::make_pair(v1,w));
    }

    
    infile.close();
}

int main(){
    std::vector< std::vector< std::pair<int,int> > > bond_energies;
    std::vector< std::vector< std::pair<int,int> > > molecule_structure;
    std::vector< std::vector< std::pair<int,int> > > lowest_energy_structure;
    std::vector<int> chain;
    int longest_chain_size, lowest_total_energy;

    read_from_file(bond_energies, molecule_structure);
    lowest_energy_structure.resize(bond_energies.size());
    
    lowest_total_energy = find_structure(bond_energies, lowest_energy_structure);
    std::cout << "PART 1: " << std::endl << "Bond energy graph:" << std::endl;
    print_adj_list(bond_energies);
    std::cout << "Graph of the lowest energy structure found:" << std::endl;
    print_adj_list(lowest_energy_structure);
    std::cout << "Total energy of the lowest energy structure: " << lowest_total_energy << std::endl;
    
    longest_chain_size = find_longest_chain(molecule_structure,chain);
    std::cout << "PART 2: " << std::endl << "Molecule structure graph:" << std::endl;
    print_adj_list(molecule_structure);
    std::cout << "Atom count in longest chain: " << longest_chain_size << std::endl;
    std::cout << "Longest chain:" << std::endl;
    std::cout << "(" << chain[0] << ")";
    for (int i=1; i<chain.size(); i++) {
        std::cout << " - (" << chain[i] << ")";
    }

    return 0;
}