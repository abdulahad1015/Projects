#include <iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<map>
#include<set>
#include<limits>
#include<time.h>
#include<chrono>
#include<stdio.h>
#include<queue>
#include<climits>
#include <omp.h>
using namespace std;

#define INFINITY 1e9
// Parallel BFS
void parallelBFS(vector<vector<int>>& graph, int n, int start) {
    vector<bool> visited(n, false);           
    vector<int> frontier = {start};           

    #pragma omp parallel 
    {
        #pragma omp single
        visited[start] = true;
    }

    int level = 0;                            

    while (!frontier.empty()) {
        // Print current level nodes
        #pragma omp parallel
        {
            #pragma omp single
            printf("Level %d: ", level++);

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < frontier.size(); ++i) {
                #pragma omp critical(print)
                {
                    printf("%d ", frontier[i]);
                }
            }

            #pragma omp single
            printf("\n");
        }

        vector<int> nextFrontier;             

        #pragma omp parallel
        {
            vector<int> localNextFrontier;

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < frontier.size(); ++i) {
                int node = frontier[i];

                #pragma omp parallel for schedule(dynamic) 
                for (int j = 0; j < graph[node].size(); ++j) {
                    int neighbor = graph[node][j];

                    if (!visited[neighbor]) {
                        bool alreadyVisited = false;

                        
                        #pragma omp critical(visit)
                        {
                            if (!visited[neighbor]) {
                                visited[neighbor] = true;
                            } else {
                                alreadyVisited = true;
                            }
                        }

                        if (!alreadyVisited) {
                            #pragma omp critical(local_push)
                            {
                                localNextFrontier.push_back(neighbor);
                            }
                        }
                    }
                }
            }

          
            #pragma omp critical(global_merge)
            {
                nextFrontier.insert(nextFrontier.end(), localNextFrontier.begin(), localNextFrontier.end());
            }

            #pragma omp barrier 
        }

        #pragma omp parallel
        {
            #pragma omp single
            frontier = move(nextFrontier);
        }
    }

    #pragma omp barrier
}


