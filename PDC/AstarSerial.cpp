#include <bits/stdc++.h>
using namespace std;

class Node
{
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

class Grid
{
public:
    vector<vector<double>> myGrid;
    map<int, Node *> allNodes;
    set<int> CurrentNodes; // keeps a track of node numbers already added
    int dimensionX, dimensionY, TotalNodes, TotalEdges;
    Grid(int, int, int);

    void FillGrid(int);

    void printPath();
};

Node::Node(int nodenum, int x, int y) : NodeNumber(nodenum), x_coordinate(x), y_coordinate(y), f(INFINITY), g(INFINITY), h(0) {}

double Node::getHeuristic()
{
    return h;
}
int Node::getNodeNumber()
{
    return NodeNumber;
}
double Node::getf()
{
    return f;
}
double Node::getg()
{
    return g;
}

void Node::setf()
{
    this->f = this->g + this->h;
}
void Node::setg(double weight)
{
    this->g = weight;
}
void Node::setHeuristic(int lastX, int lastY)
{
    h = (double)(abs(this->x_coordinate - lastX) + abs(this->y_coordinate - lastY));
}
void Node::setParentNumber(int pn)
{
    this->ParentNumber = pn;
}

int Node::getx_coordinate()
{
    return x_coordinate;
}
int Node::gety_coordinate()
{
    return y_coordinate;
}
int Node::getParentNumber()
{
    return ParentNumber;
}

Grid::Grid(int dimensionX, int dimensionY, int no_Nodes)
{
    this->dimensionX = dimensionX;
    this->dimensionY = dimensionY;
    this->TotalNodes = no_Nodes;
    myGrid.resize(no_Nodes);
    for (int i = 0; i < no_Nodes; ++i)
    {
        myGrid[i].resize(no_Nodes, 0);
    }
}

void Grid::FillGrid(int no_Edges)
{

    for (int i = 0; i < no_Edges; ++i)
    {

        int l, r, lx, ly, rx, ry;
        double weight;

        cout << "ENTER START NODE NUMBER AND END NODE NUMBER SEPARATED BY SPACES: ";
        cin >> l >> r;
        while (l < 0 || l >= TotalNodes)
        {
            cout << "INVALID VALUE ENTERED FOR FIRST NODE, IT CAN BE BETWEEN 0 AND " << TotalNodes - 1 << endl;
            cout << "ENTER AGAIN:";
            cin >> l;
        }
        while (r < 0 || r >= TotalNodes)
        {
            cout << "INVALID VALUE ENTERED FOR SECOND NODE, IT CAN BE BETWEEN 0 AND " << TotalNodes - 1 << endl;
            cout << "ENTER AGAIN:";
            cin >> r;
        }

        cout << "ENTER WEIGHT OF THE EDGE:";
        cin >> weight;

        cout << "ENTER X-COORDINATE AND Y-COORDINATE OF NODE " << l << " :";
        cin >> lx >> ly;

        cout << "ENTER X-COORDINATE AND Y-COORDINATE OF NODE " << r << " :";
        cin >> rx >> ry;

        myGrid[l][r] = weight;
        myGrid[r][l] = weight;
        if (CurrentNodes.find(l) == CurrentNodes.end())
        {
            CurrentNodes.insert(l);
            allNodes[l] = new Node(l, lx, ly);
            if (l != 0)
                allNodes[l]->setHeuristic(dimensionX - 1, dimensionY - 1);
        }
        if (CurrentNodes.find(r) == CurrentNodes.end())
        {
            CurrentNodes.insert(r);
            allNodes[r] = new Node(r, rx, ry);
            if (r != 0)
                allNodes[r]->setHeuristic(dimensionX - 1, dimensionY - 1);
        }
    }
}

void Grid::printPath()
{
    int DestinationNumber;
    for (auto it : allNodes)
    {
        if (it.second->getx_coordinate() == dimensionX - 1 && it.second->gety_coordinate() == dimensionY - 1)
        {
            DestinationNumber = it.first; // to backtrack
            break;
        }
    }
    vector<int> path;
    while (DestinationNumber != 0)
    {
        path.push_back(DestinationNumber);
        DestinationNumber = allNodes[DestinationNumber]->getParentNumber();
    }
    path.push_back(0);
    reverse(path.begin(), path.end());

    for (auto it : path)
    {
        cout << it << " ";
    }
    cout << endl;
}

priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> HeapFCost;
queue<pair<double, int>> insertThis;

int main()
{
    int dimX, dimY, noNodes, noEdges;
    cin >> dimX;
    cin >> dimY;
    cin >> noNodes;
    cin >> noEdges;

    Grid maze(dimX, dimY, noNodes);
    maze.FillGrid(noEdges);
    maze.allNodes[0]->setg(0);
    maze.allNodes[0]->setf();
    HeapFCost.push({maze.allNodes[0]->getf(), 0});

    // Serial region for processing the graph
    while (true)
    {
        pair<double, int> current;
        if (!HeapFCost.empty())
        {
            current = HeapFCost.top();
            HeapFCost.pop();
        }
        else
        {
            current = {-1, -1}; // Sentinel value to indicate no work
        }

        // If no work, exit the loop
        if (current.second == -1)
            break;

        double curr = current.first;
        int node = current.second;

        for (int i = 0; i < noNodes; ++i)
        {
            if (maze.myGrid[node][i] != 0 && maze.allNodes[node]->getg() + maze.myGrid[node][i] < maze.allNodes[i]->getg())
            {
                maze.allNodes[i]->setg(maze.allNodes[node]->getg() + maze.myGrid[node][i]);
                maze.allNodes[i]->setf();

                insertThis.push({maze.allNodes[i]->getf(), i});
                maze.allNodes[i]->setParentNumber(node);
            }
        }

        // Push new nodes into the priority queue
        while (true)
        {
            pair<double, int> toInsert;
            if (!insertThis.empty())
            {
                toInsert = insertThis.front();
                insertThis.pop();
            }
            else
            {
                toInsert = {-1, -1}; // Sentinel value
            }

            if (toInsert.second == -1)
                break;

            HeapFCost.push(toInsert);
        }
    }

    // Print the result
    cout << endl;
    maze.printPath();

    return 0;
}
