/*
╔══════════════════════════════════════════════════════════════════════════════╗
║      SMART CITY TRAFFIC & ROAD NETWORK ANALYZER  [HUMAN EDITION v3.0]        ║
║      Competitive Programming Lab — JIIT Noida                                ║
║                                                                              ║
║                                                                              ║
║                                                                              ║
╠══════════════════════════════════════════════════════════════════════════════╣
║                                                                              ║
║  WHAT THIS PROJECT DOES (In Simple Words):                                   ║
║  ──────────────────────────────────────────────────────────────────────      ║
║  Imagine you are the traffic controller of Delhi. You have 12 real           ║
║  locations — Connaught Place, India Gate, AIIMS, IGI Airport and more.       ║
║  Roads connect them. People travel. Accidents happen. VIPs move.             ║
║  Ambulances rush. Signals blink. Jams build up.                              ║
║                                                                              ║
║  This system handles ALL of it — using graph algorithms from your CP lab.    ║
║  Every feature here maps a real-world traffic problem to an algorithm.       ║
║                                                                              ║
║  ALGORITHMS → REAL WORLD MAPPING:                                            ║
║  ──────────────────────────────────────────────────────────────────────      ║
║  Dijkstra        O((V+E)logV)  → "What is the fastest route right now?"      ║
║  Bellman-Ford    O(V*E)        → "Is there any path at all? Check again."    ║
║  BFS             O(V+E)        → "Where is the nearest hospital from here?"  ║
║  DFS/FloodFill   O(V+E)        → "Which areas got cut off after accident?"   ║
║  Tarjan's Algo   O(V+E)        → "Which road, if broken, splits the city?"   ║
║  Kruskal MST     O(E log E)    → "Minimum roads to connect entire city?"     ║
║  DSU UnionFind   O(alpha V)    → "Are two locations even connected?"         ║
║  Topological Sort O(V+E)       → "In what order should signals turn green?"  ║
║  Binary Search   O(log E)      → "What is the smoothest possible path?"      ║
║  Prefix Sum      O(1) query    → "How much traffic between 8am and 10am?"    ║
║  Sliding Window  O(N)          → "Which 3-hour window has peak traffic?"     ║
║  GCD / LCM       O(log n)      → "When do all signals sync together?"        ║
║  Bitmask         O(1)          → "Which signals are currently green/red?"    ║
╚══════════════════════════════════════════════════════════════════════════════╝
*/

#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <map>
#include <iomanip>
#include <functional>
#include <chrono>
#include <string>
using namespace std;

long long custom_gcd(long long a, long long b) {
    return b == 0 ? a : custom_gcd(b, a % b);
}

// ─────────────────────────────────────────────
// COLORS
// ─────────────────────────────────────────────
#define INF  1000000000
#define LINF 1000000000000LL

const string R  = "\033[1;31m";   // Red
const string G  = "\033[1;32m";   // Green
const string Y  = "\033[1;33m";   // Yellow
const string B  = "\033[1;34m";   // Blue
const string M  = "\033[1;35m";   // Magenta
const string C  = "\033[1;36m";   // Cyan
const string W  = "\033[1;37m";   // White
const string BO = "\033[1m";      // Bold
const string DM = "\033[2m";      // Dim
const string RS = "\033[0m";      // Reset
const string BGR= "\033[41m";     // BG Red
const string BGG= "\033[42m";     // BG Green
const string BGY= "\033[43m";     // BG Yellow
const string BGM= "\033[45m";     // BG Magenta
const string BGC= "\033[46m";     // BG Cyan

// ─────────────────────────────────────────────
// GLOBAL USER INFO (set at login)
// ─────────────────────────────────────────────
string OPERATOR_NAME = "Officer";
string CITY_NAME     = "New Delhi";
int    SESSION_OPS   = 0; // how many operations done this session

// ─────────────────────────────────────────────
// UTILITY: Typing effect (makes output feel alive)
// ─────────────────────────────────────────────
void typeOut(const string& msg, int delayMs = 18) {
    for (char ch : msg) {
        cout << ch << flush;
        // simple busy wait (portable)
        auto start = chrono::steady_clock::now();
        while (chrono::duration_cast<chrono::milliseconds>(
               chrono::steady_clock::now() - start).count() < delayMs) {}
    }
    cout << "\n";
}

void pause(int ms = 400) {
    auto start = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(
           chrono::steady_clock::now() - start).count() < ms) {}
}

void section(const string& title) {
    cout << "\n" << C << BO
         << "  ╔══════════════════════════════════════════════════╗\n"
         << "  ║  " << left << setw(48) << title << "║\n"
         << "  ╚══════════════════════════════════════════════════╝\n"
         << RS;
}

void info(const string& msg)    { cout << C  << "  [i] " << RS << msg << "\n"; }
void success(const string& msg) { cout << G  << "  [+] " << RS << msg << "\n"; }
void warn(const string& msg)    { cout << Y  << "  [!] " << RS << msg << "\n"; }
void err(const string& msg)     { cout << R  << "  [x] " << RS << msg << "\n"; }
void alert(const string& msg)   { cout << BGR<< W << "  *** " << msg << " ***" << RS << "\n"; }
void good(const string& msg)    { cout << BGG<< W << "  >>> " << msg << " <<<" << RS << "\n"; }
void vip(const string& msg)     { cout << BGM<< W << "  VIP " << msg << RS << "\n"; }

// ─────────────────────────────────────────────
// NODE TYPES
// ─────────────────────────────────────────────
enum NodeType { INTERSECTION=0, HOSPITAL, FIRE_STATION, POLICE, VIP_ZONE, METRO };
string NT[] = {"Chowk","Hospital","Fire Station","Police Station","VIP Zone","Metro Station"};
string NT_EMOJI[] = {"🔀","🏥","🚒","👮","⭐","🚇"};

// ─────────────────────────────────────────────
// DELHI ROAD NETWORK PRESET
// 12 real Delhi locations, index 0-11
// ─────────────────────────────────────────────
struct DelhiPreset {
    int n = 12;
    vector<string> names = {
        "Connaught Place",   // 0  — city center
        "India Gate",        // 1  — landmark
        "AIIMS",             // 2  — hospital
        "IGI Airport",       // 3  — airport
        "Dwarka Sector 21",  // 4  — west Delhi
        "Nehru Place",       // 5  — south Delhi
        "Lajpat Nagar",      // 6  — market
        "Karol Bagh",        // 7  — market
        "ITO Chowk",         // 8  — central
        "Akshardham",        // 9  — east Delhi
        "Rohini Sector 18",  // 10 — north Delhi
        "Saket District Centre" // 11 — south Delhi
    };
    vector<NodeType> types = {
        INTERSECTION, INTERSECTION, HOSPITAL,
        VIP_ZONE, INTERSECTION, INTERSECTION,
        INTERSECTION, INTERSECTION, INTERSECTION,
        INTERSECTION, INTERSECTION, METRO
    };
    // {u, v, travel_time_minutes}
    vector<tuple<int,int,int>> roads = {
        {0,1,8},  {0,7,10}, {0,8,7},  {0,10,25},
        {1,2,12}, {1,5,15}, {1,8,10},
        {2,5,8},  {2,11,14},
        {3,4,20}, {3,5,35},
        {4,7,18}, {4,10,22},
        {5,6,9},  {5,11,10},
        {6,8,13}, {6,11,11},
        {7,8,8},  {7,10,20},
        {8,9,16},
        {9,10,28},
        {10,11,30}
    };
};

