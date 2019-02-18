// ComOrgSim.cpp : Defines the entry point for the application.
//

#include "ComOrgSim.h"
#include "ComNet.cpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

struct ctNode
{
	char *status;
};

class netEvolver
{
private:
	int currentStep;
	int timeSteps;
	int tableSize;
	ctNode *tableRoot;
	netNode *netRoot;


public:
	netEvolver(int N, int timesteps, ) {
		ctNode *changeTable = new ctNode[N];
		for (int i = 0; i < N; i++) {
			char *temp = new char[timesteps+1];
			for (int j = 0; j < timesteps+1; j++) {
				temp[j] = '#';
			}
			(changeTable[i]).status = temp;
		}
		this->tableRoot = changeTable;
		this->currentStep = 0;
		this->timeSteps = timesteps+1;
		this->tableSize = N;
	};

	~netEvolver() {
		ctNode *changeTable = this->tableRoot;
		for (int i = 0; i < this->timeSteps; i++) {
			delete[](changeTable[i]).status;
		}
		delete[]changeTable;
	};

	void writeTimeEvolution(char *filename) {
		ctNode *changeTable = this->tableRoot;
		int N = this->tableSize;
		int timesteps = this->timeSteps;
		std::ofstream outfile(filename, std::ofstream::out);
		outfile << "Node";
		for (int j = 0; j < timesteps; j++) {
			outfile << ", T" << j;
		}
		outfile << " \n";
		for (int i = 0; i < N; i++) {
			outfile << i;
			for (int j = 0; j < timesteps; j++) {
				outfile << ", " << (changeTable[i]).status[j];
			}
			outfile << " \n";
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
