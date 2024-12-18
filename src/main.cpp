#include <stdio.h>
#include "../hpp/list.hpp"
#define MEOW fprintf(stderr, "\e[0;31m" "\nmeow\n" "\e[0m");

int main(int argc, char* argv[]){
    list_t l = {};
    ListCtor(&l);
    printf(BRED "%llu\n" RESET, l.lastOperation);
    ListDump(&l);

    ListAddEnd(&l, 10);
    ListDump(&l);

    ListAddEnd(&l, 20);
    ListDump(&l);

    ListAddEnd(&l, 30);
    ListDump(&l);

    ListAddEnd(&l, 40);
    ListDump(&l);

    ListAddEnd(&l, 50);
    ListDump(&l);

    ListAddEnd(&l, 60);
    ListDump(&l);

    ListAddEnd(&l, 70);
    ListDump(&l);

    data_t* meaw = 0;
    ListGetByPos(&l, 3, &meaw);
    printf(BGRN "%lu\n" RESET, *meaw);


    //ListDump(&l);
    ListRemoveBefore(&l, 2);
    ListDump(&l);

    //l.next[4] = 6;

    ListRemoveBefore(&l, 6);
    ListDump(&l);
    ListRemoveAfter(&l, 3);
    ListDump(&l);
    ListRemoveStart(&l);
    ListDump(&l);
    ListRemoveEnd(&l);
    ListDump(&l);

    ListGetStart(&l, &meaw);
    *meaw = -666;
    ListDump(&l);

    ListDel(&l);
    ListDump(&l);

    uint64_t meow = 52;
    ListFindPos(&l, 30, &meow);
    printf(BGRN "%lu\n" RESET, meow);
    ListFindPos(&l, 40, &meow);
    printf(BGRN "%lu\n" RESET, meow);
    MEOW
    HTMLDumpGenerate(&l);


    return 0;
}