// ─────────────────────────────────────────────
// ROAD STRUCT
// ─────────────────────────────────────────────
struct Road {
    int to, weight, baseWeight;
    bool blocked, isVIPOnly;
};

// ─────────────────────────────────────────────
// ROAD NETWORK
// ─────────────────────────────────────────────
struct RoadNetwork {
    int n;
    vector<vector<Road>> adj;
    vector<tuple<int,int,int>> edgeList;
    vector<NodeType> nodeType;
    vector<string> nodeName;

    RoadNetwork() : n(0) {}
    RoadNetwork(int n) : n(n), adj(n), nodeType(n, INTERSECTION), nodeName(n) {
        for (int i = 0; i < n; i++) nodeName[i] = "Node_" + to_string(i);
    }

    void setNode(int u, NodeType t, string name) {
        nodeType[u] = t; nodeName[u] = name;
        info("Location " + BO + name + RS + " registered as " + NT[t]);
    }

    bool addRoad(int u, int v, int w, bool vipOnly = false, bool silent = false) {
        if (u<0||v<0||u>=n||v>=n||u==v) { err("Invalid nodes!"); return false; }
        adj[u].push_back({v,w,w,false,vipOnly});
        adj[v].push_back({u,w,w,false,vipOnly});
        edgeList.push_back({u,v,w});
        if (!silent)
            success("Road: " + BO + nodeName[u] + RS + " <-> " + BO + nodeName[v] +
                    RS + " | " + to_string(w) + " min" + (vipOnly?" [VIP ONLY]":""));
        return true;
    }

    void removeRoad(int u, int v) {
        auto rem = [&](int a, int b){
            auto& l = adj[a];
            l.erase(remove_if(l.begin(),l.end(),[b](const Road& r){return r.to==b;}),l.end());
        };
        rem(u,v); rem(v,u);
        edgeList.erase(remove_if(edgeList.begin(),edgeList.end(),
            [u,v](auto& e){return(get<0>(e)==u&&get<1>(e)==v)||(get<0>(e)==v&&get<1>(e)==u);}),
            edgeList.end());
        warn("Road removed: " + nodeName[u] + " <-> " + nodeName[v]);
    }

    void blockRoad(int u, int v, bool block) {
        for (auto& r:adj[u]) if(r.to==v) r.blocked=block;
        for (auto& r:adj[v]) if(r.to==u) r.blocked=block;
        if (block) alert("ROAD BLOCKED: " + nodeName[u] + " <-> " + nodeName[v]);
        else       good("ROAD CLEARED: " + nodeName[u] + " <-> " + nodeName[v]);
    }

    void updateWeight(int u, int v, int w) {
        for (auto& r:adj[u]) if(r.to==v) r.weight=w;
        for (auto& r:adj[v]) if(r.to==u) r.weight=w;
        warn("Traffic jam on " + nodeName[u] + " <-> " + nodeName[v] +
             " — travel time now " + to_string(w) + " min");
    }

    void clearAllJams() {
        for (int u=0;u<n;u++) for(auto& r:adj[u]){r.weight=r.baseWeight;r.blocked=false;}
        good("ALL ROADS CLEARED — City traffic restored to normal!");
    }

    void display() {
        section("DELHI ROAD NETWORK — Live Status");
        for (int i=0;i<n;i++) {
            cout << "  " << BO << "[" << i << "] " << nodeName[i] << RS
                 << DM << "  (" << NT[nodeType[i]] << ")" << RS << "\n";
            if (adj[i].empty()) { cout << "       └── (no roads)\n"; continue; }
            for (auto& r:adj[i]) {
                string st = r.blocked ? R+"[BLOCKED]"+RS :
                            r.weight>r.baseWeight ? Y+"[JAM "+to_string(r.weight)+"min]"+RS
                                                  : G+"[OK]"+RS;
                cout << "       └── " << nodeName[r.to] << " | "
                     << r.weight << "min " << st
                     << (r.isVIPOnly ? M+" [VIP]"+RS : "") << "\n";
            }
        }
    }
};

// ─────────────────────────────────────────────
// DSU
// Time Complexity: find O(α(n)) ≈ O(1), unite O(α(n))
// Space: O(V)
// ─────────────────────────────────────────────
struct DSU {
    vector<int> par, rnk;
    int comp;
    DSU(int n):par(n),rnk(n,0),comp(n){ iota(par.begin(),par.end(),0); }
    int find(int x){ return par[x]==x?x:par[x]=find(par[x]); }
    bool unite(int x,int y){
        int px=find(x),py=find(y);
        if(px==py) return false;
        if(rnk[px]<rnk[py]) swap(px,py);
        par[py]=px;
        if(rnk[px]==rnk[py]) rnk[px]++;
        comp--; return true;
    }
    bool connected(int u,int v){ return find(u)==find(v); }
    void rebuild(RoadNetwork& net){
        fill(par.begin(),par.end(),0); iota(par.begin(),par.end(),0);
        fill(rnk.begin(),rnk.end(),0); comp=net.n;
        for(int u=0;u<net.n;u++) for(auto& r:net.adj[u]) if(!r.blocked) unite(u,r.to);
    }
    void showComponents(int n, vector<string>& names){
        map<int,vector<int>> mp;
        for(int i=0;i<n;i++) mp[find(i)].push_back(i);
        section("CONNECTED REGIONS — DSU Analysis");
        info("Time Complexity: O(alpha(V)) per query — practically O(1)");
        int idx=1;
        for(auto& [r,nodes]:mp){
            cout << "  Region " << BO << idx++ << RS << ": { ";
            for(int v:nodes) cout << G << names[v] << RS << "  ";
            cout << "}\n";
        }
        cout << "\n  Total isolated regions: " << BO << mp.size() << RS << "\n";
    }
};

// ─────────────────────────────────────────────
// DIJKSTRA
// Time: O((V+E) log V)   Space: O(V)
// ─────────────────────────────────────────────
struct DRes { vector<int> dist, par; };

DRes dijkstra(int src, int n, vector<vector<Road>>& adj,
              bool ignoreBlock=false, bool vipMode=false, bool greenMode=false){
    vector<int> dist(n,INF), par(n,-1);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<>> pq;
    dist[src]=0; pq.push({0,src});
    while(!pq.empty()){
        auto [d,u]=pq.top(); pq.pop();
        if(d>dist[u]) continue;
        for(auto& r:adj[u]){
            if(r.blocked && !ignoreBlock) continue;
            int w = greenMode ? 0 : (vipMode ? max(1,r.weight/2) : r.weight);
            if(dist[u]+w < dist[r.to]){
                dist[r.to]=dist[u]+w;
                par[r.to]=u;
                pq.push({dist[r.to],r.to});
            }
        }
    }
    return {dist,par};
}

