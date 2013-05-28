#include "battle.h"
#include "unit.h"
#include "atlantis.h"

#include <quicklist.h>
#include <stdlib.h>

void free_battle(battle * b) {
    int i;
    ql_foreach(b->events, free);
    ql_free(b->events);
    /*
    ql_foreach(b->units[0], (void (*)(void*))free_unit);
    ql_free(b->units[0]);
    ql_foreach(b->units[1], (void (*)(void*))free_unit);
    ql_free(b->units[1]);
    freestrlist(&b->events);
    */
    for (i=0;i!=2;++i) {
        while (b->units[i]) {
            unit * u = b->units[i];
            b->units[i] = u->next;
            free_unit(u);
        }
    }
    free(b);
}