# tgraph
An implementation of Eisa Alharbi's paper on Truth Graphs (which can be found [here](https://www.researchgate.net/publication/343688223_Truth_Graph_A_Novel_Method_for_Minimizing_Boolean_Algebra_Expressions_by_Using_Graphs)) as an approach to minimizing boolean functions.

Example Usage:
This program describes the 3-path example given in the paper. At the end of reduction, the graph should contain two paths, both of length 2, one that represents BC and one that represents AC
```c
#include <stdio.h>
#include "tgraph.h"

int main() {
    // Initialize memory for graph & paths
    tgraph* t = initgraph(3);

    path* red = initpath(3);
    path* blue = initpath(3);
    path* green = initpath(3);

    // Set the number of variables
    // in each path
    red->len = 3;
    blue->len = 3;
    green->len = 3;

    // Set the values for each
    // path's variables
    red->vals[0] = 1;
    red->vals[1] = 0;
    red->vals[2] = 1;

    blue->vals[0] = 1;
    blue->vals[1] = 1;
    blue->vals[2] = 1;

    green->vals[0] = 0;
    green->vals[1] = 1;
    green->vals[2] = 1;

    // Add the paths in any
    // arbitrary order to the tgraph
    addpath(t, green);
    addpath(t, red);
    addpath(t, blue);

    // Print the reduction results
    printf("%lu\n", t->pathcnt);
    for (u64 i = 0; i < t->pathcnt; ++i) {
        printf("%lu\n", t->paths[i].len);
        for (u64 j = 0; j < t->paths[i].len; ++j) {
            printf("%lu : %u\n", t->paths[i].vars[j], t->paths[i].vals[j]);
        }
    }

    // Clean up memory
    freegraph(t);

    freepath(red);
    freepath(green);
    freepath(blue);

    return 0;
}
```