vector<int> getPath(int src, int dst, vector<int>& par){
    vector<int> p;
    for(int v=dst;v!=-1;v=par[v]) p.push_back(v);
    reverse(p.begin(),p.end());
    return (p.empty()||p[0]!=src) ? vector<int>{} : p;
}

void printPath(vector<int>& path, vector<string>& names, const string& color=G){
    for(int i=0;i<(int)path.size();i++){
        cout << color << BO << names[path[i]] << RS;
        if(i+1<(int)path.size()) cout << C << " ──> " << RS;
    }
    cout << "\n";
}

// ─────────────────────────────────────────────
// BELLMAN-FORD
// Time: O(V*E)   Space: O(V)
// ─────────────────────────────────────────────
void bellmanFord(int src, int n, vector<tuple<int,int,int>>& edges, vector<string>& names){
    section("BELLMAN-FORD — All Distances (Robust Check)");
    info("Time Complexity: O(V * E) = O(" + to_string(n) + " * " + to_string(edges.size()) + ")");
    info("Bellman-Ford is slower but guarantees correctness even with tricky road weights.");

    vector<long long> dist(n,LINF);
    dist[src]=0;
    for(int i=0;i<n-1;i++)
        for(auto& [u,v,w]:edges){
            if(dist[u]!=LINF && dist[u]+w<dist[v]) dist[v]=dist[u]+w;
            if(dist[v]!=LINF && dist[v]+w<dist[u]) dist[u]=dist[v]+w;
        }
    bool neg=false;
    for(auto& [u,v,w]:edges)
        if(dist[u]!=LINF && dist[u]+w<dist[v]) neg=true;

    if(neg) alert("NEGATIVE CYCLE DETECTED! Results may be unreliable.");
    else    success("No negative cycles. All distances verified.");

    cout << "\n  Distances from " << BO << names[src] << RS << ":\n";
    for(int i=0;i<n;i++){
        cout << "  " << setw(28) << left << names[i] << " : ";
        if(dist[i]==LINF) cout << R << "Unreachable\n" << RS;
        else cout << G << dist[i] << " min\n" << RS;
    }
}

// ─────────────────────────────────────────────
// BFS — Nearest Emergency Service
// Time: O(V+E)   Space: O(V)
// ─────────────────────────────────────────────
void bfsNearest(int src, NodeType tgt, int n, vector<vector<Road>>& adj,
                vector<NodeType>& nt, vector<string>& names){
    section("BFS — Nearest " + NT[tgt] + " by Hops");
    info("Time Complexity: O(V+E) — explores level by level");
    info("Think of it like ripples in water spreading outward from " + names[src]);

    vector<bool> vis(n,false);
    queue<pair<int,int>> q;
    q.push({src,0}); vis[src]=true;
    while(!q.empty()){
        auto [u,hops]=q.front(); q.pop();
        if(nt[u]==tgt && u!=src){
            good("Found " + NT[tgt] + ": " + names[u] + " — only " + to_string(hops) + " stop(s) away!");
            return;
        }
        for(auto& r:adj[u]) if(!r.blocked&&!vis[r.to]){ vis[r.to]=true; q.push({r.to,hops+1}); }
    }
    err("No reachable " + NT[tgt] + " found from " + names[src] + "!");
}

void nearestByTime(int src, NodeType tgt, int n, vector<vector<Road>>& adj,
                   vector<NodeType>& nt, vector<string>& names){
    info("Time Complexity: O((V+E) log V) — Dijkstra for minimum travel time");
    auto res=dijkstra(src,n,adj);
    int best=-1, bd=INF;
    for(int i=0;i<n;i++) if(nt[i]==tgt&&i!=src&&res.dist[i]<bd){ bd=res.dist[i]; best=i; }
    if(best==-1){ err("None reachable!"); return; }
    auto path=getPath(src,best,res.par);
    cout << "  Fastest " << NT[tgt] << ": " << BO << names[best] << RS
         << " — " << BO << bd << " min" << RS << " away\n  Route: ";
    printPath(path,names);
}

// ─────────────────────────────────────────────
// GREEN CHANNEL — Emergency Vehicle
// Time: O((V+E) log V)
// All signals pre-cleared. Weight overridden to 0.
// ─────────────────────────────────────────────
void greenChannel(int src, int dst, int n, vector<vector<Road>>& adj, vector<string>& names){
    section("GREEN CHANNEL — Emergency Vehicle Routing");
    typeOut(G + "  Dispatching emergency vehicle from " + names[src] + "..." + RS, 20);
    typeOut(Y + "  Contacting traffic control... clearing all signals ahead..." + RS, 20);
    typeOut(G + "  ALL SIGNALS CLEARED. GREEN CHANNEL ACTIVATED." + RS, 20);
    info("Algorithm: Dijkstra with weight=0 override | Time: O((V+E) log V)");

    auto resGreen = dijkstra(src,n,adj,true,false,true);
    auto resNorm  = dijkstra(src,n,adj,false,false,false);
    auto pathG    = getPath(src,dst,resGreen.par);
    auto pathN    = getPath(src,dst,resNorm.par);

    if(pathG.empty()){ err("NO ROUTE possible even for emergency! City network broken."); return; }

    cout << "\n" << BGG << W << "  EMERGENCY ROUTE (Signals Cleared):  " << RS << "\n  ";
    printPath(pathG, names, G);
    cout << G << "  Estimated arrival: IMMEDIATE (no signal stops)\n" << RS;

    if(!pathN.empty()){
        cout << "\n" << Y << "  Normal fastest route (for reference):\n" << RS << "  ";
        printPath(pathN, names, Y);
        cout << Y << "  Normal travel time: " << resNorm.dist[dst] << " min\n" << RS;
        cout << G << "  Time saved by Green Channel: " << BO << resNorm.dist[dst]
             << " minutes — could save a life!\n" << RS;
    }
    SESSION_OPS++;
}

// ─────────────────────────────────────────────
// VIP ROUTE — Priority Lane
// Time: O((V+E) log V)
// 50% weight reduction, VIP-only roads unlocked
// ─────────────────────────────────────────────
void vipRoute(int src, int dst, int n, vector<vector<Road>>& adj, vector<string>& names){
    section("VIP ROUTE — Priority Lane Activated");
    typeOut(M + "  VIP convoy detected. Activating priority corridor..." + RS, 20);
    typeOut(M + "  Priority lanes opened. Speed restrictions lifted." + RS, 20);
    info("Algorithm: Dijkstra with 50% weight reduction | Time: O((V+E) log V)");

    auto resVIP  = dijkstra(src,n,adj,false,true,false);
    auto resNorm = dijkstra(src,n,adj,false,false,false);
    auto pathV   = getPath(src,dst,resVIP.par);
    auto pathN   = getPath(src,dst,resNorm.par);

    if(pathV.empty()){ err("No VIP route available!"); return; }

    cout << "\n" << BGM << W << "  VIP PRIORITY ROUTE:  " << RS << "\n  ";
    printPath(pathV, names, M);
    cout << M << "  VIP travel time: " << BO << resVIP.dist[dst] << " min\n" << RS;

    if(!pathN.empty()){
        int saved = resNorm.dist[dst] - resVIP.dist[dst];
        cout << Y << "  Normal travel time: " << resNorm.dist[dst] << " min\n" << RS;
        cout << M << "  Priority advantage: " << BO << saved << " min faster!\n" << RS;
    }
    SESSION_OPS++;
}

