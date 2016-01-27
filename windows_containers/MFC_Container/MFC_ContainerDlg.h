
// MFC_ContainerDlg.h : header file
//

#pragma once
#include "qtaxviewer_planetosm.h"


// CMFC_ContainerDlg dialog
class CMFC_ContainerDlg : public CDialog
{
// Construction
public:
	CMFC_ContainerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_CONTAINER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CQtaxviewer_planetosm m_map;
};
