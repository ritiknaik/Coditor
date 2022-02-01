#include "dagraph.h"
int new(){
    return 0 ;
}
void for 69 "HELLO" 
//////current

void initgraph(graph *g, int size){
    //g=(graph *)malloc(sizeof(graph));
    g->vertex = size;
    int i,j;
    for(i = 0; i<size; i++){
        for(j = 0; j<size; j++){
            g->matrix[i][j] = 0;
        }
    }
    return;
}

graph_adj *init_graphadj( int v){
    graph_adj *g = (graph_adj *)malloc(sizeof(graph_adj));
    
    list *p = (list *)malloc(sizeof(list) * v);
    g->vertices = v;
    g->array = p;
    for (int i = 0; i < v; i++){
        g->array[i].head = NULL;
    }
    //printf("---%d\n", g->vertices);
    return g;
}

node *createNode(int v, int w){
    node *nn = (node *)malloc(sizeof(node));
    nn->data = v;
    nn->weight = w;
    nn->next = NULL;
    return nn;
}

void add_edge_adj(graph_adj *g, int src, int des, int weight){
    node *p = createNode(des, weight);
    p->next = g->array[src].head;
    g->array[src].head = p;
    return;
}

void add_undir_edge(graph_adj *g, int src, int des, int weight){
    add_edge_adj(g, src, des, weight);
    add_edge_adj(g, des, src, weight);
    return;
}

void display_graph(graph_adj *g){
    node *p;
    
    for (int i = 0; i < g->vertices; i++){
        //printf("ver - %d", g->vertices);
        p = g->array[i].head;
        printf("%d", i);
        while(p){
            printf(" -> %d|%d|", p->data, p->weight);
            p = p->next;
        }
        //printf(" -|");
        printf("\n");
    }
    return;
}




void addedge(graph *g, int src, int des){
    g->matrix[src][des] = 1;
    return;
}


void initfromfile(graph *g, char *filename){
    FILE *fp;
    fp=fopen(filename,"r");

    if(!fp)
        return;

    int vertices;
    fscanf(fp,"%d",&vertices);

    initgraph(g, vertices);
    int i, j;
    for(i = 0; i<vertices; i++){
        for(j = 0; j<vertices; j++){
            fscanf(fp, "%d", &g->matrix[i][j]);
        }
    }
    return;
}

void printgraph(graph *g){
    int n = g->vertex;
    printf("\n");
    for(int i = 0; i<g->vertex; i++){
        for(int j = 0; j<g->vertex; j++){
            printf("%d ",g->matrix[i][j]);
        }
        printf("\n");
    }
    return;
}

void bfs(graph *g, int start){
    queue q;
    init(&q,4);
    int visited[MAX] = {0};
    int i = start;
    printf("%d ", i);
    visited[i] = 1;
    enqueue(&q, i);
    while(!isEmpty(q)){
        i = dequeue(&q);
    
        for(int j = 0; j<g->vertex; j++){
            if(g->matrix[i][j] == 1 && visited[j] == 0){
                printf("%d ", j);
                visited[j] = 1;
                enqueue(&q, j);
            }
        }
    }
    return;
}

void bfs_adj(graph_adj *g, int start){
    queue q;
    init(&q,g->vertices);
    int visited[20] = {0};
    int i = start;
    printf("%d ", i);
    visited[i] = 1;
    enqueue(&q, i);
    while(!isEmpty(q)){
        i = dequeue(&q);
        node *p = g->array[i].head;
        while(p){
            if(visited[p->data] != 1){
                printf("%d ", p->data);
                visited[p->data] = 1;
                enqueue(&q, p->data);
            }
            p = p->next;
        }
        
    }
    return;
}

void dfs(graph *g, int start){
    int *visited = (int *)calloc(sizeof(int), g->vertex);
    stack s;
    init_stack(&s,g->vertex);
    push(&s,start);
    printf("%d ",start);
    visited[start] = 1;
    while(!isEmpty_stack(s)){
        int i = pop(&s);
        for(int j = 0; j<g->vertex; j++){
            if(g->matrix[i][j] == 1 && visited[j] == 0){
                printf("%d ", j);
                visited[j] = 1;
                push(&s, i);
                i = j;
                j = 0;
            }
        }
    }
    return;
}


void dfs_adj(graph_adj *g, int start){
    int *visited = (int *)calloc(sizeof(int), g->vertices);
    stack s;
    init_stack(&s,g->vertices);
    push(&s,start);
    printf("%d ",start);
    visited[start] = 1;
    while(!isEmpty_stack(s)){
        int i = pop(&s);
        node *p = g->array[i].head;
        while(p){
            if(visited[p->data] != 1){
                printf("%d ", p->data);
                visited[p->data] = 1;
                push(&s, p->data);
                p = g->array[p->data].head;
            }
            else p = p->next;
        }
    }
    return;
}

void dfs_recursive(graph *g, int start){
    static int visited[MAX] = {0}; 
    if(visited[start] == 0){
        printf("%d ", start);
        visited[start] = 1;
        for(int j = 1; j<MAX; j++){
            if(g->matrix[start][j]==1 && visited[j] == 0)
                dfs_recursive(g,j);
        }
    }
}