// ─────────────────────────────────────────────
// TARJAN'S ALGORITHM
// Time: O(V+E)   Space: O(V)
// ─────────────────────────────────────────────
struct Tarjan {
    int n, timer;
    vector<vector<Road>>& adj;
    vector<int> disc, low, par;
    vector<bool> vis, isAP;
    vector<pair<int,int>> bridges;

    Tarjan(int n, vector<vector<Road>>& adj)
        : n(n),timer(0),adj(adj),disc(n,-1),low(n,-1),
          par(n,-1),vis(n,false),isAP(n,false){}

    void dfs(int u){
        vis[u]=true; disc[u]=low[u]=timer++;
        int ch=0;
        for(auto& r:adj[u]){
            if(r.blocked) continue;
            int v=r.to;
            if(!vis[v]){
                ch++; par[v]=u; dfs(v);
                low[u]=min(low[u],low[v]);
                if(low[v]>disc[u]) bridges.push_back({u,v});
                if((par[u]==-1&&ch>1)||(par[u]!=-1&&low[v]>=disc[u])) isAP[u]=true;
            } else if(v!=par[u]) low[u]=min(low[u],disc[v]);
        }
    }

    void run(vector<string>& names){
        section("TARJAN'S ALGORITHM — Critical Infrastructure");
        info("Time Complexity: O(V+E) — single DFS pass");
        typeOut(C + "  Scanning entire road network for vulnerabilities..." + RS, 15);

        for(int i=0;i<n;i++) if(!vis[i]) dfs(i);

        cout << "\n" << R << "  CRITICAL ROADS (Bridges):\n" << RS;
        cout << DM << "  These roads, if damaged/blocked, will disconnect parts of the city.\n" << RS;
        if(bridges.empty()) good("GREAT NEWS! No single road can disconnect the city.");
        else for(auto [u,v]:bridges)
            alert("CRITICAL: " + names[u] + " <-> " + names[v] + "  [SINGLE POINT OF FAILURE]");

        cout << "\n" << R << "  CRITICAL INTERSECTIONS (Articulation Points):\n" << RS;
        cout << DM << "  Blocking these intersections will split the city into disconnected zones.\n" << RS;
        bool found=false;
        for(int i=0;i<n;i++) if(isAP[i]){
            alert("CRITICAL JUNCTION: " + names[i] + "  [DO NOT BLOCK]");
            found=true;
        }
        if(!found) good("No critical junctions. City has redundant paths everywhere.");
    }
};

// ─────────────────────────────────────────────
// KRUSKAL MST
// Time: O(E log E)   Space: O(V)
// ─────────────────────────────────────────────
void kruskalMST(int n, vector<tuple<int,int,int>>& edges, vector<string>& names){
    section("KRUSKAL'S MST — Minimum Road Infrastructure");
    info("Time Complexity: O(E log E)");
    typeOut(C + "  Finding the cheapest way to keep all of " + CITY_NAME + " connected..." + RS, 15);

    auto sorted = edges;
    sort(sorted.begin(),sorted.end(),[](auto& a,auto& b){return get<2>(a)<get<2>(b);});
    DSU dsu(n); int total=0, used=0;

    cout << "\n  Minimum roads needed:\n";
    for(auto& [u,v,w]:sorted){
        if(dsu.unite(u,v)){
            success(names[u] + " <-> " + names[v] + " | " + to_string(w) + " min");
            total+=w; used++;
        }
    }
    cout << "\n  " << BO << "Total MST cost : " << total << " min\n" << RS;
    cout << "  " << BO << "Roads used     : " << used << "/" << (n-1) << "\n" << RS;

    if(used<n-1)
        alert("CITY NOT FULLY CONNECTED! " + to_string(n-1-used) + " more roads needed.");
    else
        good("Entire city connected with minimum possible infrastructure!");
}

// ─────────────────────────────────────────────
// DSU ZONE DETECTION (DFS Flood Fill)
// Time: O(V+E)
// ─────────────────────────────────────────────
void detectZones(int n, vector<vector<Road>>& adj, vector<string>& names){
    section("ZONE ISOLATION — DFS Flood Fill");
    info("Time Complexity: O(V+E)");
    typeOut(C + "  Scanning city for isolated zones after road changes..." + RS, 15);

    vector<int> zone(n,-1); int zid=0;
    function<void(int,int)> dfs=[&](int u,int z){
        zone[u]=z;
        for(auto& r:adj[u]) if(!r.blocked&&zone[r.to]==-1) dfs(r.to,z);
    };
    for(int i=0;i<n;i++) if(zone[i]==-1) dfs(i,zid++);

    map<int,vector<int>> zones;
    for(int i=0;i<n;i++) zones[zone[i]].push_back(i);

    cout << "  Detected " << BO << zid << RS << " zone(s):\n";
    for(auto& [id,nodes]:zones){
        cout << "  Zone " << BO << id+1 << RS << ": { ";
        for(int v:nodes) cout << G << names[v] << RS << "  ";
        cout << "}\n";
    }
    if(zid>1)
        alert(to_string(zid) + " ISOLATED ZONES DETECTED! Parts of city cut off!");
    else
        good("City fully connected. No isolated zones.");
}

// ─────────────────────────────────────────────
// BINARY SEARCH — Bottleneck
// Time: O(E log E + E·alpha(V))
// ─────────────────────────────────────────────
int minBottleneck(int n, vector<tuple<int,int,int>>& edges, int src, int dst){
    vector<int> ws;
    for(auto& [u,v,w]:edges) ws.push_back(w);
    sort(ws.begin(),ws.end());
    ws.erase(unique(ws.begin(),ws.end()),ws.end());
    int lo=0,hi=(int)ws.size()-1,ans=-1;
    while(lo<=hi){
        int mid=(lo+hi)/2;
        DSU dsu(n);
        for(auto& [u,v,w]:edges) if(w<=ws[mid]) dsu.unite(u,v);
        if(dsu.connected(src,dst)){ ans=ws[mid]; hi=mid-1; }
        else lo=mid+1;
    }
    return ans;
}

