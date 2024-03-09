#include <iostream>
#include <fstream>
#include "the5.h"

void test()
{
    char inp_file[] = "inp12.txt"; // 01 to 10 is available
    int size;
    std::ifstream infile(inp_file);
    if (!infile.is_open())
    {
        std::cout << "Input file cannot be opened" << std::endl;
        std::cout << "File name: " << inp_file << std::endl;
        return;
    }
    infile >> size;
    std::vector<std::vector<int>> input_array(size, std::vector<int>(size, 0));
    std::vector<int> compilation_order;
    std::vector<std::vector<int>> cyclic_dependencies;
    bool compilable = false;

    for (int idx = 0; idx < size; idx++)
        for (int idy = 0; idy < size; idy++)
            infile >> input_array[idx][idy];

    std::cout << "Number of files: " << size << std::endl
              << "Dependency matrix: [" << std::endl;
    for (int idx = 0; idx < size; idx++)
    {
        std::cout << "                        [";
        for (int idy = 0; idy < size - 1; idy++)
        {
            std::cout << input_array[idx][idy] << ", ";
        }
        std::cout << input_array[idx][size - 1] << "]" << std::endl;
    }
    std::cout << "                   ]" << std::endl;

    run(input_array, compilable, compilation_order, cyclic_dependencies);
    if (compilable)
    {
        std::cout << "Compilation is possible." << std::endl;
        std::cout << "Sorted order: {";
        for (int idx = 0; idx < size - 1; idx++)
            std::cout << compilation_order[idx] << ", ";
        std::cout << compilation_order[size - 1] << "}" << std::endl;
    }
    else
    {
        std::cout << "Compilation is not possible. Cyclic dependencies: " << std::endl;
        for (int idx = 0; idx < (int)cyclic_dependencies.size(); idx++)
        {
            std::cout << (idx + 1) << ": ";
            for (int idy = 0; idy < (int)cyclic_dependencies[idx].size() - 1; idy++)
            {
                std::cout << cyclic_dependencies[idx][idy] << " ";
            }
            std::cout << cyclic_dependencies[idx].back() << std::endl;
        }
    }
    std::cout << "--------------------------------------------" << std::endl;
    infile.close();
}

int main()
{
    test();
    return 0;
}