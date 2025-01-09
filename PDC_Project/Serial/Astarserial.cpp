#include <bits/stdc++.h>
using namespace std;
//First the code takes input of dimension of grid
//Then number of nodes and edges
//Then two integers:u,v indicating there is an edge between u and v
//Then the weight of this edge
//Lastly, x y coordinate of u followed by x y coordinate of v
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
    set<int> CurrentNodes;
    int dimensionX, dimensionY, TotalNodes, TotalEdges;

    Grid(int, int, int);
    ~Grid();

    void FillGrid(int,istream&);
    void printPath();
};
#define RUN for(int astar=1;astar<=10000;++astar)
Node::Node(int nodenum, int x, int y)
    : NodeNumber(nodenum), x_coordinate(x), y_coordinate(y), f(INFINITY), g(INFINITY), h(0), ParentNumber(-1) {}

double Node::getHeuristic() { return h; }
int Node::getNodeNumber() { return NodeNumber; }
double Node::getf() { return f; }
double Node::getg() { return g; }

void Node::setf() { this->f = this->g + this->h; }
void Node::setg(double weight) { this->g = weight; }
void Node::setHeuristic(int lastX, int lastY) {
    h = abs(this->x_coordinate - lastX) + abs(this->y_coordinate - lastY);
}
void Node::setParentNumber(int pn) { this->ParentNumber = pn; }

int Node::getx_coordinate() { return x_coordinate; }
int Node::gety_coordinate() { return y_coordinate; }
int Node::getParentNumber() { return ParentNumber; }

Grid::Grid(int dimX, int dimY, int noNodes)
    : dimensionX(dimX), dimensionY(dimY), TotalNodes(noNodes) {
    myGrid.resize(noNodes, vector<double>(noNodes, 0));
}

Grid::~Grid() {
    for (auto& node : allNodes) {
        delete node.second;
    }
}

void Grid::FillGrid(int noEdges,istream& infile) {
    for (int i = 0; i < noEdges; ++i) {
        int l, r, lx, ly, rx, ry;
        double weight;

      
        infile >> l >> r >> weight >> lx >> ly >> rx >> ry;

     
        if (l < 0 || l >= TotalNodes || r < 0 || r >= TotalNodes) {
            cerr << "Invalid node numbers. Nodes must be in range [0, " << TotalNodes - 1 << "]." << endl;
            exit(1);
        }

        // Populate grid
        myGrid[l][r] = weight;
        myGrid[r][l] = weight;

        // Add nodes if not already added
        if (CurrentNodes.insert(l).second) {
            allNodes[l] = new Node(l, lx, ly);
            if (l != 0) allNodes[l]->setHeuristic(dimensionX - 1, dimensionY - 1);
        }
        if (CurrentNodes.insert(r).second) {
            allNodes[r] = new Node(r, rx, ry);
            if (r != 0) allNodes[r]->setHeuristic(dimensionX - 1, dimensionY - 1);
        }
    }
}

void Grid::printPath() {
    int DestinationNumber = -1;
    RUN{
    // Find the destination node
    for (auto& [nodeNum, node] : allNodes) {
        if (node->getx_coordinate() == dimensionX - 1 &&
            node->gety_coordinate() == dimensionY - 1) {
            DestinationNumber = nodeNum;
            break;
        }
    }
    }
    if (DestinationNumber == -1) {
        //cout << "No valid path found to the destination." << endl;
        return;
    }

    // Backtrack to reconstruct the path
    vector<int> path;
    while (DestinationNumber != -1) {
        path.push_back(DestinationNumber);
        DestinationNumber = allNodes[DestinationNumber]->getParentNumber();
    }
    reverse(path.begin(), path.end());

    // Print the path
    // for (int node : path) {
    //     cout << node << " ";
    // }
   // cout << endl;
}

int main() {


    ifstream infile("Astar_cases.txt");
    if (!infile) {
        cerr << "Failed to open testcases.txt" << endl;
        return 1;
    }
    auto start = chrono::high_resolution_clock::now();
    long double totaltimetaken=0;
    RUN{
    while (!infile.eof()) {
    int dimX, dimY, noNodes, noEdges;
    if(!(infile >> dimX >> dimY >> noNodes >> noEdges))break;

    Grid maze(dimX, dimY, noNodes);
    maze.FillGrid(noEdges,infile);
   
    // Initialize start node
    maze.allNodes[0]->setg(0);
    maze.allNodes[0]->setf();

   
vector<pair<double, int>> HeapFCost; 

HeapFCost.push_back({maze.allNodes[0]->getf(), 0});


while (!HeapFCost.empty()) {
    
    auto minIt = min_element(HeapFCost.begin(), HeapFCost.end(),
        [](const pair<double, int>& a, const pair<double, int>& b) {
            return a.first < b.first; 
        });

   
    double currF = minIt->first;
    int currentNode = minIt->second;

   
    HeapFCost.erase(minIt);

    for (int i = 0; i < noNodes; ++i) {
        if (maze.myGrid[currentNode][i] > 0) {
            double newG = maze.allNodes[currentNode]->getg() + maze.myGrid[currentNode][i];
            if (newG < maze.allNodes[i]->getg()) {
                maze.allNodes[i]->setg(newG);
                maze.allNodes[i]->setf();
                maze.allNodes[i]->setParentNumber(currentNode);

                
                HeapFCost.push_back({maze.allNodes[i]->getf(), i});
            }
        }
    }
}


    // Print the result
     maze.printPath();
    
    // totaltimetaken+=duration;
    }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout<<"Time taken to run generated input: "<<duration<<" seconds = "<<duration/60<<" minutes";
    return 0;
}
