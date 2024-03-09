#include <iostream>
#include <fstream>
#include "the4.h"


void read_from_file(int& X, int& Y, bool**& possible_plots){
    int number_of_plots;
    char addr[]= "inp01.txt"; // 01-10 are available
    std::ifstream infile (addr);
    if (!infile.is_open()){
        std::cout << "File \'"<< addr
                  << "\' can not be opened. Make sure that this file exists." << std::endl;
        return;
    }
    infile >> X;
    infile >> Y;
    infile >> number_of_plots;
    possible_plots = new bool*[X+1];
    for(int temp=0; temp < X+1; temp++) possible_plots[temp] = new bool[Y+1];
    for(int idx=0; idx < X+1; idx++) for(int idy=0; idy < Y+1; idy++) possible_plots[idx][idy] = false;
    for(int temp=0; temp < number_of_plots; temp++){
        std::pair<int, int> plot;
        infile >> plot.first >> plot.second;
        possible_plots[plot.first][plot.second] = true;
    }
    infile.close();
}

int main(){
    int X, Y;
    bool** input_array;
    int minimum_unused_land, plot_number=1;

    read_from_file(X, Y, input_array);

    std::cout << "X: " << X << ", Y: " << Y << std::endl;
    for(int idx=0; idx < X+1; idx++)
        for(int idy=0; idy < Y+1; idy++)
            if(input_array[idx][idy])
                std::cout << "Plot " << plot_number++ << ": (" << idx << ", " << idy << ")" << std::endl;

    minimum_unused_land = divide_land(X, Y, input_array);

    std::cout << "Unused land: " << minimum_unused_land << " m^2" << std::endl;

    for(int idx=0; idx<X+1; idx++) delete[] input_array[idx];
    delete[] input_array;
    return 0;
}