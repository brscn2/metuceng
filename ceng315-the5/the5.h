#ifndef THE5_THE5_H
#define THE5_THE5_H
#include <vector>
#include <memory>
#include <utility>
//this file will be overwritten during execution
//changes made in this file will not affectyour grade or execution
//it is shared with you so that you can work on a local environment if needed

void run(const std::vector<std::vector<int>>& dependencyMatrix, 
         bool& isCompilable,
         std::vector<int>& compileOrder, 
         std::vector<std::vector<int>>& cyclicDependencies);
#endif //THE5_THE5_H