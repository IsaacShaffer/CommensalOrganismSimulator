#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
std::default_random_engine re;
std::uniform_real_distribution<double> unif(0, 1);

int uni_int(int from, int to) {  // return a random integer in [from, to] range 
	std::uniform_int_distribution<int> dist(from, to);
	return dist(rng);
}

// First iteration: undirected graph with weighted edges
// The structure of the node
struct netNode
{
	int nConnections;
	int *Connection;
	double *Closeness;
	char Type;
};

class ComNet
{
	// Root node pointer
private:
	std::vector<netNode> netWork;
	int netSize;

	bool connectable(int a, int b) {
		bool status = true;
		// Check for existing a to b connection
		for (int i = 0; i < netWork[a].nConnections; i++) {
			if (netWork[a].Connection[i] == b) {
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
	ComNet(int N,  int minConnections, int maxConnections)
	{
		this->netSize = N;
		int i, j, k;
		netNode emptyNode;
		emptyNode.Closeness = NULL;
		emptyNode.Connection = NULL;
		emptyNode.nConnections = 0;
		emptyNode.Type = '/';
		for (i = 0; i < N; i++) {
			(netWork[i]).push_back(emptyNode);
		}
		
		double closeness;
		
		std::uniform_real_distribution<double> unif(0, 1);
		std::default_random_engine re;
		// auto random_integer = uni(rng);
		// double a_random_double = unif(re);
		int i, j, k;
		int degreeTotal = 0;
		int degree = 0;
		for (i = 0; i < netSize; i++) {
			degree = uni_int(minConnections, maxConnections);
			(netWork[i]).Connection = new int[degree];
			(netWork[i]).Closeness = new double[degree];
			netWork[i].nConnections = degree;
			netWork[i].Type = 'P';
			for (j = 0; j < degree; j++) {
				(netWork[i]).Connection[j] = -1;
				(netWork[i]).Closeness[j] = -1;
			}
			degreeTotal = degreeTotal + degree;
		}
		if (degreeTotal % 2 != 0) {
			i = uni_int(0, N-1);
			degree = netWork[i].nConnections + 1;
			degreeTotal += 1;
			delete[] (netWork[i]).Closeness;
			delete[] (netWork[i]).Connection;
			(netWork[i]).Connection = new int[degree];
			(netWork[i]).Closeness = new double[degree];
			netWork[i].nConnections = degree;
			for (j = 0; j < degree; j++) {
				(netWork[i]).Connection[j] = -1;
				(netWork[i]).Closeness[j] = -1;
			}
		}
		std::cout << N << " Nodes with " << degreeTotal / 2 << " edges.\n";
		
		std::vector<int> waitQueue(N);
		std::vector<int> doneQueue(N);

		for (i = 0; i < N; i++) {
			waitQueue[i] = -1;
			doneQueue[i] = -1;
		}
		int waitQueue_len = 0;
		int doneQueue_len = 0;
		int stuck_count = 0;
		for (i = 0; i < N; i++) {
			if ((netWork[i]).nConnections > 0) {
				waitQueue[waitQueue_len] = i;
				waitQueue_len = waitQueue_len + 1;
			}
		}
		while (waitQueue_len > 0) {
			int a = uni_int(0, waitQueue_len - 1);
			int b = uni_int(0, waitQueue_len - 1);
			while ((b == a) && (waitQueue_len > 1)){b = uni_int(0, waitQueue_len - 1);}
			int inQa = waitQueue[a];
			int inQb = waitQueue[b];
			// std::cout << inQa <<" at "<<a<< " to " << inQb<<" at "<<b << "\n";
			if (connectable(inQa, inQb)) {
				// Make a connection
				stuck_count = 0;
				i = 0;
				closeness = unif(re);
				while ((netWork[inQa]).Connection[i] >= 0) { i++; }
				// std::cout << "writing " << i + 1 << " of " << (netWork[inQa]).nConnections << " at " << inQa << " to ";
				(netWork[inQa]).Connection[i] = inQb;
				(netWork[inQa]).Closeness[i] = closeness;
				if ((netWork[inQa]).nConnections == i + 1) {
					doneQueue[doneQueue_len] = inQa;
					doneQueue_len += 1;
					for (i = a; i < waitQueue_len - 1; i++) { waitQueue[i] = waitQueue[i + 1]; }
					waitQueue[waitQueue_len] = -1;
					waitQueue_len = waitQueue_len - 1;
					if (a < b) { b = b - 1; }
				}
				i = 0;
				while ((netWork[inQb]).Connection[i] >= 0) { i++; }
				// std::cout << i + 1 << " of " << (netWork[inQb]).nConnections << " at " << inQb << "\n";
				(netWork[inQb]).Connection[i] = inQa;
				(netWork[inQb]).Closeness[i] = closeness;
				if ((netWork[inQb]).nConnections == i + 1) {
					doneQueue[doneQueue_len] = inQb;
					doneQueue_len += 1;
					for (i = b; i < waitQueue_len - 1; i++) { waitQueue[i] = waitQueue[i + 1]; }
					waitQueue[waitQueue_len] = -1;
					waitQueue_len = waitQueue_len - 1;
				}
			}
			else {
				stuck_count += 1;
				// std::cout << "Stuck Count " << stuck_count << "\n";
				if (stuck_count > waitQueue_len*(waitQueue_len - 1)+3)
				{
					// Shake it up!!
					stuck_count = 0;
					std::cout << "Stuck at " << waitQueue_len << "\n";
					int shakeUp = waitQueue_len + 3;
					for (int l = 0; l < shakeUp; l++) {
						
						int random_dQ_index = uni_int(0, doneQueue_len - 1);
						int random_start = doneQueue[random_dQ_index];
						int random_start_index = uni_int(0, (netWork[random_start]).nConnections - 1);
						int random_end = (netWork[random_start]).Connection[random_start_index];
						int random_end_index = 0;
						// Remove from list of connects on both ends
						
						while ((netWork[random_end]).Connection[random_end_index] != random_start) { random_end_index++; }
						for (j = random_end_index; j < (netWork[random_end]).nConnections - 1; j++) {
							(netWork[random_end]).Connection[j] = (netWork[random_end]).Connection[j + 1];
							(netWork[random_end]).Closeness[j] = (netWork[random_end]).Closeness[j + 1];
						}
						(netWork[random_end]).Connection[(netWork[random_end]).nConnections-1] = -1;
						(netWork[random_end]).Closeness[(netWork[random_end]).nConnections-1] = -1;

						for (j = random_start_index; j < (netWork[random_start]).nConnections - 1; j++) {
							(netWork[random_start]).Connection[j] = (netWork[random_start]).Connection[j + 1];
							(netWork[random_start]).Closeness[j] = (netWork[random_start]).Closeness[j + 1];
						}
						(netWork[random_start]).Connection[(netWork[random_start]).nConnections-1] = -1;
						(netWork[random_start]).Closeness[(netWork[random_start]).nConnections-1] = -1;
						// Fix the waitQueue and doneQueue
						
						waitQueue[waitQueue_len] = random_start;
						waitQueue_len += 1;
						for (j = random_dQ_index; j < doneQueue_len - 1; j++) {
							doneQueue[j] = doneQueue[j + 1];
						}
						doneQueue[doneQueue_len - 1] = -1;
						doneQueue_len += -1;
						j = 0;
						while ((random_end != doneQueue[j]) && (j < doneQueue_len)) { j++; }
						if (j < doneQueue_len) {
							waitQueue[waitQueue_len] = random_end;
							waitQueue_len += 1;
							for (k = j; k < doneQueue_len - 1; ++k) {
								doneQueue[k] = doneQueue[k + 1];
							}
							doneQueue[doneQueue_len - 1] = -1;
							doneQueue_len += -1;

						}


					}
					std::cout << "Shaken!\n";
				}
			}
			// std::cout << " Q len " <<waitQueue_len << "\n";
		}
		std::cout << "Done.\n";
	};
	// Default Destructor for Array of struct
	~ComNet()
	{
		int i;
		netNode* netWork = netroot;
		for (i = 0; i < netSize; i++) {
			delete[] (netWork[i]).Closeness;
			delete[] (netWork[i]).Connection;
		}
		delete[] netWork;
		this->netroot = NULL;
	};

	unsigned int size()
	{
		return this->netSize;
	};

	void writeNetAdjacency(std::string filename) {
		netNode *netWork = netroot;
		int N = netSize;
		std::ofstream outfile(filename, std::ofstream::out);
		int i, j;
		outfile << "From, To, Weight\n";
		for (i = 0; i < N; i++) {
			for (j = 0; j < (netWork[i]).nConnections; j++) {
				outfile << i << ", " << (netWork[i]).Connection[j] << ", " << (netWork[i]).Closeness[j] << "\n";
			}
		}
		outfile.close();
	};
};

int main(int argc, char *argv[]) {
	int N = atoi(argv[1]);
	int minC = atoi(argv[2]);
	int maxC = atoi(argv[3]);
	ComNet net1(N, minC, maxC);
	std::cout << "TP1\n";
	// ComNet net2(N, minC, maxC);
	net1.writeNetAdjacency("test2.txt");
}