#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <chrono>
#include <iostream>

void parallelBFS(int** graph, int n, int start) {
    bool* visited = (bool*)malloc(n * sizeof(bool));
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) visited[i] = false;

    visited[start] = true;
    int* frontier = (int*)malloc(n * sizeof(int));
    int frontierSize = 1;
    frontier[0] = start;

    int level = 0;

    while (frontierSize > 0) {
        printf("Level %d: ", level++);
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < frontierSize; ++i) {
            printf("%d ", frontier[i]);
        }
        printf("\n");

        int* nextFrontier = (int*)malloc(n * sizeof(int));
        int nextFrontierSize = 0;
        bool* nextVisited = (bool*)malloc(n * sizeof(bool));

        #pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            nextVisited[i] = visited[i];
        }

        #pragma omp parallel
        {
            int* localNextFrontier = (int*)malloc(n * sizeof(int));
            int localSize = 0;

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < frontierSize; ++i) {
                int node = frontier[i];
                for (int j = 0; graph[node][j] != -1; ++j) {
                    int neighbor = graph[node][j];
                    if (!visited[neighbor] && !nextVisited[neighbor]) {
                        bool alreadyVisited = false;

                        #pragma omp atomic capture
                        {
                            alreadyVisited = nextVisited[neighbor];
                            nextVisited[neighbor] = true;
                        }

                        if (!alreadyVisited) {
                            localNextFrontier[localSize++] = neighbor;
                        }
                    }
                }
            }

            #pragma omp critical
            {
                for (int i = 0; i < localSize; ++i) {
                    nextFrontier[nextFrontierSize++] = localNextFrontier[i];
                }
            }

            free(localNextFrontier);
        }

        #pragma omp parallel for
        for (int i = 0; i < nextFrontierSize; ++i) {
            visited[nextFrontier[i]] = true;
        }

        free(frontier);
        frontier = nextFrontier;
        frontierSize = nextFrontierSize;

        free(nextVisited);
    }

    free(visited);
    free(frontier);
}

int main() {
    int n, m;
    printf("Enter the number of vertices and edges: ");
    scanf("%d %d", &n, &m);

    int** graph = (int**)malloc(n * sizeof(int*));
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        graph[i] = (int*)malloc((n + 1) * sizeof(int));
        #pragma omp parallel for
        for (int j = 0; j <= n; ++j) {
            graph[i][j] = -1;
        }
    }

    printf("Enter the edges (u v):\n");
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

    // Test for different numbers of threads
    int threadCounts[] = {1,2,3,4,5 ,6,7,8};
    for (int t = 0; t < 7; ++t) {
        int threads = threadCounts[t];
        omp_set_num_threads(threads);
        printf("\nRunning BFS with %d threads:\n", threads);

        auto startTime = std::chrono::high_resolution_clock::now();

        parallelBFS(graph, n, 0);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> executionTime = endTime - startTime;

        printf("Execution time with %d threads: %.6f seconds\n", threads, executionTime.count());
    }

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        free(graph[i]);
    }
    free(graph);

    return 0;
}
