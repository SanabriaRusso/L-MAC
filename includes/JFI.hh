#include <iostream>
#include <fstream>

using namespace std;

void JFI(int nodes, double stas[], double &index)
{
	double num,denom;
	num = 0;
	denom = 0;
	for(int i = 0; i < nodes; i++){
		num += stas[i];
		denom += pow(stas[i],2);
	}

	index = (pow(num,2)) / (nodes*denom);
}