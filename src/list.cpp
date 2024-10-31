#include "../hpp/list.hpp"

#define MEOW fprintf(stderr, "\e[0;31m" "\nmeow\n" "\e[0m");

static errors ListVerify    (list_t* list);
static errors ListVisualDump(list_t* list);
static errors FillNext      (list_t* list);
static errors FillPrev      (list_t* list);


/*==================================================================================*/

errors ListCtor(list_t* list){

    list->data = (data_t*)calloc(LIST_SIZE, sizeof(data_t));
    if (list->data == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->next = (int64_t*)calloc(LIST_SIZE, sizeof(int64_t));
    if (list->next == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->prev = (int64_t*)calloc(LIST_SIZE, sizeof(int64_t));
    if (list->prev == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->files.visDump = fopen("visdump.dot", "w");
    list->files.logFile = fopen(list->files.logFilename, "w");

    list->size  = LIST_SIZE;
    list->free  = 1;
    *list->data = POISON;

    FillNext(list);
    FillPrev(list);

    ListDump(list);

    ListVerify(list);

    printf(CYN "list created\n" RESET);

    return OK;
}

/*==================================================================================*/

static errors FillNext(list_t* list){

    for (int i = 1; i < list->size - 1; i++){
        list->next[i] = i + 1;
    }

    list->next[list->size - 1] = 0;

    return OK;
}

/*==================================================================================*/

static errors FillPrev(list_t* list){
    for (int i = 2; i < list->size - 1; i++){
        list->prev[i] = i - 1;
    }

    list->prev[list->size - 1] = 0;

    return OK;
}

/*==================================================================================*/


errors ListDtor(list_t* list){
    if (ListVerify(list)) return ERR;

    fclose(list->files.logFile);
    fclose(list->files.visDump);

    free(list->data);
    free(list->next);
    free(list->prev);

    return OK;
}

/*==================================================================================*/

errors ListDump(list_t* list){

    printf(BGRN "=====================================\n" RESET);
    printf(BBLU "Start of dump.\n" RESET);

    if (!list){
        printf("list does not exist\n");
    }

    printf("Size:     \t%llu\n"      , list->size);
    printf("Num elems:\t%llu\n"   , list->numElem);
    printf("\n");
    printf("Next:\t%p\n"        , list->next);
    printf("Prev:\t%p\n"        , list->prev);
    printf("Head:\t%lld\n"      , list->head);
    printf("Tail:\t%lld\n"      , list->tail);
    printf("Free:\t%lld\n"      , list->free);

    printf(YEL "---------------------------\n" RESET);

    printf("data:%p\n", list->data);
    if (list->data){


        for (int i = 0; i < list->size; i++){
            printf("%0.2d  ", i);
        }

        printf("\n");

        for (int i = 0; i < list->size; i++){
            printf("%0.2lld  ", list->data[i]);
        }

        printf("\n");
    }

    else{
        printf("data is empty.\n");
    }

    printf(YEL "---------------------------\n" RESET);

    printf("next:%p\n", list->next);
    if (list->next){

        for (int i = 0; i < list->size; i++){
            printf("%0.2d  ", i);
        }

        printf("\n");

        for (int i = 0; i < list->size; i++){
            printf("%0.2lld  ", list->next[i]);
        }

        printf("\n");
    }

    else{
        printf("next is empty.\n");
    }

    printf(YEL "---------------------------\n" RESET);

    printf("prev:%p\n", list->prev);
    if (list->prev){

        for (int i = 0; i < list->size; i++){
            printf("%0.2d  ", i);
        }

        printf("\n");

        for (int i = 0; i < list->size; i++){
            printf("%0.2lld  ", list->prev[i]);
        }

        printf("\n");
    }

    else{
        printf("prev is empty.\n");
    }

    printf(BBLU "End of dump.\n" RESET);
    printf(BGRN "=====================================\n" RESET);

    /* --------------- */
    // VISUAL DUMP:
    /* --------------- */
    ListVisualDump(list);

    return OK;
}

/*==================================================================================*/

static errors ListVisualDump(list_t* list){
    list->files.visDump = fopen("visdump.dot", "w");
    fprintf(list->files.visDump, "digraph G{\n");

    fprintf(list->files.visDump, "\trankdir=LR;\n");

    for (int i = 0; i < list->numElem + 1; i++){
        fprintf(list->files.visDump,
        "\tnode%0.3d [shape=Mrecord; label = \" { %0.3d | data = %3.0lld | next = %lld | prev = %lld } \"];\n",
        i, i, list->data[i], list->next[i], list->prev[i]);
    }

    fprintf(list->files.visDump, "\n");

    for (int i = 0; i < list->numElem; i++){
        fprintf(list->files.visDump,
        "\tnode%0.3d -> node%0.3d [ weight=1000; color=white; ];\n",
        i, i + 1
        );
    }

    fprintf(list->files.visDump, "\n");

    fprintf(list->files.visDump,
        "\tnode000 -> node000 [ color=red; ];\n"
    );

    for (int64_t i = list->head; i != 0; i = list->next[i]){
        fprintf(list->files.visDump,
        "\tnode%0.3lld -> node%0.3lld [ color=red;];\n",
        i, list->next[i]
        );
    }

    fprintf(list->files.visDump, "\n");

    fprintf(list->files.visDump,
        "\tnode000 -> node000 [ color=blue; ];\n"
    );

    for (int64_t i = list->tail; i != 0; i = list->prev[i]){
        fprintf(list->files.visDump,
        "\tnode%0.3lld -> node%0.3lld [ color=blue; constraint = false];\n",
        i, list->prev[i]
        );
    }

    fprintf(list->files.visDump,
    "header [shape=Mrecord; label = \" { head = %lld | tail = %lld | numElems = %lld } \"]\n"
    "{ rank = same; \"header\"; \"node000\"; }",
    list->head, list->tail, list->numElem
    );

    fprintf(list->files.visDump, "}\n");

    fclose(list->files.visDump);
    getchar();
    return OK;
}

/*==================================================================================*/

static errors ListVerify(list_t* list){

    return OK;
}

/*==================================================================================*/

static errors ListBeginList(list_t* list, data_t dataElem){

    list->data[list->free] = dataElem;

    list->head  = list->free; //free == 1
    list->tail  = list->free;

    list->free  = list->next[list->free];
    list->next[list->prev[list->free]] = 0;
    list->prev[list->free] = 0;

    list->numElem++;
    return OK;
}



/*==================================================================================*/

errors  ListAddStart(list_t* list, data_t dataElem){
    if (ListVerify(list)) return ERR;

    if (list->numElem == 0){
        ListBeginList(list, dataElem);
        return OK;
    }


    list->data[list->free] = dataElem;

    int64_t temp = list->next[list->free];
                   list->next[list->free] = list->head;

    list->prev[list->head] = list->free;
    list->prev[list->free] = 0;

    list->head = list->free;
                 list->free = temp;


    list->numElem++;
    return OK;
}

/*==================================================================================*/

errors  ListAddEnd(list_t* list, data_t dataElem){
    if (ListVerify(list)) return ERR;

    if (list->numElem == 0){
        ListBeginList(list, dataElem);
        return OK;
    }

    list->data[list->free] = dataElem;

    int64_t temp = list->next[list->free];
                   list->next[list->free] = 0;

    list->prev[list->free] = list->tail;
    list->next[list->tail] = list->free;

    list->tail = list->free;
                 list->free = temp;

    list->prev[list->free] = 0;

    list->numElem++;
    return OK;
}

/*==================================================================================*/

errors  ListAddAfter(list_t* list, data_t dataElem, uint64_t param){
    if (ListVerify(list)) return ERR;
    //verify list contains param
    int64_t temp2 = list->next[list->free];

    list->data[list->free] = dataElem;

    list->prev[list->next[list->free]] = 0;
    list->prev[list->free] = param;
    list->prev[list->next[param]] = list->free;

    int64_t temp = list->next[param];
                   list->next[param] = list->free;
                            list->next[list->free] = temp;

    list->free = temp2;
    list->numElem++;
    return OK;
}
