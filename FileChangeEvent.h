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


// FileChangeEvent.h: interface for the CFileChangeEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECHANGEEVENT_H__1B890621_FEC8_11D2_B9E0_704B7C000000__INCLUDED_)
#define AFX_FILECHANGEEVENT_H__1B890621_FEC8_11D2_B9E0_704B7C000000__INCLUDED_

#if _MSC_VER > 1000
// instruct the compiler to include this header only once
#pragma once
#endif // _MSC_VER > 1000

//
// Modified by : Enrico Di Lorenzo, 2002/06/30
// - Added comments
// - Merged all classes in this file
// - Modified removeFile() and addFile() to forbid concurrent access to data
//   structures by different threads
// - Added m_ulFileCode, m_uiFileCategory parameters in CFileInfo to keep trace of a file token
//   used when posting a message indicating a file change, to identify the 
//   file in the WPARAM and LPARAM (and thus without having to pass the name)  
// 
//
// Author   : Franky Braem
// Created  : 30/04/1999
// Modified : 17/05/1999 - Use the drive of the path instead of current drive
//	      18/05/1999 - Use the current path if no path specified

// disable the warning number C4786, issued when the string for a title or subtitle pragma 
// exceeded the maximum allowable length and was truncated. The debugger cannot debug code
// with symbols longer than 255 characters, but STL often creates symbols longer than that.
#pragma warning(disable:4786)

#include <string>
#include <map>
#include <vector>

// include for multi-thread support (semaphores, etc.)
#include <afxmt.h>

// nAlarm values
#define FCE_CHANGED		0x0001
#define FCE_REMOVED		0x0002
#define FCE_CREATED		0x0003

class CFileInfo  
{
public:
	CFileInfo(const std::string &sName, unsigned int fileCategory = 0, unsigned long fileCode = 0);
	CFileInfo()
	{
		m_sName = _T("");
	}
	virtual ~CFileInfo();
	inline std::string getName(void) const { return m_sName; }
	inline std::string getPath(void) const { return m_sPath; }
	inline unsigned int getCategory(void) const { return m_uiFileCategory; }
	inline unsigned long getCode(void) const { return m_ulFileCode; }
	inline COleDateTime getDate(void) const { return m_dtFileDate; }
	inline void setDate(const COleDateTime &dtDate) { m_dtFileDate = dtDate; }

protected:
	std::string m_sName;
	std::string m_sPath;
	unsigned int m_uiFileCategory;
	unsigned long m_ulFileCode;
	COleDateTime m_dtFileDate;
};

class CFilePath
{
public:
	CFilePath(const std::string &sPath) { m_sName = sPath; m_nCount = 1; }
	CFilePath() { 
		m_sName = _T(""); m_nCount = 1; 
	}

	inline bool operator<(CFilePath &filepath)
	{ 
		return( getName() < filepath.getName() );
	}
	inline std::string getName(void) const { return m_sName; }
	inline void operator++(int) { m_nCount++; }
	inline void operator--(int) { m_nCount--; }
	inline int getCount(void) { return m_nCount; }
protected :
	std::string m_sName;
	int m_nCount;
};

// costruct a map where the key is the file name and the data is a CFileInfo structure
typedef std::map<std::string, CFileInfo> FileInfoMap;
// construct a vector for storing the directory paths of the files being watched
typedef std::vector<CFilePath> FilePathVector;

class CFileChangeEvent  
{
public:
	CFileChangeEvent();
	virtual ~CFileChangeEvent();
	bool addFile(const std::string &sFileName, unsigned int fileCategory, unsigned long fileCode);
	void removeFile(const std::string &sFileName);
	void startWatch(void);
	void stopWatch(void);
	inline bool isWatching(void) const { return(m_clsWatchThread != NULL); }
protected:
	static UINT FileChangeWatch(LPVOID lpParam);
	void threadRemoveFile(const std::string &sFileName);
	void FileSetDate(const std::string &sFileName, const COleDateTime &dtDate);
	// this function is virtual because must be overridden to be notified of the
	// file change event
	virtual void OnFileAlarm(unsigned int nAlarm, const std::string &sFileName, 
							unsigned int fileCategory, unsigned long fileCode) {}	

	CCriticalSection m_clsCriticalSect;
	CEvent m_evStopWatch;
	FileInfoMap m_mapFileInfoMap;
	FilePathVector m_vectPaths;
	CWinThread *m_clsWatchThread;

};

#endif // !defined(AFX_FILECHANGEEVENT_H__1B890621_FEC8_11D2_B9E0_704B7C000000__INCLUDED_)
