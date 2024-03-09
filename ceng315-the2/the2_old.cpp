#include <string>
#include <iostream>

// do not add extra libraries here
/*
    arr       : array to be sorted, in order to get points this array should contain be in sorted state before returning
    ascending : true for ascending, false for descending
    size      : number of elements in the array
*/
int counting_sort(std::string *arr, int k, long unsigned int digit, int size, bool ascending)
{
    int iterations = 0;
    int *C = new int[k];

    for (int i = 0; i < k; i++)
    {
        // std::cout << "Initializing to zero"
        //           << "\n";
        C[i] = 0;
    }

    for (int j = 0; j < size; j++)
    {
        // std::cout << "Setting count array"
        //           << "\n";
        iterations++;
        if (arr[j][digit] == '\0')
            C[0] = C[0] + 1;
        else
            C[arr[j][digit] - 64] = C[arr[j][digit] - 64] + 1;
    }

    if (ascending)
        for (int i = 1; i < k; i++)
        {
            // std::cout << "Updating count array (less than)"
            //           << "\n";
            iterations++;
            C[i] = C[i] + C[i - 1];
        }
    else
        for (int i = k - 2; i >= 0; i--)
        {
            iterations++;
            C[i] = C[i] + C[i + 1];
        }

    std::string *newArr = new std::string[size];

    for (int j = size - 1; j > -1; j--)
    {
        iterations++;

        int index = arr[j][digit] == '\0' ? 0 : arr[j][digit] - 64;
        // std::cout << "Inserting the sorted array"
        //           << "j: " << j
        //           << " index: " << index
        //           << "\n";
        newArr[C[index] - 1] = arr[j];
        C[index] = C[index] - 1;
    }

    for (int i = 0; i < size; i++)
    {
        iterations++;
        arr[i] = newArr[i];
    }

    return iterations;
}

int radix_string_sort(std::string *arr, int size, bool ascending)
{
    int iterations = 0;

    /*
        HANDLE DIFFERENT LENGTHS
    */
    int max_length = -1;
    for (int i = 0; i < size; i++)
    {
        max_length = (int)arr[i].length() > max_length ? arr[i].length() : max_length;
    }

    for (int i = 0; i < size; i++)
    {
        int slength = arr[i].length();
        if (slength != max_length)
        {

            arr[i].resize(max_length);
        }
    }

    /*
        CALL COUNTING SORT
    */
    for (int i = max_length - 1; i >= 0; i--)
    {
        iterations += counting_sort(arr, 27, i, size, ascending);
    }

    return iterations;
}

int main()
{
    int size = 6;
    std::string arr[size] = {"PAPATYA", "LALE", "MENEKSE", "AKSAMSEFASI", "BEGONVIL", "KARANFIL"};

    int iterations = radix_string_sort(arr, size, false);

    std::cout << "Iterations: " << iterations << "\n";
    for (int i = 0; i < size; i++)
    {
        std::cout << arr[i] << "\n";
    }
    return 0;
}