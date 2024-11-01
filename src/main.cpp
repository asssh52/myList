#include <stdio.h>
#include "../hpp/list.hpp"

int main(int argc, char* argv[]){
    list_t l = {};
    ListCtor(&l);
    ListDump(&l);
    ListAddEnd(&l, 10);
    ListDump(&l);
    ListAddEnd(&l, 20);
    ListDump(&l);
    ListAddStart(&l, 5);
    ListDump(&l);

    ListAddAfter(&l, 15, 2);
    ListDump(&l);
    ListAddBefore(&l, 17, 2);
    ListDump(&l);
    return 0;
}
