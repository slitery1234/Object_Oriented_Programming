#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class node
{
public:
    unsigned int sendTo(int destinationID)
    {
        return routing_table[destinationID];
    }
    // Node initial, initial id and routing table
    void initial(int n,int inputid)
    {
        id = inputid;
        // Set memory to routing_table
        routing_table = new int[n];

        // Initial routing_table
        for(int i=0; i<n; i++)
            routing_table[i] = -1;
        routing_table[id] = id;

        return;
    }
    void routing_table_set(int dest,int value)
    {
        routing_table[dest] = value;
        return;
    }
    void debug(int n)
    {
        for(int i=0; i<n; i++)
            cout <<routing_table[i] << " ";
        cout << "\n";
    }
private:
    int *routing_table;
    unsigned int id;
};

// BFS to find route
int BFS(int n,vector<int> graphex[],int start,int dest,int CDS[],node nodes[])
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
        for(int i=0; i<graphex[f].size(); i++)
        {
            if(visited[graphex[f][i]] == 0)
            {
                q.push(graphex[f][i]);
                last_node[graphex[f][i]] = f;
                visited[graphex[f][i]] = 1;
            }
        }
    }

    return last_node[dest];
}

// BFS in three steps to find CDS
void BFS_3(int n,vector<int> graph[],int start,int dest,int CDS[],node nodes[])
{
    queue<int> q;
    int visited[n],last_node[n],level[n];
    for(int i=0; i<n; i++)
    {
        visited[i] = 0;
        last_node[i] = -1;
        level[i] = 0;
    }
    q.push(start);
    visited[start] = 1;

    while(!q.empty())
    {
        int f = q.front();
        q.pop();
        if(f == dest)
            break;
        if(level[f] == 4)
            return;
        for(int i=0; i<graph[f].size(); i++)
        {
            if(visited[graph[f][i]] == 0)
            {
                q.push(graph[f][i]);
                last_node[graph[f][i]] = f;
                visited[graph[f][i]] = 1;
                // Set level to find distance < 3
                level[graph[f][i]] = level[f] + 1;
            }
        }
    }

    int tmp = dest;
    // Set route into routing table
    while(tmp != start)
    {
        CDS[tmp] = 1;
        nodes[tmp].routing_table_set(start,last_node[tmp]);
        tmp = last_node[tmp];
    }
    CDS[start] = 1;
    return;
}

void build_MIS(int n,vector<int> graph[],int MIS[])
{
    int node_active[n];
    // Mark all node active
    for(int i=0; i<n; i++)
        node_active[i] = 1;

    for(int i=0; i<n; i++)
    {
        // If the node is still active
        if(node_active[i] == 1)
        {
            int larger = 0;
            for(int j=0; j<graph[i].size(); j++)
            {
                // Find all active neighbor
                if( (node_active[graph[i][j]] == 1) && (i > graph[i][j]) )
                    larger = 1;
            }
            // If they are all smaller than i
            // Set i as MIS
            if(larger == 0)
            {
                MIS[i] = 1;
                for(int j=0; j<graph[i].size(); j++)
                    node_active[graph[i][j]] = 0;
            }
        }
    }
    return;
}

void build_CDS(int n,vector<int> graph[],int MIS[],int CDS[],node nodes[])
{
    // Do route from a MIS node to another
    for(int i=0;i<n;i++)
    {
        if(MIS[i] == 1)
        {
            for(int j=0;j<n;j++)
            {
                if(MIS[j] == 1)
                {
                    BFS_3(n,graph,i,j,CDS,nodes);
                }
            }
        }
    }
    return;
}
// If the node is not in CDS
// Set its proxy node
void set_proxy(int n,vector<int> graph[],int CDS[],node nodes[])
{
    for(int i=0;i<n;i++)
    {
        if(CDS[i] == 0)
        {
            int proxy = n;
            // Find the smallest node in CDS as its proxy
            for(int j=0;j<graph[i].size();j++)
            {
                if(proxy > graph[i][j] && CDS[graph[i][j]] == 1)
                    proxy = graph[i][j];
            }
            // Set their routing table as their proxy
            // Then, Set every node's routing table to them as to their proxy
            for(int j=0;j<n;j++)
            {
                if(nodes[i].sendTo(j) == -1)
                    nodes[i].routing_table_set(j,proxy);
                if(nodes[j].sendTo(i) == -1)
                {
                    int tmp = BFS(n,graph,proxy,j,CDS,nodes);
                    nodes[j].routing_table_set(i,tmp);
                }
            }
        }
    }
}

// To avoid BFS into a node which is not CDS
void kill_graph(int n,vector<int> graph[],vector<int> graphex[],int CDS[])
{
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<graph[i].size();j++)
        {
            if(CDS[graph[i][j]] == 1)
                graphex[i].push_back(graph[i][j]);
        }
    }
    return;
}

void set_routing_table(int n,vector<int> graph[],node nodes[])
{
    // Initial MIS and CDS
    int MIS[n],CDS[n];
    for(int i=0;i<n;i++)
    {
        MIS[i] = 0;
        CDS[i] = 0;
    }
    build_MIS(n,graph,MIS);
    build_CDS(n,graph,MIS,CDS,nodes);

    vector<int> graphex[n];
    kill_graph(n,graph,graphex,CDS);
    set_proxy(n,graphex,CDS,nodes);

    // Find if there has routing table which hasn't been set
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(nodes[i].sendTo(j) == -1)
            {
                int tmp = BFS(n,graphex,j,i,CDS,nodes);
                nodes[i].routing_table_set(j,tmp);
            }
        }
    }
/*
    for(int i=0;i<n;i++)
        nodes[i].debug(n);
*/
    return;

}


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
    // Initial nodes
    for(int i=0; i<n; i++)
        nodes[i].initial(n,i);

    set_routing_table(n,graph,nodes);


    for(int i=0; i<n; i++)
        nodes[i].debug(n);


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
13 15
0 0 3
1 0 4
2 0 6
3 0 9
4 1 6
5 1 5
6 1 8
7 1 11
8 2 10
9 4 5
10 3 7
11 7 8
12 7 12
13 9 10
14 10 11
*/
/*
9 9
0 0 1
1 0 2
2 0 3
3 2 4
4 4 7
5 7 6
6 5 8
7 5 4
8 6 8
*/
/*
7 6
0 0 1
1 1 2
2 0 4
3 4 6
4 6 3
5 6 5
*/
