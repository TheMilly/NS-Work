#ifndef BPACKET_H_
#define BPACKET_H_

#include "packet.h"

struct hdr_myHeader {
	// your data fields
	int myData;
	int myArray_[100];
	//int &getMyData() { return myData; }

	// necessary for access
	static int offset_;
	inline static int& offset() { return offset_; }
	inline static hdr_myHeader* access(const Packet* p) {
		return (hdr_myHeader*) p->access(offset_);
	}
};


#endif /*CLUSTERINGPACKET_H_*/
