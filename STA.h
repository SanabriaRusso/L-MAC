/*This is an implementation of L-MAC protocol.
In principle: 
    1) from bootup stations keep a backoff counter,
    even if they are not backlogged.
*/
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <string>
#include <array>
#include <map>
#include <bitset>
#include "Aux.h"
#include "FIFO.h"
#include "includes/pickNewPacket.hh"
#include "includes/computeBackoff.hh"
#include "includes/erasePacketsFromQueue.hh"
#include "includes/preparePacketForTransmission.hh"
#include "includes/dumpStationLog.hh"


//Suggested value is MAXSTAGE+1
#define MAXSTAGE 5
#define MAX_RET 6


using namespace std;

component STA : public TypeII
{
    public:
        void Setup();
        void Start();
        void Stop();

    public:

        int nodesInSim; //Number of nodes in the whole simulation

    	//Node-specific characteristics
        int id;
        int K;  //max queue size
        int protocol;   //0 = DCF; 1 = L-MAC; 2= A-L-MAC
        int L;  //Packet length

        //Queue management
        double inPackets;
        double admittedPackets;
        double erased; //Erased from the queue
        double dropped;

        //Statistics
        double accummTimeBetSxTx;
        double throughput;
        double collisions;
        ofstream statistics;

    private:
    	//Contention
        double backoffCounter;
        int backoffStage;
        int ITx;    //I transmitted: 0 = no; 1 = yes

        //Transmissions
        double tx;      //Number of transmission attempts
        double sxTx;    //Successfull transmissions
        double colTx;   //Collisions
        int backlog;    //Is there something in the queue?
        int retAttempt;
        int ack;        //Did we received an ACK for last transmission?

        //Private queue management
        Packet packet;
        FIFO <Packet> MAC;  //The MAC queue


    public:
        //Connections
        inport void inline in_slot(SLOT_notification &slot);
        inport void inline in_packet(Packet &packet);
        outport void out_packet(Packet &packet);
};

void STA :: Setup()
{

};

void STA :: Start()
{
    inPackets = 0;
    admittedPackets = 0;
    erased = 0;
    dropped = 0;
    backoffCounter = 0;
    backoffStage = 0;
    tx = 0;
    sxTx = 0;
    colTx = 0;
    backlog = -1;
    ITx = 0;
    retAttempt = 0;
    ack = 0;

    //Packet details
    packet.L = L;
    packet.source = id;

    //Statistics
    accummTimeBetSxTx = 0;
};

void STA :: Stop()
{

    //Gathering data from last packet before stop
    if(backlog == 1) accummTimeBetSxTx += double(SimTime() - packet.contention_time);

    cout << "-Station-" << id << ":" << endl;
    if(admittedPackets - erased - MAC.QueueSize() == 0){
        cout << "\tTotal throughput: " << sxTx*L*8.0 / SimTime() << endl;
        cout << "\tCollisions: " << colTx/tx << endl;
        cout << "\tReceived: " << inPackets << " packets" << endl;
        cout << "\tDropped: " << dropped << " packets" << endl;
        cout << "\tAvg. seconds between sxTx " << accummTimeBetSxTx / sxTx << endl;

        //Statistics
        throughput = sxTx*L*8.0 / SimTime();
        collisions = colTx/tx;
        accummTimeBetSxTx /= sxTx;

        //Creating the output file
        string logName = "sta-" + to_string(id) + ".log";        
        string routeToFile = "Results/stations/" + logName;
        statistics.open(routeToFile.c_str(), ios::app);
        dumpStationLog(statistics,nodesInSim,throughput,collisions,accummTimeBetSxTx/sxTx);
        statistics.close();

    }else{
        cout << "Something's wrong with either the erasure or admittance of packets" << endl;
    }

};

void STA :: in_slot(SLOT_notification &slot)
{	
    switch(slot.status)
    {
        //Empty slot
        case 0:
            switch(backlog){
                case -1:
                    backlog = 0;
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                    break;
                case 1:
                    if(backoffCounter > 0) backoffCounter--;
                    break;
                default:
                    if(backoffCounter > 0) backoffCounter--;
                    if(MAC.QueueSize() > 0)
                    {
                        backlog = 1;
                        pickNewPacket(packet,MAC,id,SimTime());
                    }
                    //The moment the stations checks if its previous tx slot is empty or not
                    if(ITx = 1){
                        ack = 1;
                        computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                        ack = 0;
                        ITx = 0;
                    }
                    break;
            }
            // if(backlog == 1){
                // if(backoffCounter > 0){
                    // backoffCounter--;
                // }
            // }else if(backlog == 0){
                // if(MAC.QueueSize() > 0)
                // {
                    // backlog = 1;
                    // pickNewPacket(packet,MAC,id,SimTime());
                // }
            // }else if(backlog == -1){    //entering at bootup to start counter
                // backlog = 0;
                // computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
            // }
            break;
        //Successful transmission
        case 1:
            //If this station transmitted in observed slot,
            //we assign a new backoff counter
            if(ITx == 1){
                if(backlog == 1){
                    sxTx++;
                    ack = 1;
                    accummTimeBetSxTx += double(SimTime() - packet.contention_time);
                    erasePacketsFromQueue(MAC,erased);
                    ITx = 0;    //resetting to zero
                    if(protocol != 2) backoffStage = 0;   //resetting backoff stage for DCF and L-MAC
                    backlog = 0;
                    if(MAC.QueueSize() > 0) backlog = 1;
                    if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                    ack = 0;    //ack only used to determine the backoff counter
                }else if(backlog == 0)
                //This is the event of observing someone else's transmission
                //in what it is supposed to be an empty slot.
                //Ack is zero.
                {
                    ITx = 0;    //resetting to zero
                    if(MAC.QueueSize() > 0) backlog = 1;
                    if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                }
            }else
            //If we observe a sxTx slot, we decrement the counter.
            //Only the sta involved in the sxTx will not decrement it.
            //This ensures fairness by preventing the sx sta from tx again
            //drawing a backoff equal to zero.
            {
                backoffCounter--;
            }
            break;
        default:
            if(ITx == 1){
                if(backlog == 1){
                    colTx++;
                    retAttempt++;
                    backoffStage = min(backoffStage + 1,MAXSTAGE);
                    ITx = 0;
                    if(retAttempt == MAX_RET){
                        accummTimeBetSxTx = double(SimTime() - packet.contention_time);
                        erasePacketsFromQueue(MAC,erased);
                        dropped++;
                        backoffStage = 0;   //Resetting due to drop
                        retAttempt = 0;
                        backlog = 0;
                        if(MAC.QueueSize() > 0) backlog = 1;
                        if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                    }
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                }else if(backlog == 0){
                //This is the event of observing someone else's transmission
                //in what it is supposed to be an empty slot.
                //Ack is zero.
                {
                    ITx = 0;    //resetting to zero
                    if(MAC.QueueSize() > 0) backlog = 1;
                    if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                }
            }else{
                if(backoffCounter > 0){
                    backoffCounter--;
                }
            }
            break;
    }

    //Transmission
    if(backoffCounter == 0){
        if(backlog != -1){ 
            ITx = 1;
            if(backlog == 1){
                preparePacketForTransmission(packet,id,SimTime());
                tx++;              
                out_packet(packet);
        }
    }
};

void STA :: in_packet(Packet &packet)
{
    inPackets++;
    if(MAC.QueueSize() < K)   //is there space in the node's queue?
    {
        admittedPackets++;
        packet.queuing_time = SimTime();
        MAC.PutPacket(packet);
    }

}

