#include "PAgent.h"
#include "PPacket.h"
#include "ip.h"
#include <cstdlib>
#include </home/parul/Desktop/ns-allinone-2.35/ns-2.35/Eigen/Eigen/Dense>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace Eigen;
using namespace std;

static class PAgentClass: public TclClass {
public:
	PAgentClass() : TclClass("Agent/PAgent") {}
	TclObject* create(int, const char*const*) {
		return (new PAgent());
	}
} class_PAgent;


PAgent::PAgent() : Agent(PT_MYHEADER) ,
					myTimer(*this) {
	bind("waitInterval_", &waitInterval_);
}

void PAgent::recv(Packet* p, Handler*) {
	hdr_myHeader* my=  hdr_myHeader::access(p);

	//printf("%d, %d\n", my->consensusIter_, my->powerIter_);
	//printf("SS: %d\n", myStateSize);

	//Write received data values to a file
	consensusIterRx_ = consensusIterRx_+1;
	string str_rxd = "/home/parul/Desktop/ns-allinone-2.35/ns-2.35/Simple/output/Rxvd_";
	string result_rxd;
	char numstr_rxd[256]; // enough to hold all numbers up to 64-bits
	sprintf(numstr_rxd, "%d", CI_);
	result_rxd = str_rxd + "CI_" + numstr_rxd;
	sprintf(numstr_rxd, "%d", my->powerIter_);
	result_rxd = result_rxd + "_piter_" + numstr_rxd;
	sprintf(numstr_rxd, "%d", myNNval);
	result_rxd = result_rxd + "_NN_" + numstr_rxd;
	sprintf(numstr_rxd, "%d", myStateSize);
	result_rxd = result_rxd + "_SigD_" + numstr_rxd + ".tr";

	FILE * rxdFile = fopen (result_rxd.c_str(),"a+");
	fprintf(rxdFile,"%d ", my->sentFrom_);	
	fprintf(rxdFile,"%d ",id_);
	fprintf(rxdFile,"%d ",consensusIter_);
	fprintf(rxdFile,"%d ",consensusIterRx_);
	
	printf("TxNode: %d, RxNode: %d\n", my->sentFrom_, id_);
	printf("Curr %f, Rxvd %f\n", v(2), my->myArray[2]); 
	for (int i = 0; i < myStateSize; i++) {
		v(i) = 0.5*(my->myArray[i]+v(i));
		fprintf(rxdFile,"%.4f ",v(i));	
	}
	//printf("Final %f\n", v(2)); 

	fprintf(rxdFile,"\n");
	fclose(rxdFile);

	Packet::free(p);
}


void PAgent::sendData2() {
	Packet* p = allocpkt();
	hdr_ip *ip = hdr_ip::access(p);
	hdr_cmn* cmn=  hdr_cmn::access(p);
	hdr_myHeader* my=  hdr_myHeader::access(p);

	
	for (int i = 0; i < myStateSize; i++) {
		my->myArray[i] = v(i);
	}

	my->consensusIter_ = consensusIter_;
	consensusIter_ = consensusIter_+1;
	my->powerIter_ = powerIter_;
	//printf("Tx-1 Node: %d\n", id_);
	my->sentFrom_ = id_;
	cmn->size() = 4096;
	cmn->ptype() = PT_MYHEADER;
	ip->daddr() = -1;
	ip->dport() = here_.port_;
	send(p, 0);	

	if (consensusIter_ < 10) {
		//printf("Sent by%d\n", id_);
		//waitInterval_ = (waitInterval_+1)*(consensusIter_+10);
		waitInterval_ = rand() % 10 +rand();
		printf("Consensus Iter is %d, Wait time is %d\n", consensusIter_, waitInterval_);
		myTimer.resched(10);
	}
}


int PAgent::command(int argc, const char*const* argv) {
	if (argc == 3) {
		if(strcmp(argv[1], "InitilizeNodes") ==0) {
			id_ = atoi(argv[2]);

			//Write initial data values to a file
			string str_Init = "/home/parul/Desktop/ns-allinone-2.35/ns-2.35/Simple/output/Init_";
			string result_Init;
			char numstr_Init[256]; // enough to hold all numbers up to 64-bits
			sprintf(numstr_Init, "%d", CI_);
			result_Init = str_Init + "CI_" + numstr_Init;
			sprintf(numstr_Init, "%d", powerIter_);
			result_Init = result_Init + "_piter_" + numstr_Init;
			sprintf(numstr_Init, "%d", consensusIter_);
			result_Init = result_Init + "_citer_" + numstr_Init;			
			sprintf(numstr_Init, "%d", myNNval);
			result_Init = result_Init + "_NN_" + numstr_Init;
			sprintf(numstr_Init, "%d", myStateSize);
			result_Init = result_Init + "_SigD_" + numstr_Init + ".tr";
			
			FILE * traceFile = fopen (result_Init.c_str(),"at");
			fprintf (traceFile,"%d ",id_);
			std::srand((unsigned int) (time(0) + id_ + CI_));
			v = MatrixXf::Random(myStateSize,1);
			for (int i = 0; i < myStateSize; i++) {
				v(i) = v(i)+1;	
				fprintf(traceFile,"%.4f ", v(i));
			}

			fprintf(traceFile,"\n");
			fclose(traceFile);

			return (TCL_OK);
		}
	}


	if (argc == 3) {
		if (strcmp(argv[1], "setTimer") == 0) {
			int Ttime = atoi(argv[2]);	
			myTimer.resched(Ttime);
			return (TCL_OK);
		}
	}


	if (argc == 3) {
		if (strcmp(argv[1], "sendData") == 0) {
			int Ttime = atoi(argv[2]);	
			sendData2();			
			return (TCL_OK);
		}
	}


	if (argc == 3) {
		if (strcmp(argv[1], "cancelTimer") == 0) {
			int Ttime = atoi(argv[2]);	
			myTimer.cancel();
			return (TCL_OK);
		}
	}

	if (argc == 2) {
		if (strcmp(argv[1], "normalize") == 0) {
			float z = v.norm();
			for (int i = 0; i<3; i++) {
				v(i) = v(i)/z;
			}
			return (TCL_OK);
		}
	}


	//Sets Attributes of Agent
	// Number of State (Signal Dimension)
	if(strcmp(argv[1], "setDataLen") ==0) {
		myStateSize = atoi(argv[2]);
		return (TCL_OK);
	}

	// Total Number of Nodes 
	if(strcmp(argv[1], "setNNval") ==0) {
		printf("I am in setNNval function");
		myNNval = atoi(argv[2]);
		return (TCL_OK);
	}

	// Power Iteration number for trace analyis
	if(strcmp(argv[1], "setPowerIter") ==0) {
		powerIter_ = atoi(argv[2]);
		return (TCL_OK);
	}

	// Consensus Iteration number for trace analyis
	if(strcmp(argv[1], "setConsensusIter") ==0) {
		consensusIter_ = atoi(argv[2]);
		return (TCL_OK);
	}

	if(strcmp(argv[1], "setCI") ==0) {
		CI_ = atoi(argv[2]);
		return (TCL_OK);
	}


	return (Agent::command(argc, argv));
}


void myTimeout() {
	printf("hello_2");
}
