#ifndef _CPOINTS_H_
#define _CPOINTS_H_

#define POINTS_MAX		10000		// 100% 

struct CPoint
{
	int lP;		// Raw value
	int dwLog;	// Scaling value % (10000 = 100%)
	CPoint(int lP, int dwLog) : lP(lP), dwLog(dwLog) {};
};


#endif
