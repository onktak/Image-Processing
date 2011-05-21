
typedef struct {
    int val;
    void *next; // just a  node pointer
    void *itemPtr; // just an item pointer, deal with the back-forth reference.
} node;

typedef struct {
    node *hd, *tl;
} Item;

long makeset(int a);
long find (long a);
void Union (long s1, long s2);

long head (long a);
int nodeValue(long a);
long nextNode (long a);


