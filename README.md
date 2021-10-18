# tgraph
An implementation of Eisa Alharbi's paper on Truth Graphs (which can be found [here](https://www.researchgate.net/publication/343688223_Truth_Graph_A_Novel_Method_for_Minimizing_Boolean_Algebra_Expressions_by_Using_Graphs)) as an approach to minimizing boolean functions.

Example Usage:
This program describes the 3-path example given in the paper. At the end of reduction, the graph should contain two paths, both of length 2, one that represents BC and one that represents AC
```c
#include <stdio.h>
#include "tgraph.h"

int main() {

    tgraph* t = initgraph(3);

    path* red = initpath(3);
    path* blue = initpath(3);
    path* green = initpath(3);

    red->len = 3;
    red->vars[0] = 0;
    red->vars[1] = 1;
    red->vars[2] = 2;

    blue->len = 3;
    blue->vars[0] = 0;
    blue->vars[1] = 1;
    blue->vars[2] = 2;

    green->len = 3;
    green->vars[0] = 0;
    green->vars[1] = 1;
    green->vars[2] = 2;

    red->vals[0] = 1;
    red->vals[1] = 0;
    red->vals[2] = 1;

    blue->vals[0] = 1;
    blue->vals[1] = 1;
    blue->vals[2] = 1;

    green->vals[0] = 0;
    green->vals[1] = 1;
    green->vals[2] = 1;

    addpath(t, green);
    addpath(t, red);
    addpath(t, blue);

    printf("tgraph has %lu distinct paths\n", t->pathcnt);
    for (u64 i = 0; i < t->pathcnt; ++i) {
        printf("Path %lu: \n", t->paths[i].len);
        for (u64 j = 0; j < t->paths[i].len; ++j) {
            printf("\t Var %lu:%u\n", t->paths[i].vars[j], t->paths[i].vals[j]);
        }
    }

    freegraph(t);

    freepath(red);
    freepath(green);
    freepath(blue);

    return 0;
}
```
