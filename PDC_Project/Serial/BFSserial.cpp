#include <bits/stdc++.h>
#include<stdio.h>
#include<pqStubs.h>
#include<acc_prof.h>
#include<chrono>
#include<activation.h>
using namespace std;

#define int long long
#define double long double
#define best bestfirst
#define RUN for (int bfss = 1; bfss <= 1000000; ++bfss)
#define bfs bfs
#define dfs bfs
#define astar asta
typedef float fl;

void bfs(int** graph, int n, int start) {
    int* visited = (int*)calloc(n, sizeof(int));
    int* queue = (int*)malloc(n * sizeof(int));
    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int current = queue[front++];
        //printf("%d ", current);

        for (int i = 0; graph[current][i] != -1; ++i) {
            int neighbor = graph[current][i];
            if (!visited[neighbor]) {
                visited[neighbor] = 1;
                queue[rear++] = neighbor;
            }
        }
    }

    free(visited);
    free(queue);
}

int main() {
    ifstream inputFile("BFS_cases.txt");
    if (!inputFile) {
        cerr << "Error: Unable to open input file BFS_cases.txt\n";
        return 1;
    }

    auto startTime = chrono::high_resolution_clock::now();
    RUN {
        int n, m;
        inputFile >> n >> m;
        if (inputFile.fail()) break;

        int** graph = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; ++i) {
            graph[i] = (int*)malloc((n + 1) * sizeof(int));
            for (int j = 0; j <= n; ++j) graph[i][j] = -1;
        }

        for (int i = 0; i < m; ++i) {
            int u, v;
            inputFile >> u >> v;
            if (inputFile.fail()) break;

            int j = 0;
            while (graph[u][j] != -1) j++;
            graph[u][j] = v;

            j = 0;
            while (graph[v][j] != -1) j++;
            graph[v][j] = u;
        }

        bfs(graph, n, 0);
        printf("\n");

        // Free graph memory
        for (int i = 0; i < n; ++i) {
            free(graph[i]);
        }
        free(graph);
    }

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> executionTime = endTime - startTime;

    inputFile.close();
    printf("Time taken to run generated input %.3f seconds =  %.3f minutes", executionTime*1000,(executionTime*1000)/60);
    return 0;
}
