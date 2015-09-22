#define BETA 95e-2

using namespace std;

void computeBackoff(int &backoffStage, const int MAXSTAGE, double &counter, int backlog, int ack){
	int CWmin = 16;
	if(backlog == 1)
	{
		counter = rand() % (int) (pow(2,min(backoffStage,MAXSTAGE)) * CWmin -1);
	}else
	{
		backoffStage = 0;
		counter = 0;
	}
}