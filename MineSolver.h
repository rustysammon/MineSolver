// MineSolver.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CMineSolverApp:
// See MineSolver.cpp for the implementation of this class
//

class CMineSolverApp : public CWinApp
{
public:
	CMineSolverApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMineSolverApp theApp;