// ComOrgSim.cpp : Defines the entry point for the application.
//

#include "ComNet.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

struct ctNode
{
	std::vector<char> status;
};

class netEvolver
{
private:
	bool status_set;
	int currentStep;
	std::vector<ctNode> evolutionTable;

	int time_to_last(int node) {
		int i, time;
		int recs_available = evolutionTable[node].status.size();
		char currentStatus = evolutionTable[node].status[recs_available - 1];
		time = 0;
		while ((time < recs_available) && (evolutionTable[node].status[recs_available-time-1]==currentStatus)) {
			time++;
		}
		return time;
	}

	ctNode makeEmptyNode() {
		ctNode emptyNode;
		emptyNode.status.clear();
		return emptyNode;
	}

public:
	netEvolver(int N) {
		int i;
		this->currentStep = 0;
		this->status_set = false;
		
		for (i = 0; i < N; i++) { evolutionTable.push_back(makeEmptyNode()); }
	};

	void writeTimeEvolution(std::string filename) {
		std::ofstream outfile(filename, std::ofstream::trunc);
		int i, j;
		outfile << "Node";
		for (i = 0; i < evolutionTable[0].status.size(); i++) { outfile << ", " << i; }
		outfile << "\n";
		int node_num = 0;
		for (auto &node_it : evolutionTable) {
			outfile << node_num;
			for (auto &t_step : node_it.status) {
				outfile << ", " << t_step;
			}
			outfile << "\n";
			node_num++;
		}
		outfile.close();
	};

	void setNetStatus(std::vector<char> initialize) {
		int i;
		if (!status_set) {
			if (evolutionTable.size() == initialize.size()) {
				i = 0;
				for (auto &node : evolutionTable) {
					node.status.push_back(initialize[i]);
					i++;
				}
				status_set = true;
			}
			else {
				std::cout << "Evolution Table has " << evolutionTable.size() << " elements. You handed off " << initialize.size() << " inital statuses.\n";
			}
		}
	}

	/*
	Statuses: C - Cleared. I - Infected/colonized. T - Temporarily Immune.
	*/
	void evolve(comNet netWork, int timesteps, double p_clear, int base_clear, double p_self_colonize ) {
		int i, j, k, l, t_since_last_event;
		double infection_prob;
		bool acquired_cleared;
		std::vector<int> neighbors;
		std::vector<double> closeness;
		if (status_set) {
			for (i = currentStep; i < timesteps+currentStep; i++) {
				std::cout << "Current Time: " << i+1 << "\n";
				for (j = 0; j < evolutionTable.size(); j++) {
					// std::cout << "Current Node: " << j << " with status: "<<evolutionTable[j].status[i]<<"\n";
					t_since_last_event = time_to_last(j);
					// If currently cleared...
					if (evolutionTable[j].status[i] == 'C') {
						acquired_cleared = false;
						// Probability of self colonizing currently uniform. Should be exponential?
						if (unif(re) < p_self_colonize) { acquired_cleared = true; }
						// std::cout << "Passed self-col step\n";
						neighbors = netWork.neighbors(j);
						closeness = netWork.closeness(j);
						for (k = 0; k < neighbors.size(); k++) {
							// std::cout << "Checking neighbor " << neighbors[k] << " with infection status: " << evolutionTable[neighbors[k]].status[i] << "\n";
							if (evolutionTable[neighbors[k]].status[i] == 'I') {
								if (unif(re) < closeness[k]) { acquired_cleared = true; }
								/*

								This is where the expontential times closeness comes in. Or does it?

								*/
							}
						}
						if (acquired_cleared) {
							evolutionTable[j].status.push_back('I');
						}
						else {
							evolutionTable[j].status.push_back('C');
						}						
					}
					// If currently colonized...
					if (evolutionTable[j].status[i] == 'I') {
						acquired_cleared = false;
						// Probability of clearing currently uniform. Should be exponential?
						if (unif(re) < p_clear) { acquired_cleared = true; }
						if (acquired_cleared) {
							evolutionTable[j].status.push_back('T');
						}
						else {
							evolutionTable[j].status.push_back('I');
						}
					}
					// If currently immune...
					if (evolutionTable[j].status[i] == 'T') {
						if (t_since_last_event > base_clear) {
							evolutionTable[j].status.push_back('C');
						}
						else {
							evolutionTable[j].status.push_back('T');
						}
						// Check to see how long immunity has lasted. Cancel beyond base_clear.
					}
				}
				
			}
		} else {
			std::cout << "Set intial status distribution first!\n";
		}
	}

	/* Too much power. Instead of this function just make the variable public. Which is a BAD idea.
	int current_step() {
		return this->currentStep;
	}*/

	void current_step(int setStep) {
		this->currentStep = setStep;
	}

};

int main(int argc, char *argv[])
{
	int N, minConnections, maxConnections, timesteps;
	int i, j, k;

	if (argc != 6) { // argc should be at least 2 for correct execution
	  // We print argv[0] assuming it is the program name
		std::cout << "usage: " << argv[0] << "networkSize minNodeConnections maxNodeConnections timesteps <output filename>\n";
		exit(1);
	}
	std::string filename = argv[5];
	if (atoi(argv[1]) < 20) {
		N = 20;
	}
	else { N = atoi(argv[1]); }
	if (((atoi(argv[3]) <= atoi(argv[2])) || (atoi(argv[3]) > N)) || (atoi(argv[2]) < 0)) {
		std::cout << "Invalid Connection Limits\n";
		exit(3);
	}
	else {
		minConnections = atoi(argv[2]);
		maxConnections = atoi(argv[3]);
	}
	if (atoi(argv[4]) < 1) {
		timesteps = 1;
	}
	else {
		timesteps = atoi(argv[4]);
	}
	/////////////// ACTUAL MAIN BLOCK ////////////////////////////
	std::vector<char> inits;
	// Make a network
	comNet randNet(N, minConnections, maxConnections);
	std::cout << "Net Contstruction Complete\n";
	randNet.writeNetEdgelist("ForDisplay.csv");

	// Initialize Evolver
	netEvolver evolvedNet(randNet.node_count());
	std::cout << "Evolver initialized.\n";
	for (i = 0; i < randNet.node_count(); i++) {
		if (randNet.depth(i) == 0) {
			inits.push_back('I');
		}
		else {
			inits.push_back('C');
		}
	}
	std::cout << "Initial statuses list complete.\n";
	evolvedNet.setNetStatus(inits);
	std::cout << "Initial statuses set. \n";
	evolvedNet.evolve(randNet, timesteps, 0.25, 5, 0.01);
	std::cout << "Evolution complete.\n";
	evolvedNet.writeTimeEvolution(filename);
	std::cout << "Done.\n";
	return 0;
}
