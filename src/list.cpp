#include "../hpp/list.hpp"

#define MEOW fprintf(stderr, "\e[0;31m" "\nmeow\n" "\e[0m");

enum errors{
    OK              =   0,
    ERR             =   1,
    MEM_ERR         =   2,
    NULL_ERR        =   3,
    OVERFLOW_ERR    =   4,
    LINK_ERR        =   5,
    INDEX_ERR       =   6
};

enum operations{
    CTOR        = 1,
    DTOR        = 2,

    ADD_START   = 3,
    ADD_END     = 4,
    ADD_AFTER   = 5,
    ADD_BEFORE  = 6,

    REM_START   = 7,
    REM_END     = 8,
    REM_AFTER   = 9,
    REM_BEFORE  = 10,

    FIND_POS    = 11,
    GETBY_POS   = 12,

    GET_START   = 13,
    GET_END     = 14,
    GET_AFTER   = 15,
    GET_BEFORE  = 16,

    DEL         = 17
};

static int ListVerify           (list_t* list);
static int ListVisualDump       (list_t* list);
static int FillNext             (list_t* list);
static int FillPrev             (list_t* list);
static int FillData             (list_t* list);
static int DoDot                (list_t* list);
static int HTMLGenerateHead     (list_t* list);
static int HTMLGenerateBody     (list_t* list);
static int GetLastOperation     (list_t* list, char** lastOper);



/*==================================================================================*/