// ─────────────────────────────────────────────
// SIGNAL SCHEDULER — Topological Sort (Kahn's)
// Time: O(V+E)
// ─────────────────────────────────────────────
struct SignalScheduler {
    int n;
    vector<vector<int>> dag;
    vector<string> sigNames;
    SignalScheduler(int n):n(n),dag(n),sigNames(n){
        for(int i=0;i<n;i++) sigNames[i]="Signal_"+to_string(i);
    }
    void addDep(int a, int b){
        dag[a].push_back(b);
        info(sigNames[a] + " must turn green BEFORE " + sigNames[b]);
    }
    void schedule(){
        section("TRAFFIC SIGNAL SCHEDULE — Topological Sort");
        info("Time Complexity: O(V+E) — Kahn's BFS algorithm");
        typeOut(C + "  Computing optimal signal activation sequence..." + RS, 15);

        vector<int> ind(n,0);
        for(int u=0;u<n;u++) for(int v:dag[u]) ind[v]++;
        queue<int> q;
        for(int i=0;i<n;i++) if(ind[i]==0) q.push(i);
        vector<int> order;
        while(!q.empty()){
            int u=q.front(); q.pop(); order.push_back(u);
            for(int v:dag[u]) if(--ind[v]==0) q.push(v);
        }
        if((int)order.size()!=n){
            alert("CIRCULAR DEPENDENCY! Two signals waiting for each other. DEADLOCK!");
            return;
        }
        cout << "\n  Activation Sequence:\n  ";
        for(int i=0;i<(int)order.size();i++){
            cout << BGG << W << " " << sigNames[order[i]] << " " << RS;
            if(i+1<(int)order.size()) cout << C << " --> " << RS;
        }
        cout << "\n";
        good("Signal schedule computed! No deadlocks detected.");
    }
};

// ─────────────────────────────────────────────
// ANALYTICS — Prefix Sum + Sliding Window
// ─────────────────────────────────────────────
struct Analytics {
    vector<int> load;
    int n;
    vector<long long> prefix;
    Analytics(vector<int>& d):load(d),n((int)d.size()),prefix(d.size()+1,0){
        for(int i=0;i<n;i++) prefix[i+1]=prefix[i]+load[i];
    }
    long long rangeSum(int l,int r){ return prefix[r+1]-prefix[l]; }

    void heatmap(){
        section("24-HOUR TRAFFIC HEATMAP — " + CITY_NAME);
        info("Prefix Sum precomputed in O(N). Each range query now O(1).");
        cout << "\n  " << BO << "Hour  Load    Visual Bar\n" << RS;
        cout << "  " << string(55,'-') << "\n";
        int mx=*max_element(load.begin(),load.end());
        for(int i=0;i<n;i++){
            int bars=(int)(35.0*load[i]/mx);
            string col = load[i]>mx*0.75 ? R :
                         load[i]>mx*0.5  ? Y : G;
            string label = (i<10?" ":"")+to_string(i)+":00";
            cout << "  " << label << "  " << setw(5) << load[i] << "  "
                 << col << string(bars,'|') << RS << "\n";
        }
    }

    void congestionWindow(int K){
        if(K>n){ err("Window size too large!"); return; }
        long long mx=0; int best=0;
        for(int i=0;i+K<=n;i++){
            long long s=rangeSum(i,i+K-1);
            if(s>mx){ mx=s; best=i; }
        }
        section("PEAK CONGESTION WINDOW (K=" + to_string(K) + " hours)");
        info("Time Complexity: O(N) sliding window using prefix sums");
        typeOut(Y + "  Scanning all " + to_string(K) + "-hour windows for peak load..." + RS, 15);
        cout << "\n  Peak window    : " << BO << best << ":00 to " << (best+K-1) << ":00\n" << RS;
        cout << "  Total vehicles : " << BO << mx << "\n" << RS;
        cout << "  Avg per hour   : " << BO << fixed << setprecision(1) << (double)mx/K << "\n" << RS;
        if(mx > 1500) alert("SEVERE CONGESTION ALERT! Authorities should be notified.");
        else if(mx > 800) warn("Moderate congestion in this window. Monitor closely.");
        else good("Traffic levels manageable in this window.");
    }

    void signalBitmask(int mask, int count){
        section("SIGNAL STATES — Bitmask Control");
        info("Bitmask = " + to_string(mask) + "  (each bit = 1 signal)");
        info("Time Complexity: O(1) per signal check");
        for(int i=0;i<count;i++){
            bool grn=(mask>>i)&1;
            cout << "  Signal " << setw(2) << i << " : "
                 << (grn ? BGG+W+" GREEN " : BGR+W+" RED   ") << RS
                 << (grn ? G+"  (vehicles may proceed)" : R+"  (stop)")
                 << RS << "\n";
        }
    }

    void gcdLcmCycles(vector<int>& cyc){
        section("SIGNAL CYCLE SYNC — GCD & LCM");
        info("GCD: longest common base interval. LCM: when all signals fully sync.");
        info("Time Complexity: O(log min(a,b)) per pair");
        int g = cyc[0];
        for(int c : cyc) g = custom_gcd(g, c);
        long long l = cyc[0];
        for(int c : cyc) l = l / custom_gcd(l, (long long)c) * c;
        cout << "\n  GCD of all cycles : " << BO << g << " seconds\n" << RS;
        cout << "  LCM (full sync)   : " << BO << l << " seconds (" << l/60 << " min)\n" << RS;
        typeOut(G + "  All signals will be perfectly synced every " +
                to_string(l) + " seconds." + RS, 15);
    }
};

// ─────────────────────────────────────────────
// ACCIDENT SIMULATION
// ─────────────────────────────────────────────
void simulateAccident(RoadNetwork& net, DSU& dsu){
    section("ACCIDENT SIMULATION");
    typeOut(R + "  BREAKING: An accident has been reported in " + CITY_NAME + "!" + RS, 20);
    int u,v;
    cout << "  Enter the road involved (node_id1 node_id2): ";
    cin >> u >> v;
    if(u<0||u>=net.n||v<0||v>=net.n){ err("Invalid nodes!"); return; }
    net.blockRoad(u,v,true);
    dsu.rebuild(net);
    typeOut(Y + "  Rerouting traffic... analyzing impact..." + RS, 20);
    detectZones(net.n, net.adj, net.nodeName);
    info("Use option [9] to find new shortest paths around the accident.");
    SESSION_OPS++;
}

// ─────────────────────────────────────────────
// JAM SIMULATION
// ─────────────────────────────────────────────
void simulateJam(RoadNetwork& net){
    section("TRAFFIC JAM SIMULATOR");
    typeOut(Y + "  Incoming report: heavy traffic on a road segment..." + RS, 15);
    int u,v,factor;
    cout << "  Enter road (u v) and jam multiplier (2=2x slower, 3=3x): ";
    cin >> u >> v >> factor;
    int nw=0;
    for(auto& r:net.adj[u]) if(r.to==v){ nw=r.baseWeight*factor; break; }
    if(nw==0){ err("Road not found!"); return; }
    net.updateWeight(u,v,nw);
    typeOut(Y + "  Traffic jam active! Dijkstra will now route around it." + RS, 15);
    SESSION_OPS++;
}

