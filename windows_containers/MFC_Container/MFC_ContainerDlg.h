
// MFC_ContainerDlg.h : header file
//

#pragma once
#include "qtaxviewer_planetosm.h"
#include "afxwin.h"
#include <list>

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
	//we need to load a PNG to geomarker, when we want to draw icons on map first time;
	std::list<double> m_list_old_lats;
	std::list<double> m_list_old_lons;
	bool m_bInited;
	double m_currentLat;
	double m_currentLon;
	double m_direct;
	int	   m_nTimer;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	CQtaxviewer_planetosm m_map;
	CListBox m_list_msg;
	CStatic m_static_msg;
	void evt_MessageQtaxviewerPlanetosm(LPCTSTR p0);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
