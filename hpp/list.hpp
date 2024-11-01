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

    int64_t*    prev;

    uint64_t    free;
    uint64_t    size;
    uint64_t    numElem;

    files_t     files;

} list_t;

int  ListCtor        (list_t* list);
int  ListDtor        (list_t* list);
int  ListDump        (list_t* list);
int  ListAddStart    (list_t* list, data_t dataElem);
int  ListAddEnd      (list_t* list, data_t dataElem);
int  ListAddAfter    (list_t* list, data_t dataElem, uint64_t param);
int  ListAddBefore   (list_t* list, data_t dataElem, uint64_t param);
int  ListGet();
int  ListRem();
int  ListDel();
