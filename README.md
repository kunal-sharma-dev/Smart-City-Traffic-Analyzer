Smart City Traffic & Road Network Analyzer

A graph-based traffic management and route optimization system built in C++ with an interactive terminal interface. Models a real city's road network and supports 28 operations including routing, analysis, and emergency planning.

🚀 How to Run

Prerequisites: g++ compiler (C++17 or above)

bash
# Step 1 — Clone the repo
git clone https://github.com/kunal-sharma-dev/Smart-City-Traffic-Analyzer.git
cd Smart-City-Traffic-Analyzer

# Step 2 — Compile
g++ -std=c++17 -O2 -o traffic main.cpp

# Step 3 — Run
./traffic

When prompted for intersections, press 0 to load the Delhi city preset (12 locations, 22 roads) and explore all features interactively.

✨ Features
Shortest Path Routing using Dijkstra's Algorithm
Critical Road Detection using Tarjan's Bridge Algorithm
Connectivity Analysis using DFS and DSU (Disjoint Set Union)
Bellman-Ford for alternative path detection
Bottleneck Path Analysis
Zone Isolation and Region Detection
Emergency Route Planning (nearest hospital / fire station / police)
Green Channel Mode for ambulances
VIP Route Mode for restricted roads
Traffic Signal Sync using GCD/LCM
Bitmask-based signal state tracking
Topological Sort for signal ordering
Auto Demo Mode
Delhi City Preset (12 locations, 22 roads, 28 menu options)
🛠️ Tech Stack
C++ (C++17)
STL (priority_queue, unordered_map, vector, stack)
Graph Algorithms
Data Structures
📐 Algorithms Used
Algorithm	Purpose	Complexity
Dijkstra's	Shortest path routing	O((V+E) log V)
Tarjan's Bridge Detection	Critical road identification	O(V+E)
Bellman-Ford	Alternative path / negative weights	O(V*E)
DFS / BFS	Connectivity and traversal	O(V+E)
DSU (Union-Find)	Region and zone detection	O(α V)
Topological Sort	Signal ordering	O(V+E)
Priority Queue	Efficient routing	O(log V)
🗺️ Project Overview

The project models a city's road network as a weighted graph where:

Nodes represent locations/intersections (hospitals, metro stations, VIP zones, fire stations, police stations)
Edges represent roads with travel time weights
Traffic conditions influence route selection

The system identifies optimal routes, critical roads whose removal disconnects the network, emergency routing, and supports VIP/Green Channel priority modes.

👥 Team

Developed as a Competitive Programming Lab Project at Jaypee Institute of Information Technology (JIIT), Noida.
