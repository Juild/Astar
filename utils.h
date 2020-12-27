//
//  utils.h
//  Algorithm
//
//  Created by Jc on 18/11/20.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>
#define true 0 == 0

typedef struct Node
{
    unsigned long id;
    double lat, lon;
    unsigned short nsucc;
    unsigned long *successors;
    struct Node *parent;
    double g, h;
    char which_queue;
    struct Node *next;
}Node;

enum whichQueue {NONE, OPEN, CLOSED};


typedef  struct Queue {
    Node *start;
    unsigned long len;
}Queue;

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss); exit(errcode);
}

Node *binary_search(unsigned long goal, Node *arr, unsigned long len)
{
    unsigned long L = 0, R = len - 1, C = 0;
    while(true)
    {
        C = (R + L)/2;
        if(arr[C].id == goal) return arr + C;
        if( (arr[C].id) < goal) L = C + 1;
        else R = C - 1;
        if(R == C  || L == C){ return NULL; break;}
    }

}
#endif /* utils_h */

