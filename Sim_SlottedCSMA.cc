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
// #include "BatchPoissonSource.h"
#include "BatchPoissonSource_enhanced.h"
#include "stats/stats.h"



using namespace std;

component SlottedCSMA : public CostSimEng
{
	public:
		void Setup(int Sim_Id, int NumNodes, int PacketLength, double Bandwidth, int Batch, int Stickiness, int ECA, int fairShare, float channelErrors, float slotDrift,float percentageDCF, int maxAggregation, int howManyACs, int simSeed);
		void Stop();
		void Start();		

	public:
		Channel channel;
//		SatNode [] stas;
		STA [] stas;
		BatchPoissonSource [] sources;

	private:
		int SimId;
		int Nodes;
		double Bandwidth_;
		int PacketLength_;
		double percentageEDCA_;
		int Batch_;
		float drift;
		double intCut, decimalCut, cut; 
		

};

void SlottedCSMA :: Setup(int Sim_Id, int NumNodes, int PacketLength, double Bandwidth, int Batch, int Stickiness, int ECA, int fairShare, float channelErrors, float slotDrift, float percentageEDCA, int maxAggregation, int howManyACs, int simSeed)
{
	SimId = Sim_Id;
	Nodes = NumNodes;
	drift = slotDrift;

	stas.SetSize(NumNodes);
	sources.SetSize(NumNodes);

	// Channel	
	channel.Nodes = NumNodes;
	channel.out_slot.SetSize(NumNodes);
	channel.error = channelErrors;

	// Sat Nodes
	//Determining the cut value for assigning different protocols
	if(percentageEDCA == 0 || percentageEDCA == 1)
	{
		intCut = percentageEDCA;
	}else
	{
		cut = NumNodes * percentageEDCA;
		decimalCut = modf(cut, &intCut);
		
		if(decimalCut > 0.5)
		{
			intCut++;	
		}
	}
	
	for(int n=0;n<NumNodes;n++)
	{
		// Node
		stas[n].node_id = n;
		stas[n].K = 1000;
		stas[n].system_stickiness = Stickiness;
		stas[n].ECA = ECA;
		stas[n].fairShare = fairShare;
		//stas[n].driftProbability = slotDrift;
		stas[n].cut = intCut;     
		stas[n].maxAggregation = maxAggregation;
		stas[n].L = PacketLength;


		// Traffic Source
		sources[n].L = PacketLength;
		sources[n].categories = howManyACs;
		sources[n].packetsGenerated = 0;
		for(int i = 0; i < 4; i++) sources[n].packetsInAC.at(i) = 0;		

		// The percentage of generated packets destined to a specific AC
		// This distribution of the load produces predictable plots
		sources[n].BKShare = 100; // 40%
		sources[n].BEShare = 60;  // 30%
		sources[n].VIShare = 30;  // 15%
		sources[n].VOShare = 15;  // 15%
		sources[n].MaxBatch = Batch;
	}

	for(int i = 0; i < NumNodes; i++)
	{
		sources[i].packet_rate = Bandwidth/(PacketLength * 8);
		stas[i].saturated = 1;
		if(Bandwidth < 10e6) stas[i].saturated = 0;
		
	}
	// }
	//*DEBUG
	// for(int i = 0; i < NumNodes; i++)
	// {
	// 	cout << "Node-" << i << " packet rate: " << sources[i].packet_rate << endl;
	// }
	//End traffic source
	
	// Connections
	for(int n=0;n<NumNodes;n++)
	{
        connect channel.out_slot[n],stas[n].in_slot;
		connect stas[n].out_packet,channel.in_packet;
		connect sources[n].out,stas[n].in_packet;
	}


	Bandwidth_ = Bandwidth;
	PacketLength_ = PacketLength;
	Batch_ = Batch;
	percentageEDCA_ = percentageEDCA;
		

};

void SlottedCSMA :: Start()
{
	cout << ("--------------- Starting ---------------") << endl;
};

void SlottedCSMA :: Stop()
{	
	//--------------------------------------------------------------//
	//-------------------Writing the results------------------------//
	//--------------------------------------------------------------//

	//CLI output variables
	
};

int main(int argc, char *argv[])
{


		
	SlottedCSMA test;
	
	test.Seed = simSeed;
		
	test.StopTime(SimTime);

	test.Setup(MaxSimIter,NumNodes,PacketLength,Bandwidth,Batch,Stickiness, ECA, fairShare, channelErrors, slotDrift, percentageEDCA, maxAggregation, howManyACs, simSeed);
	
	test.Run();


	return(0);
};
