#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <stdexcept>

// std::random_device r;     // only used once to initialise (seed) engine
// std::seed_seq seed{ r(),r(),r(),r(),r(),r(),r(),r() };
std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());    // random-number engine used (Mersenne-Twister in this case)

double runif() {  // return a random integer in [from, to] range 
	double temp = 0;
	std::uniform_real_distribution<double> unif(0, 1);
	while ((temp <= 0) || (temp >= 1)) { temp = unif(rng); }
	return temp;
}

int uni_int(int from, int to) {  // return a random integer in [from, to] range 
	std::uniform_int_distribution<int> dist(from, to);
	return dist(rng);
}

// First iteration: undirected graph with weighted edges
// The structure of the node
struct netNode
{
	std::vector<int> Connection;
	std::vector<double> Closeness;
	int Depth;
	char Type;
};

class comNet
{
	// Root node pointer
private:

	std::vector<netNode> netWork;
	int netDiameter = 0;
	int minConnections = 1;
	int maxConnections = 2;

	bool undirected() {
		bool UD = true;
		int start_node_loc = 0;
		int conn_loc = 0;
		for (auto start_node : netWork) {
			conn_loc = 0;
			for (auto conn : start_node.Connection) {
				if (conn >= 0) {
					if ((std::find(netWork[conn].Connection.begin(), netWork[conn].Connection.end(), start_node_loc) == netWork[conn].Connection.end()) ||
					(std::find(netWork[conn].Closeness.begin(), netWork[conn].Closeness.end(), start_node.Closeness[conn_loc]) == netWork[conn].Closeness.end())) {
						UD = false;
						// std::cout << start_node << " not found in node " << conn << "\n";
						// std::cin.ignore();
					}
				}
				conn_loc++;
			}
			start_node_loc++;
		}
		return UD;
	}

	void cleanUpQueue(std::vector<int> &Queue) {
		std::vector<int> tempIntVec = Queue;
		for (auto &node : tempIntVec) {
			if (availableConnections(node) == 0) {
				Queue.erase(std::remove(Queue.begin(), Queue.end(), node), Queue.end());
			}
		}
	}

