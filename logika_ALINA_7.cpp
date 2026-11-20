#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <time.h>

void generateMatrix(int* G, int n)
{
    // Инициализация 000
    for (int i = 0; i < n * n; i++)
        G[i] = 0;

    //С вероятностью p добавляем ребро (i,j)
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++) 
        {
            if (rand() % 100 < 50)
            {
                G[i * n + j] = 1;
                G[j * n + i] = 1;
            }
        }
    }
}

void printMatrix(int* G, int n)
{
    printf("Матрица смежности:\n    ");
    for (int j = 0; j < n; j++) printf("%2d ", j + 1);
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%2d: ", i + 1);
        for (int j = 0; j < n; j++)
            printf("%2d ", G[i * n + j]);
        printf("\n");
    }
}

//рекурсивный DFS по матрице смеж.

void DFS_rec(int v, int* G, bool* NUM, int n)
{
    NUM[v] = true;
    printf("%d ", v + 1);

    for (int i = 0; i < n; i++)
    {
        if (G[v * n + i] == 1 && !NUM[i])
            DFS_rec(i, G, NUM, n);
    }
}

 //Обход всего графа алгоритм ПОГ
void DFS_matrix(int* G, int n)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }

    for (int i = 0; i < n; i++) NUM[i] = false;

    printf("DFS (рекурсивный, матрица смежности): ");
    for (int v = 0; v < n; v++)
        if (!NUM[v])
            DFS_rec(v, G, NUM, n);
    printf("\n");

    free(NUM);
}

typedef struct Node {
    int v;
    struct Node* next;
} Node;

//добав в конец списка для сорт по возрастан
void addEdge(Node** adj, int u, int v)
{
    Node* p = (Node*)malloc(sizeof(Node));
    if (!p) { printf("Ошибка выделения памяти!\n"); exit(1); }
    p->v = v;
    p->next = NULL;

    //список пуст -> новый элемент стан 1м
    if (adj[u] == NULL) {
        adj[u] = p;
    }
    else {
        Node* current = adj[u];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = p;
    }
}

void buildAdjList(int* G, int n, Node** adj)
{
    for (int i = 0; i < n; i++) adj[i] = NULL;

    //мы идем по j от 0 до n-1 а добавляем в конец
    //строго по возрастанию
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (G[i * n + j] == 1)
                addEdge(adj, i, j);
}

void printAdjList(Node** adj, int n)
{
    printf("Список смежности:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%2d: ", i + 1);
        Node* p = adj[i];
        while (p)
        {
            printf("- %d ", p->v + 1);
            p = p->next;
        }
        printf("\n");
    }
}

void freeAdjList(Node** adj, int n)
{
    for (int i = 0; i < n; i++)
    {
        Node* p = adj[i];
        while (p) { Node* t = p->next; free(p); p = t; }
        adj[i] = NULL;
    }
}

void DFS_list_rec(int v, Node** adj, bool* NUM)
{
    NUM[v] = true;
    printf("%d ", v + 1);
    for (Node* p = adj[v]; p; p = p->next)
        if (!NUM[p->v])
            DFS_list_rec(p->v, adj, NUM);
}

void DFS_adjList(Node** adj, int n)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }

    for (int i = 0; i < n; i++) NUM[i] = false;

    printf("DFS (рекурсивный, списки смежности):   ");
    for (int v = 0; v < n; v++)
        if (!NUM[v])
            DFS_list_rec(v, adj, NUM);
    printf("\n");

    free(NUM);
}


//нерекурсивный DFSстек

void DFS_iter(int* G, int n)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }

    for (int i = 0; i < n; i++) NUM[i] = false;

    int stackSize = n * n;
    int* stack = (int*)malloc(stackSize * sizeof(int));
    if (!stack) { printf("Ошибка выделения памяти!\n"); free(NUM); exit(1); }

    int top = 0;

    printf("DFS (нерекурсивный, стек, матрица):     ");
    for (int start = 0; start < n; start++)
    {
        if (NUM[start]) continue;
        stack[top++] = start;

        while (top > 0)
        {
            int v = stack[--top];
            if (NUM[v]) continue;

            NUM[v] = true;
            printf("%d ", v + 1);

            //порядок обхода= рекурс. версиии
            for (int i = n - 1; i >= 0; i--)
                if (G[v * n + i] == 1 && !NUM[i])
                    stack[top++] = i;
        }
    }
    printf("\n");

    free(stack);
    free(NUM);
}

int main(void)
{
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    int n;
    printf("Введите количество вершин: ");
    if (scanf_s("%d", &n) != 1 || n < 2)
    {
        printf("Некорректный размер.\n");
        return 1;
    }

    int* G = (int*)calloc(n * n, sizeof(int));
    if (!G) { printf("Ошибка выделения памяти!\n"); return 1; }

    Node** adj = (Node**)malloc(n * sizeof(Node*));
    if (!adj) { printf("Ошибка выделения памяти!\n"); free(G); return 1; }

    generateMatrix(G, n);
    printMatrix(G, n);

    buildAdjList(G, n, adj);
    printf("\n");
    printAdjList(adj, n);
    printf("\n");

    DFS_matrix(G, n);
    DFS_adjList(adj, n);
    DFS_iter(G, n);

    freeAdjList(adj, n);
    free(adj);
    free(G);

    return 0;
}