// ─────────────────────────────────────────────
// TIME COMPLEXITY TABLE
// ─────────────────────────────────────────────
void showComplexities(){
    section("ALGORITHM TIME COMPLEXITY REFERENCE");
    cout << BO << "  " << setw(20) << left << "Algorithm"
         << setw(22) << "Time Complexity"
         << setw(10) << "Space"
         << "Real-World Use\n" << RS;
    cout << "  " << string(70,'-') << "\n";

    vector<tuple<string,string,string,string>> T = {
        {"Dijkstra",       "O((V+E) log V)", "O(V)","Fastest route in real-time"},
        {"Bellman-Ford",   "O(V * E)",       "O(V)","Verifying paths, edge cases"},
        {"BFS",            "O(V+E)",         "O(V)","Nearest hospital by hops"},
        {"DFS/FloodFill",  "O(V+E)",         "O(V)","Zone isolation after block"},
        {"Tarjan's",       "O(V+E)",         "O(V)","Find critical roads/junctions"},
        {"Kruskal MST",    "O(E log E)",     "O(E)","Min infrastructure planning"},
        {"DSU find",       "O(alpha(V))~1",  "O(V)","Connectivity check"},
        {"Topo Sort",      "O(V+E)",         "O(V)","Signal activation order"},
        {"Binary Search",  "O(log E)",       "O(1)","Smooth bottleneck path"},
        {"Prefix Sum",     "O(1) query",     "O(N)","Any hour-range traffic query"},
        {"Sliding Window", "O(N)",           "O(1)","Peak congestion detection"},
        {"GCD/LCM",        "O(log n)",       "O(1)","Signal sync period"},
        {"Bitmask",        "O(1)",           "O(1)","Signal state control"},
    };
    for(auto& [alg,tc,sp,use]:T)
        cout << "  " << Y  << setw(20) << left << alg << RS
             << G  << setw(22) << tc << RS
             << C  << setw(10) << sp << RS
             << DM << use << RS << "\n";
    cout << "\n  " << DM << "V=Vertices(Intersections), E=Edges(Roads), N=Data points, alpha=inverse Ackermann\n" << RS;
}

// ─────────────────────────────────────────────
// LOGIN SCREEN
// ─────────────────────────────────────────────
void loginScreen(){
    cout << "\033[2J\033[H"; // clear screen
    cout << C << BO;
    cout << "\n\n";
    cout << "  ╔══════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                          ║\n";
    cout << "  ║     SMART CITY TRAFFIC & ROAD NETWORK ANALYZER           ║\n";
    cout << "  ║              HUMAN EDITION  v3.0                         ║\n";
    cout << "  ║                                                          ║\n";
    cout << "  ╚══════════════════════════════════════════════════════════╝\n";
    cout << RS;

    pause(300);
    cout << "\n" << Y;
    typeOut("  Welcome to the Smart City Traffic Control System.", 20);
    typeOut("  This system helps manage roads, routes & signals for an entire city.", 15);
    cout << RS;

    cout << "\n" << BO << "  Please enter your name, Officer: " << RS;
    getline(cin >> ws, OPERATOR_NAME);

    cout << "\n" << BO << "  City name [press Enter for 'New Delhi']: " << RS;
    string tmp;
    getline(cin, tmp);
    if (!tmp.empty()) CITY_NAME = tmp;

    typeOut("\n" + G + "  Welcome, Officer " + OPERATOR_NAME + "!", 20);
    typeOut(G + "  You are now managing traffic for " + CITY_NAME + ".", 20);
    typeOut(G + "  The city is counting on you.", 20);
    pause(400);
}

// ─────────────────────────────────────────────
// AUTO DEMO MODE — Delhi City Scenario
// Runs a full pre-scripted scenario automatically
// ─────────────────────────────────────────────
void runDemoMode(RoadNetwork& net, DSU& dsu, SignalScheduler& sched){
    cout << "\033[2J\033[H";
    section("AUTO DEMO MODE — Delhi Smart City Scenario");
    typeOut(C + "  Hello " + OPERATOR_NAME + "! Starting automated demo of ALL features...", 15);
    typeOut(C + "  We will simulate a full day in the life of " + CITY_NAME + "'s traffic system.", 15);
    pause(500);

    // ── STEP 1: Build Delhi network ──
    DelhiPreset dp;
    section("STEP 1: Building Delhi Road Network");
    typeOut(G + "  Loading 12 real Delhi locations and their roads...", 15);
    for(int i=0;i<dp.n;i++) net.setNode(i, dp.types[i], dp.names[i]);
    for(auto& [u,v,w]:dp.roads) { net.addRoad(u,v,w,false,true); dsu.unite(u,v); }
    success("Delhi road network loaded! " + to_string(dp.roads.size()) + " roads, 12 locations.");
    pause(600);

    // ── STEP 2: Display Network ──
    section("STEP 2: Road Network Overview");
    net.display();
    pause(600);

    // ── STEP 3: Connectivity ──
    section("STEP 3: Connectivity Check — DSU");
    info("Checking: Can someone travel from Connaught Place to IGI Airport?");
    pause(300);
    if(dsu.connected(0,3))
        good("YES! Connaught Place and IGI Airport ARE connected.");
    else
        err("NO! They are NOT connected.");
    dsu.showComponents(net.n, net.nodeName);
    pause(600);

    // ── STEP 4: Shortest Path ──
    section("STEP 4: Shortest Route — Dijkstra");
    typeOut(C + "  Finding fastest route: Connaught Place --> Nehru Place...", 15);
    auto res = dijkstra(0, net.n, net.adj);
    auto path = getPath(0, 5, res.par);
    info("Time Complexity: O((V+E) log V)");
    cout << "  Fastest Route: "; printPath(path, net.nodeName);
    cout << G << "  Total time: " << BO << res.dist[5] << " min\n" << RS;
    pause(600);

    // ── STEP 5: Green Channel ──
    section("STEP 5: Green Channel — Emergency Vehicle");
    typeOut(R + "  EMERGENCY: Ambulance needed from Connaught Place to AIIMS!", 20);
    greenChannel(0, 2, net.n, net.adj, net.nodeName);
    pause(600);

    // ── STEP 6: VIP Route ──
    section("STEP 6: VIP Route — Priority Convoy");
    typeOut(M + "  VIP convoy departing from India Gate to IGI Airport.", 20);
    vipRoute(1, 3, net.n, net.adj, net.nodeName);
    pause(600);

    // ── STEP 7: Tarjan's ──
    section("STEP 7: Critical Infrastructure — Tarjan's Algorithm");
    typeOut(R + "  Scanning Delhi road network for vulnerable points...", 15);
    Tarjan tarjan(net.n, net.adj);
    tarjan.run(net.nodeName);
    pause(600);

    // ── STEP 8: MST ──
    section("STEP 8: Minimum Road Network — Kruskal's MST");
    typeOut(C + "  Which minimum roads keep Delhi connected? Finding out...", 15);
    kruskalMST(net.n, net.edgeList, net.nodeName);
    pause(600);

    // ── STEP 9: Accident Simulation ──
    section("STEP 9: Accident Simulation");
    typeOut(R + "  BREAKING NEWS: Major accident on India Gate <-> AIIMS road!", 20);
    net.blockRoad(1, 2, true);
    dsu.rebuild(net);
    detectZones(net.n, net.adj, net.nodeName);
    typeOut(Y + "  Re-routing traffic around accident...", 15);
    auto res2 = dijkstra(0, net.n, net.adj);
    auto path2 = getPath(0, 2, res2.par);
    if(!path2.empty()){
        cout << "  New route to AIIMS: "; printPath(path2, net.nodeName);
        cout << Y << "  New travel time: " << res2.dist[2] << " min (was faster before)\n" << RS;
    }
    pause(600);

    // ── STEP 10: Clear + Jam ──
    section("STEP 10: Road Cleared + Traffic Jam");
    typeOut(G + "  Accident cleared. Road reopened.", 15);
    net.blockRoad(1, 2, false);
    typeOut(Y + "  But now: Heavy traffic jam on Connaught Place <-> Karol Bagh!", 15);
    net.updateWeight(0, 7, 35); // 3.5x jam
    auto res3 = dijkstra(0, net.n, net.adj);
    auto path3 = getPath(0, 10, res3.par);
    cout << "  Rerouted to Rohini: "; printPath(path3, net.nodeName);
    cout << Y << "  Travel time with jam: " << res3.dist[10] << " min\n" << RS;
    pause(600);

    // ── STEP 11: Analytics ──
    section("STEP 11: 24-Hour Traffic Analytics");
    vector<int> tdata = {90,70,55,40,35,80,200,450,600,520,480,510,
                          550,530,490,520,600,720,650,500,350,250,180,120};
    Analytics analytics(tdata);
    analytics.heatmap();
    analytics.congestionWindow(3);
    pause(600);

    // ── STEP 12: Signals ──
    section("STEP 12: Signal Scheduling + Bitmask + GCD");
    sched.addDep(0,1); sched.addDep(1,2); sched.addDep(0,3); sched.addDep(3,4);
    sched.schedule();
    analytics.signalBitmask(0b10110101, 8);
    vector<int> cyc = {30, 45, 60};
    analytics.gcdLcmCycles(cyc);
    pause(600);

    // ── STEP 13: Bottleneck ──
    section("STEP 13: Min Bottleneck Path — Binary Search");
    info("Finding smoothest path from Connaught Place to Akshardham...");
    info("Time Complexity: O(E log E + E * alpha(V))");
    int bn = minBottleneck(net.n, net.edgeList, 0, 9);
    if(bn == -1) err("No path!");
    else {
        cout << "  Minimum bottleneck edge on best path: " << BO << bn << " min\n" << RS;
        good("This means no single road on this path is worse than " + to_string(bn) + " min!");
    }
    pause(600);

    // ── STEP 14: Complexity Table ──
    showComplexities();
    pause(400);

    // ── FINAL ──
    section("DEMO COMPLETE!");
    typeOut(G + "  All " + to_string(13) + " features demonstrated successfully!", 15);
    typeOut(G + "  Great job, Officer " + OPERATOR_NAME + "! " + CITY_NAME + " is in safe hands.", 15);
    typeOut(C + "  Returning to main menu...", 15);
    pause(800);
    net.clearAllJams();
    dsu.rebuild(net);
}

