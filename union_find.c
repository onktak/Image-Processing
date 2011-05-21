
#include <stdio.h>
#include <stdlib.h>
#include "union_find.h"

long makeset (int a) {

    Item *newSet = (Item*)malloc(sizeof(Item));
    newSet->hd = (node*)malloc(sizeof(node));
    newSet->tl = newSet->hd;
    node *shd = newSet->hd;
    shd->val = a;
    shd->itemPtr = newSet;
    shd->next = 0;
    return (long) newSet;
}

long find (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->itemPtr);
}
long head (long a) {
    Item *setPtr = (Item*) a;
    return (long)(setPtr->hd);
}
int nodeValue (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->val);
}
long nextNode (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->next);
}

void Union (long s1, long s2) {

    //change head pointers in Set s2
    Item *set2 = (Item*) s2;
    node *cur = set2->hd;

    Item *set1 = (Item*) s1;

    while (cur != 0) {
        cur->itemPtr = set1;
        cur = cur->next;
    }
    //join the tail of the set to head of the input set
    (set1->tl)->next = set2->hd;
    set1->tl = set2->tl;
    free(set2);

}
