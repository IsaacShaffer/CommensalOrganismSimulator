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
	comNet *netWork;
	int currentStep;
	int timeSteps;
	std::vector<ctNode> evolutionTable;

public:
	netEvolver(comNet in_netWork) {
		int N = in_netWork.node_count();
		int i;
		this->netWork = &in_netWork;
		ctNode emptyNode;
		
		for (i = 0; i < N; i++) { evolutionTable.push_back(emptyNode); }
	};

	void writeTimeEvolution(std::string filename) {
		std::ofstream outfile(filename, std::ofstream::out);
		int i, j;
		outfile << "Header\n";
		int node_num = 0;
		for (auto &node_it : evolutionTable) {
			node_num++;
			outfile << node_num;
			for (auto &t_step : node_it.status) {
				outfile << ", " << t_step;
			}
			outfile << "\n";
		}
		outfile.close();
	};

	void setNetStatus(char *intialize) {
		int i;
		if (!status_set) {
			i = 0;
			for (auto &node : evolutionTable) {
				node.status[0] = intialize[i];
				i++;
			}
			status_set = true;
		}
	}

	void evolve(int timesteps, double p_clear, int base_clear, double p_self_colonize ) {
		std::vector<char> nextStep;
		int i, j;
		if (status_set) {
			
			for (i = 0; i < timesteps; i++) {
				j = 0;
				for (auto &node : evolutionTable) {

					j++;
				}
				
			}
		} else {
			std::cout << "Set intial status distribution first!\n";
		}
	}

	int current_step(int setStep) {
		return this->currentStep;
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
	std::vector<int> distances;
	comNet randNet(N, minConnections, maxConnections);
	std::cout << "Net Contstruction Complete\n";
	netEvolver evolvedNet(randNet);
	distances = randNet.path_lengths(21);
	// for (auto &it : randNet.neighbors(0)) { std::cout << "0 connected to " << it << "\n"; }
	// for (i = 0; i < distances.size(); i++) { std::cout << "21 to " << i<<": "<< distances[i] << "\n"; }
	// std::cout << "Diameter = " << randNet.diameter() << "\n";
	randNet.writeNetEdgelist("ForDisplay.csv");
	return 0;
}
