
/******************************************************************
 * PROGRAM: Implementation of Linked-list representation of disjoi-*
 * nt sets in C++ without weighted union optimization. *
 * makeset, find takes O(1), Union takes O(n). Testing *
 * code is in the main method. Implements CLRS Chap 21.1 *
 * AUTHOR: Bo Tian (bt288 at cam.ac.uk) drop me an email if you *
 * have any questions. *
 * LICENSE: Creative Commons Attribution 3.0 Unported *
 * http://creativecommons.org/licenses/by/3.0/ *
 *
 * 
 * Added head, nodeValue and nextNode functions for easy testing of the 
 * data structure build up. 
 *
 *******************************************************************/


#include <iostream>
using namespace std;


template<class T> class ListSet {
    private:
        struct Item;
        struct node {
            int val;
            node *next;
            Item *itemPtr;
        };
        struct Item {
            node *hd, *tl;
        };

    public:
        ListSet() { }
        long makeset(T a);
        long find (long a);
        void Union (long s1, long s2);

        long head (long a);
        int nodeValue(long a);
        long nextNode (long a);
};

template<class T> long ListSet<T>::makeset (T a) {
    Item *newSet = new Item;
    newSet->hd = new node;
    newSet->tl = newSet->hd;
    node *shd = newSet->hd;
    shd->val = a;
    shd->itemPtr = newSet;
    shd->next = 0;
    return (long) newSet;
}

template<class T> long ListSet<T>::find (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->itemPtr);
}
template<class T> long ListSet<T>::head (long a) {
    Item *setPtr = (Item*) a;
    return (long)(setPtr->hd);
}
template<class T> int ListSet<T>::nodeValue (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->val);
}

template<class T> long ListSet<T>::nextNode (long a) {
    node *ptr = (node*)a;
    return (long)(ptr->next);
}

template<class T> void ListSet<T>::Union (long s1, long s2) {

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
    delete set2;

}

