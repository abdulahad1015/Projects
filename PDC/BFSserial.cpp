#include <bits/stdc++.h>

void bfs(int** graph, int n, int start) {
    
    int* visited = (int*)calloc(n, sizeof(int));
    int* queue = (int*)malloc(n * sizeof(int));
    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int current = queue[front++];
        printf("%d ", current);

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
    int n, m;
    scanf("%d %d", &n, &m);
    auto startTime = std::chrono::high_resolution_clock::now();

    int** graph = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        graph[i] = (int*)malloc((n + 1) * sizeof(int));
        for (int j = 0; j <= n; ++j) graph[i][j] = -1;
    }

    for (int i = 0; i < m; ++i) {
        int u, v;
        scanf("%d %d", &u, &v);
        int j = 0;
        while (graph[u][j] != -1) j++;
        graph[u][j] = v;

        j = 0;
        while (graph[v][j] != -1) j++;
        graph[v][j] = u;
    }

    
     
    bfs(graph, n, 0);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> executionTime = endTime - startTime;

    printf("Execution time: %.6f\n", executionTime.count()*1000);

    for (int i = 0; i < n; ++i) free(graph[i]);
    free(graph);

    return 0;
}
