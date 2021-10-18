#include "tgraph.h"
#include <string.h>
#include <stdio.h>

/***********************************/
/** Path memory utility functions **/
/***********************************/
path* initpath(u64 vcnt) {
   path* o = malloc(sizeof(path));;

   o->len = 0;
   o->cap = vcnt;
   o->vars = malloc(sizeof(u64) * o->cap);
   o->vals = malloc(sizeof(_Bool) * o->cap);
   for (u64 i = 0; i < vcnt; ++i) o->vals[i] = i;

   return o;
}

void freepath(path* p) {
    free(p->vars);
    free(p->vals);
    free(p);
}

/***********************************/
/**     Path utility functions    **/
/***********************************/
/* Copies a to b */
void copypath(path* b, path* a) {
    memcpy(b->vars, a->vars, sizeof(u64) * a->len);
    memcpy(b->vals, a->vals, sizeof(bool) * a->len);
    b->len = a->len;
}

/* Checks if a and b are equivalent */
bool samepath(path* b, path* a) {
    if (a->len != b->len) return false;
    for (u64 i = 0; i < a->len; ++i) {
        if (a->vars[i] != b->vars[i]) return false;
        if (a->vals[i] != b->vals[i]) return false;
    }
    return true;
}


/** Reduction Rules **/
/* This function is kind of a mess */
void reducepath(path* a, path* b) {
    u64 agreements = 0;
    u64 missingA = UINT64_MAX;
    u64 missingB = UINT64_MAX;
    u64 valDiffA = UINT64_MAX;
    u64 valDiffB = UINT64_MAX;
    u64 indA = 0;
    u64 indB = 0;

    // Search through both arrays
    // All arrays are sorted on input to a tgraph, so we can use that
    // This loop counts the number of divergences between 2 paths,
    // both in terms of different variables and different values
    while (indA < a->len && indB < b->len) {
        if (a->vars[indA] < b->vars[indB]) {
            // Record variable in a missing in b
            missingA = indA;
            indA++;
        } else if (a->vars[indA] > b->vars[indB]) {
            // Record variable in b missing in a
            missingB = indB;
            indB++;
        } else {
            if (a->vals[indA] == b->vals[indB]) {
                // Record variable & value agreement
                agreements++;
            } else {
                if (valDiffB == UINT64_MAX) {
                    // Record first value disagreement
                    valDiffA = indA;
                    valDiffB = indB;
                }
            }
            indA++;
            indB++;
        }
    }

    // To reduce, we need there to be 1 difference between 2 paths
    // As such, we expect the number of agreements to be the
    // length of the smallest path - 1
    u64 expectedAgreements = min(a->len, b->len);
    if (agreements == expectedAgreements - 1) {
        // if valDiffA is still equal to UINT64_MAX
        // the loop did not find a disagreeing value
        if (valDiffA == UINT64_MAX) {
            // If the lengths of the two paths are equal,
            // we can remove the variable in question from both,
            // or whichever one has the variable
            if (a->len == b->len) {
                // If missingA is not equal to UINT64_MAX
                // b is missing a variable a has
                if (missingA != UINT64_MAX) {
                    // Edge case for removal with a path of length 1
                    if (a->len == 1) {
                        a->len--;
                        b->len--;
                    } else {
                        // Edge case for removing last item of path
                        if (missingA == a->len - 1) {
                            a->len--;
                        } else {
                            // Actually removing an item for somewhere in the middle of a path
                            memmove(a->vars + missingA, a->vars + missingA + 1, sizeof(u64) * (a->len - (missingA + 1)));
                            memmove(a->vals + missingA, a->vals + missingA + 1, sizeof(bool) * (a->len - (missingA + 1)));
                            a->len--;
                        }
                    }
                }
                // Same code as for missingA, but for if a is missing a variable b has
                // The case with diverging values is already handled by the earlier
                // (valDiffA == UINT64_MAX) if statement, so we don't need to handle
                // it here
                if (missingB != UINT64_MAX) {
                    if (missingB == b->len - 1) {
                        b->len--;
                    } else {
                        memmove(b->vars + missingB, b->vars + missingB + 1, sizeof(u64) * (b->len - (missingB + 1)));
                        memmove(b->vals + missingB, b->vals + missingB + 1, sizeof(bool) * (b->len - (missingB + 1)));
                        b->len--;
                    }
                }
            } else if (a->len < b->len) {
                // If a is shorter than b, we can only remove
                // variables from b, so if missingB is UINT64_MAX
                // the variable to remove is in path a
                if (missingB != UINT64_MAX) {
                    if (missingB == b->len - 1) {
                        b->len--;
                    } else {
                        memmove(b->vars + missingB, b->vars + missingB + 1, sizeof(u64) * (b->len - (missingB + 1)));
                        memmove(b->vals + missingB, b->vals + missingB + 1, sizeof(bool) * (b->len - (missingB + 1)));
                        b->len--;
                    }
                }
            } else if (a->len > b->len) {
                // If b is shorter than a, we can only remove
                // variables from a, so if missingA is UINT64_MAX
                // the variable to remove is in path b
                if (missingA != UINT64_MAX) {
                    if (missingA == a->len - 1) {
                        a->len--;
                    } else {
                        memmove(a->vars + missingA, a->vars + missingA + 1, sizeof(u64) * (a->len - (missingA + 1)));
                        memmove(a->vals + missingA, a->vals + missingA + 1, sizeof(bool) * (a->len - (missingA + 1)));
                        a->len--;
                    }
                }
            }
            return;
        } else {
            // If we are here, the loop found a disagreeing value
            // if the two paths are equal, we can remove the variable from
            // both paths
            if (a->len == b->len) {
                // Edge len == 1 removal case
                // Unlike for the missing variable cases, if a value disagrees
                // since we know we only have 1 disagreement, we know
                // both values are valid if any one of them is
                if (a->len == 1) {
                    a->len--;
                    b->len--;
                    return;
                } else {
                    // If valDiffA is the last element, we can remove it
                    // by decreasing the number of elements
                    if (valDiffA == a->len - 1) {
                        a->len--;
                    } else {
                        // Otherwise we overwrite it
                        memmove(a->vars + valDiffA, a->vars + valDiffA + 1, sizeof(u64) * (a->len - (valDiffA + 1)));
                        memmove(a->vals + valDiffA, a->vals + valDiffA + 1, sizeof(bool) * (a->len - (valDiffA + 1)));
                        a->len--;
                    }
                    // Same as above
                    if (valDiffB == b->len - 1) {
                        b->len--;
                    } else {
                        memmove(b->vars + valDiffB, b->vars + valDiffB + 1, sizeof(u64) * (b->len - (valDiffB + 1)));
                        memmove(b->vals + valDiffB, b->vals + valDiffB + 1, sizeof(bool) * (b->len - (valDiffB + 1)));
                        b->len--;
                    }
                    return;
                }
            } else if (a->len < b->len) {
                // If a is shorter than b, we can only remove the variable in question
                // from b
                if (valDiffB == b->len - 1) {
                    b->len--;
                } else {
                    memmove(b->vars + valDiffB, b->vars + valDiffB + 1, sizeof(u64) * (b->len - (valDiffB + 1)));
                    memmove(b->vals + valDiffB, b->vals + valDiffB + 1, sizeof(bool) * (b->len - (valDiffB + 1)));
                    b->len--;
                }
            } else {
                // Our final case to handle:
                // If b is shorter than a, we can only remove the
                // variable of the value disagreement in a
                if (a->len == 1) {
                    a->len--;
                    return;
                }
                if (valDiffA == a->len - 1) {
                    a->len--;
                } else {
                    memmove(a->vars + valDiffA, a->vars + valDiffA + 1, sizeof(u64) * (a->len - (valDiffA + 1)));
                    memmove(a->vals + valDiffA, a->vals + valDiffA + 1, sizeof(bool) * (a->len - (valDiffA + 1)));
                    a->len--;
                }
            }
        }
    }
}

