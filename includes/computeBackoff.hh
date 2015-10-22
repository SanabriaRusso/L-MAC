#define CYCLE 8 

using namespace std;

void computeBackoff(int backoffStage, const int MAXSTAGE, double &counter, 
	int backlog, int ack, std::bitset<CYCLE> slots,	double gamma){

	int CWmin = CYCLE;
	int txDebug = 0;

	if(backlog != 3)
	{
		if(ack == 1){
			counter = CWmin -1;
			if(txDebug == 1) cout << "Success: " << counter << endl;
			return;
		}
		
		if(gamma < 1){
			int coin = rand() % (int)(100 - 1) + 1;
			if(txDebug == 1) cout << "Coin: " << coin << endl;
			if(coin < (int) (gamma * 100)){
				counter = CWmin -1;
				if(txDebug == 1) cout << "Reusing the slot after collision " << coin << endl;
				return;
			}else{
				if(backoffStage == 0){
					do{
					// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
					counter = rand() % (int)  (CWmin -1) + 1;		
					}while( (counter == 0) && (slots.test(counter)) );
					if(txDebug == 1) cout << "Not reusing, random " << counter << endl;
				}else{
					do{
					// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
					counter = rand() % (int)  (pow(2,backoffStage) * CWmin -1) + 1;		
					}while(counter == 0);
					if(txDebug == 1) cout << "Not reusing, random " << counter << endl;				
				}
			}
		}else{
			if(backoffStage == 0){
				do{
					// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
					counter = rand() % (int)  (CWmin -1) + 1;		
				}while( (counter == 0) && (slots.test(counter)) );
				if(txDebug == 1) cout << "Not reusing, random " << counter << endl;
			}else{
				do{
					// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
					counter = rand() % (int)  (pow(2,backoffStage) * CWmin -1) + 1;		
				}while(counter == 0);
				if(txDebug == 1) cout << "Not reusing, random " << counter << endl;
			}
		}


		// if(coin < (int) (gamma * 100)){
		// 	counter = CWmin -1;
		// 	if(txDebug == 1) cout << "Reusing the slot after collision " << coin << endl;
		// 	return;
		// }else{
		// 	if(backoffStage == 0){
		// 		do{
		// 			// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
		// 			counter = rand() % (int)  (CWmin -1) + 1;		
		// 		}while( (counter == 0) && (slots.test(counter)) );
		// 		if(txDebug == 1) cout << "Not reusing, random " << counter << endl;
		// 	}else{
		// 		do{
		// 			// if(counter != 0) cout << "Reusing counter: " << counter << " becasue (" << slots.test(counter) << ")" << endl;
		// 			counter = rand() % (int)  (pow(2,backoffStage) * CWmin -1) + 1;		
		// 		}while(counter == 0);
		// 		if(txDebug == 1) cout << "Not reusing, random " << counter << endl;				
		// 	}
		// }		
	}

}
