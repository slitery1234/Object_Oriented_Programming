#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// Use queue to implement BFS
// Save information at last_node
int BFS(int n,vector<int> graph[],int start,int dest)
{
    queue<int> q;
    int visited[n],last_node[n];
    for(int i=0; i<n; i++)
    {
        visited[i] = 0;
        last_node[i] = -1;
    }
    q.push(start);
    visited[start] = 1;

    while(!q.empty())
    {
        int f = q.front();
        q.pop();
        if(f == dest)
            break;
        for(int i=0; i<graph[f].size(); i++)
        {
            if(visited[graph[f][i]] == 0)
            {
                q.push(graph[f][i]);
                last_node[graph[f][i]] = f;
                visited[graph[f][i]] = 1;
            }
        }
    }
    return last_node[dest];
}

class node
{
public:
    unsigned int sendTo(int destinationID)
    {
        return routing_table[destinationID];
    }
    // Make routing_table when node initial
    void initial(int n,int inputid,vector<int> graph[])
    {
        id = inputid;
        // Set memory to routing_table
        routing_table = new int[n];
        // Set routing_table[i]
        for(int i=0; i<n; i++)
        {
            if(i != id)
                routing_table[i] = BFS(n,graph,i,id);
            else
                routing_table[i] = i;
        }
        return;
    }

private:
    int *routing_table;
    unsigned int id;
};

int main()
{
    int n,links;
    cin >> n >> links;
    // Read input links
    int linkID,nodeA,nodeB;
    vector<int> graph[n];
    for(int i=0; i<links; i++)
    {
        cin >> linkID >> nodeA >> nodeB;
        // Save the node i's neighbor in graph[i]
        graph[nodeA].push_back(nodeB);
        graph[nodeB].push_back(nodeA);
    }
    node nodes[n];
    // Initial nodes and build routing_table
    for(int i=0;i<n;i++)
        nodes[i].initial(n,i,graph);

    // Read input flows
    int flows;
    cin >> flows;
    int flowID,source,dest;
    for(int i=0; i<flows; i++)
    {
        cin >> flowID >> source >> dest;
        cout << flowID << " ";

        // Print answer
        while(source != dest)
        {
            cout << source << " ";
            source = nodes[source].sendTo(dest);
        }
        cout << dest << "\n";
    }
    return 0;
}

/*
6 8
0 0 1
1 0 3
2 1 3
3 1 5
4 2 3
5 2 4
6 2 5
7 4 5
*/