// ─────────────────────────────────────────────
// MAIN MENU
// ─────────────────────────────────────────────
void printMenu(){
    cout << C << BO
         << "\n  ╔══════════════════════════════════════════════════╗\n"
         << "  ║   " << CITY_NAME << " Traffic Control — " << OPERATOR_NAME
         << string(max(0,(int)(22-(int)CITY_NAME.size()-(int)OPERATOR_NAME.size())),' ')
         << "      ║\n"
         << "  ║   Session Operations: " << SESSION_OPS
         << string(25,' ') << " ║\n"
         << "  ╚══════════════════════════════════════════════════╝\n" << RS;

    cout << Y;
    cout << "\n  --- ROAD NETWORK ---\n";
    cout << "  [1]  Add Road              [2]  Remove Road\n";
    cout << "  [3]  Display Network       [4]  Set Location Type\n";
    cout << "  [5]  Update Road Weight\n";
    cout << "\n  --- CONNECTIVITY ---\n";
    cout << "  [6]  Check Connectivity    [7]  Show Regions (DSU)\n";
    cout << "  [8]  Zone Isolation (DFS)\n";
    cout << "\n  --- ROUTING ---\n";
    cout << "  [9]  Shortest Route        [10] All Distances\n";
    cout << "  [11] Bellman-Ford          [12] Bottleneck Path\n";
    cout << "  [13] Nearest Emergency\n";
    cout << "\n  --- PRIORITY MODES ---\n";
    cout << "  [14] " << BGG << W << " GREEN CHANNEL " << RS << Y << "   [15] " << BGM << W << " VIP ROUTE " << RS << Y << "\n";
    cout << "\n  --- ANALYSIS ---\n";
    cout << "  [16] Tarjan (Bridges/APs)  [17] Kruskal MST\n";
    cout << "\n  --- SIMULATION ---\n";
    cout << "  [18] Accident Simulation   [19] Traffic Jam\n";
    cout << "  [20] Clear All Roads\n";
    cout << "\n  --- SIGNALS & ANALYTICS ---\n";
    cout << "  [21] Add Signal Dependency [22] Signal Schedule\n";
    cout << "  [23] Traffic Heatmap       [24] Signal Bitmask\n";
    cout << "  [25] Signal Cycle GCD/LCM\n";
    cout << "\n  --- SYSTEM ---\n";
    cout << "  [26] Time Complexity Table [27] Load Delhi Demo\n";
    cout << "  [28] " << BGC << W << " AUTO DEMO MODE " << RS << Y << "\n";
    cout << "  [0]  Exit\n" << RS;
    cout << BO << "\n  Officer " << OPERATOR_NAME << ", enter command: " << RS;
}