/*************************************/
/** tgraph memory utility functions **/
/*************************************/
tgraph* initgraph(u64 vcnt) {
    tgraph* o = malloc(sizeof(tgraph));;

    o->varcnt = vcnt;
    o->pathcnt = 0;
    o->pathcap = 64;
    o->paths = malloc(sizeof(path) * o->pathcap);;
    for (u64 i = 0; i < o->pathcap; ++i) {
        (o->paths + i)->len = 0;
        (o->paths + i)->cap = o->varcnt;
        (o->paths + i)->vars = malloc(sizeof(u64) * (o->paths + i)->cap);;
        (o->paths + i)->vals = malloc(sizeof(_Bool) * (o->paths + i)->cap);;
    }

    return o;
}

void freegraph(tgraph* t) {
    for (u64 i = 0; i < t->pathcap; ++i) {
        free((t->paths + i)->vars);
        free((t->paths + i)->vals);
    }
    free(t->paths);
    free(t);
}

void reallocgraph(tgraph* t) {
    u64 oldcap = t->pathcap;
    t->pathcap *= 2;
    t->paths = realloc(t->paths, sizeof(path) * t->pathcap);
    if (t->paths == NULL) {
        printf("Graph reallocation failed\n");
        exit(0);
    }
    for (u64 i = oldcap; i < t->pathcap; ++i) {
        path *o = t->paths + i;
        o->len = 0;
        o->cap = t->varcnt;
        o->vars = malloc(sizeof(u64) * o->cap);;
        o->vals = malloc(sizeof(_Bool) * o->cap);;
    }
}

/*************************************/
/**    tgraph utility functions     **/
/*************************************/
// Removes a single path from a tgraph's pathlist
void removepath(tgraph* t, u64 ind) {
    for (u64 i = ind + 1; i < t->pathcnt; ++i) {
        copypath(t->paths + (ind - 1), t->paths + ind);
    }
    t->pathcnt--;
}

// Checks all paths in a tgraph's pathlist and removes 0-paths and identical paths
void purgepaths(tgraph* t) {
    for (u64 i = 0; i < t->pathcnt; ++i) {
        for (u64 j = i + 1; j < t->pathcnt; ++j) {
            if (t->paths[j].len == 0) {
                removepath(t, j);
            } else if (samepath(t->paths + i, t->paths + j)) {
                removepath(t, j);
            }
        }
    }
}

void addpath(tgraph* t, path* p) {
    // No need for complicated logic when there are no paths in a tgraph
    if (t->pathcnt == 0) {
        copypath(t->paths, p);
        t->pathcnt++;
    } else {
        // In case we're running up against memory bounds with the number of paths
        if (t->pathcap == t->pathcnt) reallocgraph(t);
        // Apply one reduction step against each existing path
        for (u64 i = 0; i < t->pathcnt; ++i) {
            reducepath(t->paths + i, p);
        }
        // After reduction rules have been applied, if path is not 0-length
        // add it to the list
        if (p->len != 0) {
            copypath(t->paths + t->pathcnt, p);
            t->pathcnt++;
        }
        // and then remove any 0-paths or identical paths in the graph
        purgepaths(t);
    }
}