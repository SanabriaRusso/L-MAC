#define BETA 95e-2

using namespace std;

void computeBackoff(int backoffStage, const int MAXSTAGE, double &counter, int backlog, int ack){
	int CWmin = 16;
	int txDebug = 0;

	if(backlog != 3)
	{
		if(ack == 1){
			counter = CWmin -1;
			return;
		}

		int coin = rand() % (int)(100 - 1) + 1;
		if(txDebug == 1) cout << "Coin: " << coin << endl;
		if(coin < (int) ((BETA/CWmin) * 100)){
			counter = CWmin -1;
			if(txDebug == 1) cout << "Reusing the slot after collision" << coin << endl;
			return;
		}else{
			//A random backoff that is not the cycle length or zero. That why -3,
			//so we keep uniformity in the random selection
			do{
				counter = rand() % (int)  (pow(2,backoffStage) * CWmin -3);			
			}while(counter > 0 && counter != CWmin-1);
			if(txDebug == 1) cout << "Not reusing, random " << counter << endl;
		}		
	}

}