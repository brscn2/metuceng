// this file is for you for testing purposes, it won't be included in evaluation.

#include <iostream>
#include <fstream>
#include "the2.h"

void file_input(std::string*& input_array, int& size, bool& ascending){
    std::string file_name = "inp06.txt"; // inp01-inp10 are available.
    std::ifstream infile (file_name);
    if(!infile.is_open()){
        std::cout << "Input file cannot be opened" << std::endl;
        std::cout << "File name: " << file_name << std::endl;
        return;
    }
    infile >> ascending;
    infile >> size;
    input_array = new std::string[size];
    for(int j=0; j<size; j++){
        infile >> input_array[j];
    }
    return;
}

void test(){
    int number_of_iteration, size;
    bool ascending;
    std::string* input_array;
    file_input(input_array, size, ascending);
    std::cout << "Size: " << size << std::endl <<
                 "Ascending: " << ascending << std::endl <<
                 "Array elements: {";
    for(int idx=0; idx < size - 1; idx++) std::cout << input_array[idx] << ", ";
    std::cout << input_array[size-1] << "}" << std::endl;
    number_of_iteration = radix_string_sort(input_array, size, ascending);
    std::cout << "Number of iterations: " << number_of_iteration << std::endl <<
                 "Sorted array: {";
    for(int idx=0; idx<size-1; idx++) std::cout << input_array[idx] << ", ";
    std::cout << input_array[size-1] << "}" << std::endl;
    return;
}

int main(){
    test();
	return 0;
}
