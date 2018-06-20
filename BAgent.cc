
#include "BAgent.h"
#include "BPacket.h"
#include "ip.h"
#include <cstdlib>
#include <string>
#include <iostream>

static class BAgentClass: public TclClass {
public:
	BAgentClass() : TclClass("Agent/BAgent") {}
	TclObject* create(int, const char*const*) {
		return (new BAgent());
	}
} class_BAgent;


BAgent::BAgent() : Agent(PT_MYHEADER) ,
					myTimer(*this) {
	bind("myVal_",&myVal_);
}

void BAgent::recv(Packet* p, Handler*) {
	hdr_ip *ip = hdr_ip::access(p);
	hdr_myHeader* my=  hdr_myHeader::access(p);

	string str = "/home/parul/Desktop/ns-allinone-2.35/ns-2.35/Simple/Results/averages_";
	FILE * traceFile =fopen (str.c_str(),"a+");

	//This print is to show the sending and receiving nodes
	//So we know which values were sent through within the arrays
	printf("Agent %d is receiving packet from Agent %d\n", addr(), ip->saddr() );
	
        char charStr[32]; // enough to hold all numbers up to 64-bits
	sprintf(charStr, "receivingNodeID: %d  averagedArray: ", addr());	
	fprintf(traceFile,charStr);	
	
	//As a note: Each agent's data is the [(agent #) + 1]
	//For Example: agent(0) has an array of 5 elements all of value 1
	//agent(1) has an array of 5 elements all of value 2
	//The averaging rounds down due to integer values,
	//Example: (1 + 2)/2 => 1.5 => 1
	
	for(int a = 0; a < 100; a++){

	//Each element of each receiving node's array is displayed 
	//before the averaging, and after the averaging
	cout << "myState_ before avg: " << myState_[a] << "\n";

	myState_[a] = (myState_[a] + my->myArray_[a])/2;

	
        char charString[32]; // enough to hold all numbers up to 64-bits
	sprintf(charString, "%d ", myState_[a]);
	fprintf(traceFile,charString);
	
	
	cout << "myState_ after avg: " << myState_[a] << "\n";
	}
	
	
	char enterStr[32]; // enough to hold all numbers up to 64-bits
	sprintf(enterStr, "\n");
	fprintf(traceFile,enterStr);
	

	//printf("average myVal: %d\n", myVal_);
	//printf("Agent %d receives packet with value %d from Agent %d\n",
			//addr(),my->myData, ip->saddr() );
	fclose(traceFile);
	Packet::free(p);
}


int BAgent::command(int argc, const char*const* argv) {
	if (argc == 5) {
        	if(strcmp(argv[1], "setArray_i") == 0) {
            		int index = atoi(argv[2]);     
            		id_ = atoi(argv[3]);     
            		myState_[index] = atof(argv[4]);
 
  	          	//Write the value to a trace file
        	    	//string str = "/home/parul/Desktop/results_ns2/init_";	
			
			string str = "/home/parul/Desktop/ns-allinone-2.35/ns-2.35/Simple/Results/init_";
			char nodeID[32];
			sprintf(nodeID, "%d", id_);
			str += nodeID;
			

        	    	//string result;
        	    	char numstr[32]; // enough to hold all numbers up to 64-bits
			sprintf(numstr, "index: %d  myState: %d \n", index, myState_[index]);
			cout << myState_[index] << "\n";

        	    	//sprintf(numstr, "%d", powerNum_);
        	    	//result = str + "piter_" + numstr;
        	    	//sprintf(numstr, "%d", roundNum_);
        	    	//result = result + "_riter_" + numstr;             
        	    	//sprintf(numstr, "%d", myNNval);
        	    	//result = result + "_NN_" + numstr;
        	    	//sprintf(numstr, "%d", myStateSize);
        	    	//result = result + "_SigD_" + numstr + ".tr";
        	    	FILE * traceFile =fopen (str.c_str(),"a+");
 	
        	    	// index is 0 means start of new value, so we write the node id
        	    	//if (index == 0){
        	        	//printf("file name %s\n", result.c_str());     
        	        	//fprintf (traceFile,"%d ",id_);
        	    	//string sampleVal = to_string(myState_[index]);
        	    	fprintf(traceFile,numstr);
        	    	fclose(traceFile);
 			
        	    	return (TCL_OK);
        	}
    	}	

	if (argc == 3) {
		if (strcmp(argv[1], "sendTo") == 0) {
			Packet* p = allocpkt();
			hdr_ip *ip = hdr_ip::access(p);
			hdr_cmn* cmn = hdr_cmn::access(p);
			hdr_myHeader* my=  hdr_myHeader::access(p);

			for (int k = 0; k < 100; k++){
			my->myArray_[k] = myState_[k];
			//cout << "sending node state: " << myState_[0] << "\n";
			}

			//my->myData=1234;
			cmn->size()	= 4;
			cmn->ptype() = PT_MYHEADER;
			ip->daddr()= atoi(argv[2]);
			ip->dport()= here_.port_;
			
			send(p, 0);
			return (TCL_OK);
		}
	}

	if (argc == 2) {
		if (strcmp(argv[1], "ping") == 0) {
			printf("pong");
			return (TCL_OK);
		}
	}
	return (Agent::command(argc, argv));
}

void BTimeout() {

}
