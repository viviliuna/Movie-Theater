/*
THIS CODE WAS WRITTEN BY:
Viviana Luna
COP 3502C Assignment 6
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// globals
#define MAXLEN 19
#define TABLESIZE 300007
int MONEY = 100000;
int COMPLEXITY = 0;

// structs here
typedef struct item {
  char name[MAXLEN + 1];
  int quantity;
  int saleprice;
} item;

// this one is for linked lists
typedef struct node {
  item *iPtr;
  struct node *next;
} node;

typedef struct hashtable {
  node **lists;
  int size;
} hashtable;

// function prototypes here: linked lists
void print(node *front);
node *insert_front(node *front, char word[], int quantity);
node *delete (node *front, char word[]);
void dellist(node *p);
int search(node *front, char word[]);
node *findItemList(hashtable *h, char word[]);
node *findItem(node *front, char word[]);

// node *createNode(node *p, char word[]);
// node *createNode(char word[]);
node *createNode(char word[], int quantity, int saleprice);

// function prototypes here: hash tables
// pass in something other than a hash table for this first func
// you can malloc for it in the function
void initTable(hashtable *h);
int hashvalue(char word[]);
void insertTable(hashtable *h, char word[], int quantity);
int searchTable(hashtable *h, char word[]);
void deleteTable(hashtable *h, char word[]);
void killTable(hashtable *h);
// need function to calculate complexity based on search

// function prototypes here: general
void buy(hashtable *h, char word[], int quantity, int price);
void sell(hashtable *h, char word[], int quantity);
void change_price(hashtable *h, char word[], int new_price);
void freeAll(hashtable *h);

int main() {

  // declaring variables
  int n, quantity, totalprice, new_price;
  char command[MAXLEN + 1], word[MAXLEN + 1];
  // wait im not sure if im supposed to do this in main or just globally
  COMPLEXITY = 0;

  // scanf for n
  scanf("%d", &n);

  // create/ malloc for hash table here
  // redo this omg ToT
  hashtable *new_hashtable = (hashtable *)malloc(sizeof(hashtable));

  // is a mem check necessary?
  //  if(new_hashtable == NULL){

  // }

  // send to initTable to start the hash table
  initTable(new_hashtable);

  // scan for command
  for (int i = 0; i < n; i++) {
    scanf("%s", command);

    // command 1: buy
    if (strcmp(command, "buy") == 0) {
      // scanf for the name and points
      scanf("%s", word);
      scanf("%d", &quantity);
      scanf("%d", &totalprice);

      // send to buy function
      buy(new_hashtable, word, quantity, totalprice);

    }
    // command 2: sell
    else if (strcmp(command, "sell") == 0) {
      scanf("%s", word);
      scanf("%d", &quantity);

      // send to sub function
      sell(new_hashtable, word, quantity);
    }

    // command 3: change_price
    else {
      scanf("%s", word);
      scanf("%d", &new_price);

      // send to change_price function
      change_price(new_hashtable, word, new_price);
    }
  }

  // print total remaining money after all transactions
  printf("%d\n", MONEY);

  // print total complexity too
  printf("%d\n", COMPLEXITY);

  // freeing memory after everything
  killTable(new_hashtable);
  //free(new_hashtable);

  return 0;
}

// from pdf
int hashfunc(char *word, int size) {
  int len = strlen(word);
  int res = 0;
  for (int i = 0; i < len; i++)
    res = (1151 * res + (word[i] - 'a')) % size;
  return res;
}

// this might be a void function
// command 1: buy
void buy(hashtable *h, char word[], int quantity, int price) {
  // if statements to check the 3 conditions
  // if item is in list, execute usual calculations
  node *itemfind = findItemList(h, word);
  
  if (itemfind != NULL) {
    // search the linked list for the item
    // update quantity here
    itemfind->iPtr->quantity += quantity;

    // update price here
    MONEY -= price;

    // we need to implement a failsafe for if the money drops below 0
    // the system needs to either throw an error or return "false" or something
    // if MONEY <= 0 { return "EXIT" or something}

    printf("%s %d %d\n", word, itemfind->iPtr->quantity, MONEY);

  }
  // if it isnt in the list, add the item to the front of the table, then do
  // usual calc dont forget to update money, that will be part of the
  // calculations
  else {
    // insert the item into the list
    insertTable(h, word, quantity);

    // confirm search for item
    // node *item2 = findItemList(h, word);

    // update quantity here
    // theres a seg fault here :(
    // item2->iPtr->quantity += quantity;
    
    int hash = hashfunc(word, TABLESIZE);

    // update price here
    MONEY -= price;

    printf("%s %d %d\n", word, h->lists[hash]->iPtr->quantity, MONEY);
  }
  // return 0;
}

// command 2: sell
void sell(hashtable *h, char word[], int quantity) {
  // this time, item is guaranteed to be in the inventory
  // you can keep an if statement in to check/ prevent errors
  //if (searchTable(h, word) == 1) {
    node *current_item = findItemList(h, word);

    if (current_item->iPtr->quantity >= quantity) {
      // decreasing the quantity as sold
      current_item->iPtr->quantity -= quantity;

      MONEY += current_item->iPtr->saleprice * quantity;

      printf("%s %d %d\n", word, current_item->iPtr->quantity, MONEY);
    }
    // this means there's not enough to sell as in current_item quan < quan
    // pdf says to just sell all of the stock
    else {
        MONEY += current_item->iPtr->saleprice * current_item->iPtr->quantity;
      current_item->iPtr->quantity = 0;
    //   MONEY += current_item->iPtr->saleprice * current_item->iPtr->quantity;


      printf("%s %d %d\n", word, current_item->iPtr->quantity, MONEY);
    }
  //}
  // means item is not in inventory
  //  else{

  // }

  // return 0;
}

// command 3: change_price
void change_price(hashtable *h, char word[], int new_price) {
  // also guaranteed to be in inventory
  //if (searchTable(h, word) == 1) {
    node *current_item = findItemList(h, word);

    current_item->iPtr->saleprice = new_price;
  //}
  // item not in inventory
  //  else{
  // printf("Not in list\n");
  // }

  // update the item's price
  // return 0;
}

// Post-condition: Initializes the struct pointed to by h to be a valid empty
// hash table.
void initTable(hashtable *h) {
  h->lists = (node **)malloc(TABLESIZE * sizeof(node *));
  h->size = TABLESIZE;
  // Set all of these to NULL!
//   for (int i = 0; i < TABLESIZE; i++) {
  for (int i = 0; i < h->size; i++) {
    h->lists[i] = NULL;
  }
}

void killTable(hashtable *h) {
  int i;
  // Set all of these to NULL!
  for (i = 0; i < TABLESIZE; i++) {
    dellist(h->lists[i]);
  }
  free(h->lists);
  free(h);
}

// Post-condition: Calculates a hash value for word.
int hashvalue(char word[]) {
  int i, sum = 0;
  // Basically represents the value of word in base 128 (according to ascii
  // values) and returns its value mod the TABLE_SIZE.
  for (i = 0; i < strlen(word); i++)
    sum = (128 * sum + (int)(word[i])) % TABLESIZE;
  return sum;
}

// Pre-condition: h points to a valid hash table.
// Post-condition: word will be inserted into the table h.
void insertTable(hashtable *h, char word[], int quantity) {
  int hashval;
  hashval = hashfunc(word, TABLESIZE);
  h->lists[hashval] = insert_front(h->lists[hashval], word, quantity);
}

// searching linked list for the item
node *findItemList(hashtable *h, char word[]) {
  int hashval;
  hashval = hashfunc(word, TABLESIZE);
  return findItem(h->lists[hashval], word);
}

// traverse the list to search for item in list
node *findItem(node *front, char *word) {
  while (front != NULL) {
      COMPLEXITY += 1;
    if (strcmp(front->iPtr->name, word) == 0)
      return front;
    front = front->next;
  }
  // else, item was not foud
  return NULL;
}

// Pre-condition: h points to a valid hash table.
// Post-condition: 1 will be returned iff word is stored in the table pointed to
// by h. Otherwise, 0 is returned.
int searchTable(hashtable *h, char word[]) {
  int hashval;
  hashval = hashvalue(word);
  return search(h->lists[hashval], word);
}

// Pre-condition: front points to the head of a linked list structure.
// Post-condition: if val is stored in the list, 1 will be returned,
// otherwise 0 will be.
int search(node *front, char word[]) {
  while (front != NULL) {
    COMPLEXITY += 1;
    if (strcmp(front->iPtr->name, word) == 0)
      return 1;
    front = front->next;
  }
  // decrease complexity if found?
  // is this necessary?
  COMPLEXITY -= 1;
  return 0;
}

// Pre-condition: h points to a valid hash table.
// Post-condition: deletes word from the table pointed to by h, if word is
// stored here. If not, no change is made to the table pointed
// to by h.
void deleteTable(hashtable *h, char word[]) {
  int hashval;
  hashval = hashvalue(word);
  h->lists[hashval] = delete (h->lists[hashval], word);
}

// function to create a node in the linked list
//  node *createNode(node *p, char word[]){
// node *createNode(char word[]) {
node *createNode(char word[], int quantity, int saleprice) {
  // first malloc for item struct, can't point to iPtr if it doesnt exist yet
  item *new = (item *)malloc(sizeof(item));
    node *temp = (node *)malloc(sizeof(node));


  // Create temp node and set next = NULL.
  // node *temp = (node*)malloc(sizeof(node));
  // strcpy(temp->iPtr->name, word);
  strcpy(new->name, word);

//   new->quantity = 0;
//   new->saleprice = 0;
  new->quantity = quantity;
  new->saleprice = saleprice;


//   node *temp = (node *)malloc(sizeof(node));
  temp->iPtr = new;
  temp->next = NULL;
  // temp->next = NULL;

  // if(p == NULL){
  //     p = temp;
  // }
  // else{
  //     temp->next = NULL;
  //     p = temp;
  // }

  // Return the new node.
  // return p;
  return temp;
}

// Pre-condition: front points to the head of a linked list structure.
// Post-condition: a new node storing num will be inserted to the front of
// this list and a pointer to this node will be returned.
// isnt there supposed to be a 4th parameter, saleprice?
node *insert_front(node *front, char word[], int quantity) {
  node *temp;
    COMPLEXITY += 1;
  // Create temp node and link it to front.
  //temp = (node *)malloc(sizeof(node));
  temp = createNode(word, quantity, 0);
  //temp->iPtr->quantity = quantity;
  // strcpy(temp->iPtr->name, word);
  temp->next = front;

  // Return the new front of the list.
  return temp;
}

// Pre-condition: front points to the head of a linked list structure.
// Post-condition: each value in the list will be printed, in order.
void print(node *front) {
  while (front != NULL) {
    printf("%s ", front->iPtr->name);
    front = front->next;
  }
  printf("\n");
}

// Pre-condition: front points to the head of a linked list structure.
// Post-condition: The first node that contains the value num will be
// removed from the list. A pointer to the front of the
// list will be returned. If no such value is stored,
// the list will remain unchanged.
node *delete (node *front, char word[]) {
  node *temp, *del;
  temp = front;
  // Only need to delete if the list is not null.
  if (temp != NULL) {

    // Take care of the case where first node needs to be deleted.
    if (strcmp(temp->iPtr->name, word) == 0) {
      del = temp->next;
      free(temp);
      return del;
    }

    // Otherwise, loop until you find the node to delete and do so.
    while (temp->next != NULL) {
      if (strcmp(temp->next->iPtr->name, word) == 0) {
        del = temp->next;
        temp->next = temp->next->next;
        free(del);
        return front;
      }
      temp = temp->next;
    }
  }
  return front;
}

// Pre-condition: p points to a linked list.
// Post-condition: All the memory for the linked list pointed to by p is
// freed for use.
void dellist(node *p) {
  node *temp;
  // Recursively free each element in the list.
  if (p != NULL) {
    free(p->iPtr);
    temp = p->next;
    dellist(temp);
    free(p);
  }
}

// free function
void freeAll(hashtable *h) {
  // free linked list hash table
  for (int i = 0; i < TABLESIZE; i++) {
    dellist(h->lists[i]);
  }

  // frees hashtable linked list
  free(h->lists);

  // free the table as a whole
  free(h);
}

/*

  ^----^
 (. w .  )  THE RETURN OF CHEESECAKE!!
<|       |>
 |       |
 U------U

*/