	void shakeItUp(std::vector<int> &waitQueue, int startNode) {
		int i, j, k, currentNode, edgesToBreak, currentDepth;
		std::vector<int> maxConnectedNet, temp_path_lens, adjNodes;
		int random_netIndex, random_start, random_start_index;
		int random_end, random_end_index;
		bool found_removeable_edge, connection_in_Queue;
		double closeness;
		/* Find fully connected and closed inner net to break open 
		   and largest set of certainly connected nodes (to keep connected)
		if (!undirected()) {
			std::cout << "Idiot! You did not build an undirected graph!\n";
			std::cin.ignore();
		}
		*/
		while (waitQueue.size() >0) {
			currentNode = waitQueue[uni_int(0, waitQueue.size()-1)];
			j = availableConnections(currentNode);
			for (int node:waitQueue) {
				if (availableConnections(node) > j) {
					currentNode = node;
					j = availableConnections(node);
				}
			}
			// std::cout << "Working on " << currentNode << ". Available connections: " << availableConnections(currentNode) << "\n";
			edgesToBreak = floor(availableConnections(currentNode) / 2);
			// std::cout << "Edges: " << edgesToBreak << "\n";
			// Adding node to middle does not break network. Add anywhere.
			if (edgesToBreak > 0) {
				// std::cout << "Inserting node " << currentNode << "\n";
				while (edgesToBreak > 0) {
					found_removeable_edge = false;
					maxConnectedNet.clear();
					temp_path_lens = path_lengths(startNode);
					for (i = 0; i < temp_path_lens.size();i++) {
						if (temp_path_lens[i] < netWork.size()) {
							maxConnectedNet.push_back(i);
						}
					}
					found_removeable_edge = false;
					while (!found_removeable_edge) {
						random_netIndex = uni_int(0, maxConnectedNet.size() - 1);
						random_start = maxConnectedNet[random_netIndex];
						for (int end_node : netWork[random_start].Connection) {
							if ((connectable(currentNode, random_start) && (connectable(currentNode, end_node)))) {
								found_removeable_edge = true;
								// std::cout << "Can split " << random_start << " from " << end_node << "\n";
							}
						}
					}
					random_end = -1;
					while (!connectable(currentNode, random_end)) {
						random_start_index = uni_int(0, (netWork[random_start]).Connection.size() - 1);
						random_end = netWork[random_start].Connection[random_start_index];
					}
					random_end_index = 0;
					while (netWork[random_end].Connection[random_end_index] != random_start) { random_end_index++; }
					// Inserting waitQueue currentNode
					// std::cout << "RSI: " << random_start_index << " REI: " << random_end_index << "\n";
					// std::cout << "SND: " << netWork[random_start].Connection.size() << " RED: " << netWork[random_end].Connection.size() << "\n";
					closeness = runif(); 
					// std::cout << "Ummmm " << closeness << "-----" << runif()<< "\n";
					netWork[random_start].Connection[random_start_index] = currentNode;
					netWork[random_start].Closeness[random_start_index] = closeness;
					
					k = firstOpenIndex(currentNode);
					netWork[currentNode].Connection[k] = random_start;
					netWork[currentNode].Closeness[k] = closeness;
					// std::cout << "Ummmm " << closeness << "\n";
					closeness = runif();
					netWork[random_end].Connection[random_end_index] = currentNode;
					netWork[random_end].Closeness[random_end_index] = closeness;

					k = firstOpenIndex(currentNode);
					netWork[currentNode].Connection[k] = random_end;
					netWork[currentNode].Closeness[k] = closeness;
					edgesToBreak--;
				}
			}
			else {
				// Adding leftover connection could break things. Check that new connection is still network. Swap.
				// Find an in-network edge removable edge from a connectable random_start
				// First check for connections within waitQ.
				// std::cout << "And into the ones...\n";
				// std::cin.ignore();
				connection_in_Queue = false;
				i = 0;
				while ((i < waitQueue.size()) && (!connection_in_Queue)) {
					if (connectable(currentNode, waitQueue[i])) {
						connection_in_Queue = true;
						closeness = runif();
						k = firstOpenIndex(waitQueue[i]);
						netWork[waitQueue[i]].Connection[k] = currentNode;
						netWork[waitQueue[i]].Closeness[k] = closeness;
						k = firstOpenIndex(currentNode);
						netWork[currentNode].Connection[k] = waitQueue[i];
						netWork[currentNode].Closeness[k] = closeness;
					}
					i++;
				}
				// If no connections within waitQ, find a removable edge to swap into waitQueue
				if (!connection_in_Queue) {
					maxConnectedNet.clear();
					temp_path_lens = path_lengths(startNode);
					for (i = 0; i < temp_path_lens.size(); i++) {
						if (temp_path_lens[i] < netWork.size()) {
							maxConnectedNet.push_back(i);
						}
					}
					found_removeable_edge = false;
					while (!found_removeable_edge) {
						// First check to see if it's connectable to currentNode
						random_netIndex = uni_int(0, maxConnectedNet.size() - 1);
						random_start = maxConnectedNet[random_netIndex];
						while (!connectable(currentNode, random_start)) {
							random_netIndex = uni_int(0, maxConnectedNet.size() - 1);
							random_start = maxConnectedNet[random_netIndex];
						}
						// std::cout << "Disconnecting " << random_start << "\n";
						// Then check to see if it has a removeable edge by
						// Systematically exploring the list of connections
						i = 0;
						while ((i < netWork[random_start].Connection.size()) && (!found_removeable_edge)) {
							closeness = netWork[random_start].Closeness[i];
							random_end = netWork[random_start].Connection[i];
							if (random_end >= 0) {
								random_end_index = 0;
								// Find the start node by index in the end_node connections
								while (netWork[random_end].Connection[random_end_index] != random_start) { 
									// if (random_end_index > netWork[random_end].Connection.size()) {
									//	 std::cout << netWork[random_end].Connection[random_end_index] << "\n";
									// }
									random_end_index++;
								}
								adjNodes.clear();
								adjNodes.push_back(random_start);
								adjNodes.push_back(random_end);
								// if (!nodesAreConnected(adjNodes)) {
								//	std::cout << random_start << " and " << random_end << "Not connected before clip\n";
								//	std::cin.ignore();
								//}

								// Breaking them
								// std::cout << "Disconnecting " << random_start << " from " << random_end << "... ";
								netWork[random_start].Connection[i] = -1;
								netWork[random_start].Closeness[i] = -1;
								netWork[random_end].Connection[random_end_index] = -1;
								netWork[random_end].Closeness[random_end_index] = -1;
								adjNodes.clear();
								adjNodes.push_back(random_start);
								adjNodes.push_back(random_end);
								// Checking the connection from the broken ends
								if (nodesAreConnected(adjNodes)) {
									found_removeable_edge = true;
									random_start_index = i;
								}
								else {
								// If ends not connected, reconnect and drive on.
								// std::cout << "Reconnecting.\n";
								// std::cout << "Random start index: " << i << " random_start degree: " << netWork[random_start].Connection.size() << "\n";
								netWork[random_start].Connection[i] = random_end;
								netWork[random_start].Closeness[i] = closeness;
								// std::cout << "Random end index: " << random_end_index << " random_end degree: " << netWork[random_end].Connection.size() << "\n";
								netWork[random_end].Connection[random_end_index] = random_start;
								netWork[random_end].Closeness[random_end_index] = closeness;
								}
							}
							i++;
						}
					}
					// Singleton case: Singletons connect to any node.
					// Possible different treatment other singleton case.
					// std::cout << "connecting " << random_start << " to " << currentNode << "\n";
					closeness = runif();
					netWork[random_start].Connection[random_start_index] = currentNode;
					netWork[random_start].Closeness[random_start_index] = closeness;
					k = firstOpenIndex(currentNode);
					netWork[currentNode].Connection[k] = random_start;
					netWork[currentNode].Closeness[k] = closeness;
					// Return random_end to wait_Q if not alread there
					if (std::find(waitQueue.begin(), waitQueue.end(), random_end) == waitQueue.end()) { waitQueue.push_back(random_end); }
				}
			}
			cleanUpQueue(waitQueue);
			if (!undirected()) { std::cout << "Broke some shit in the Shake Up module. Network no longer undirected...\n"; }
			// std::cout << "WaitQ at " << waitQueue.size() << " nodes.\n";
		}//end while waitQueue loop
	}

