//
//  main.c
//  Astar
//
//  Created by Jc on 17/11/20.
//

#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Functions
char *skip(char **buff, char* field, char *delim, unsigned long n) // function to skip fields
{
    for(int i = 0; i < n; ++i) {field = strsep(buff, delim);}
    return field;
}


void join(Node *arr, unsigned long len, unsigned long A, unsigned long B, char *one_way)
{
    Node *a = NULL, *b = NULL;
    // Now we have to search for the nodes A and (maybe) B
    a = binary_search(A, arr, len);    
    if(a != NULL)
    {
        b = binary_search(B, arr, len);
        if(b != NULL)
        {
            a ->nsucc += 1;
            if( a ->nsucc > 2)
                a ->successors = (unsigned long*)realloc(a ->successors, (a->nsucc)*sizeof(unsigned long));
            if((a ->successors) == NULL) puts("fail allocating succesors");
            a ->successors[a->nsucc - 1] = b -> id;
            
            if(*one_way == '\0')
            {
                b ->nsucc += 1;
                if( b ->nsucc > 2)
                    b ->successors = (unsigned long*)realloc(b ->successors, (b->nsucc)*sizeof(unsigned long));
                if((b ->successors) == NULL) puts("fail allocating succesors");
                b ->successors[b ->nsucc - 1] = a -> id;
                
            }
        }
    }
}

void read_file(FILE *file, Node *nodes, unsigned long nnodes, char delim)
{
    // Read the file line by line
    char *line_buff = NULL;
    unsigned long line_buff_size = 0;
    long line_size = 0;
    char *field = NULL;
    char *one_way = NULL;
    unsigned long key_A = 0;
    unsigned long node_index = 0;
    unsigned long way_index = 0;
    unsigned long *way_arr = NULL;
    unsigned int way_arr_len = 0;
    // This is an auxiliary pointer the functions strtoul and strtod need when converting the string
    char *tmp = NULL;
    for(int i = 0; i < 3; ++i) line_size = getline(&line_buff, &line_buff_size, file); // get the first 3 lines and do nothing
    while(true)
    {
        line_size = getline(&line_buff, &line_buff_size, file);
        if(line_size == -1) break; // There is no line it means we arrived to the end of the file.

        field = strsep(&line_buff, &delim); // node, way or relation
        if(*field == 'r')// if it's a relation we are not interested
        {
            break;
        }
        switch (*field)
        {
            case 'w': // if it's a way
                one_way = skip(&line_buff, field, &delim, 7); // this is the @one_way field
                tmp = skip(&line_buff, field, &delim, 1); // maxspeed
                
                while(line_buff != NULL)
                {
                    key_A = strtoul(strsep(&line_buff, &delim), &tmp, 10);
                    ++way_arr_len;
                    way_arr = (unsigned long *)realloc(way_arr, way_arr_len*sizeof(unsigned long));
                    way_arr[way_arr_len -1] = key_A;
                }
                for(int i = 0, j = 1; j < way_arr_len; ++i, ++j)
                    join(nodes, nnodes, way_arr[i], way_arr[j], one_way);
                free(way_arr);
                way_arr_len = 0;
                way_arr = NULL;
                ++way_index;
                break;
            default: // default we process it as a node
                field = strsep(&line_buff, &delim); // this is the @id, because it's the consecutive field
                // We set the @id of the node
                nodes[node_index].id = strtoul(field, &tmp, 10);
                field = strsep(&line_buff, &delim); // this is the @name, "
                // We set the @name of the node
//                strcpy(nodes[node_index].name, field);
                field = skip(&line_buff, field, &delim, 7); // skip the next six fields, we don't care, we get the latitude
                // We set the latitude of the node
                nodes[node_index].lat = strtod(field, &tmp);
                field = strsep(&line_buff, &delim); // This is the longitude
                // Same for the longitude
                nodes[node_index].lon = strtod(field, &tmp);
                nodes[node_index].nsucc = 0; // this is important when building the graph
                // We're done we don't care about any other thing in the node line.
                ++node_index; // We've processed one more node, so we change the index to the following element of the vector nodes.
                break;
        }
        line_buff = NULL;
        
    }

    printf("Nodes, ways: %lu, %lu\n", node_index, way_index);


}
void init_ptrs(Node *arr, unsigned long len)
{
    for(int i = 0; i < len; ++i)
    {
        arr[i].successors = NULL;
        arr[i].successors = (unsigned long *)malloc(2*sizeof(unsigned long));
    }
}

int main(int argc, char *argv[])
{
    FILE *file;
    unsigned long nnodes = 23895681UL;
    Node *nodes;
    
    if((nodes = (Node *)malloc(nnodes*sizeof(Node))) == NULL)
    {
       puts("when allocating memory for the nodes vector");
       return 1;
    }
    init_ptrs(nodes, nnodes);
    file = fopen("spain.csv", "r");
    if(file == NULL) {puts("No file!"); return 1;}
    read_file(file, nodes, nnodes, '|');
    fclose(file);

    // Declare binary file
    FILE *fin = NULL;
    char name[] = "graph.bin";
    int i;
    /* Computing the total number of successors */
    unsigned long ntotnsucc = 0;
    for(int i = 0; i < nnodes; ++i) ntotnsucc += nodes[i].nsucc;
    /* Setting the name of the binary file */
//    strcpy(name, argv[1]);
    strcpy(strrchr(name, '.'), ".bin");
    if ((fin = fopen (name, "wb")) == NULL)
    {
        puts("the output binary data file cannot be opened");
        return 1;
    }
    /* Global data −−− header */
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin) +
    fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
    {
        puts("when initializing the output binary data file");
        return 1;
    }
    /* Writing all nodes */
    if( fwrite(nodes, sizeof(Node), nnodes, fin) != nnodes )
    {
        puts("when writing nodes to the binary data file");
        return 1;
    }
    /* Writing sucessors in blocks */
    for(i=0; i < nnodes; i++) if(nodes[i].nsucc)
    {
        if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin) != nodes[i].nsucc )
        {
                puts("when writing edges to the output binary data file");
            return 1;
        }
    }
    fclose(fin);
    free(nodes);

    return 0;
}
