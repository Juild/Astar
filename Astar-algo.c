//
//  main.c
//  Algorithm
//
//  Created by Jc on 17/11/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
static FILE *fin = NULL;
static Node *nodes = NULL;
static unsigned long *allsuccessors = NULL;
static unsigned long nnodes = 23895681;
static unsigned long ntotnsucc = 0;
static double R = 6371e3;
void read_binary_file()
{
    char name[] = "graph.bin";
    
    if ((fin = fopen (name, "r")) == NULL)
    ExitError("the data file does not exist or cannot be opened", 11);
    /* Global data −−− header */
    if( fread(&nnodes, sizeof(unsigned long), 1, fin) +
    fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 ) ExitError("when reading the header of the binary data file", 12);
    /* getting memory for all data */
    if((nodes = (Node *) malloc(nnodes*sizeof(Node))) == NULL)
                ExitError("when allocating memory for the nodes vector", 13);
    if( (
    allsuccessors = (unsigned long *) malloc(ntotnsucc*sizeof(unsigned long)) ) == NULL)
                ExitError("when allocating memory for the edges vector", 15);
    /* Reading all data from file */
    unsigned long x = fread(nodes, sizeof(Node), nnodes, fin);
    printf("%lu\n",x);
    if( x!= nnodes )
    ExitError("when reading nodes from the binary data file", 17); if(fread(allsuccessors, sizeof(unsigned long), ntotnsucc, fin) != ntotnsucc)
                ExitError("when reading sucessors from the binary data file", 18);
       fclose(fin);
    /* Setting pointers to successors */
    for(int i = 0; i < nnodes; i++)
        if(nodes[i].nsucc)
        {
            nodes[i].successors = allsuccessors;
            allsuccessors += nodes[i].nsucc;
        }
}
double h(double lat1, double lat2, double lon1, double lon2)
{
    const double phi = lat1 * M_PI/180;
    const double phi2 = lat2 * M_PI/180;
    const double lambda = (lon2-lon1) * M_PI/180;
//    double d = acos( sin(phi)*sin(phi2) + cos(phi)*cos(phi2) *cos(lambda) ) * R;
    const double dphi = (lat2-lat1) * M_PI/180;

    const double a = sin(dphi/2) * sin(dphi/2) +
              cos(phi) * cos(phi2) *
              sin(lambda/2) * sin(lambda/2);
    const double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c; // in metres
    return d;
}
void printstatus(Queue *Q) // for debug purposes
{
    Node *iter;
    iter = Q ->start;
    puts("--------STATUS-------");
    while(iter != NULL)
    {
        printf("%f -> ", iter -> g + iter ->h);
        iter = iter  -> next;
    }
    puts("---------------------");
}
void rm(Queue *Q, Node *node)
{
    Node *iter = Q ->start;
    while(true)
    {
        if(iter ->next ->id == node ->id)
        {
            iter ->next = node ->next;
            break;
        }
        iter = iter ->next;
    }
}
void add(Queue *Q, Node **node)
{
    Node *iter = Q -> start;
    (*node) ->next = NULL;
    while(true)
    {
        if(iter ->next != NULL)
        {
//            puts("if");
            if( (iter -> next -> g + iter -> next -> h) >= ((*node)->g + (*node)->h))
            {
                (*node) ->next = iter -> next;
                iter ->next = (*node);
                break;
            }
        }else
        {
//            puts("else");
            iter ->next = (*node);
            break;
        }
        iter = iter ->next;
    }
}
void init_gh(Node *arr, unsigned long len)
{
    for(unsigned long i = 0; i < len; ++i)
    {
        arr[i].g = 0; arr[i].h = 0; arr[i].parent = NULL; arr[i].next = NULL; arr[i].which_queue = NONE;
        
    }
}

void printpath(Node *node) // for debug purposes
{
    Node *iter;
    iter = node;
    int n = 0;
    while (iter ->parent != NULL) {
        ++n;
        iter = iter ->parent;
    }
    printf("Nodes: %d\n", n);
    printf("Distance: %f\n", node ->g);
}
int main(int argc, const char * argv[])
{
    read_binary_file();
    /* We start the Astar algorithm */
    
    //Initialize status
    Node *root = binary_search(240949599, nodes, nnodes);
    Node *goal = binary_search(195977239, nodes, nnodes);
    // init g and h for all nodes
    puts("Initializing nodes");
    init_gh(nodes, nnodes);
    puts("Nodes initialized");
    root->g = 0;
    root->h = h(root ->lat, goal->lat, root ->lon, goal ->lon);
    Queue Q;
    Q.start = root;
    Q.len = 1;
    printf("Root id is: %lu\n", Q.start ->id); // Great!!
    // Now the list is initialized with the root node
    int i;
    Node *node_successor = NULL;
    double cost  = 0;
    int iter = 0;
    printf("nsucc: %d\n", binary_search(240950216, nodes, nnodes) ->nsucc);
    while(true)
    {
//        printf("Node id: %lu, nsucc: %d\n", Q.start ->id, Q.start ->nsucc);
        if(Q.start ->id == goal ->id)
        {
            printf("Found it ID: %lu\n", Q.start ->id);
            break;
        }
        for(i = 0; i < Q.start ->nsucc; ++i)
        { 
//            printf("%i\n", i);
            node_successor = binary_search(Q.start ->successors[i], nodes, nnodes);
            cost =  Q.start ->g  +  h(Q.start->lat, node_successor ->lat,  Q.start ->lon, node_successor ->lon);
//            printf("Successor: %lu, cost: %f\n", node_successor ->id, cost);
            if(node_successor -> which_queue == OPEN )
            {
//                puts("if");
                if(node_successor ->g <= cost) continue;
                else
                {
                    rm(&Q, node_successor);
                    node_successor ->g = cost;
                    add(&Q, &node_successor);
                    node_successor->parent = Q.start;
                    iter = -1;
                    continue;
                }
            }else if(node_successor -> which_queue == CLOSED )
            {
//                puts("elif");
                if(node_successor ->g <= cost) continue;
                node_successor -> which_queue = OPEN;
                node_successor ->g = cost;
                add(&Q, &node_successor);
                ++Q.len;
            }else
            {
//                puts("else");
                node_successor->h = h(node_successor->lat , goal ->lat, node_successor ->lon, goal ->lon);
                node_successor ->which_queue = OPEN;
                node_successor ->g = cost;
                add(&Q, &node_successor);
                ++Q.len;
                
            }
            node_successor->parent = Q.start;
        }
        Q.start ->which_queue = CLOSED;
        Q.start = Q.start ->next;
//        printf("h: %f\n", Q.start ->h);
//        ++iter;
//        if(iter == -1){
//            printstatus(&Q);
//            exit(11);
//        }
//        if(iter % 10000 == 0)
//        {
//            printf("ID: %lu\n", Q.start ->id);
//            printf("len: %lu\n", Q.len);
//        }
//        printstatus(&Q);
//        printf("ID = %lu\n", Q.start ->id);
//        if(iter == 10) break;
        
        if(Q.start == NULL) break;
        
    }
    printpath(goal);
    
    return 0;
}
