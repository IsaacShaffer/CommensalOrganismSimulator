// ComOrgSim.cpp : Defines the entry point for the application.
//

#include "ComOrgSim.h"
#include "ComNet.cpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

struct ctNode
{
	std::vector<char> status;
};

class netEvolver
{
private:
	ComNet *netWork;
	int currentStep;
	int timeSteps;
	std::vector<ctNode> evolutionTable;

public:
	netEvolver(ComNet in_netWork) {
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

	void setNetStatus(int timestep, char *intialize) {
		ctNode *changeTable = this->tableRoot;
		int step;
		if (timestep < 0) { step = 0; }
		if (timestep > this->timeSteps) { step = timeSteps; }
		for (int i = 0; i < this->tableSize; i++) {
			(changeTable[i]).status[step] = intialize[i];
		}
	}

	void evolve(ComNet *netWork, int timesteps) {
		char *nextStep = new [this->tableSize];
		int lastStep = this->currentStep + timesteps;
		if (lastStep > this->timeSteps) {
			lastStep = this->timeSteps;
		}
		if (timesteps >= 1) {

		}
		delete[] nextStep;
	}

	int current_step(int setStep) {
		return this->current_step;
	}

};




int main(int argc, char *argv[])
{
	if (argc != 6) { // argc should be at least 2 for correct execution
	  // We print argv[0] assuming it is the program name
		cout << "usage: " << argv[0] << "networkSize minNodeConnections maxNodeConnections timesteps <output filename>\n";
		exit(1);
	}
	char *filename = argv[5];
	int N, minConnections, maxConnections, timesteps;
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

	
	return 0;
}
