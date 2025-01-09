#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

class Node {
    int NodeNumber, x_coordinate, y_coordinate, ParentNumber;
    double f, g, h;

public:
    Node(int, int, int);

    void setf();
    void setg(double);
    void setHeuristic(int, int);
    void setParentNumber(int);

    double getHeuristic();
    int getNodeNumber();
    double getf();
    double getg();
    int getx_coordinate();
    int gety_coordinate();
    int getParentNumber();
};

class Grid {
public:
    vector<vector<double>> myGrid;
    map<int, Node*> allNodes;
    set<int> CurrentNodes; // keeps track of node numbers already added
    int dimensionX, dimensionY, TotalNodes, TotalEdges;
    int DestinationNumber;

    Grid(int, int, int);

    void FillGrid(int);

    void printPath();
};

Node::Node(int nodenum, int x, int y) 
    : NodeNumber(nodenum), x_coordinate(x), y_coordinate(y), f(INFINITY), g(INFINITY), h(0) {}

double Node::getHeuristic() {
    return h;
}
int Node::getNodeNumber() {
    return NodeNumber;
}
double Node::getf() {
    return f;
}
double Node::getg() {
    return g;
}
void Node::setf() {
    this->f = this->g + this->h;
}
void Node::setg(double weight) {
    this->g = weight;
}
void Node::setHeuristic(int lastX, int lastY) {
    h = (double)(abs(this->x_coordinate - lastX) + abs(this->y_coordinate - lastY));
}
void Node::setParentNumber(int pn) {
    this->ParentNumber = pn;
}
int Node::getx_coordinate() {
    return x_coordinate;
}
int Node::gety_coordinate() {
    return y_coordinate;
}
int Node::getParentNumber() {
    return ParentNumber;
}

Grid::Grid(int dimensionX, int dimensionY, int no_Nodes) 
    : dimensionX(dimensionX), dimensionY(dimensionY), TotalNodes(no_Nodes) {
    myGrid.resize(no_Nodes);
    for (int i = 0; i < no_Nodes; ++i) {
        myGrid[i].resize(no_Nodes, 0);
    }
}

void Grid::FillGrid(int no_Edges) {
    cout << "ENTER DESTINATION NODE COORDINATES: ";
    int destX, destY;
    cin >> destX >> destY;

    // Track the destination node by coordinates
    DestinationNumber = -1;

    for (int i = 0; i < no_Edges; ++i) {
        int l, r, lx, ly, rx, ry;
        double weight;

        cout << "ENTER START NODE NUMBER AND END NODE NUMBER SEPARATED BY SPACES: ";
        cin >> l >> r;

        while (l < 0 || l >= TotalNodes || r < 0 || r >= TotalNodes) {
            cout << "INVALID NODE NUMBERS. PLEASE RE-ENTER: ";
            cin >> l >> r;
        }

        cout << "ENTER WEIGHT OF THE EDGE (MUST BE POSITIVE): ";
        cin >> weight;
        while (weight <= 0) {
            cout << "INVALID WEIGHT. ENTER AGAIN: ";
            cin >> weight;
        }

        cout << "ENTER X-COORDINATE AND Y-COORDINATE OF NODE " << l << " : ";
        cin >> lx >> ly;

        cout << "ENTER X-COORDINATE AND Y-COORDINATE OF NODE " << r << " : ";
        cin >> rx >> ry;

        myGrid[l][r] = weight;
        myGrid[r][l] = weight;

        // Add nodes if they do not already exist
        if (CurrentNodes.find(l) == CurrentNodes.end()) {
            CurrentNodes.insert(l);
            allNodes[l] = new Node(l, lx, ly);
            allNodes[l]->setHeuristic(destX, destY);
            if (lx == destX && ly == destY) DestinationNumber = l;
        }

        if (CurrentNodes.find(r) == CurrentNodes.end()) {
            CurrentNodes.insert(r);
            allNodes[r] = new Node(r, rx, ry);
            allNodes[r]->setHeuristic(destX, destY);
            if (rx == destX && ry == destY) DestinationNumber = r;
        }
    }

    if (DestinationNumber == -1) {
        cout << "ERROR: Destination node not defined in the grid." << endl;
        exit(1);
    }
}

void Grid::printPath() {
    vector<int> path;
    int current = DestinationNumber;

    while (current != 0) {
        path.push_back(current);
        current = allNodes[current]->getParentNumber();
    }

    path.push_back(0);
    reverse(path.begin(), path.end());

    for (int node : path) {
        cout << node << " ";
    }
    cout << endl;
}

int main() {
    int dimX, dimY, noNodes, noEdges;
    cout << "ENTER GRID DIMENSIONS (X Y): ";
    cin >> dimX >> dimY;
    cout << "ENTER NUMBER OF NODES: ";
    cin >> noNodes;
    cout << "ENTER NUMBER OF EDGES: ";
    cin >> noEdges;

    Grid maze(dimX, dimY, noNodes);
    maze.FillGrid(noEdges);

    maze.allNodes[0]->setg(0);
    maze.allNodes[0]->setf();

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> HeapFCost;
    HeapFCost.push({maze.allNodes[0]->getf(), 0});

    #pragma omp parallel
    {
        while (!HeapFCost.empty()) {
            pair<double, int> current;

            #pragma omp critical
            {
                if (!HeapFCost.empty()) {
                    current = HeapFCost.top();
                    HeapFCost.pop();
                } else {
                    current = {-1, -1}; // Sentinel value
                }
            }

            if (current.second == -1) break;

            int node = current.second;
            double currentCost = current.first;

            for (int i = 0; i < noNodes; ++i) {
                if (maze.myGrid[node][i] != 0) {
                    double tentative_g = maze.allNodes[node]->getg() + maze.myGrid[node][i];

                    if (tentative_g < maze.allNodes[i]->getg()) {
                        #pragma omp critical
                        {
                            maze.allNodes[i]->setg(tentative_g);
                            maze.allNodes[i]->setf();
                            maze.allNodes[i]->setParentNumber(node);
                            HeapFCost.push({maze.allNodes[i]->getf(), i});
                        }
                    }
                }
            }
        }
    }

    cout << "SHORTEST PATH: ";
    maze.printPath();

    return 0;
}
