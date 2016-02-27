
// MFC_Container.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFC_Container.h"
#include "MFC_ContainerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_ContainerApp

BEGIN_MESSAGE_MAP(CMFC_ContainerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFC_ContainerApp construction

CMFC_ContainerApp::CMFC_ContainerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFC_ContainerApp object

CMFC_ContainerApp theApp;


// CMFC_ContainerApp initialization

BOOL CMFC_ContainerApp::InitInstance()
{
	CWinApp::InitInstance();


	AfxEnableControlContainer();


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("QPlanetOSM windows activex container demo"));

	CMFC_ContainerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

