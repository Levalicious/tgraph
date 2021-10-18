#pragma once
#include "types.h"

typedef struct {
    u64 len;
    u64 cap;
    u64* vars;
    bool* vals;
} path;

path* initpath(u64 vcnt);
void freepath(path* p);

void copypath(path* b, path* a);
bool samepath(path* b, path* a);

void reducepath(path* a, path* b);

typedef struct {
    u64 varcnt;
    u64 pathcnt;
    u64 pathcap;
    path* paths;
} tgraph;

tgraph* initgraph(u64 vcnt);
void freegraph(tgraph* t);

void reallocgraph(tgraph* t);

void removepath(tgraph* t, u64 ind);
void purgepaths(tgraph* t);
void addpath(tgraph* t, path* p);