#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
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

class ComNet
{
	// Root node pointer
private:

	std::vector<netNode> netWork;

	bool connectable(const int a, const int b) {
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
	ComNet(const int N,  const int minConnections, const int maxConnections)
	{
		int i, j, k;		
		double closeness;
		netNode emptyNode;
		std::uniform_real_distribution<double> unif(0, 1);
		std::default_random_engine re;
		// auto random_integer = uni(rng);
		// double a_random_double = unif(re);
		int degreeTotal = 0;
		int degree = 0;
		int stuck_count = 0;

		for (i = 0; i < N; i++) {
			emptyNode.Type = 'P';
			// Choose your degree dirstribution here
			degree = uni_int(minConnections, maxConnections);
			for (j = 0; j < degree; j++) {
				emptyNode.Connection.push_back(-1);
				emptyNode.Closeness.push_back(-1);
			}
			degreeTotal = degreeTotal + degree;
			netWork.push_back(emptyNode);
			emptyNode.Connection.erase(emptyNode.Connection.begin(), emptyNode.Connection.end());
			emptyNode.Closeness.erase(emptyNode.Closeness.begin(), emptyNode.Closeness.end());
		}
		if (degreeTotal % 2 != 0) {
			i = uni_int(0, N-1);
			netWork[i].Connection.push_back(-1);
			netWork[i].Closeness.push_back(-1);
			degreeTotal += 1;
		}
		// std::cout << netWork.size() << " Nodes with " << degreeTotal / 2 << " edges.\n";
		
		std::vector<int> waitQueue;
		std::vector<int> doneQueue;
		
		for (i = 0; i < N; i++) {
			if (netWork[i].Connection.size() > 0) {
				waitQueue.push_back(i);
			}
		}

		while (waitQueue.size() > 0) {
			int a = uni_int(0, waitQueue.size() - 1);
			int b = uni_int(0, waitQueue.size() - 1);
			while ((b == a) && (waitQueue.size() > 1)){b = uni_int(0, waitQueue.size() - 1);}
			int inQa = waitQueue[a];
			int inQb = waitQueue[b];
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
				// std::cout << "Done writing.\n";
			}
			else {
				stuck_count++;
				// std::cout << "Stuck Count " << stuck_count << "\n";
				if (stuck_count > waitQueue.size()*(waitQueue.size() - 1)+3)
				{
					// Shake it up!!
					stuck_count = 0;
					// std::cout << "Stuck at " << waitQueue.size() << "\n";
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
						doneQueue.erase(doneQueue.begin()+random_dQ_index);
						
						int dQ_end_index = 0;
						while ((doneQueue[dQ_end_index] != random_end) && (dQ_end_index < doneQueue.size())) { dQ_end_index++; }
						if (dQ_end_index < doneQueue.size()) {
							waitQueue.push_back(random_end);
							doneQueue.erase(doneQueue.begin()+dQ_end_index);
						}

					}
					// std::cout << "Shaken!\n";
				}
			}
			// std::cout << " Q len " <<waitQueue.size() << "\n";
		}
		// std::cout << "Done.\n";
	}
	// Default Destructor for Array of struct
	
	int node_count()
	{
		return netWork.size();
	}

	void writeNetAdjacency(std::string filename) {
		std::ofstream outfile(filename, std::ofstream::out);
		int i,j;
		outfile << "From, To, Weight\n";
		i = 0;
		for (auto &it : netWork) {
			i++;
			for (j = 0; j < it.Connection.size(); j++) {
				outfile << i << ", " << it.Connection[j] << ", " << it.Closeness[j] << "\n";
			}
		}
		outfile.close();
	}
};