	bool nodesAreConnected(std::vector<int> node_list) {
		int i;
		int start_node = node_list[uni_int(0, node_list.size() - 1)];
		bool connectd = true;
		// path_lengths not returned in order
		std::vector<int> path_lens = path_lengths(start_node);
		for (auto node : node_list) {
			if (path_lens[node] >= netWork.size()) { connectd = false; }
		}
		return connectd;
	}

	bool connectable(int a, int b) {
		bool status = true;
		// No Invalid Nodes
		if ((a < 0) || (b < 0)) { status = false; }
		// No self connections
		if (a == b) { status = false; }
		// No connecting full a node that's already full
		// if ((availableConnections(a) == 0) || (availableConnections(b) == 0)) { status = false; }
		// Check for existing a to b connection
		for (auto it = netWork[a].Connection.begin(); it != netWork[a].Connection.end(); ++it) {
			if (*it == b) {
				status = false;
			}
		}
		// Require at least one type 'P' node in connection
		if ((netWork[a].Type != 'P') && (netWork[b].Type != 'P')) {
			status = false;
		}
		return status;
	}

	netNode makeEmptyNode() {
		netNode emptyNode;
		// Choose your degree dirstribution here - maybe a function later
		int degree = uni_int(minConnections, maxConnections);
		// --------------------------------------------------
		// Choose Node Types - Just people for alpha
		emptyNode.Type = 'P';
		// --------------------------------------------------
		emptyNode.Depth = -1;
		for (int j = 0; j < degree; j++) {
			emptyNode.Connection.push_back(-1);
			emptyNode.Closeness.push_back(-1);
		}
		return emptyNode;
	}

	int availableConnections(int node) {
		int space = 0;
		for (auto &conn : netWork[node].Connection) {
			if (conn < 0) { space += 1; }
		}
		return space;
	}

	int availableConnections(netNode node) {
		int space = 0;
		for (auto &conn : node.Connection) {
			if (conn < 0) { space += 1; }
		}
		return space;
	}

	int firstOpenIndex(int node) {
		int i = 0;
		while ((netWork[node].Connection[i] >= 0) && (i < netWork[node].Connection.size())) { i++; }
		if (i >= netWork[node].Connection.size()) { i = -1; }
		return i;
	}

