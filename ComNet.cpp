#include <random>

using namespace std;
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
	netNode *netroot;
	unsigned int netSize;

public:
	// Default Constructor for Array
	ComNet(int N, int maxConnections, int minConnections)
	{
		this->netSize = N;
		netNode* netWork = new netNode[N];
		this->netroot = netWork;

		std::random_device rd;     // only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(minConnections, maxConnections); // guaranteed unbiased
		std::uniform_real_distribution<double> unif(0, 1);
		std::default_random_engine re;
		// auto random_integer = uni(rng);
		// double a_random_double = unif(re);
		int i, j;
		int degreeTotal = 0;
		for (i = 0; i < netSize; i++) {
			auto degree = uni(rng);
			netWork[i].Connection = new int[degree];
			netWork[i].Closeness = new double[degree];
			netWork[i].nConnections = degree;
			netWork[i].Type = 'P';
			for (j = 0; j < degree; j++) {
				netWork[i].Connection[j] = -1;
				netWork[i].Closeness[j] = 0;
			}
			degreeTotal = degreeTotal + degree;
		}
		for (i = 0; i < netSize - 1; i++)
		{
			for (j = i; j < netSize; j++)
			{
				
			}
		}
	}
	// Default Destructor for Prefix Trie
	~ComNet()
	{
		int i;
		netNode* netWork = netroot;
		for (i = 0; i < netSize; i++) {
			delete[] netWork[i].Closeness;
			delete[] netWork[i].Connection;
		}
		delete[] netWork;
		this->netroot = NULL;
	}

	unsigned int size()
	{
		return this->netSize;
	}

}