#include <stdio.h>
#include "../hpp/list.hpp"

int main(int argc, char* argv[]){
    list_t l = {};
    ListCtor(&l);
    ListAddEnd(&l, 10);
    ListDump(&l);

    ListAddEnd(&l, 20);
    ListDump(&l);

    ListAddEnd(&l, 30);
    ListDump(&l);

    ListAddAfter(&l, 15, 1);
    ListAddAfter(&l, 16, 2);
    ListAddAfter(&l, 17, 3);
    ListAddAfter(&l, 18, 4);
    ListDump(&l);
    return 0;
}