	int firstOpenIndex(netNode node) {
		int i = 0;
		while (node.Connection[i] >= 0) { i++; }
		return i;
	}

	std::vector<std::string> split(std::string line, std::string delimiter) {
		std::vector<std::string> dataOut;
		std::string whitespaces(" \t\f\v\n\r");
		std::string tempWord;
		std::size_t word_start, word_end, start_pos = 0;
		//Split line at delimiters
		std::size_t found = line.find_first_of(delimiter);
		while (found != std::string::npos)
		{
			tempWord = line.substr(start_pos, found - start_pos);
			// Strip word then strip lead/trail space
			word_start = tempWord.find_first_not_of(whitespaces);
			if (word_start != std::string::npos) {
				word_end = tempWord.find_last_not_of(whitespaces);
				if (word_end != std::string::npos) { tempWord.erase(word_end + 1); }
				dataOut.push_back(tempWord.substr(word_start));
			}
			else { dataOut.push_back(""); }
			start_pos = found + 1;
			found = line.find_first_of(delimiter, start_pos);
		}
		// Catch Last in line
		if (start_pos != std::string::npos) {
			tempWord = line.substr(start_pos);
			word_start = tempWord.find_first_not_of(whitespaces);
			if (word_start != std::string::npos) {
				word_end = tempWord.find_last_not_of(whitespaces);
				if (word_end != std::string::npos) { tempWord.erase(word_end + 1); }
				dataOut.push_back(tempWord.substr(word_start));
			}
			else { dataOut.push_back(""); }
		}
		return dataOut;
	}

