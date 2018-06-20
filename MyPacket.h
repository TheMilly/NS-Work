#ifndef MYPACKET_H_
#define MYPACKET_H_

#include "packet.h"
#include </home/parul/Desktop/ns-allinone-2.35/ns-2.35/Eigen/Eigen/Dense>

using namespace Eigen;

struct hdr_myHeader {
	// your data fields
	//The size must be hard coded and match (>=) NewMyAgent 
	float myArray[100]; 
	//int myArraySize;
	int powerIter_;
	int consensusIter_;
	//int NNval_;
	int sentFrom_;
	//int &getMyData() { return myData; } NOTUSED


	// necessary for access
	static int offset_;
	inline static int& offset() { return offset_; }
	inline static hdr_myHeader* access(const Packet* p) {
		return (hdr_myHeader*) p->access(offset_);
	}
};


#endif /*CLUSTERINGPACKET_H_*/
