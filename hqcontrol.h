#ifndef HQCONTROL_H_
#define HQCONTROL_H_

#include "VUserComm.h"

class HQControl
{
public:
	static BOOL	JudgeAdvHQRights();
	static BOOL IsValidProgram1();
	static BOOL IsValidProgram2();

	static BOOL TestUnStableNetWorkStatus();
public:
	static long UnStable_LastTime;
	static long UnStable_Cnt;
};

#endif

