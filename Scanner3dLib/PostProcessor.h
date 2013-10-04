#ifndef POST_PROCESSOR
#define POST_PROCESSOR

#include "ListItem.h"
class PostProcessor
{
public:
	/*
	Merge allocates a new list as well as allocating new points 
	in the list
	*/
	void Merge(List *outlist);
	/* 
	the composite function gets all the points from the 
	scanner frames, the only new thing allocated is the List itself, no new points
	*/
	void Composite(List *outlist);
	void SaveData(char * filename, List *lstpnts);
	PostProcessor(void);
	~PostProcessor(void);

};

#endif
