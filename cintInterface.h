// Header file for CCintIF class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CINTIF_H
#define CINTIF_H

#include "geometry.h"

class CFM
{
public:
	C3DShell *GetFastHenryModel();
	void RotateModelFar(float angle, float x, float y, float z);
	void RotateModelNear(float angle, float x, float y, float z);
	void Wait(unsigned long milliseconds);

protected:

};

#endif