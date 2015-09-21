#include <iostream>
#include <fstream>

using namespace std;

void dumpWholeData(ofstream &file, int nodes, double throughput, double fractCol, double col, double avgTimeBSxTx)
{
	file << nodes << " " << throughput << " " << fractCol << " " << col << " " 
	<< avgTimeBSxTx << endl;
}