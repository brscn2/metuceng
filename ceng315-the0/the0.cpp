// #include "the0.h"
#include "iostream"

using namespace std;

void insertionSort(int *arr, long &comparison, long &swap, int size)
{
    int i = 1;

    while (i < size)
    {
        int x = arr[i];
        int j = i - 1;

        while ((1 + ++comparison) && j >= 0 && arr[j] > x)
        {
            arr[j + 1] = arr[j];
            swap++;
            j--;
        }

        arr[j + 1] = x;
        i++;
    }
}

int main()
{
    int arr[6] = {0, -5, -5, -5, 4, 1};
    long comparison = 0;
    long swap = 0;
    insertionSort(arr, comparison, swap, 6);

    for (int i = 0; i < 5; i++)
    {
        cout << arr[i] << ", ";
    }
    cout << "\n"
         << "Swaps: " << swap << " Comparisons: " << comparison << "\n";
}
