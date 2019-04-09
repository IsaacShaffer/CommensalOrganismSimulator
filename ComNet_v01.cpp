#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937_64 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
std::default_random_engine re;
std::uniform_real_distribution<double> unif(0, 1);

struct netNode
{
	std::vector<int> Connection;
	std::vector<double> Closeness;
	char Type;
};

int uni_int(int from, int to) {  // return a random integer in [from, to] range 
	std::uniform_int_distribution<int> dist(from, to);
	return dist(rng);
}

// First iteration: undirected graph with weighted edges
// The structure of the node

class comNet
{
	// Root node pointer
private:

	std::vector<netNode> netWork;
	int netDiameter = 0;

	void ShakeItUp(std::vector<int> &waitQueue, std::vector<int> &doneQueue) {
		int shakeUp = waitQueue.size() + 3;
		for (int l = 0; l < shakeUp; l++) {
			
			int random_dQ_index = uni_int(0, doneQueue.size() - 1);
			int random_start = doneQueue[random_dQ_index];
			int random_start_index = uni_int(0, (netWork[random_start]).Connection.size() - 1);
			int random_end = netWork[random_start].Connection[random_start_index];
			int random_end_index = 0;
			while (netWork[random_end].Connection[random_end_index] != random_start) { random_end_index++; }

			// 
			// Remove from list of connects on both ends
			netWork[random_start].Connection.erase(netWork[random_start].Connection.begin() + random_start_index);
			netWork[random_start].Closeness.erase(netWork[random_start].Closeness.begin() + random_start_index);
			netWork[random_end].Connection.erase(netWork[random_end].Connection.begin() + random_end_index);
			netWork[random_end].Closeness.erase(netWork[random_end].Closeness.begin() + random_end_index);

			netWork[random_start].Connection.push_back(-1);
			netWork[random_start].Closeness.push_back(-1);
			netWork[random_end].Connection.push_back(-1);
			netWork[random_end].Closeness.push_back(-1);

			// Fix the waitQueue and doneQueue

			waitQueue.push_back(random_start);
			doneQueue.erase(doneQueue.begin() + random_dQ_index);

			int dQ_end_index = 0;
			while ((doneQueue[dQ_end_index] != random_end) && (dQ_end_index < doneQueue.size())) { dQ_end_index++; }
			if (dQ_end_index < doneQueue.size()) {
				waitQueue.push_back(random_end);
				doneQueue.erase(doneQueue.begin() + dQ_end_index);
			}

		} //end for Shake Loop
	}

