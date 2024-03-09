#ifndef THE6_THE6_H
#define THE6_THE6_H
#include <vector>
#include <utility>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>

//updating this file will not change the execution in the VPL

int find_structure(std::vector< std::vector<std::pair<int,int> > > &bond_energies, std::vector< std::vector<std::pair<int,int> > > &lowest_energy_structure);

int find_longest_chain(std::vector< std::vector<std::pair<int,int> > > &molecule_structure, std::vector<int>& chain);


#endif //THE6_THE6_H