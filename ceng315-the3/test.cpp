// this file is for you for testing purposes, it will not be included in evaluation.

#include <iostream>
#include <fstream>
#include "the3.h"

void file_input(std::string& sequence_A, std::string& sequence_B, int& gap, int& mismatch, int& match){
    std::string file_name = "inp05.txt"; // inp01-inp10 are available.
    std::ifstream infile (file_name);
    if(!infile.is_open()){
        std::cout << "Input file cannot be opened" << std::endl;
        std::cout << "File name: " << file_name << std::endl;
        return;
    }
    infile >> sequence_A;
    infile >> sequence_B;
    infile >> gap;
    infile >> mismatch;
    infile >> match;
    return;
}

void test(){
    std::string sequence_A;
    std::string sequence_B;
    int gap, mismatch, match, highest_alignment_score_p1, highest_alignment_score_p2;
    std::string  possible_alignment_p1;
    std::string  possible_alignment_p2;
    int call_count = 0;
    
    file_input(sequence_A,sequence_B,gap,mismatch,match);
    std::cout << "Sequence A: " << sequence_A << std::endl <<
                 "Sequence B: " << sequence_B << std::endl <<
                 "gap: " << gap << std::endl <<
                 "mismatch: " << mismatch << std::endl <<
                 "match: " << match << std::endl;
    std::cout << "PART 1:" << std::endl;
    highest_alignment_score_p1 = recursive_alignment(sequence_A,sequence_B,gap,mismatch,match,possible_alignment_p1,call_count);
    std::cout << "Highest match score: " << highest_alignment_score_p1 << std::endl <<
                 "Possible alignment: " << std::endl;
    std::cout << possible_alignment_p1 << std::endl;

    std::cout << "PART 2:" << std::endl;
    highest_alignment_score_p2 = dp_table_alignment(sequence_A,sequence_B,gap,mismatch,match,possible_alignment_p2);
    std::cout << "Highest match score: " << highest_alignment_score_p2 << std::endl <<
                 "Possible alignment: " << std::endl;
    std::cout << possible_alignment_p2 << std::endl;
    return;
}

int main(){
    test();
	return 0;
}
