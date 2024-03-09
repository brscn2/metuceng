// #include "the4.h"
// #include <math.h>
#include <iostream>

// do not add extra libraries here

int divide_land_aux(int X, int Y, bool **possible_plots)
{
    int **dp_table = new int *[X + 1];
    for (int i = 0; i < X + 1; i++)
    {
        dp_table[i] = new int[Y + 1];
    }

    // initialize base cases
    for (int i = 0; i < Y + 1; i++)
    {
        dp_table[0][i] = 0;
    }
    for (int i = 0; i < X + 1; i++)
    {
        dp_table[i][0] = 0;
    }


    for (int i = 1; i < X + 1; i++)
    {
        for (int j = 1; j < Y + 1; j++)
        {

            int max = 0;

            /*
                Try the whole plot
            */
            if (possible_plots[i][j] == true)
            {
                max = i * j;
            }

            /*
                Try different horizontal divisions
            */
            for (int k = 1; k <= i / 2; k++)
            {
                int possible_division = dp_table[k][j] + dp_table[i - k][j];
                max = max > possible_division ? max : possible_division;
            }

            /*
                Try different vertical divisions
            */
            for (int k = 1; k <= j / 2; k++)
            {
                int possible_division = dp_table[i][k] + dp_table[i][j - k];
                max = max > possible_division ? max : possible_division;
            }

            dp_table[i][j] = max;
        }
    }

    for (int i = 0; i < X + 1; i++)
    {
        for (int j = 0; j < Y + 1; j++)
            std::cout << dp_table[i][j] << "\t";
        std::cout << std::endl;
    }

    return dp_table[X][Y];
}

int divide_land(int X, int Y, bool **possible_plots)
{
    int used_land = divide_land_aux(X, Y, possible_plots);
    return (X * Y) - used_land;
}

int main()
{
    int X = 11, Y = 7;
    bool **possible_plots;
    possible_plots = new bool *[601];
    for (int i = 0; i < 601; ++i)
    {
        possible_plots[i] = new bool[601];
    }
    for (int i = 0; i < 601; i++)
    {
        for (int j = 0; j < 601; j++)
        {
            possible_plots[i][j] = false;
        }
    }

    possible_plots[4][5] = true;
    possible_plots[6][7] = true;

    int result = divide_land(X, Y, possible_plots);
    std::cout << result << std::endl;
    return 0;
}