// ─────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────
int main(){

    loginScreen();

    int n;
    cout << "\n" << BO << "  How many intersections in your city? [Press 0 for Delhi preset]: " << RS;
    cin >> n;

    RoadNetwork net;
    DSU* dsu_ptr = nullptr;

    if(n == 0){
        // Load Delhi preset
        DelhiPreset dp;
        n = dp.n;
        net = RoadNetwork(n);
        dsu_ptr = new DSU(n);
        typeOut(G + "  Loading Delhi city preset...", 15);
        for(int i=0;i<dp.n;i++) net.setNode(i, dp.types[i], dp.names[i]);
        for(auto& [u,v,w]:dp.roads){ net.addRoad(u,v,w,false,true); dsu_ptr->unite(u,v); }
        good("Delhi city loaded! 12 locations, " + to_string(dp.roads.size()) + " roads.");
    } else {
        net = RoadNetwork(n);
        dsu_ptr = new DSU(n);
        success("City initialized with " + to_string(n) + " intersections.");
    }

    DSU& dsu = *dsu_ptr;
    SignalScheduler scheduler(n);
    vector<int> tdata = {90,70,55,40,35,80,200,450,600,520,480,510,
                          550,530,490,520,600,720,650,500,350,250,180,120};
    Analytics analytics(tdata);

    int choice;
    do {
        printMenu();
        cin >> choice;
        SESSION_OPS++;

        if(choice==1){
            int u,v,w; char vf;
            cout << "  Source Dest TravelTime VIPonly?(y/n): ";
            cin >> u >> v >> w >> vf;
            net.addRoad(u,v,w, vf=='y');
            dsu.unite(u,v);

        } else if(choice==2){
            int u,v;
            cout << "  Road to remove (u v): "; cin >> u >> v;
            net.removeRoad(u,v); dsu.rebuild(net);

        } else if(choice==3){
            net.display();

        } else if(choice==4){
            int u,t; string nm;
            cout << "  Node index, Type(0=Chowk 1=Hospital 2=Fire 3=Police 4=VIP 5=Metro), Name: ";
            cin >> u >> t >> nm;
            if(u>=0&&u<n&&t>=0&&t<=5) net.setNode(u,(NodeType)t,nm);
            else err("Invalid input!");

        } else if(choice==5){
            int u,v,w;
            cout << "  Road (u v) and new time: "; cin >> u >> v >> w;
            net.updateWeight(u,v,w);

        } else if(choice==6){
            int u,v;
            cout << "  Check connectivity between (u v): "; cin >> u >> v;
            dsu.rebuild(net);
            info("Time Complexity: O(alpha(V)) ≈ O(1) with path compression");
            if(dsu.connected(u,v))
                good("CONNECTED: " + net.nodeName[u] + " <-> " + net.nodeName[v]);
            else
                err("NOT CONNECTED: " + net.nodeName[u] + " <-> " + net.nodeName[v]);

        } else if(choice==7){
            dsu.rebuild(net);
            dsu.showComponents(n, net.nodeName);

        } else if(choice==8){
            detectZones(n, net.adj, net.nodeName);

        } else if(choice==9){
            int s,d;
            cout << "  Source Dest: "; cin >> s >> d;
            auto res = dijkstra(s,n,net.adj);
            auto path = getPath(s,d,res.par);
            info("Time Complexity: O((V+E) log V)");
            if(path.empty()) err("No path from " + net.nodeName[s] + " to " + net.nodeName[d] + "!");
            else {
                cout << "  Route: "; printPath(path, net.nodeName);
                cout << G << "  Time : " << BO << res.dist[d] << " min\n" << RS;
            }

        } else if(choice==10){
            int s;
            cout << "  Source node: "; cin >> s;
            auto res = dijkstra(s,n,net.adj);
            info("Time Complexity: O((V+E) log V)");
            cout << "\n  Distances from " << BO << net.nodeName[s] << RS << ":\n";
            for(int i=0;i<n;i++){
                cout << "  " << setw(28) << left << net.nodeName[i] << " : ";
                if(res.dist[i]==INF) cout << R << "Unreachable\n" << RS;
                else cout << G << res.dist[i] << " min\n" << RS;
            }

        } else if(choice==11){
            int s; cout << "  Source: "; cin >> s;
            bellmanFord(s,n,net.edgeList,net.nodeName);

        } else if(choice==12){
            int s,d; cout << "  Source Dest: "; cin >> s >> d;
            info("Time Complexity: O(E log E + E * alpha(V))");
            int bn = minBottleneck(n,net.edgeList,s,d);
            if(bn==-1) err("No path!");
            else {
                good("Min bottleneck: " + to_string(bn) + " min");
                info("No single road on the best path exceeds " + to_string(bn) + " min.");
            }

        } else if(choice==13){
            int s,t; cout << "  Source, Type(1=Hospital 2=Fire 3=Police 5=Metro): "; cin >> s >> t;
            bfsNearest(s,(NodeType)t,n,net.adj,net.nodeType,net.nodeName);
            nearestByTime(s,(NodeType)t,n,net.adj,net.nodeType,net.nodeName);

        } else if(choice==14){
            int s,d; cout << "  Emergency: Source Dest: "; cin >> s >> d;
            greenChannel(s,d,n,net.adj,net.nodeName);

        } else if(choice==15){
            int s,d; cout << "  VIP: Source Dest: "; cin >> s >> d;
            vipRoute(s,d,n,net.adj,net.nodeName);

        } else if(choice==16){
            Tarjan t(n,net.adj); t.run(net.nodeName);

        } else if(choice==17){
            kruskalMST(n,net.edgeList,net.nodeName);

        } else if(choice==18){
            simulateAccident(net,dsu);

        } else if(choice==19){
            simulateJam(net);

        } else if(choice==20){
            net.clearAllJams(); dsu.rebuild(net);

        } else if(choice==21){
            int a,b; cout << "  Signal A before Signal B (a b): "; cin >> a >> b;
            scheduler.addDep(a,b);

        } else if(choice==22){
            scheduler.schedule();

        } else if(choice==23){
            analytics.heatmap();
            int K; cout << "  Window size (hours): "; cin >> K;
            analytics.congestionWindow(K);
            int l,r; cout << "  Range query [l r]: "; cin >> l >> r;
            if(l>=0&&r<analytics.n&&l<=r)
                cout << "  Vehicles hrs " << l << "-" << r << ": "
                     << BO << analytics.rangeSum(l,r) << RS << "\n";

        } else if(choice==24){
            int mask,cnt; cout << "  Bitmask and signal count: "; cin >> mask >> cnt;
            analytics.signalBitmask(mask,cnt);

        } else if(choice==25){
            int m; cout << "  Number of signals: "; cin >> m;
            vector<int> cyc(m);
            cout << "  Cycle durations (sec): ";
            for(int& c:cyc) cin >> c;
            analytics.gcdLcmCycles(cyc);

        } else if(choice==26){
            showComplexities();

        } else if(choice==27){
            // Reload Delhi preset
            DelhiPreset dp;
            net = RoadNetwork(dp.n);
            n = dp.n;
            for(int i=0;i<dp.n;i++) net.setNode(i,dp.types[i],dp.names[i]);
            for(auto& [u,v,w]:dp.roads){ net.addRoad(u,v,w,false,true); dsu.unite(u,v); }
            good("Delhi preset loaded!");

        } else if(choice==28){
            // Full auto demo
            RoadNetwork demoNet(12);
            DSU demoDsu(12);
            SignalScheduler demoSched(12);
            runDemoMode(demoNet, demoDsu, demoSched);

        } else if(choice!=0){
            err("Invalid choice! Try again, Officer " + OPERATOR_NAME + ".");
        }

    } while(choice != 0);

    cout << "\n" << C << BO;
    typeOut("  Thank you for your service, Officer " + OPERATOR_NAME + "!", 20);
    typeOut("  " + CITY_NAME + " is safer because of you.", 20);
    typeOut("  Smart City Analyzer shutting down. Goodbye!", 15);
    cout << RS;

    delete dsu_ptr;
    return 0;
}
