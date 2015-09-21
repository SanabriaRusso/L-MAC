#include <iostream>
#include <fstream>

using namespace std;

void dumpStationLog(ofstream &staFile, int nodes, double throughput, double col, double timeBSxTx)
{
	staFile << nodes << " " << throughput << " " << col << " " << timeBSxTx << endl;
}