	std::vector<std::vector<std::string> > getData(std::string fileName) {
		std::ifstream file(fileName);
		std::vector<std::string> vec;
		std::string delimiter(",;");
		std::vector<std::vector<std::string> > dataList;

		std::string line = "";
		// Iterate through each line and split the content using delimeter
		while (getline(file, line))
		{
			vec = split(line, delimiter);
			dataList.push_back(vec);
		}
		// Close the File
		file.close();

		return dataList;
	}

public:
	// Default Constructor for Random Network 
	comNet(const int Nmin, const int minCon, const int maxCon)
	{
		int i, j, k, l;
		double closeness = 0;
		int avgDegree = 0;
		int degree = 0;
		int degreeTotal = 0;
		bool stick_or_loop = false;
		std::vector<int> tempIntVec;
		netNode emptyNode;

		std::uniform_real_distribution<double> unif(0, 1);
		std::default_random_engine re;
		minConnections = minCon;
		maxConnections = maxCon;
		// Validate maxConnections and minConnections
		if (minConnections < 1) { minConnections = 1; }
		if (maxConnections < 2) { maxConnections = 2; }
		if (minConnections > maxConnections) {
			int temp = minConnections;
			minConnections = maxConnections;
			maxConnections = temp;
		}
		// Make new random nodes -- assign node degrees in makeEmptyNode function above
		for (i = 0; i < Nmin; i++) {
			emptyNode = makeEmptyNode();
			// std::cout << "Node " << i << " has Degree " << emptyNode.Connection.size() << "\n";
			degreeTotal += emptyNode.Connection.size();
			netWork.push_back(emptyNode);
		}
		// Check for minimum connected undirected graph conditions 
		if (maxConnections > 2) {
			while ((degreeTotal % 2 != 0) || (edge_count() < Nmin - 1)) {
				i = uni_int(0, Nmin - 1);
				if (netWork[i].Connection.size() < maxConnections) {
					netWork[i].Connection.push_back(-1);
					netWork[i].Closeness.push_back(-1);
					degreeTotal += 1;
				}
			}
		}
		else {
			// maxConnections==2 dictates a loop or stick
			// Make a stick
			stick_or_loop = true;
			i = 0;
			for (auto &node : netWork) {
				node.Closeness.clear();
				node.Connection.clear();
				if (i == 0) {
					node.Connection.push_back(i + 1);
					node.Closeness.push_back(runif());
				}
				else if (i == netWork.size() - 1) {
					node.Connection.push_back(i - 1);
					node.Closeness.push_back(netWork[i - 1].Closeness[0]);
				}
				else {
					node.Connection.push_back(i + 1);
					node.Closeness.push_back(runif());
					node.Connection.push_back(i - 1);
					node.Closeness.push_back(netWork[i - 1].Closeness[0]);
				}
				i++;
			}
			// Randomly make a loop (unless minConnections==2 dictates a loop)
			if ((uni_int(0, 1)) || (minConnections == 2)) {
				netWork[0].Connection.push_back(netWork.size()-1);
				netWork[0].Closeness.push_back(runif());
				netWork[netWork.size() - 1].Connection.push_back(0);
				netWork[netWork.size() - 1].Closeness.push_back(netWork[0].Closeness[1]);
			}
		}
		// TODO: Complete and Nearly Complete Networks
		std::cout << netWork.size() << " Nodes with " << edge_count()*2 << " open connections.\n";
		// Main Network Assembly
		int startNode = 0;
		std::vector<int> waitQueue, depthQueue;
		std::vector<int> connectableWaits;
		int currentDepth = 0;
		int depthQCap = 0;
		int nextDepthQCap, randomConnection;
		bool isNotStuck = true;
		j = 0;
		// initial waitQueue and find start node
		for (i = 0; i < netWork.size(); i++) {
			if (netWork[i].Connection.size() > j) {
				startNode = i;
				j = netWork[i].Connection.size();
			}
			waitQueue.push_back(i);
		}
		netWork[startNode].Depth = 0;

		if (stick_or_loop) { 
			waitQueue.clear();
			startNode = uni_int(0, netWork.size() - 1);
		}
		// Start building!
		while ((waitQueue.size() > 0) && isNotStuck) {
			// If depthQ has been cleared move on to next depth
			if (depthQCap == 0) {
				depthQueue.clear();
				//Find all nodes at current depth and put in depthQueue
				for (i = 0; i < netWork.size(); i++) {
					if ((netWork[i].Depth == currentDepth) && (availableConnections(i) > 0)) {
						depthQueue.push_back(i);
						depthQCap += availableConnections(i);
					}
				}
				currentDepth += 1;
			}
			// std::cout << "Depth " << currentDepth-1 << " has capacity: " << depthQCap << "\n";
			//for (auto &node : depthQueue) {
			//	std::cout << "dQ node " << node << " with " << availableConnections(node) << " slots.\n";
			//}
			// Start adding nodes to current depth
			nextDepthQCap = 0;
			for (auto &node_i : depthQueue) {
				// std::cout << "Expecting to make " << availableConnections(node_i) << " connections. " << depthQCap << " remaining.\n";
				for (k = 0; k < netWork[node_i].Connection.size();k++) {
					// If no connections at this index...
					if (netWork[node_i].Connection[k] < 0) {
						// Find available connections in the waitQueue
						// std::cout << "Attemping to connect node " << node_i << " at index " << k << "... \n";
						connectableWaits.clear();
						for (l = 0; l < 5; l++) {
							randomConnection = waitQueue[uni_int(0, waitQueue.size() - 1)];
							if (connectable(node_i, randomConnection)) {
								if ((depthQCap == 1) && (nextDepthQCap == 0)) {
									if (availableConnections(randomConnection) > 1) {
										connectableWaits.push_back(randomConnection);
									}
								}
								else {
									connectableWaits.push_back(randomConnection);
								}
							}
						}
						if (connectableWaits.size() == 0) {
							for (i = 0; i < waitQueue.size(); i++) {
								if (connectable(node_i, waitQueue[i])) {
									// Check for bridges at the end of the depthQueue
									if ((depthQCap == 1) && (nextDepthQCap == 0)) {
										if (availableConnections(waitQueue[i]) > 1) {
											connectableWaits.push_back(waitQueue[i]);
										}
									}
									else {
										connectableWaits.push_back(waitQueue[i]);
									}
								}
							}
						}
						//std::cout << "Found " << connectableWaits.size() << " open nodes -- ";
						if (connectableWaits.size() > 0) { // Make a connection
							randomConnection = connectableWaits[uni_int(0, connectableWaits.size() - 1)];
							closeness = runif();
							netWork[node_i].Connection[k] = randomConnection;
							netWork[node_i].Closeness[k] = closeness;
							//std::cout << "." << availableConnections(randomConnection) <<".";
							j = firstOpenIndex(randomConnection);
							// std::cout << ".";
							netWork[randomConnection].Connection[j] = node_i;
							netWork[randomConnection].Closeness[j] = closeness;
							netWork[randomConnection].Depth = currentDepth;
							// std::cout << "." << availableConnections(randomConnection);
							depthQCap += -1;
							if (std::find(depthQueue.begin(), depthQueue.end(), randomConnection) != depthQueue.end()) {
								depthQCap += -1;
							}
							nextDepthQCap += availableConnections(randomConnection);
							//std::cout << " Connected. \n";
							// Adjust waitQueue for full nodes
							if (availableConnections(randomConnection) == 0) {
								waitQueue.erase(std::remove(waitQueue.begin(), waitQueue.end(), randomConnection), waitQueue.end());
								// std::cout << " Removing InNode " << randomConnection << " from waitQ.\n";
							}
						}
						else {
							// std::cout << "Whaaa.....\n";
						}
					} // Open Connection Condition
				} // Connections loop
				// std::cout << "Remaining depthQCap = "<<depthQCap;
				if (availableConnections(node_i) == 0) {
					waitQueue.erase(std::remove(waitQueue.begin(), waitQueue.end(), node_i), waitQueue.end());
					// std::cout << " -- Removed OutNode " << node_i << " from waitQ and depthQ --";
				}
				// std::cout << " and done!\n";
			} // Depth loop
			// Make sure the depthQueue is connected 
			if (depthQCap > 0) {
				// Take out the trash
				cleanUpQueue(depthQueue);
				// std::cout << "WaitQ: ";
				// for (int node : waitQueue) { std::cout << node << ", "; }
				// std::cout << "\n";

				// Make final connection or
				if (((waitQueue.size() == 2) && connectable(waitQueue[0],waitQueue[1])) &&
					((availableConnections(waitQueue[0]) ==1) && (availableConnections(waitQueue[1]==1)))) {
					closeness = runif();
					i = firstOpenIndex(waitQueue[0]);
					netWork[waitQueue[0]].Connection[i] = waitQueue[1];
					netWork[waitQueue[0]].Closeness[i] = closeness;
					if (netWork[waitQueue[0]].Depth < 0) { netWork[waitQueue[0]].Depth = currentDepth; }
					i = firstOpenIndex(waitQueue[1]);
					netWork[waitQueue[1]].Connection[i] = waitQueue[0];
					netWork[waitQueue[1]].Closeness[i] = closeness;
					if (netWork[waitQueue[1]].Depth < 0) { netWork[waitQueue[1]].Depth = currentDepth; }
					waitQueue.clear();
				}
				else {
					int waitQCap = 0;
					cleanUpQueue(waitQueue);
					for (auto &node : waitQueue) {
						waitQCap += availableConnections(node);
					}
					// std::cout << "WaitQ at " << waitQueue.size() << " with  " << waitQCap << " open edges. Stuck...";
					// Stuck. We have a furry monster... Plan A. Add a couple more nodes.
					/* 
					std::cout << " adding nodes...\n";
					std::vector<int> newWaitQueue;
					int waitQCap, minNewNodes;
					int MORE_NODES = 0;
					while (waitQCap > minConnections) {
						minNewNodes = 0;
						waitQCap = 0;
						for (auto &node : waitQueue) { 
							i = availableConnections(node);
							waitQCap += i;
							if (i > minNewNodes) { minNewNodes = i; }
						}
						// Give 'em some nodes.
						if (ceil(waitQCap / maxConnections) > minNewNodes) {
							MORE_NODES = ceil(waitQCap / maxConnections);
						}
						else {
							MORE_NODES = minNewNodes;
						}
						std::cout << "Pumping in " << MORE_NODES << " more nodes.\n";
						for (i = 0; i < MORE_NODES; i++) {
							emptyNode = makeEmptyNode();
							netWork.push_back(emptyNode);
							newWaitQueue.push_back(netWork.size() - 1);
						}
						// Try 'n connect 'em all. Just make sure yer node degrees are in the min/max range. 
						while () {

						}
						}
					} */
					//Stuck. Plan B. Tuck that shit in and hope for the best.
					//Likely slight change in Node Degree distribution...
					//Add flag to network writer.
					// std::cout << " Shaking it up...\n";
					shakeItUp(waitQueue, startNode);

				}
			} 
			/*
			else {
				// Check then remove from waitQueue
				int sanityCheck = 0;
				for (auto &node : depthQueue) {
					// std::cout << "dQ node " << node << " has " << availableConnections(node) << ".\n";
					if (availableConnections(node) == 0) { sanityCheck += 1; }
				}
				if (depthQueue.size() == sanityCheck) {
					// std::cout << "Depth " << currentDepth-1 << " complete. waitQueu has "<<waitQueue.size()<<" nodes.\n";
					
					if (!undirected()) {
						std::cout << "D'oh! Not directed. \n";
						for (auto node : netWork[startNode].Connection) {
							std::cout << "To " << node << "--";
							for (int val : netWork[node].Connection) {
								if (val >= 0) { std::cout << "From " << val << "\n"; }
							}
							if (std::find(netWork[node].Connection.begin(), netWork[node].Connection.end(), startNode) == netWork[node].Connection.end()) {
								std::cout << "Looking for " << node << "\n";
							}
						}
						std::cin.ignore();

					}
				}
				else {
				// FREAK OUT!!! Something went wrong... BAIL!!
				std::cout << "Oh shit....\n";
				isNotStuck = false;
				} 
			}
			*/
		} // Main While Loop
		// Make sure node depth is correct (especially after a shake, stick or loop)
		std::vector<int> currentDQueue, nextDQueue, toDoQueue;
		for (i = 0; i < netWork.size(); i++) { toDoQueue.push_back(i); }
		currentDepth = 0;
		currentDQueue.push_back(startNode);
		while (toDoQueue.size() > 0) {
			for (int node : currentDQueue) {
				if (std::find(toDoQueue.begin(), toDoQueue.end(), node) != toDoQueue.end()) {
					netWork[node].Depth = currentDepth;
					for (int node2 : netWork[node].Connection) {
						nextDQueue.push_back(node2);
					}
					toDoQueue.erase(std::remove(toDoQueue.begin(), toDoQueue.end(), node), toDoQueue.end());
				}
			}
			currentDepth++;
			currentDQueue.clear();
			currentDQueue = nextDQueue;
			nextDQueue.clear();
		}
		// Last ditch error handling
		if (!isConnected()) {
			std::cout << "Something went wrong. Network not connected... sigh...";
		}

	}
	// Constructor from saved network
	comNet(const std::string filename) {
		minConnections = ceil(average_degree());
		maxConnections = floor(average_degree());

		readNetEdgelist(filename);
		for (auto node : netWork) {
			if (node.Connection.size() < minConnections) {
				minConnections = node.Connection.size();
			}
			else {
				if (node.Connection.size() > maxConnections) {
					maxConnections = node.Connection.size();
				}
			}
		}
	}
	// Standard Network Value Returns
	std::vector<int> neighbors(int Node)
	{
		return netWork[Node].Connection;
	}

