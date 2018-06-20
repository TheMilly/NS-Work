#ifndef PAGENT_H_
#define PAGENT_H_
#include "agent.h"
#include "timer-handler.h"
#include "stdlib.h"
#include "PPacket.h"
#include "ip.h"
#include </home/parul/Desktop/ns-allinone-2.35/ns-2.35/Eigen/Eigen/Dense>

using namespace Eigen;


class PAgent : public Agent {
public:
	PAgent();
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
		MyTimer(PAgent &a) : agent(a) {}
		void expire(Event *e) { 
			agent.sendData2();
		};
		PAgent &agent;
	} myTimer;

};

#endif /*CLUSTERINGAGENT_H_*/