int ListCtor(list_t* list){

    list->data = (data_t*)calloc(LIST_SIZE, sizeof(data_t));
    if (list->data == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->next = (int64_t*)calloc(LIST_SIZE, sizeof(int64_t));
    if (list->next == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->prev = (int64_t*)calloc(LIST_SIZE, sizeof(int64_t));
    if (list->prev == nullptr){ printf("memory error\n"); return MEM_ERR;}

    list->files.visDumpName     = "visdump.dot";
    list->files.htmlDumpName    = "htmldump.html";

    list->files.htmlDump =  fopen(list->files.htmlDumpName, "w");
    list->files.visDump  =  fopen(list->files.visDumpName,  "w");
    list->files.logFile  =  fopen(list->files.logFilename,  "w");

    list->size  = LIST_SIZE;
    list->free  = 1;

    FillNext(list);
    FillPrev(list);
    FillData(list);

    ListVerify(list);

    printf(CYN "list created\n" RESET);

    list->lastOperation = CTOR;
    return OK;
}

/*==================================================================================*/

static int FillNext(list_t* list){

    list->next[0] = 0;

    for (int i = 1; i < list->size - 1; i++){
        list->next[i] = i + 1;
    }

    list->next[list->size - 1] = 0;

    return OK;
}

/*==================================================================================*/

static int FillPrev(list_t* list){

    list->prev[0] = 0;

    for (int i = 1; i < list->size; i++){
        list->prev[i] = -1;
    }

    return OK;
}


/*==================================================================================*/

static int FillData(list_t* list){

    for (int i = 0; i < list->size; i++){
        list->data[i] = POISON;
    }

    return OK;
}

/*==================================================================================*/


int ListDtor(list_t* list){
    if (ListVerify(list)) return ERR;

    fclose(list->files.logFile);
    fclose(list->files.visDump);

    free(list->data);
    free(list->next);
    free(list->prev);

    list->lastOperation = DTOR;
    return OK;
}

/*==================================================================================*/

int ListDump(list_t* list){

    printf(BGRN "=====================================\n" RESET);
    printf(BBLU "Start of dump:#%llu.\n" RESET, list->numDump + 1);

    if (!list){
        printf("list does not exist\n");
    }

    printf("Size:     \t%llu\n"      , list->size);
    printf("Num elems:\t%llu\n"   , list->numElem);
    printf("\n");
    printf("Next:\t%p\n"        , list->next);
    printf("Prev:\t%p\n"        , list->prev);
    printf("Head:\t%lld\n"      , list->next[0]);
    printf("Tail:\t%lld\n"      , list->prev[0]);
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

    ListVisualDump(list);
    return OK;
}

/*==================================================================================*/

static int ListVisualDump(list_t* list){
    if (list->numDump == 0) HTMLGenerateHead(list);

    list->files.visDump = fopen(list->files.visDumpName, "w");
    fprintf(list->files.visDump, "digraph G{\n");

    fprintf(list->files.visDump, "\trankdir=LR;\n");
    fprintf(list->files.visDump, "\tbgcolor=\"#f8fff8\";\n");


    fprintf(list->files.visDump,
        "\tnode%0.3d [fontname=\"SF Pro\"; shape=Mrecord; style=filled; fillcolor=\"#e6f2ff\";label = \" { %0.3d } | { data = %3.0lld } | { next = %lld } | { prev = %lld } \"];\n",
        0, 0, list->data[0], list->next[0], list->prev[0]
    );

    for (int64_t i = 0; i < list->size; i++){
        //last modified
        if (i == list->lastModified){
            if (list->prev[i] != -1){
                fprintf(list->files.visDump,
                    "\tnode%0.3lld [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#56A200\"; fillcolor=\"#e1f9c6\";label = \" { %0.3lld } | { data = %3.0lld } | { next = %lld } | { prev = %lld } \"];\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
            }
            else{
                fprintf(list->files.visDump,
                    "\tnode%0.3lld [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#FF2B2E\"; fillcolor=\"#F5A1A2\";label = \" { %0.3lld } | { data = %3.0lld } | { next = %lld } | { prev = %lld } \"];\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
            }
        }

        //free
        else if (list->prev[i] == -1){
            fprintf(list->files.visDump,
                    "\tnode%0.3lld [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#01B5AE\" ;fillcolor=\"#C6F9F7\";label = \" { %0.3lld } | { data = %3.0lld } | { next = %lld } | { prev = %lld } \"];\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }

        //elems
        else
        {
            fprintf(list->files.visDump,
                    "\tnode%0.3lld [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#58A8FF\" fillcolor=\"#d5e9ff\";label = \" { %0.3lld } | { data = %3.0lld } | { next = %lld } | { prev = %lld } \"];\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }

        //weight 1000
        fprintf(list->files.visDump,
        "\tnode%0.3lld -> node%0.3lld [ penwidth=3; weight=1000; style=\"invis\" ];\n",
        i, (i + 1) % 16
        );

        //arrows

        if (list->prev[list->next[i]] == i && list->next[list->prev[i]] == i){
            fprintf(list->files.visDump,
                "\tnode%0.3lld -> node%0.3lld [ penwidth=3; dir=both; color=\"#5ab12f\"; constraint = false; style=\"bold\"];\n",
                i, list->next[i]
            );

        }

        else{
            //next
            if (list->prev[i] != -1){
                fprintf(list->files.visDump,
                    "\tnode%0.3lld -> node%0.3lld [ penwidth=3; color=\"#fd4381\"; constraint = false; style=\"bold\"];\n",
                    i, list->next[i]
                );
            }

            else{
                fprintf(list->files.visDump,
                    "\tnode%0.3lld -> node%0.3lld [ penwidth=3; color=\"#01E5EE\"; constraint = false; style=\"bold\"];\n",
                    i, list->next[i]
                );
            }
            //next

            //prev
            if (list->prev[i] != -1 && list->next[list->prev[i]] != -1){
                fprintf(list->files.visDump,
                    "\tnode%0.3lld -> node%0.3lld [ penwidth=3 color=\"#3474f5\"; style=\"bold\" ];\n",
                    i, list->prev[i]
                );


            }
            //prev
        }
        fprintf(list->files.visDump, "\n");
    }

    fprintf(list->files.visDump, "\n");

    fprintf(list->files.visDump,
        "\theader [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#58A8FF\"; fillcolor=\"#e6f2ff\"; label = \" { head = %lld | tail = %lld | numElems = %lld | free = %lld} \"]\n"
        "\t{ rank = same; \"header\"; \"node000\"; }\n"
        "\theader -> node%0.3lld [ penwidth=3; color=\"#01E5EE\"; constraint = false; style=\"bold\"];\n",
        list->next[0], list->prev[0], list->numElem, list->free, list->free
    );

    fprintf(list->files.visDump, "}\n");

    fclose(list->files.visDump);

    DoDot(list);
    HTMLGenerateBody(list);

    return OK;
}

/*==================================================================================*/

static int DoDot(list_t* list){
    char command[100]   = {};
    char out[100]       = {};

    const char* startOut= "./bin/png/output";
    const char* endOut  = ".png";

    snprintf(out, 100, "%s%llu%s", startOut, list->numDump, endOut);
    snprintf(command, 100, "dot -Tpng %s > %s", list->files.visDumpName, out);
    system(command);

    list->numDump++;
    return OK;
}

/*==================================================================================*/
static int HTMLGenerateHead(list_t* list){
    fprintf(list->files.htmlDump, "<html>\n");

    fprintf(list->files.htmlDump, "<head>\n");
    fprintf(list->files.htmlDump, "</head>\n");

    fprintf(list->files.htmlDump, "<body style=\"background-color:#f8fff8;\">\n");

    return OK;
}

static int HTMLGenerateBody(list_t* list){
    char* lastOper = "meow";
    GetLastOperation(list, &lastOper);

    fprintf(list->files.htmlDump, "<div style=\"text-align: center;\">\n");

    fprintf(list->files.htmlDump, "\t<h2 style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Dump: %llu</h2>\n", list->numDump);
    fprintf(list->files.htmlDump, "\t<p style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Last operation: %s</p>\n", lastOper);
    fprintf(list->files.htmlDump, "\t<img src=\"./bin/png/output%llu.png\">\n\t<br>\n\t<br>\n\t<br>\n", list->numDump - 1);

    fprintf(list->files.htmlDump, "</div>\n");

    return OK;
}

int HTMLDumpGenerate(list_t* list){

    fprintf(list->files.htmlDump, "</body>\n");
    fprintf(list->files.htmlDump, "</html>\n");

    return OK;
}

static int GetLastOperation(list_t* list, char** lastOper){
    switch(list->lastOperation){
        case CTOR:{
            *lastOper = "ListCtor";
            break;
        }

        case DTOR:{
            *lastOper = "ListDtor";
            break;
        }

        case ADD_START:{
            *lastOper = "ListAddStart";
            break;
        }

        case ADD_END:{
            *lastOper = "ListAddEnd";
            break;
        }

        case ADD_AFTER:{
            *lastOper = "ListAddAfter";
            break;
        }

        case ADD_BEFORE:{
            *lastOper = "ListAddBefore";
            break;
        }

        case REM_START:{
            *lastOper = "ListRemoveStart";
            break;
        }

        case REM_END:{
            *lastOper = "ListRemoveEnd";
            break;
        }

        case REM_AFTER:{
            *lastOper = "ListRemoveAfter";
            break;
        }

        case REM_BEFORE:{
            *lastOper = "ListRemoveBefore";
            break;
        }

        case FIND_POS:{
            *lastOper = "ListFindPos";
            break;
        }

        case GETBY_POS:{
            *lastOper = "ListGetByPos";
            break;
        }

        case GET_START:{
            *lastOper = "ListGetStart";
            break;
        }

        case GET_END:{
            *lastOper = "ListGetEnd";
            break;
        }

        case GET_AFTER:{
            *lastOper = "ListGetAfter";
            break;
        }

        case GET_BEFORE:{
            *lastOper = "ListGetBefore";
            break;
        }

        case DEL:{
            *lastOper = "ListDel";
            break;
        }

        default:{
            *lastOper = "Unknown operation";
            break;
        }
    }

    return OK;
}
/*==================================================================================*/

static int ListVerify(list_t* list){
    if (!list) return NULL_ERR;

    if (!list->data || !list->next || !list->prev) return NULL_ERR;

    if (list->numElem > list->size) return OVERFLOW_ERR;

    for (int64_t i = list->next[0], ctr = 0; i != 0 && ctr < list->numElem; i = list->next[i], ctr++){
        if (i != list->next[list->prev[i]]) return LINK_ERR;
    }

    for (int64_t i = list->prev[0], ctr = 0; i != 0 && ctr < list->numElem; i = list->prev[i], ctr++){
        if (i != list->prev[list->next[i]]) return LINK_ERR;
    }

    return OK;
}

/*==================================================================================*/

int ListAddAfter(list_t* list, data_t dataElem, uint64_t param){
    if (ListVerify(list)) return ERR;
    //verify list contains param
    int64_t temp2 = list->next[list->free];

    list->data[list->free] = dataElem;

    list->prev[list->next[list->free]] = -1;
    list->prev[list->free] = param;
    list->prev[list->next[param]] = list->free;

    int64_t temp = list->next[param];
                   list->next[param] = list->free;
                            list->next[list->free] = temp;

    list->free = temp2;

    list->lastModified = list->next[param];
    list->numElem++;

    list->lastOperation = ADD_AFTER;
    return OK;
}

/*==================================================================================*/

int ListAddBefore(list_t* list, data_t dataElem, uint64_t param){
    if (ListVerify(list)) return ERR;

    ListAddAfter(list, dataElem, list->prev[param]);

    list->lastOperation = ADD_BEFORE;
    return OK;
}

/*==================================================================================*/

int ListAddStart(list_t* list, data_t dataElem){
    if (ListVerify(list)) return ERR;

    ListAddAfter(list, dataElem, 0);

    list->lastOperation = ADD_START;
    return OK;
}

/*==================================================================================*/

int ListAddEnd(list_t* list, data_t dataElem){
    if (ListVerify(list)) return ERR;

    ListAddAfter(list, dataElem, list->prev[0]);

    list->lastOperation = ADD_END;
    return OK;
}

/*==================================================================================*/

int ListRemoveAfter(list_t* list, uint64_t param){
    if (ListVerify(list)) return ERR;
    //verify list contains param
    list->data[list->next[param]] = POISON;

    uint64_t nextEl = list->next[list->next[param]];
    uint64_t delEl  = list->next[param];

    list->prev[list->free]  = -1;
    list->next[delEl]       = list->free;
    list->prev[delEl]       = -1;

    list->free = delEl;
    list->next[param]   = nextEl;
    list->prev[nextEl]  = param;


    list->lastModified = list->free;
    list->numElem--;

    list->lastOperation = REM_AFTER;
    return OK;
}

/*==================================================================================*/

int ListRemoveBefore(list_t* list, uint64_t param){
    if (ListVerify(list)) return ERR;

    ListRemoveAfter(list, list->prev[list->prev[param]]);

    list->lastOperation = REM_BEFORE;
    return OK;
}

/*==================================================================================*/

int ListRemoveStart(list_t* list){
    if (ListVerify(list)) return ERR;

    ListRemoveAfter(list, 0);

    list->lastOperation = REM_START;
    return OK;
}

/*==================================================================================*/

int ListRemoveEnd(list_t* list){
    if (ListVerify(list)) return ERR;

    ListRemoveBefore(list, 0);

    list->lastOperation = REM_END;
    return OK;
}

/*==================================================================================*/

int ListFindPos(list_t* list, data_t elem, uint64_t* retValue){
    if (ListVerify(list)) return ERR;

    for (int64_t i = list->next[0]; i != 0; i = list->next[i]){
        if (elem == list->data[i]){
            *retValue = i;

            list->lastOperation = FIND_POS;
            return OK;
        }
    }

    *retValue = 0;

    list->lastOperation = FIND_POS;
    return OK;
}

/*==================================================================================*/

int ListGetByPos(list_t* list, uint64_t index, data_t** retValue){
    if (ListVerify(list)) return ERR;

    index--;
    if (index > list->numElem || index < 0) return INDEX_ERR;

    for (int64_t i = list->next[0], counter = 0; i != 0; i = list->next[i], counter++){
        if (counter == index){
            *retValue = list->data + i;
            break;
        }
    }

    list->lastOperation = GETBY_POS;
    return OK;
}

/*==================================================================================*/

int ListDel(list_t* list){
    if (ListVerify(list)) return ERR;

    list->numElem = 0;
    list->free  = 1;

    FillNext(list);
    FillPrev(list);
    FillData(list);

    list->lastModified = -1;
    list->lastOperation = DEL;
    return OK;
}

/*==================================================================================*/

int ListGetAfter(list_t* list, uint64_t param, data_t** retValue){
    if (ListVerify(list)) return ERR;

    *retValue = list->data + list->next[param];

    list->lastOperation = GET_AFTER;
    return OK;
}

/*==================================================================================*/

int ListGetBefore(list_t* list, uint64_t param, data_t** retValue){
    if (ListVerify(list)) return ERR;

    *retValue = list->data + list->prev[param];

    list->lastOperation = GET_BEFORE;
    return OK;
}

/*==================================================================================*/

int ListGetStart(list_t* list, data_t** retValue){
    if (ListVerify(list)) return ERR;

    *retValue = list->data + list->next[0];

    list->lastOperation = GET_START;
    return OK;
}

/*==================================================================================*/

int ListGetEnd(list_t* list, data_t** retValue){
    if (ListVerify(list)) return ERR;

    *retValue = list->data + list->prev[0];

    list->lastOperation = GET_END;
    return OK;
}
