/*
	Channel Component
*/

#define DATARATE 11E6 // Data Transmission Rate
#define PHYRATE 1E6
//Complying with 802.11n at 2.4 GHz
#define SLOT 9e-06 
#define DIFS 28e-06
#define SIFS 10e-06
#define LDBPS 256
#define TSYM 4e-06
#define SIG_EXT 6e-06
			
#include "Aux.h"

component Channel : public TypeII
{

	public:
		void Setup();
		void Start();
		void Stop();
			
	public:
		int Nodes;
		float error;

		// Connections
		outport [] void out_slot(SLOT_notification &slot);	
		inport void in_packet(Packet &packet);

		// Timers
		Timer <trigger_t> slot_time; // Duration of current slot
		Timer <trigger_t> rx_time; // Time to receive all packets transmitted in current slot
		
		inport inline void NewSlot(trigger_t& t1);
		inport inline void EndReceptionTime(trigger_t& t2);

		Channel () { 
			connect slot_time.to_component,NewSlot; 
			connect rx_time.to_component,EndReceptionTime;
		}

	private:
		double number_of_transmissions_in_current_slot;
		double succ_tx_duration, collision_duration; // Depends on the packet(s) size(s)
		double empty_slot_duration;
		double TBack;
		double L_max;
		int MAC_H, PLCP_PREAMBLE, PLCP_HEADER;
		int aggregation;
		float errorProbability;
		int rate48;	//	are we using the 48mbps metrics for tx duration?


	public: // Statistics
		double collision_slots, empty_slots, succesful_slots, total_slots;
		signed long long slotNum;
};

void Channel :: Setup()
{

};

void Channel :: Start()
{
	number_of_transmissions_in_current_slot = 0;
	succ_tx_duration = 0;
	collision_duration = 0;
	empty_slot_duration = 9e-06;

	collision_slots = 0;
	empty_slots = 0;
	succesful_slots = 0;
	total_slots = 0;
	slotNum = 0;
	L_max = 0;
	
	MAC_H = 272;
	PLCP_PREAMBLE = 144; 
	PLCP_HEADER = 48;
	
	TBack = 32e-06 + ceil((16 + 256 + 6)/LDBPS) * TSYM;

	aggregation = 1;
	errorProbability = 0;

	rate48 = 1;

	slot_time.Set(SimTime()); // Let's go!	
};

void Channel :: Stop()
{
	cout << "---- Channel ----" << endl;
	if(empty_slots+collision_slots+succesful_slots == total_slots){
		cout << "(%)Empty slots: " << empty_slots/total_slots << " Sx slots: " << succesful_slots/total_slots << " Col. slots: " << collision_slots/total_slots << endl;
	}else{
		cout << "The slot count does not match. Discard these results" << endl;
	}
	cout << "-----------------" << endl;


};

void Channel :: NewSlot(trigger_t &)
{
	//printf("%f ***** NewSlot ****\n",SimTime());

	SLOT_notification slot;
	slotNum++;

	slot.status = number_of_transmissions_in_current_slot;
	slot.num = slotNum;

	number_of_transmissions_in_current_slot = 0;
	L_max = 0;

	for(int n = 0; n < Nodes; n++) out_slot[n](slot); // We send the SLOT notification to all connected nodes
	rx_time.Set(SimTime());	// To guarantee that the system works correctly. :)
}

void Channel :: EndReceptionTime(trigger_t &)
{
	if(number_of_transmissions_in_current_slot==0) 
	{
		slot_time.Set(SimTime()+SLOT);
		empty_slots++;
	}
	if(number_of_transmissions_in_current_slot == 1)
	{
		slot_time.Set(SimTime()+succ_tx_duration);		
		succesful_slots++;
	}
	if(number_of_transmissions_in_current_slot > 1)
	{
		collision_slots++;	
		slot_time.Set(SimTime()+collision_duration);
	}
	total_slots++; //Just to control that total = empty + successful + collisions
}


void Channel :: in_packet(Packet &packet)
{
	number_of_transmissions_in_current_slot++;
	if(packet.L > L_max) L_max = packet.L;
	succ_tx_duration = (SIFS + 32e-06 + ceil((16 + aggregation*(32+(L_max*8)+288) + 6)/LDBPS)*TSYM + SIFS + TBack + DIFS + empty_slot_duration);
	if(rate48 == 1)
	{
		//JUST FOR SINGLE FRAME TRANSMISSIONS.
		//IT CURRENTLY DOES NOT SUPPORT AGGREGATION
		//Calculating the duration of a transmission in 48Mbps according to durations-wifi-ofdm.xlsx
		// (bits for ack are: service, MAC, FCS, tail)
		// ACK = PLCP + ceil((bits)/NDBPS)* SymbolTime + pause
		double ACK = 20.0 + ceil(134.0/192.0) * 4.0 + 6.0;
		// (bits are: service, MAC Header, LLC Header, IP Header, UDP Header, PAYLOAD, FCS, Tail).
		// T = PLCP + ceil((bits)/NDBPS)* SymbolTime + pause + SIFS + ACK + DIFS + CWaverage*SLOT;
		double frame = 20.0 + ceil((L_max*8.0 + 534.0)/192.0) * 4.0 + 6.0;
		//Last 9 is an empty slot and 7.5 is CWmin/2
		succ_tx_duration = frame * 1e-06 + SIFS + ACK * 1e-06 + DIFS + 9.0 * 7.5 * 1e-06;
	}
	collision_duration = succ_tx_duration;
}