// Read graph and run BFS
void runparallelBFS() {
    ifstream infile("BFS_cases.txt");
    if (!infile.is_open()) {
        cerr << "Error opening BFS_cases.txt" << endl;
        return;
    }

    int n, m;
    infile >> n >> m;
    vector<vector<int>> graph(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        infile >> u >> v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    vector<int> threadCounts = {2, 3, 4, 5, 6, 7, 8};

    for (int threads : threadCounts) {
        omp_set_num_threads(threads);

        auto startTime = chrono::high_resolution_clock::now();

        parallelBFS(graph, n, 0);

        auto endTime = chrono::high_resolution_clock::now();
        chrono::duration<double> executionTime = endTime - startTime;

        printf("Execution time with %d threads: %.6f seconds\n", threads, executionTime.count());
    }
}

// THIS SECTION IS FOR A* SEARCH
class Node {
public:
    int NodeNumber, x_coordinate, y_coordinate, ParentNumber;
    double f, g, h;

    Node(int nodenum, int x, int y)
        : NodeNumber(nodenum), x_coordinate(x), y_coordinate(y),
          f(INFINITY), g(INFINITY), h(0), ParentNumber(-1) {}

    void setf() { f = g + h; }
    void setg(double weight) { g = weight; }
    void setHeuristic(int lastX, int lastY) {
        h = abs(x_coordinate - lastX) + abs(y_coordinate - lastY);
    }
    void setParentNumber(int pn) { ParentNumber = pn; }
};

// Grid class for A*
class Grid {
public:
    vector<vector<double>> myGrid;
    map<int, Node*> allNodes;
    int dimensionX, dimensionY, TotalNodes;

    Grid(int dimX, int dimY, int noNodes)
        : dimensionX(dimX), dimensionY(dimY), TotalNodes(noNodes) {
        myGrid.resize(noNodes, vector<double>(noNodes, 0));
    }

    ~Grid() {
        #pragma omp parallel for
        for (auto& node : allNodes) {
            delete node.second;
        }
    }

    void FillGrid(int noEdges, istream& infile) {
        #pragma omp parallel
        for (int i = 0; i < noEdges; ++i) {
            int l, r, lx, ly, rx, ry;
            double weight;
            infile >> l >> r >> weight >> lx >> ly >> rx >> ry;

            #pragma omp critical
            {
                myGrid[l][r] = weight;
                myGrid[r][l] = weight;
                if (!allNodes.count(l)) {
                    allNodes[l] = new Node(l, lx, ly);
                }
                if (!allNodes.count(r)) {
                    allNodes[r] = new Node(r, rx, ry);
                }
                allNodes[l]->setHeuristic(dimensionX - 1, dimensionY - 1);
                allNodes[r]->setHeuristic(dimensionX - 1, dimensionY - 1);
            }
        }
    }

    void printPath(int destination) {
        vector<int> path;

        #pragma omp parallel
        {
            #pragma omp single nowait
            while (destination != -1) {
                #pragma omp critical
                path.push_back(destination);
                destination = allNodes[destination]->ParentNumber;
            }
        }

        reverse(path.begin(), path.end());
        for (int node : path) {
            cout << node << " ";
        }
        cout << endl;
    }
};

// Run parallel A* algorithm
void runparallelAstar() {
    ifstream infile("A_Starcases.txt");
    if (!infile.is_open()) {
        cerr << "Error opening A_Starcases.txt" << endl;
        return;
    }

    auto start = chrono::high_resolution_clock::now();

    while (!infile.eof()) {
        int dimX, dimY, noNodes, noEdges;
        if (!(infile >> dimX >> dimY >> noNodes >> noEdges)) break;

        Grid maze(dimX, dimY, noNodes);
        maze.FillGrid(noEdges, infile);

        maze.allNodes[0]->setg(0);
        maze.allNodes[0]->setf();

        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
        pq.push({maze.allNodes[0]->f, 0});

        while (!pq.empty()) {
            int currentNode = pq.top().second;
            pq.pop();

            #pragma omp parallel 
            #pragma omp for
            for (int i = 0; i < noNodes; ++i) {
                if (maze.myGrid[currentNode][i] > 0) {
                    double newG = maze.allNodes[currentNode]->g + maze.myGrid[currentNode][i];

                    #pragma omp critical
                    {
                        if (newG < maze.allNodes[i]->g) {
                            maze.allNodes[i]->setg(newG);
                            maze.allNodes[i]->setf();
                            maze.allNodes[i]->setParentNumber(currentNode);
                            pq.push({maze.allNodes[i]->f, i});
                        }
                    }
                }
            }
            #pragma omp barrier
        }

        maze.printPath(noNodes - 1);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Time taken to run A* generated input: " << duration << " seconds = " << duration / 60 << " minutes\n";
}


//THIS SECTION IS FOR GREEDYBFS. THE ONLY DIFFERNECE BETWEEN THIS AND A* IS HUERISTIC CALCULATION

#define INF std::numeric_limits<double>::infinity()

class Node1 {
public:
    int nodeNum, x, y, parentNum;
    double h; // Heuristic value

    Node1(int n = -1, int x_coord = -1, int y_coord = -1)
        : nodeNum(n), x(x_coord), y(y_coord), h(INF), parentNum(-1) {}

    void setHeuristic(int goalX, int goalY) {
        h = abs(x - goalX) + abs(y - goalY);
    }
};

class Grid1 {
public:
    vector<vector<double>> graph;
    vector<Node1> nodes;

    Grid1(int n) : graph(n, vector<double>(n, INF)), nodes(n) {}

    void addEdge(int u, int v, double weight) {
        graph[u][v] = weight;
        graph[v][u] = weight;
    }
};

void runGreedyBFS(){
    int gridDimX, gridDimY, totalNodes, totalEdges;

    ifstream infile("GreedyBFS_cases.txt");
    if (!infile) {
        cerr << "Failed to open Astar_cases.txt" << endl;
        return;
    }

    infile >> gridDimX >> gridDimY >> totalNodes >> totalEdges;

    Grid1 maze(totalNodes);
    for (int i = 0; i < totalNodes; ++i) {
        int x, y;
        infile >> x >> y;
        maze.nodes[i] = Node1(i, x, y);
    }

    for (int i = 0; i < totalEdges; ++i) {
        int u, v;
        double weight;
        infile >> u >> v >> weight;
        maze.addEdge(u, v, weight);
    }

    vector<double> heuristic(totalNodes, INF);
    vector<int> parent(totalNodes, -1);
    heuristic[0] = 0;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    pq.push({0, 0});

#pragma omp parallel num_threads(4)
    {
#pragma omp master
        {
            while (!pq.empty()) {
                auto [currentH, currentNode] = pq.top();
                pq.pop();

                #pragma omp parallel for schedule(dynamic)
                for (int i = 0; i < totalNodes; ++i) {
                    if (maze.graph[currentNode][i] < INF) {
                        double newH = abs(maze.nodes[i].x - gridDimX) + abs(maze.nodes[i].y - gridDimY);
                        if (newH < heuristic[i]) {
                            heuristic[i] = newH;
                            parent[i] = currentNode;
                            pq.push({newH, i});
                        }
                    }
                }
            }
        }
        #pragma omp barrier
    }

    vector<int> path;
    int currentNode = totalNodes - 1;
    while (currentNode != -1) {
        path.push_back(currentNode);
        currentNode = parent[currentNode];
    }
    reverse(path.begin(), path.end());

    for (int node : path) {
        cout << node << " ";
    }
}
int main() {
    printf("ENTER 1 TO RUN BFS FROM BFS_CASES.TXT\nENTER 2 TO RUN A* FROM A_STARCASES.TXT\nENTER 3 TO RUN GREEDY BFS FROM GREEDYBFS_CASES");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            runparallelBFS();
            break;
        case 2:
            runparallelAstar();
            break;
        case 3:
            runGreedyBFS();
        default:
            printf("Invalid choice!\n");
            break;
    }

    return 0;
}
