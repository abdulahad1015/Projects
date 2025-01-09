#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <stdbool.h>
using namespace std;

#define INF std::numeric_limits<double>::infinity()

class Node {
public:
    int nodeNum, x, y, parentNum;
    double f, g, h;

    Node(int n, int x_coord, int y_coord) : nodeNum(n), x(x_coord), y(y_coord), f(INF), g(INF), h(0), parentNum(-1) {}
    
    void setF() { f = g + h; }
    void setG(double weight) { g = weight; }
    void setH(int goalX, int goalY) { h = abs(x - goalX) + abs(y - goalY); }
};

class Grid {
public:
    vector<vector<double>> graph;
    vector<Node> nodes;

    Grid(int n) : graph(n, vector<double>(n, INF)), nodes(n, Node(-1, -1, -1)) {}
    void addEdge(int u, int v, double weight);
};

void Grid::addEdge(int u, int v, double weight) {
    graph[u][v] = weight;
    graph[v][u] = weight;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int gridDimX, gridDimY, totalNodes, totalEdges;
    vector<Node> nodes;
    Grid* maze = nullptr;

    if (rank == 0) {
        ifstream infile("Astar_cases.txt");
        if (!infile) {
            cerr << "Failed to open Astar_cases.txt" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        infile >> gridDimX >> gridDimY >> totalNodes >> totalEdges;

        maze = new Grid(totalNodes);

        for (int i = 0; i < totalNodes; ++i) {
            int x, y;
            infile >> x >> y;
            maze->nodes[i] = Node(i, x, y);
        }

        for (int i = 0; i < totalEdges; ++i) {
            int u, v;
            double weight;
            infile >> u >> v >> weight;
            maze->addEdge(u, v, weight);
        }
    }

    MPI_Bcast(&gridDimX, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&gridDimY, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&totalNodes, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<double> localG(totalNodes, INF);
    vector<int> localParent(totalNodes, -1);

    if (rank == 0) {
        localG[0] = 0;
        localParent[0] = -1;
    }

    vector<int> taskDistribution(totalNodes);
    int chunkSize = totalNodes / size;
    for (int i = 0; i < size; ++i) {
        fill(taskDistribution.begin() + i * chunkSize, taskDistribution.begin() + (i + 1) * chunkSize, i);
    }

    vector<bool> processed(totalNodes, false);
    bool finished = false;

    while (!finished) {
        int globalBestNode = -1;
        double globalBestF = INF;

        for (int i = 0; i < totalNodes; ++i) {
            if (taskDistribution[i] == rank && !processed[i]) {
                if (localG[i] < globalBestF) {
                    globalBestF = localG[i];
                    globalBestNode = i;
                }
            }
        }

        struct {
            double fValue;
            int node;
        } localBest{globalBestF, globalBestNode}, globalBest;

        MPI_Allreduce(&localBest, &globalBest, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        if (globalBest.node == -1) {
            finished = true;
        } else {
            processed[globalBest.node] = true;

            for (int v = 0; v < totalNodes; ++v) {
                if (maze->graph[globalBest.node][v] < INF) {
                    double newG = localG[globalBest.node] + maze->graph[globalBest.node][v];

                    if (newG < localG[v]) {
                        localG[v] = newG;
                        localParent[v] = globalBest.node;
                    }
                }
            }
        }

        MPI_Allreduce(MPI_IN_PLACE, &finished, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        vector<int> path;
        int currentNode = totalNodes - 1;
        while (currentNode != -1) {
            path.push_back(currentNode);
            currentNode = localParent[currentNode];
        }
        reverse(path.begin(), path.end());

        for (int node : path) {
            cout << node << " ";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
