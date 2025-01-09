#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

// Parallel BFS
void parallelBFS(vector<vector<int>>& graph, int n, int start) {
    vector<bool> visited(n, false);
    vector<int> frontier = {start};
    visited[start] = true;

    int level = 0;

    while (!frontier.empty()) {
        printf("Level %d: ", level++);

        // Print frontier nodes
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < frontier.size(); ++i) {
            #pragma omp critical
            {
                printf("%d ", frontier[i]);
            }
        }
        printf("\n");

        vector<int> nextFrontier;

        #pragma omp parallel
        {
            vector<int> localNextFrontier;

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < frontier.size(); ++i) {
                int node = frontier[i];
                for (int neighbor : graph[node]) {
                    if (!visited[neighbor]) {
                        bool alreadyVisited = false;

                        // Mark as visited safely
                        #pragma omp critical
                        {
                            if (!visited[neighbor]) {
                                visited[neighbor] = true;
                            } else {
                                alreadyVisited = true;
                            }
                        }

                        if (!alreadyVisited) {
                            localNextFrontier.push_back(neighbor);
                        }
                    }
                }
            }

            // Merge local frontiers
            #pragma omp critical
            {
                nextFrontier.insert(nextFrontier.end(), localNextFrontier.begin(), localNextFrontier.end());
            }
        }

        frontier = move(nextFrontier);
    }
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

// Node class for A* Algorithm
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

            #pragma omp parallel for
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
        }

        maze.printPath(noNodes - 1);
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Time taken to run A* generated input: " << duration << " seconds = " << duration / 60 << " minutes\n";
}

int main() {
    printf("ENTER 1 TO RUN BFS FROM BFS_CASES.TXT\nENTER 2 TO RUN A* FROM A_STARCASES.TXT\n");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            runparallelBFS();
            break;
        case 2:
            runparallelAstar();
            break;
        default:
            printf("Invalid choice!\n");
            break;
    }

    return 0;
}ls
