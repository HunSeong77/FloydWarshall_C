#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)
#define INF INT_MAX
#define MAX_LEN 100

typedef struct Cell{
    int weight;
    int parent;
} Cell;

Cell graph[101][101];
int size, source, destination;
int stopover[3];

void init_graph();
void read_size(FILE*);
void read_edges(FILE*);
void floyd();
void print_path_direct();
void print_path_contour();
void print_graph();

int main(void){

    // GET FILE DIRECTORY
    char fileDir[200] = "C:/Codes/C/DSA/shortestPath/graph.txt";
    printf("File Directory : ");
    scanf_s("%s", fileDir, 200);

    // FILE READ
    FILE* fp = fopen(fileDir, "r");
    if(fp == NULL){
        printf("file open Error\n");
        return 0;
    }

    // get size, source, destination and passing route
    read_size(fp);
    printf("\nsource : %d, destination : %d\n", source, destination);
    if(stopover[0] != source) printf("stopovers : %d", stopover[0]);
    if(stopover[1] != source) printf(", %d", stopover[1]);
    if(stopover[2] != source) printf(", %d", stopover[2]);
    printf("\n\n");
    // initialize graph and read edge information
    init_graph();
    read_edges(fp);
    fclose(fp);

    // apply floyd-warshall algorithm first
    floyd();
    // find path
    print_path_direct();
    printf("\n\n");
    print_path_contour();
    return 0;
}


void init_graph()
{
    for(size_t i = 0; i <= size; ++i){
        for(size_t j = 0; j <= size; ++j){
            graph[i][j].weight = INF;
            graph[i][j].parent = -1;
        }
    }
}

void read_size(FILE* f)
{
    char buff[MAX_LEN];
    int i = 0;
    int temp[6] = {0};
    fgets(buff, MAX_LEN, f);
    char* p = strtok(buff, " ");
    while(p){
        temp[i++] = atoi(p);
        p = strtok(NULL, " ");
    }

    size = temp[0]; source = temp[1]; destination = temp[2];
    // if stopover is empty, set stopover source.
    stopover[0] = temp[3] == 0 ? source : temp[3]; 
    stopover[1] = temp[4] == 0 ? source : temp[4];
    stopover[2] = temp[5] == 0 ? source : temp[5];
}

void read_edges(FILE* f)
{
    char str[15];
    for(size_t i = 1; i <= size; ++i)
        for(size_t j = 1; j <= size; ++j){
            fscanf(f, "%s", str);
            graph[i][j].weight = strcmp(str, "INF") == 0 ? INF : atoi(str);
        }
}

void print_graph()
{
    for(size_t i = 1; i <= size; ++i){
        for(size_t j = 1; j <= size; ++j)
            printf("%d ", graph[i][j].weight);
        printf("\n");
    }
}

void floyd()
{
    for(size_t i = 1; i <= size; ++i)
        graph[i][i].weight = 0;
    
    for(size_t k = 1; k <= size; ++k)
        for(size_t i = 1; i <= size; ++i)
            for(size_t j = 1; j <= size; ++j){
                if((long long)graph[i][j].weight > \
                (long long)graph[i][k].weight + graph[k][j].weight){
                    graph[i][j].weight = graph[i][k].weight + graph[k][j].weight;
                    graph[i][j].parent = k;
                }
            }
}

void print_path(int start, int end)
{
    if(start == end) return;
    int parent = graph[start][end].parent;
    if(parent == -1){
        printf("%4d", end);
    }
    else {
        print_path(start, parent);
        print_path(parent, end);
    }
    return;
}

void print_path_direct()
{
    int weight = 0;
    printf("** direct path source to destination**\n");
    if(graph[source][destination].weight == INF){
        printf("path doesn't exist\n");
        return;
    }
    printf("path : %4d", source);
    print_path(source, destination);
    printf("\ncost : %4d", graph[source][destination].weight);
}


int select_contour(){

    long long int cost012 = (long long)graph[source][stopover[0]].weight + graph[stopover[0]][stopover[1]].weight\
    + graph[stopover[1]][stopover[2]].weight + graph[stopover[2]][destination].weight;

    long long int cost021 = (long long)graph[source][stopover[0]].weight + graph[stopover[0]][stopover[2]].weight\
    + graph[stopover[2]][stopover[1]].weight + graph[stopover[1]][destination].weight;

    long long int cost102 = (long long)graph[source][stopover[1]].weight + graph[stopover[1]][stopover[0]].weight\
    + graph[stopover[0]][stopover[2]].weight + graph[stopover[2]][destination].weight;

    long long int cost120 = (long long)graph[source][stopover[1]].weight + graph[stopover[1]][stopover[2]].weight\
    + graph[stopover[2]][stopover[0]].weight + graph[stopover[0]][destination].weight;

    long long int cost201 = (long long)graph[source][stopover[2]].weight + graph[stopover[2]][stopover[0]].weight\
    + graph[stopover[0]][stopover[1]].weight + graph[stopover[1]][destination].weight;

    long long int cost210 = (long long)graph[source][stopover[2]].weight + graph[stopover[2]][stopover[1]].weight\
    + graph[stopover[1]][stopover[0]].weight + graph[stopover[0]][destination].weight;

    long long int cost0 = cost012 < cost021 ? cost012 : cost021;
    long long int cost1 = cost102 < cost120 ? cost102 : cost120;
    long long int cost2 = cost201 < cost210 ? cost201 : cost210;

    if(cost0 <= cost1 && cost0 <= cost2){
        if(cost012 == cost0) return 1;
        else return 2;
    }
    if(cost1 <= cost0 && cost1 <= cost2){
        if(cost102 == cost1) return 3;
        else return 4;
    }
    if(cost201 == cost2) return 5;
    return 6;
}

void print_path_contour()
{
    // if there is no stopover, do not print anything.
    if(stopover[0] == source) return;

    int weight = 0;
    printf("** contour passing stopovers **\n");
    int contour = select_contour();
    int stop[3];
    switch(contour) {
        case 1 : 
            stop[0] = stopover[0]; stop[1] = stopover[1];
            stop[2] = stopover[2]; break;
        case 2 :
            stop[0] = stopover[0]; stop[1] = stopover[2];
            stop[2] = stopover[1]; break;
        case 3 :
            stop[0] = stopover[1]; stop[1] = stopover[0];
            stop[2] = stopover[2]; break;
        case 4 :
            stop[0] = stopover[1]; stop[1] = stopover[2];
            stop[2] = stopover[0]; break;
        case 5 :
            stop[0] = stopover[2]; stop[1] = stopover[0];
            stop[2] = stopover[1]; break;
        case 6 :
            stop[0] = stopover[2]; stop[1] = stopover[1];
            stop[2] = stopover[0]; break;
    }
    if(graph[source][stop[0]].weight == INF ||\
        graph[stop[0]][stop[1]].weight == INF ||\
        graph[stop[1]][stop[2]].weight == INF ||\
        graph[stop[2]][destination].weight == INF){
            printf("path doesn't exist.\n");
            return;
        }
    weight += graph[source][stop[0]].weight;
    weight += graph[stop[0]][stop[1]].weight;
    weight += graph[stop[1]][stop[2]].weight;
    weight += graph[stop[2]][destination].weight;

    printf("path : %4d", source);
    print_path(source, stop[0]); if(source != stop[0]) printf("*");
    print_path(stop[0], stop[1]); if(source != stop[1]) printf("*");
    print_path(stop[1], stop[2]); if(source != stop[2]) printf("*");
    print_path(stop[2], destination);
    printf("\ncost : %4d", weight);

}