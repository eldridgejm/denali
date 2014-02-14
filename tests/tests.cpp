#include <UnitTest++.h>
#include <iostream>

#include <string>
#include <set>

double wenger_vertex_values[] = 
    // 0   1   2   3   4   5   6   7   8   9  10  11
    { 25, 62, 45, 66, 16, 32, 64, 39, 58, 51, 53, 30 };

const size_t n_wenger_vertices = sizeof(wenger_vertex_values)/sizeof(double);

unsigned int wenger_edges[][2] =
   {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {6, 7}, {7, 8}, {9, 10}, {10, 11}, {0, 3},
    {1, 4}, {2, 5}, {3, 6}, {4, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}, {0, 4},
    {1, 5}, {3, 7}, {4, 8}, {6, 10}, {7, 11}};

const size_t n_wenger_edges = sizeof(wenger_edges)/sizeof(unsigned int[2]);



int main()
{

    return UnitTest::RunAllTests();
}
