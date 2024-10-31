#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "colors.hpp"

const int64_t LIST_SIZE = 16;
const int64_t POISON    = 52;

typedef int64_t data_t;

typedef struct files{
    char* logFilename;
    FILE* logFile;

    char* visDumpName;
    FILE* visDump;

} files_t;

typedef struct list{

    data_t*    data;

    int64_t*    next;
    int64_t     head;

    int64_t*    prev;
    int64_t     tail;

    uint64_t    free;
    uint64_t    size;
    uint64_t    numElem;

    files_t     files;

} list_t;

static enum errors{
    OK      =   0,
    ERR     =   1,
    MEM_ERR =   2
};

errors  ListCtor        (list_t* list);
errors  ListDtor        (list_t* list);
errors  ListDump        (list_t* list);
errors  ListAddStart    (list_t* list, data_t dataElem);
errors  ListAddEnd      (list_t* list, data_t dataElem);
errors  ListAddAfter    (list_t* list, data_t dataElem, uint64_t param);
errors  ListGet();
errors  ListRem();
errors  ListDel();
