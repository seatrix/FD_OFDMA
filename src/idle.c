#include <stdlib.h>
#include "idle.h"
#include "frameGen.h"

extern double gElapsedTime;
extern std11 gStd;
extern simSpec gSpec;

void idel(staInfo sta[], apInfo *ap, int *numTx, bool *fEmpty){
	int i;
	int minBackoff = gStd.cwMax;
	double t;
	int nodeID;
	bool isSta;

	if(*fEmpty==false){
		for(i=0; i<gSpec.numSTA; i++){
			if((minBackoff > sta[i].backoffCount)&&(sta[i].buffer[0].lengthMsdu!=0)){
				minBackoff = sta[i].backoffCount;
			}
		}
		if((minBackoff > ap->backoffCount)&&(ap->buffer[0].lengthMsdu!=0)){
			minBackoff = ap->backoffCount;
		}

		for(i=0; i<gSpec.numSTA; i++){
			if(sta[i].buffer[0].lengthMsdu!=0){
				sta[i].backoffCount -= minBackoff;
				if(sta[i].backoffCount==0){
					(*numTx)++;
					sta[i].fTx = true;
				}
			}
		}
		if(ap->buffer[0].lengthMsdu!=0){
			ap->backoffCount -= minBackoff;
			if(ap->backoffCount==0){
				(*numTx)++;
				ap->fTx = true;
			}
		}

		gElapsedTime += (double)(gStd.slot * minBackoff);
	}else{
		nodeID = rand() % (gSpec.numSTA + 1);
		if(nodeID==gSpec.numSTA){
			//Arrive frame to AP.
			isSta = false;
			t = poisson(isSta);
			ap->buffer[0].lengthMsdu = traffic(isSta);
			ap->buffer[0].timeStamp = gElapsedTime + t;
			ap->sumFrameLengthInBuffer += ap->buffer[0].lengthMsdu;
		}else{
			//Arrive frame to sta[nodeID].
			isSta = true;
			t = poisson(isSta);
			sta[nodeID].buffer[0].lengthMsdu = traffic(isSta);
			sta[nodeID].buffer[0].timeStamp = gElapsedTime + t;
			ap->sumFrameLengthInBuffer += sta[nodeID].buffer[0].lengthMsdu;
		}
		gElapsedTime += t;
	}

}
