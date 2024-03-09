#include "the1.h"

//You may write your own helper functions here

void swapElements(unsigned short &first, unsigned short &second)
{
    unsigned short temp = first;
    first = second;
    second = temp;
}

int findMedian(unsigned short *arr, int start, int end, int mid)
{
    int median;

    if ((arr[start] > arr[end] && arr[start] < arr[mid]) || (arr[start] < arr[end] && arr[start] > arr[mid]))
        median = start;
    else if ((arr[end] > arr[start] && arr[end] < arr[mid]) || (arr[end] < arr[start] && arr[end] > arr[mid]))
        median = end;
    else if ((arr[mid] > arr[start] && arr[mid] < arr[end]) || (arr[mid] < arr[start] && arr[mid] > arr[end]))
        median = mid;
    /*else
        median = start;*/
    else if ((arr[start] == arr[end]) && arr[end] > arr[mid])
        median = start;
    else if ((arr[start] == arr[end]) && arr[end] < arr[mid])
        median = start;
    else if ((arr[end] == arr[mid]) && arr[end] > arr[start])
        median = mid;
    else if ((arr[end] == arr[mid]) && arr[end] < arr[start])
        median = mid;
    else if ((arr[start] == arr[mid]) && arr[start] > arr[end])
        median = start;
    else if ((arr[start] == arr[mid]) && arr[start] < arr[end])
        median = start;
    

    return median;
}

int partition(unsigned short *arr, int size, long &swap, double &distances, double &max_dist, bool median_of_3)
{

    double distance;
    if (median_of_3 && size > 2)
    {
        int medianIdx = findMedian(arr, 0, size - 1, (size - 1) / 2);
        if (medianIdx != (size - 1))
        {
            swap++;
            distance = (size - 1) > medianIdx ? (size - 1) - medianIdx : medianIdx - (size - 1);
            max_dist = distance > max_dist ? distance : max_dist;
            distances += distance;
            swapElements(arr[size - 1], arr[medianIdx]);
        }
    }

    int X = arr[size - 1];
    int i = -1;

    for (int j = 0; j < size - 1; j++)
    {
        if (arr[j] >= X)
        {
            i++;
            swap++;
            distance = j > i ? j - i : i - j;
            max_dist = distance > max_dist ? distance : max_dist;
            distances += distance;
            swapElements(arr[i], arr[j]);
        }
    }
    swap++;
    distance = (size - 1) > (i + 1) ? (size - 1) - (i + 1) : (i + 1) - (size - 1);
    max_dist = distance > max_dist ? distance : max_dist;
    distances += distance;

    swapElements(arr[i + 1], arr[size - 1]);
    return i + 1;
}

int hoarePartition(unsigned short *arr, int size, long &swap, double &distances, double &max_dist, bool median_of_3)
{
    double distance;
    if (median_of_3 && size > 2)
    {
        int medianIdx = findMedian(arr, 0, size - 1, (size - 1) / 2);
        if (medianIdx != ((size - 1) / 2))
        {
            swap++;
            distance = ((size - 1) / 2) > medianIdx ? ((size - 1) / 2) - medianIdx : medianIdx - ((size - 1) / 2);
            max_dist = distance > max_dist ? distance : max_dist;
            distances += distance;
            swapElements(arr[(size - 1) / 2], arr[medianIdx]);
        }
    }

    int X = arr[(size - 1) / 2];
    int i = -1;
    int j = size;

    while (true)
    {
        do
        {
            j--;
        } while (arr[j] < X);

        do
        {
            i++;
        } while (arr[i] > X);

        if (i < j)
        {
            swap++;
            distance = i > j ? i - j : j - i;
            max_dist = distance > max_dist ? distance : max_dist;
            distances += distance;
            swapElements(arr[i], arr[j]);
        }
        else
        {
            return j;
        }
    }
}

int quickSort(unsigned short *arr, long &swap, double &avg_dist, double &max_dist, bool hoare, bool median_of_3, int size)
{
    // Your code here
    int callCount = 1;

    if (!hoare && size > 1)
    {
        double currentDistances = 0;
        avg_dist *= swap;
        int p = partition(arr, size, swap, currentDistances, max_dist, median_of_3);
        if (swap != 0) 
            avg_dist = (avg_dist + currentDistances) / swap;

        // from first element to p - 1
        callCount += quickSort(arr, swap, avg_dist, max_dist, hoare, median_of_3, p);
        // from p + 1 th element to size - 1
        callCount += quickSort(arr + p + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - (p + 1));
    }

    if (hoare && size > 1)
    {
        double currentDistances = 0;
        avg_dist *= swap;
        int p = hoarePartition(arr, size, swap, currentDistances, max_dist, median_of_3);
        if (swap != 0)
            avg_dist = (avg_dist + currentDistances) / swap;
        // from first element to p
        callCount += quickSort(arr, swap, avg_dist, max_dist, hoare, median_of_3, p + 1);
        // from p + 1 th element to size - 1
        callCount += quickSort(arr + p + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - (p + 1));
    }

    return callCount;
}
