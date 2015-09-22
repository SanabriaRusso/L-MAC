#define BETA 95e-2

using namespace std;

void computeBackoff(int backoffStage, const int MAXSTAGE, double &counter, int backlog, int ack){
	int CWmin = 16;

	if(backlog != -1)
	{
		if(ack == 1){
			counter = CWmin -1;
			return;
		}

		srand(time(NULL)); // Seed the time
		int coin = rand() % (int)(100 - 1) + 1;

		if(coin < (int) ((BETA/CWmin) * 100)){
			counter = CWmin -1;
			return;
		}else{
			//A random backoff that is not the cycle length
			counter = rand() % (int) (CWmin -2);
		}		
	}

}