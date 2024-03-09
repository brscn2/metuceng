//This file is entirely for your test purposes. 
//This will not be evaluated, you can change it and experiment with it as you want.
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include "the0.h"

//the0.h only contains declaration of the function insertionSort which is:
//void insertionSort(int* arr, long &comparison, long & swap, int size);

using namespace std;

void randomFill(int*& arr, int size, int minval, int interval)
{
	arr = new int [size];
	for (int i=0; i <size; i++)
	{
		arr[i] = minval + (random() % interval);
	}
}

void print_to_file(int* arr, int size)
{
	ofstream ofile;
	ofile.open("sorted.txt");
	for(int i=0;i<size; i++) 
		ofile<<arr[i]<<endl;
}

void read_from_file(int*& arr, int& size)
{

	char addr[]= "input01.txt";
	ifstream infile (addr);
	
	if (!infile.is_open())
	{
		cout << "File \'"<< addr 
			<< "\' can not be opened. Make sure that this file exists." <<endl;
		return;
	}
	infile >> size;
	arr = new int [size];
	
	for (int i=0; i<size;i++) {
		
		infile >> arr[i];
	}
	
}
	

void test()
{
	
	clock_t begin, end;
	double duration;
	
	//data generation and initialization- you may test with your own data
	long comparison=0;
	long swap=0;
	int size=25000;
	int minval=0;
	int interval=size*10;
	int *arr;
	
	//Randomly generate initial array:
	//randomFill(arr, size, minval, interval);
	
	//Read the test inputs. input01.txt through input05.txt exists.
	read_from_file(arr, size);
	
	//data generation or read end
	
	if ((begin = clock() ) ==-1) 
		cerr << "clock error" << endl;
	
	//Function call for the solution
	insertionSort(arr, comparison, swap, size);
	//Function end
	
	if ((end = clock() ) ==-1) 
		cerr << "clock error" << endl;
	
	//Calculate duration and print output

	duration = ((double) end - begin) / CLOCKS_PER_SEC;
	cout << "Duration: " << duration << " seconds." <<endl;
	cout<<"Number of Comparisons: " << comparison <<endl;
	cout<<"Number of Swaps: " << swap <<endl;
	print_to_file(arr,size);
	//Calculation and output end
	
}

int main()
{
	srandom(time(0));
	test();
	return 0;
}