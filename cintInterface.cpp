// This file contains the functions required to link CINT with FastModel

// include precompiled header only if not using makecint
//#ifndef __MAKECINT__ 
#include "stdafx.h"
//#endif

#include "cintInterface.h"

#ifdef __MAKECINT__ 

// needed prototypes for makecint
class CWinApp;
class CFastModelApp;
class CFastModelDoc;
CWinApp* AfxGetApp();

#else // if __MAKECINT__ not defined, can include regular headers 

#include "fastmodel.h"
#include "fastmodelDoc.h"
#include "fastmodelView.h"

#endif // #ifdef __MAKECINT__  

C3DShell *CFM::GetFastHenryModel()
{
	CFastModelApp *theApp;
	CFastModelDoc *currentDoc;

	theApp = (CFastModelApp*)AfxGetApp();

	currentDoc = theApp->GetFastModelDoc();

	return currentDoc->m_pFastHenryModel;
}

// TBC Warning: as this function is called by the interpreted thread,
// if the view list changes (e.g. is closed), GetNextView may fail or
// randomly access memory
void CFM::RotateModelFar(float angle, float x, float y, float z)
{
	CFastModelApp *theApp;
	CFastModelDoc *currentDoc;
	POSITION pos;
	CView *pView;

	theApp = (CFastModelApp*)AfxGetApp();

	currentDoc = theApp->GetFastModelDoc();

	pos = currentDoc->GetFirstViewPosition();

	// TBC warning: we are rotating all the FastModel views!!
	while (pos != NULL)
	{
		pView = currentDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CFastModelView)))
			((CFastModelView *)pView)->RotateFar(angle, x, y, z);
	}   
}

// TBC Warning: as this function is called by the interpreted thread,
// if the view list changes (e.g. is closed), GetNextView may fail or
// randomly access memory
void CFM::RotateModelNear(float angle, float x, float y, float z)
{
	CFastModelApp *theApp;
	CFastModelDoc *currentDoc;
	POSITION pos;
	CView *pView;

	theApp = (CFastModelApp*)AfxGetApp();

	currentDoc = theApp->GetFastModelDoc();

	pos = currentDoc->GetFirstViewPosition();

	// TBC warning: we are rotating all the FastModel views!!
	while (pos != NULL)
	{
		pView = currentDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CFastModelView)))
			((CFastModelView *)pView)->RotateNear(angle, x, y, z);
	}   
}

void CFM::Wait(unsigned long milliseconds)
{
	HANDLE dummyevent;

	// standard security, manual reset, initially not signaled, no name
	dummyevent = CreateEvent( NULL, TRUE, FALSE, NULL);

	// wait on an event which will never become signaled -> the time-out
	// is used as timer
	WaitForSingleObject(dummyevent, milliseconds);

	CloseHandle(dummyevent);
}

