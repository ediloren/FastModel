/***************************************************************************
*                                                                          *
*   Copyright (c) 2017                                                     *
*   FastFieldSolvers S.R.L.  http://www.fastfieldsolvers.com               *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU Lesser General Public License (LGPL)     *
*   as published by the Free Software Foundation; either version 2 of      *
*   the License, or (at your option) any later version.                    *
*   for detail see the LICENCE text file.                                  *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU Library General Public License for more details.                   *
*                                                                          *
*   You should have received a copy of the GNU Library General Public      *
*   License along with this program; if not, write to the Free Software    *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307   *
*   USA                                                                    *
*                                                                          *
***************************************************************************/


// FileChangeEvent.cpp: implementation of the CFileChangeEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileChangeEvent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CFileInfo class
//////////////////////////////////////////////////////////////////////

CFileInfo::CFileInfo(const std::string &sName, unsigned int fileCategory, unsigned long fileCode)
{
	char szPath[_MAX_PATH];
	char szDrive[_MAX_DRIVE];
	_splitpath(sName.c_str(), szDrive, szPath, NULL, NULL);

	m_sName = sName;
	m_uiFileCategory = fileCategory;
	m_ulFileCode = fileCode;
	if ( strlen(szDrive) == 0 )
	{
		// if for some reason sName did not contain the drive letter, 
		// this means that path is unknown -> get current directory as path
		GetCurrentDirectory(_MAX_PATH, szPath);
		m_sPath = szPath;
	}
	else
	{
		// otherwise build path (e.g. c:\documents\fastmodel\ )
		m_sPath = szDrive;
		m_sPath += szPath;
	}

	// get info about the file
	HANDLE f = CreateFile(m_sName.c_str(), 0, //// FILE_SHARE_DELETE | // illegal parameter under Win95/Me
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// note that if f is NULL, that is, the file still does not exist,
	// the m_dtFileDate member will result invalid when using GetStatus();
	// therefore, it is possible to watch also for file creation, other than file modification
	if ( f != INVALID_HANDLE_VALUE )
	{
		FILETIME FileModifyTime;
		// get info of last write time
		GetFileTime (f, NULL, NULL, &FileModifyTime);
		// store time in COleDateTime structure; this is equivalent to CTime, 
		// but allows a smaller granularity (about 1ms instead of 1s) 
		m_dtFileDate = FileModifyTime;

		CloseHandle(f);
	}
	else
		m_dtFileDate.SetStatus(COleDateTime::invalid);
}

CFileInfo::~CFileInfo()
{

}


//////////////////////////////////////////////////////////////////////
// CFileChangeEvent class
//////////////////////////////////////////////////////////////////////

// constructor also initializes the CEvent m_evStopWatch
CFileChangeEvent::CFileChangeEvent() : m_evStopWatch(FALSE, TRUE)
{
	m_clsWatchThread = NULL;

}

CFileChangeEvent::~CFileChangeEvent()
{
	if ( m_clsWatchThread )
		stopWatch();

	// delete the CFileInfo map
	m_mapFileInfoMap.clear();

	// deleting the path vector
	m_vectPaths.clear();
}

void CFileChangeEvent::startWatch(void)
{
	// only if file watching have not already been started
	if ( !m_clsWatchThread )
	{
		// uses a CEvent to communicate with the thread; if reset, loops forever,
		// if set, exits gracefully
		m_evStopWatch.ResetEvent();
		// begin thread; if cannot begin, m_clsWatchThread will be NULL
		m_clsWatchThread = AfxBeginThread(FileChangeWatch,(LPVOID)this);
	}
}

void CFileChangeEvent::stopWatch(void)
{
	DWORD status, err;

	// only if file watching have actually been started
	if ( m_clsWatchThread )
	{
		m_evStopWatch.SetEvent();

		// wait for the thread to stop, with a 500 msec timeout
		status = WaitForSingleObject(m_clsWatchThread->m_hThread, 500);
		// if object did not terminate ok, check error code;
		// note that status == WAIT_FAILED and err == INVALID_HANDLE
		// is normal if the thread has exited before the WaitForSingleObject() call
		if(status != WAIT_OBJECT_0)
			err = GetLastError();

		m_clsWatchThread = NULL;
	}
}

// the function returns 'true' if the file has been newly added to watch list;
// returns 'false' if the file was already under watch
bool CFileChangeEvent::addFile(const std::string &sFileName, unsigned int fileCategory, unsigned long fileCode)
{
	BOOL bEvent;
	FilePathVector::iterator it;

	// check if filename is already in the map; if so, return
	if ( m_mapFileInfoMap.find(sFileName) != m_mapFileInfoMap.end() )
		return false;

	// Only if file watching have been started, stop (and restart); otherwise, do nothing
	// (this allows to pre-add all files to be watched without having to stop and 
	// start the process every time).
	// Note that it is necessary to stop thread before changing the strucures it accesses
	// Note also that, of course, if a file change event happens while the thread is 
	// stopped, it will be missed
	bEvent = (m_clsWatchThread != NULL);
	if ( bEvent )
	{
		stopWatch();
	}

	// build a new CFileInfo structure for the specified file
	CFileInfo f(sFileName, fileCategory, fileCode);
	// and insert it in the map
	m_mapFileInfoMap.insert(std::make_pair(sFileName, f));

	for (it = m_vectPaths.begin(); it != m_vectPaths.end(); it++)
	{
		// if the path was already in the vector
		if ( (*it).getName() == f.getPath() )
		{
			// access the underlying structure (CFilePath) and increase its counter by one,
			// then break out from loop immediately (do not add anything to the list,
			// as the directory is already being watched)
			(*it)++;
			break;
		}
	}
	// if the path wasn't already in the vector
	if( it == m_vectPaths.end() ) 
		// add the new path at the end of the vector
		m_vectPaths.push_back(CFilePath(f.getPath()));

	// only if file watching had been started, restart; otherwise, do nothing
	// (this allows to pre-add all files to be watched without having to stop and 
	// start the process every time)
	if ( bEvent )
	{
		startWatch();
	}

	return true;
}

// Removes a file from the watch list.
// If the file was not in the watch list, do nothing.
void CFileChangeEvent::removeFile(const std::string &sFileName)
{
	BOOL bEvent;

	// check if filename is already in the map; if not, return (do nothing)
	if ( m_mapFileInfoMap.find(sFileName) == m_mapFileInfoMap.end() )
		return;

	// Only if file watching have been started, stop (and restart); otherwise, do nothing
	// (this allows to delete files to be watched without having to stop and 
	// start the process every time).
	// Note that it is necessary to stop thread before changing the strucures it accesses
	// TBC warning: to avoid changing the data structures while the thread is accessing them,
	// should wait for actual thread exiting, before continuing
	bEvent = (m_clsWatchThread != NULL);
	if ( bEvent )
	{
		stopWatch();
	}

	threadRemoveFile(sFileName);

	// only if file watching had been started, restart; otherwise, do nothing
	// (this allows to pre-add all files to be watched without having to stop and 
	// start the process every time)
	if ( bEvent )
	{
		startWatch();
	}
}

// This function is used by the public removeFile(), but it is called also
// by the worker thread, so it must not stop and restart the thread itself.
void CFileChangeEvent::threadRemoveFile(const std::string &sFileName)
{
	CFileInfo f(sFileName);
	FileInfoMap::iterator itFIM;
	CSingleLock singlelock(&m_clsCriticalSect);

	// multi-thread safe implementation: the function is called 
	// by both removeFile() and the worker thread
	singlelock.Lock(INFINITE);

	// remove path from watch list, if going to be empty (to be removed file was last one)
	for (FilePathVector::iterator it = m_vectPaths.begin(); it != m_vectPaths.end(); it++)
	{
		// find the path in the vector
		if ( (*it).getName() == f.getPath() )
		{
			// decrease the counter of the number of files being watched in this directory 
			(*it)--;
			// if no more files, delete CFilePath object and erase element from vector
			// (counter starts from one, see CFilePath constructor)
			if ( (*it).getCount() <= 0 )
			{
				m_vectPaths.erase(it);
			}
			break;
		}
	}

	// remove file name from file info map

	itFIM = m_mapFileInfoMap.find(sFileName);
	// check esistence in map (should always exist, but better cheking)
	if ( itFIM != m_mapFileInfoMap.end() ) {
		// remove from map
		m_mapFileInfoMap.erase(sFileName);
	}

}

// sets a file date 
void CFileChangeEvent::FileSetDate(const std::string &sFileName, const COleDateTime &dtDate)
{
	FileInfoMap::iterator itFIM;
	CSingleLock singlelock(&m_clsCriticalSect);

	// multi-thread safe implementation (should any other function modify 
	// the data structure)
	singlelock.Lock(INFINITE);

	// change date in file info map

	itFIM = m_mapFileInfoMap.find(sFileName);
	// check esistence in map 
	if ( itFIM != m_mapFileInfoMap.end() ) {
		// set date
		itFIM->second.setDate(dtDate);
	}
}

// this is the worker thread watching the files
UINT CFileChangeEvent::FileChangeWatch(LPVOID lpParam)
{
	FileInfoMap::iterator itFIM;
	FileInfoMap copymap;
	HANDLE *hChangeHandles;
	COleDateTime dtDate, dtDate2;
	FILETIME dtFileDate;
	FILETIME dtCreateDate;
	int i;

	// retrieves pointer to the class
	CFileChangeEvent *pFileChangeEvent = (CFileChangeEvent *) lpParam;

	// construct a vector of as many HANDLEs as paths in the path vector plus one;
	// the additional one will be the CEvent used to communicate to the thread
	// to stop watching
	hChangeHandles = new HANDLE[pFileChangeEvent->m_vectPaths.size() + 1];
 
	if( pFileChangeEvent->m_vectPaths.size() == 0) {
		i=0;
	}

	while ( 1 )	//endless loop
	{
		// fill up the handles
		hChangeHandles[0] = pFileChangeEvent->m_evStopWatch;
		for (i = 0; i < pFileChangeEvent->m_vectPaths.size(); i++)
		{
			// set up change events on given directories
			hChangeHandles[i+1] = FindFirstChangeNotification(pFileChangeEvent->m_vectPaths[i].getName().c_str(), 
															  FALSE,
															  FILE_NOTIFY_CHANGE_LAST_WRITE
															  | FILE_NOTIFY_CHANGE_SIZE
															  | FILE_NOTIFY_CHANGE_FILE_NAME
															 );
		}

		// start waiting for any of the events; anyway, every 2000ms, stop waiting and 
		// check for any file modification (in case some file change event was missed)
		DWORD dwResult = WaitForMultipleObjects(pFileChangeEvent->m_vectPaths.size() + 1,
			hChangeHandles, FALSE, 2000);

		// sleep for 10ms, while waiting for any other file change (i.e. multiple
		// file deletion)
		Sleep(10);

		// the return value minus WAIT_OBJECT_0 is the # of the signalled event; if more than
		// one event have been signalled, it is the smallest # of them, so having set the 
		// stop watching event as first one assures that this event is always caught 

		// watch stopped 
		if ( dwResult == WAIT_OBJECT_0 )	
		{
			break;
		}
		
		// othervise, if not stop event, compute the index of the event
		int nPathIdx = dwResult - WAIT_OBJECT_0 - 1;

		// Search for changed files

		// Copy the file map to a temporary map. In this way, the copied map
		// can be scanned without errors even if the original map is changed
		// during the loop (i.e. removing a file from the map), either here
		// or by the OnFileAlarm() handler.
		// Warning: as a side effect, any change made in the copied map is NOT
		// applied to the original map; to this purpose, must use dedicated
		// functions scanning the original map (e.g. for changing file date stamp)
		copymap = pFileChangeEvent->m_mapFileInfoMap;

		// scan all the map (multiple events could have been occurred)
		for (itFIM = copymap.begin(); itFIM != copymap.end(); itFIM++)
		{
			// Could scan only files in directories notified by WaitForMultipleObjects;
			// but if a change happens in the meanwhile (e.g. as happens when deleting
			// multiple files), the event will be missed. So it's better to check all files

			// if the path and name of the map element matches the path and name with change  
			//if ( itFIM->second->getPath() == pFileChangeEvent->m_vectPaths[nPathIdx]->getName() )
			//

			// get info about the file under watch, to see if it is it that was modified
			HANDLE f = CreateFile(itFIM->second.getName().c_str(), 
								  0,
								  // FILE_SHARE_DELETE | // illegal parameter under Win95/Me
								  FILE_SHARE_READ
								  | FILE_SHARE_WRITE,
								  NULL,
								  OPEN_EXISTING,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);

			if ( f != INVALID_HANDLE_VALUE )
			{
				GetFileTime (f, &dtCreateDate, NULL, &dtFileDate);
				CloseHandle(f);
				// if the COleDateTime was invalid at the time of adding to watch list
				// for the given file, that means it has been created
				if ( itFIM->second.getDate().GetStatus() != COleDateTime::valid )
				{
					// set the new 'last modify' date
					pFileChangeEvent->FileSetDate(itFIM->second.getName(), dtCreateDate);
					// fire alarm
					pFileChangeEvent->OnFileAlarm(FCE_CREATED, itFIM->second.getName(), 
										itFIM->second.getCategory(), itFIM->second.getCode());
				}
				else
				{
					dtDate = dtFileDate;
					dtDate2 = itFIM->second.getDate();

					// verifiy if the file has been modified
					if ( itFIM->second.getDate() < dtDate )
					{
						// set the new 'last modify' date
						pFileChangeEvent->FileSetDate(itFIM->second.getName(), dtDate);
						// fire alarm
						pFileChangeEvent->OnFileAlarm(FCE_CHANGED, itFIM->second.getName(), 
										itFIM->second.getCategory(), itFIM->second.getCode());
					}
				}
			}
			// Otherwise, if file does not exist any more, signal removal
			// Note that the file is NOT removed from watch list; it's up
			// to the OnFileAlarm handler to remove the file from the list,
			// if required
			else
			{
				// Must remove from the watch list, otherwise, since this
				// is a worker thread, if the main app delays for some 
				// reason the removal and a file operation happens on any
				// of the watched directories, a new FCE_REMOVED alarm will 
				// be fired for the same file.
				pFileChangeEvent->threadRemoveFile(itFIM->second.getName());
				// fire alarm
				pFileChangeEvent->OnFileAlarm(FCE_REMOVED, itFIM->second.getName(), 
									itFIM->second.getCategory(), itFIM->second.getCode());
			}
		}

	}

	// this point is reached only if the worker thread is stopped by signalling 
	// the CEvent m_evStopWatch 
	delete[] hChangeHandles;
	return 0;
}


