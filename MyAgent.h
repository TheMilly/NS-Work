#ifndef MYAGENT_H_
#define MYAGENT_H_
#include "agent.h"
#include "timer-handler.h"
#include "stdlib.h"
#include "MyPacket.h"
#include "ip.h"
#include </home/parul/Desktop/ns-allinone-2.35/ns-2.35/Eigen/Eigen/Dense>

using namespace Eigen;


class MyAgent : public Agent {
public:
	MyAgent();
	int command(int argc, const char*const* argv);
	int id_;
	int consensusIter_ = 0;
	int consensusIterRx_ = 0;
	int powerIter_ = 0;
	int myStateSize=3;
	int myNNval=5; // why??
	int waitInterval_;
	int CI_;
	// The size must be hard coded and match (>=) TCL Nodes
	//double myState_[100];
	//std::srand();
	MatrixXd t = MatrixXd::Constant(3,1,10);
	MatrixXf v = MatrixXf::Random(80,1);
	void recv(Packet* p, Handler*);
	void sendData2();

	void myTimeout();

	class MyTimer : public TimerHandler {
	public:
		MyTimer(MyAgent &a) : agent(a) {}
		void expire(Event *e) { 
			agent.sendData2();
		};
		MyAgent &agent;
	} myTimer;

};

#endif /*CLUSTERINGAGENT_H_*/