	int depth(int node) {
		return netWork[node].Depth;
	}

	std::vector<double> closeness(int Node)
	{
		return netWork[Node].Closeness;
	}
	
	double average_degree() {
		double average;
		int total = 0;

		for (auto &node : netWork) {
			total += node.Connection.size();
		}
		average = total / netWork.size();
		return average;
	}

	int node_count()
	{
		return netWork.size();
	}

	int average_weight() {
		double total_weight = 0;
		int edge_count = 0;
		for (auto node : netWork) {
			for (auto weight : node.Closeness) {
				total_weight += weight;
				edge_count++;
			}
		}
		return (total_weight / edge_count);
	}

	int edge_count() {
		// Assumes Undirected
		int total_edges = 0;
		for (auto &node : netWork) {
			total_edges += node.Connection.size();
		}
		return total_edges / 2;
	}

	std::vector<int> path_lengths(int a)
	{
		// Dijkstra's algorithm. Could be better with Thorup (1999)
		int v, w, i;
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

	// THIS ALGORITHM IS BROKEN. IT NEEDS TO SPAN THE WHOLE NETWORK.
	// OR BE RE-CREATED FOR A DIFFERENT PURPOSE
	std::vector<int> path_lengths(int start_node, std::vector<int> node_list)
	{
		/* Dijkstra's algorithm. Could be better with Thorup (1999)
		   Modified to operate on a subnet defined in node_list
		   Assumes start_node in node_list. 
		   Output distance will be sorted by node: smallest-->largest. */
		int v, w, i;
		std::vector<int> dist;
		std::vector<int> Q;
		for (i = 0; i < netWork.size(); i++) {
			dist.push_back(netWork.size() + 1);
		}

		for (i = 0; i < node_list.size(); i++) {
			dist.push_back(node_list.size() + 1);
			Q.push_back(node_list[i]);
		}
		dist[start_node] = 0;

		while (Q.size() > 0) {
			v = 0;
			for (i = 0; i < Q.size(); i++) { if (dist[Q[i]] < dist[Q[v]]) { v = i; } }
			w = Q[v];
			Q.erase(Q.begin() + v);
			for (auto &toNode : netWork[w].Connection) {
				if (std::find(node_list.begin(), node_list.end(), toNode) != node_list.end()) {
					if (dist[w] + 1 < dist[toNode]) { dist[toNode] = dist[w] + 1; }
				}
			}
		}
		// Filter dist by node_list. 
		for (i = netWork.size() - 1; i >= 0; i--) {
			if (std::find(node_list.begin(), node_list.end(), i) == node_list.end()){
				dist.erase(dist.begin() + i);
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
		int i, j;
		int ret_val = 0;
		std::ofstream outfile(filename, std::ofstream::trunc);
		outfile << "Source, Target, Weight\n";   //, Type, Depth\n";
		i = 0;
		for (auto &it : netWork) {
			for (j = 0; j < it.Connection.size(); j++) {
				// std::cout << i << ", " << it.Connection[j] << ", " << it.Closeness[j] << ", " << it.Type << ", " << it.Depth << "\n";
				outfile << i << ", " << it.Connection[j] << ", " << std::setprecision(std::numeric_limits<long double>::digits10) << it.Closeness[j] << "\n"; //<< ", " << it.Type << ", " << it.Depth
			}
			i++;
		}
		outfile.close();
	}

	void readNetEdgelist(std::string filename) {
		int i, j;
		int fromNode, toNode, Depth;
		double weight;
		char Type;
		std::vector<std::vector<std::string>> data = getData(filename);
		netWork.clear();
		for (auto line : data) {
			if (line.size() > 4) {
				throw std::invalid_argument("Not an edge list.");
				netWork.clear();
				break;
			}
			try {
				fromNode = std::stoi(line[0]);
				toNode = std::stoi(line[1]);
				weight = std::stod(line[2]);
				Type = line[3][0];
				Depth = std::stoi(line[4]);
				netWork[fromNode].Connection.push_back(toNode);
				netWork[fromNode].Closeness.push_back(weight);
				netWork[fromNode].Type = Type;
				netWork[fromNode].Depth = Depth;
			}
			catch (const std::exception &e) {
				// Catch and ignore Header and other invalid lines
				// std::cout << e.what() << "\n";
			}
		}
		if (undirected()) {
			std::cout << "This network is undirected and ";
			if (isConnected()) {
				std::cout << "connected.\n";
			}
			else {
				std::cout << "not connected.\n";
			}
		}
		else {
			std::cout << "This is a directed network.\n";
		}
	}

	// NEEDS WORK
	void readAdjacencyMatrix(std::string filename) {
		int i, j;
		int fromNode, toNode, Depth;
		double weight;
		char Type;
		std::vector<std::vector<std::string>> data = getData(filename);
		netWork.clear();
		for (i = 0; i < data.size(); i++) {
			for (j = 0; j < data[i].size();j++) {
				if (data[i].size() != data.size()) {
					throw std::invalid_argument("Not an adjacency matrix.");
					netWork.clear();
					break;
				}
				try {
					fromNode = i;
					toNode = j;
					weight = std::stod(data[i][j]);
					Type = '\0';
					Depth = 0;
					if (weight != 0) {
						netWork[fromNode].Connection.push_back(toNode);
						netWork[fromNode].Closeness.push_back(weight);
						netWork[fromNode].Type = Type;
						netWork[fromNode].Depth = Depth;
					}
				}
				catch (const std::exception &e) {
					// Catch and ignore Header and other invalid lines
					// std::cout << e.what() << "\n";
				}
			}
		}
		if (undirected()) {
			std::cout << "This network is undirected and ";
			if (isConnected()) {
				std::cout << "connected.\n";
			}
			else {
				std::cout << "not connected.\n";
			}
		}
		else {
			std::cout << "This is a directed network.\n";
		}
	}

};
