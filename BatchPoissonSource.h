/*
	Poisson source. 
*/
			
#include "Aux.h"

#define MAXSEQ 1024

component BatchPoissonSource : public TypeII
{
	public:
		// Connections
		outport void out(Packet &packet);	

		// Timer
		Timer <trigger_t> inter_packet_timer;
		inport inline void new_packet(trigger_t& t);

		BatchPoissonSource () { 
			connect inter_packet_timer.to_component,new_packet;
		}

	public:
		int L;
		long int seq;
		int MaxBatch;	
		double packet_rate;
		double packetsGenerated;
	
	public:
		void Setup();
		void Start();
		void Stop();
			
};

void BatchPoissonSource :: Setup()
{

};

void BatchPoissonSource :: Start()
{
	inter_packet_timer.Set(Exponential(1/packet_rate));
	seq = 0;
};
	
void BatchPoissonSource :: Stop()
{
	//DEBUG
	// cout << "---- Source ----" << endl;
	// cout << "Generated " << packetsGenerated << " packets" << endl;

};

void BatchPoissonSource :: new_packet(trigger_t &)
{
	Packet packet;
	int RB = (int) Random(MaxBatch)+1;
	packet.L = L;
	packet.aggregation = 1;
	packet.queuing_time = SimTime();
	out(packet);
	packetsGenerated++;
	inter_packet_timer.Set(SimTime()+Exponential(RB/packet_rate));	
};