	bool connectable(int a, int b) {
		bool status = true;
		// Check for existing a to b connection
		for (auto it = netWork[a].Connection.begin(); it != netWork[a].Connection.end(); ++it) {
			if (*it == b) {
				status = false;
			}
		}
		// Disallow self-connections and require at least on type 'P' node in connection
		if ((status && ((netWork[a].Type == 'P') || (netWork[b].Type == 'P'))) && (a != b)) { 
			status = true;
		}
		else {
			status = false;
		}
		return status;
	}

public:
	// Default Constructor for Array
	comNet(const int N,  const int minConnections, const int maxConnections)
	{
		int i, j, k, l;		
		double closeness;
		netNode emptyNode;
		std::uniform_real_distribution<double> unif(0, 1);
		std::default_random_engine re;
		// auto random_integer = uni(rng);
		// double a_random_double = unif(re);
		int degreeTotal = 0;
		int degree = 0;
		int stuck_count = 0;
		std::vector<int> openNodes;

		netDiameter = 0;
		for (i = 0; i < N; i++) {
			// Choose your degree dirstribution here
			degree = uni_int(minConnections, maxConnections);
			emptyNode.Type = 'P';
			std::cout << "Node " << i << " has Degree " << degree << "\n";
			degreeTotal += degree;
			//
			if (openNodes.size()>1) {
				k = uni_int(0, openNodes.size() - 1);
				int inNode = openNodes[k];
				emptyNode.Connection.push_back(inNode);
				emptyNode.Closeness.push_back(unif(re));
				l = 0;
				while (netWork[inNode].Connection[l] >= 0) { l += 1; }
				netWork[inNode].Connection[l] = i;
				netWork[inNode].Closeness[l] = emptyNode.Closeness[0];
				for (j = 1; j < degree; j++) {
					emptyNode.Connection.push_back(-1);
					emptyNode.Closeness.push_back(-1);
				}
				netWork.push_back(emptyNode);
				if (degree > 1) { openNodes.push_back(i); }
				if (l == netWork[inNode].Connection.size()-1) {
					openNodes.erase(openNodes.begin() + k);
				}
			}
			else {
				for (j = 0; j < degree; j++) {
					emptyNode.Connection.push_back(-1);
					emptyNode.Closeness.push_back(-1);
				}
				netWork.push_back(emptyNode);
				openNodes.push_back(i);
			}
			emptyNode.Connection.erase(emptyNode.Connection.begin(), emptyNode.Connection.end());
			emptyNode.Closeness.erase(emptyNode.Closeness.begin(), emptyNode.Closeness.end());
		}

		while ((degreeTotal % 2 != 0) || (edge_count() < (N+ N/10))) {
			i = uni_int(0, N-1); 
			if (netWork[i].Connection.size() < maxConnections) {
				netWork[i].Connection.push_back(-1);
				netWork[i].Closeness.push_back(-1);
				degreeTotal += 1;
			}
		}

		std::cout << netWork.size() << " Nodes with " << edge_count() << " edges.\n";
		if (isConnected()) { std::cout << "Connected.\n"; }
		std::vector<int> waitQueue = openNodes;
		std::vector<int> doneQueue;
		
		while (waitQueue.size() > 0)  {
			int a = uni_int(0, waitQueue.size() - 1);
			int b = uni_int(0, waitQueue.size() - 1);
			while ((b == a) && (waitQueue.size() > 1)){b = uni_int(0, waitQueue.size() - 1);}
			int inQa = waitQueue[a];
			int inQb = waitQueue[b];
			// std::cout << "Queue size: " << waitQueue.size() << "\n";
			// std::cout << inQa <<" at "<<a<< " to " << inQb<<" at "<<b << "\n";
			if (connectable(inQa, inQb)) {
				// Make a connection
				stuck_count = 0;
				i = 0;
				closeness = unif(re);
				while (netWork[inQa].Connection[i] >= 0) { i++; }
			    // std::cout << "writing " << i + 1 << " of " << netWork[inQa].Connection.size() << " at " << inQa << " to ";
				netWork[inQa].Connection[i] = inQb;
				(netWork[inQa]).Closeness[i] = closeness;
				if (netWork[inQa].Connection.size() == i + 1) {
					doneQueue.push_back(inQa);
					waitQueue.erase(waitQueue.begin() + a);
					if (a < b) { b += -1; }
				}
				i = 0;
				while (netWork[inQb].Connection[i] >= 0) { i++; }
				// std::cout << i + 1 << " of " << netWork[inQb].Connection.size() << " at " << inQb << "\n";
				netWork[inQb].Connection[i] = inQa;
				netWork[inQb].Closeness[i] = closeness;
				if (netWork[inQb].Connection.size() == i + 1) {
					doneQueue.push_back(inQb);
					waitQueue.erase(waitQueue.begin() + b);
				}
			}
			else {
				stuck_count++;
				// std::cout << "Stuck Count " << stuck_count << "\n";
				if (stuck_count > waitQueue.size()*(waitQueue.size() - 1)+3)
				{
					// Shake it up!!
					stuck_count = 0;
					ShakeItUp(waitQueue, doneQueue);
					std::cout << "Stuck at " << waitQueue.size() << "\n";
				} //end if Stuck
			} // end if Connectable points
			if ((waitQueue.size() == 0) && !isConnected()) {
				stuck_count = 0;
				ShakeItUp(waitQueue, doneQueue);
				std::cout << "Connectedness Shake!\n";
			}
		} // end while Main Loop
	}
	// Standard Network Value Returns
	std::vector<int> neighbors(int Node)
	{
		return netWork[Node].Connection;
	}

	std::vector<double> closeness(int Node)
	{
		return netWork[Node].Closeness;
	}

	int node_count()
	{
		return netWork.size();
	}

	int edge_count() {
		int total_edges = 0;
		for (auto &node : netWork) {
			total_edges += node.Connection.size();
		}
		return total_edges/2;
	}

	std::vector<int> path_lengths(int a)
	{
		// Dijkstra's algorithm. Could be better with Thorup (1999)
		int v,w,i;
		std::vector<int> dist;
		std::vector<int> Q; 
		for (i = 0; i < netWork.size(); i++) {
			dist.push_back(netWork.size() + 1);
			Q.push_back(i);
		}
		dist[a] = 0;
		
		while (Q.size() > 0) {
			v = 0;
			for (i = 0; i < Q.size(); i++) { if (dist[Q[i]] < dist[Q[v]]) { v = i; } }
			w = Q[v];
			Q.erase(Q.begin() + v); 
			for (auto &toNode : netWork[w].Connection) {
				if (dist[w] + 1 < dist[toNode]) { dist[toNode] = dist[w] + 1; }
			}
		}
		return dist;
	}

	int diameter()
	{
		int i;
		if (netDiameter == 0) {
			for (i = 0; i < netWork.size(); i++) {
				for (auto &it : path_lengths(i)) {
					if (it > netDiameter) { netDiameter = it; }
				}
			}
		}
		return netDiameter;
	}

	bool isConnected() {
		int a = uni_int(0, netWork.size() - 1);
		bool connected = true;
		for (auto &pathLength : path_lengths(a)) {
			if (pathLength > netWork.size()) { connected = false; }
		}
		return connected;
	}

	void writeNetEdgelist(std::string filename) {
		int i,j;
		int ret_val = 0;
		std::ofstream outfile(filename, std::ofstream::trunc);
		outfile << "From, To, Weight\n";
		i = 0;
		for (auto &it : netWork) {
			for (j = 0; j < it.Connection.size(); j++) {
				outfile << i << ", " << it.Connection[j] << ", " << it.Closeness[j] << "\n";
			}
			i++;
		}
		outfile.close();
	}
};