#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <time.h>

// Генерация матрицы смежности
void generateMatrix(int* G, int n)
{
    for (int i = 0; i < n * n; i++)
        G[i] = 0;

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

// Вывод матрицы смежности
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

typedef struct Node {
    int v;
    struct Node* next;
} Node;

// Добавление в конец списка для сорт по возраст
void addEdge(Node** adj, int u, int v)
{
    Node* p = (Node*)malloc(sizeof(Node));
    if (!p) { printf("Ошибка выделения памяти!\n"); exit(1); }
    p->v = v;
    p->next = NULL;

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

// Рекурсивный DFS по матрице
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

void DFS_matrix(int* G, int n, int start_v)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }
    for (int i = 0; i < n; i++) NUM[i] = false;

    printf("DFS (рекурсивный, матрица): ");

    clock_t start_time = clock();
    //time_t start_time = time(NULL);

    DFS_rec(start_v, G, NUM, n);
    // Если граф несвяз обходим оставшиеся
    for (int v = 0; v < n; v++)
        if (!NUM[v]) DFS_rec(v, G, NUM, n);

    clock_t end_time = clock();
    //time_t end_time = time(NULL);

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nВремя (рекурсивный, матрица): %f сек.\n\n", time_spent);

    //double time_spent = difftime(end_time, start_time);
    //printf("\nВремя (рекурсивный, матрица): %f сек.\n\n", time_spent);

    free(NUM);
}

// Рекурсивный DFS по спискам смежности
void DFS_list_rec(int v, Node** adj, bool* NUM)
{
    NUM[v] = true;
    printf("%d ", v + 1);
    for (Node* p = adj[v]; p; p = p->next)
        if (!NUM[p->v])
            DFS_list_rec(p->v, adj, NUM);
}

void DFS_adjList_rec(Node** adj, int n, int start_v)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }
    for (int i = 0; i < n; i++) NUM[i] = false;

    printf("DFS (рекурсивный, списки смежности): ");

    clock_t start_time = clock(); 

    DFS_list_rec(start_v, adj, NUM);
    for (int v = 0; v < n; v++)
        if (!NUM[v]) DFS_list_rec(v, adj, NUM);

    clock_t end_time = clock();

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nВремя (рекурсивный, списки): %f сек.\n\n", time_spent);

    free(NUM);
}

// Нерекурсивный DFS по матрице
void DFS_iter_matrix(int* G, int n, int start_v)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }
    for (int i = 0; i < n; i++) NUM[i] = false;

    int stackSize = n * n;
    int* stack = (int*)malloc(stackSize * sizeof(int));
    if (!stack) { printf("Ошибка выделения памяти!\n"); free(NUM); exit(1); }
    int top = 0;

    printf("DFS (нерекурсивный, матрица):     ");

    clock_t start_time = clock();

    //обрабатываем выбранную вер
    stack[top++] = start_v;
    while (top > 0 || start_v < n - 1)
    {
        if (top > 0) {
            int v = stack[--top];
            if (NUM[v]) continue;
            NUM[v] = true;
            printf("%d ", v + 1);

            //соседи в стек в обрат пор
            for (int i = n - 1; i >= 0; i--)
                if (G[v * n + i] == 1 && !NUM[i])
                    stack[top++] = i;
        }
        else {
            //стек пуст ищем непосещенные верш
            bool found = false;
            for (int i = 0; i < n; i++) {
                if (!NUM[i]) {
                    stack[top++] = i;
                    found = true;
                    break;
                }
            }
            if (!found) break; //посещено всее
        }
    }

    clock_t end_time = clock();

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nВремя (нерекурсивный, матрица): %f сек.\n\n", time_spent);

    free(stack);
    free(NUM);
}

// НЕРЕКУРСИВНЫЙ DFS по спискам смежности
void DFS_iter_adjList(Node** adj, int n, int start_v)
{
    bool* NUM = (bool*)malloc(n * sizeof(bool));
    if (!NUM) { printf("Ошибка выделения памяти!\n"); exit(1); }
    for (int i = 0; i < n; i++) NUM[i] = false;

    int stackSize = n * n;
    int* stack = (int*)malloc(stackSize * sizeof(int));
    if (!stack) { printf("Ошибка выделения памяти!\n"); free(NUM); exit(1); }
    int top = 0;

    printf("DFS (нерекурсивный, списки):      ");

    clock_t start_time = clock();

    stack[top++] = start_v;
    while (top > 0 || start_v < n - 1)
    {
        if (top > 0) {
            int v = stack[--top];
            if (NUM[v]) continue;
            NUM[v] = true;
            printf("%d ", v + 1);

            int temp[100]; 
            int count = 0;
            for (Node* p = adj[v]; p; p = p->next) {
                if (!NUM[p->v]) {
                    temp[count++] = p->v;
                }
            }
            //в стек в обратном порядке кладем
            for (int i = count - 1; i >= 0; i--) {
                stack[top++] = temp[i];
            }
        }
        else {
            //обход от старт зокончился но остались верш
            bool found = false;
            for (int i = 0; i < n; i++) {
                if (!NUM[i]) {
                    stack[top++] = i;
                    found = true;
                    break;
                }
            }
            if (!found) break;
        }
    }

    clock_t end_time = clock(); 

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nВремя (нерекурсивный, списки): %f сек.\n\n", time_spent);

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

    int start_node;
    printf("Введите начальную вершину (от 1 до %d): ", n);
    if (scanf_s("%d", &start_node) != 1 || start_node < 1 || start_node > n)
    {
        printf("Некорректная начальная вершина.\n");
        return 1;
    }
    int start_v = start_node - 1;

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

    DFS_matrix(G, n, start_v);
    DFS_adjList_rec(adj, n, start_v);
    DFS_iter_matrix(G, n, start_v);
    DFS_iter_adjList(adj, n, start_v);

    freeAdjList(adj, n);
    free(adj);
    free(G);

    return 0;
}