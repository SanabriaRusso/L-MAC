#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "./COST/cost.h"
#include <deque>
#include "Channel.h"
#include "STA.h"
#include "BatchPoissonSource.h"
#include "stats/stats.h"
#include "includes/dumpWholeData.hh"

using namespace std;

component SlottedCSMA : public CostSimEng
{
	public:
		void Setup(int NumNodes, int PacketLength, double Bandwidth, int protocol, double channelErrors, int simSeed);
		void Stop();
		void Start();		

	public:
		Channel channel;
		STA [] stas;
		BatchPoissonSource [] sources;

	private:
		int Nodes;
		double Bandwidth_;
		int PacketLength_;
};

void SlottedCSMA :: Setup(int NumNodes, int PacketLength, double Bandwidth, int protocol, double channelErrors, int simSeed)
{
	Nodes = NumNodes;
	stas.SetSize(NumNodes);
	sources.SetSize(NumNodes);

	// Channel	
	channel.Nodes = NumNodes;
	channel.out_slot.SetSize(NumNodes);
	channel.error = channelErrors;
	for(int n = 0; n < NumNodes; n++){
		// Node
		stas[n].id = n;
		stas[n].K = 1000;	//maximum queue size
		stas[n].protocol = protocol;
		stas[n].L = PacketLength;
		stas[n].nodesInSim = NumNodes;

		// Traffic Source
		sources[n].L = PacketLength;
		sources[n].packetsGenerated = 0;
		sources[n].MaxBatch = 1;
		sources[n].packet_rate = Bandwidth/(PacketLength * 8);
	}
	
	// Connections
	for(int n=0;n<NumNodes;n++)
	{
        connect channel.out_slot[n],stas[n].in_slot;
		connect stas[n].out_packet,channel.in_packet;
		connect sources[n].out,stas[n].in_packet;
	}

	//Copy of the variables to be used in the Stop() method.
	Bandwidth_ = Bandwidth;
	PacketLength_ = PacketLength;
	Nodes = NumNodes;
};

void SlottedCSMA :: Start()
{
	cout << ("--------------- Starting ---------------") << endl;
};

void SlottedCSMA :: Stop()
{	

	double acummThroughput = 0;
	double avgFracCollisions = 0;
	double totalCollisions = channel.collision_slots;
	double avgTimeBetSxTx = 0;
	for(int i = 0; i < Nodes; i++){
		acummThroughput += stas[i].throughput;
		avgFracCollisions += stas[i].collisions;
		avgTimeBetSxTx += stas[i].accummTimeBetSxTx;
	}
	// acummThroughput = channel.totalBitsSent / SimTime();
	avgFracCollisions /= Nodes;
	avgTimeBetSxTx /= Nodes;


	cout << "--------------- Results ---------------" << endl;
	cout << "Aggregated Throughput: " << acummThroughput << endl;
	cout << "Avg. Frac. Collisions: " << avgFracCollisions << endl;
	cout << "Avg. Time. Bet. SxTx: " << avgTimeBetSxTx << endl;

	//--------------------------------------------------------------//
	//-------------------Writing the results------------------------//
	//--------------------------------------------------------------//

	ofstream output;
	output.open("Results/output.txt", ios::app);
	dumpWholeData(output,Nodes,acummThroughput,avgFracCollisions,totalCollisions,avgTimeBetSxTx);
	// output.close();
	
};

int main(int argc, char *argv[])
{
	double SimTime;
	int NumNodes;
	int PacketLength;
	double Bandwidth;
	int protocol;	//0 = DCF, 1 = L-MAC
	double channelErrors;
	int simSeed;
	SlottedCSMA test;
	
	if(argc == 8){
		SimTime = atof(argv[1]);
		NumNodes = atoi(argv[2]);
		PacketLength = atoi(argv[3]);
		Bandwidth = atof(argv[4]);
		protocol = atoi(argv[5]);
		channelErrors = atof(argv[6]);
		simSeed = atoi(argv[7]);

	}else{
		cout << "ALERT! Lacking parameters: simTime NumNodes PacketLength Bandwidth protocol channelErrors simSeed" << endl;
		cout << argc << endl;
		return(1);
	}

	test.Seed = simSeed;
	test.StopTime(SimTime);
	test.Setup(NumNodes,PacketLength,Bandwidth, protocol, channelErrors, simSeed);
	test.Run();
	return(0);
};
