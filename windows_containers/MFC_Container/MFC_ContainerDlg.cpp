
// MFC_ContainerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <unordered_map>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "MFC_Container.h"
#include "MFC_ContainerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//! convert string key-value pairs to map.
std::unordered_map<_tstring, _tstring> string2map(LPCTSTR str);
//!export a PNG file to temp folder
_tstring export_res();

// CMFC_ContainerDlg dialog



CMFC_ContainerDlg::CMFC_ContainerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MFC_CONTAINER_DIALOG, pParent)
	, m_currentLat(0)
	, m_currentLon(0)
	, m_direct (0)
	, m_nTimer(1)
	, m_bInited (false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	srand((unsigned int)time(0));
	m_direct = rand() % 360;
}

void CMFC_ContainerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QTAXVIEWER_PLANETOSM, m_map);
	DDX_Control(pDX, IDC_LIST_MSG, m_list_msg);
	DDX_Control(pDX, IDC_STATIC_MOUSEMOVE, m_static_msg);
}

BEGIN_MESSAGE_MAP(CMFC_ContainerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CMFC_ContainerDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CMFC_ContainerDlg::OnBnClickedButtonPause)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFC_ContainerDlg message handlers

BOOL CMFC_ContainerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFC_ContainerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFC_ContainerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//MFC framework is old enough, that it does not support automatically size-management.
void CMFC_ContainerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (cx && cy)
	{
		if (IsWindow(this->GetSafeHwnd()))
		{
			if (IsWindow(this->m_map.GetSafeHwnd()))
			{
				m_map.MoveWindow(0, 80, cx, cy-80);
				CWnd * w = GetDlgItem(IDC_LIST_MSG);
				CRect rect;
				w->GetWindowRect(&rect);
				ScreenToClient(&rect);
				if (w)
					w->MoveWindow(rect.left, rect.top, cx - rect.left-4, rect.Height());
			}
		}

	}
}
BEGIN_EVENTSINK_MAP(CMFC_ContainerDlg, CDialog)
	ON_EVENT(CMFC_ContainerDlg, IDC_QTAXVIEWER_PLANETOSM, 9, CMFC_ContainerDlg::evt_MessageQtaxviewerPlanetosm, VTS_BSTR)
END_EVENTSINK_MAP()


void CMFC_ContainerDlg::evt_MessageQtaxviewerPlanetosm(LPCTSTR p0)
{
	std::unordered_map<_tstring, _tstring> paras = string2map(p0);
	CString add_msg = p0;
	if (paras[_T("name")] == _T("MOUSE_MOVE"))
	{
		m_static_msg.SetWindowText(add_msg);
	}
	else
	{
		int ct = m_list_msg.GetCount();
		m_list_msg.InsertString(ct, add_msg);
		m_list_msg.SetCurSel(ct);
		while (m_list_msg.GetCount() >= 1024)
			m_list_msg.DeleteString(0);
	}
}


void CMFC_ContainerDlg::OnBnClickedButtonPlay()
{
	//if not inited, load icons
	if (m_bInited == false)
	{
		m_bInited = true;
			
		//load png
		_tstring fm = export_res();
		if (fm.size())
		{
			CString cmd;
			cmd.Format(_T("function=add_resource;name=arrow;"
				"filename=%s;centerx=31;centery=31;"
				),
				fm.c_str()
				);
			m_map.osm_layer_call_function(_T("geomarker1"), cmd);
		}

	}
	this->SetTimer(m_nTimer, 200, 0);
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
}


void CMFC_ContainerDlg::OnBnClickedButtonPause()
{
	this->KillTimer(m_nTimer);
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
}


void CMFC_ContainerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimer)
	{
		this->KillTimer(m_nTimer);
		const double pi = 3.14159265355;
		double s = 180 * (30.0 / 6300) / pi;
		double dx = s * cos(m_direct/180.0*pi) / cos (m_currentLat * pi /180.0);
		double dy = s * sin(m_direct / 180.0*pi);
		m_currentLat += dy;
		m_currentLon += dx;

		if (m_currentLat <= -85)
		{
			m_currentLat = -85;
			m_direct = rand() % 90 + 45;
		}
		else if (m_currentLat >= 85)
		{
			m_currentLat = 85;
			m_direct = -(rand() % 90 + 45);
		}
		m_currentLon += 360 * 2;
		m_currentLon = fmod(m_currentLon+180, 360)-180;


		
		CString cmd;
		cmd.Format(_T("function=update_icon;name=point1;"
			"lat=%.8lf;lon=%.8lf;"
			"icon=arrow;scale=0.5;rotate=%.2lf;smooth=1;"
			),
			m_currentLat, m_currentLon, 90-m_direct
			);
		m_map.osm_layer_call_function(_T("geomarker1"), cmd);

		cmd.Format(_T("function=update_props;name=point1;"
			"lat=%.8lf;lon=%.8lf;direct=%.8lf;"
			),
			m_currentLat, m_currentLon, m_direct
			);
		m_map.osm_layer_call_function(_T("geomarker1"), cmd);

		//Lines
		m_list_old_lats.push_front(m_currentLat);
		m_list_old_lons.push_front(m_currentLon);
		const int maxsz = 128;
		while (m_list_old_lats.size() > maxsz)
		{
			m_list_old_lats.pop_back();
			m_list_old_lons.pop_back();
		}
		int c = 0;

		double lastlat = m_currentLat, lastlon = m_currentLon;
		for (
			std::list<double>::iterator latp = m_list_old_lats.begin(), lonp = m_list_old_lons.begin();
			latp != m_list_old_lats.end() && lonp != m_list_old_lons.end();
			++latp, ++lonp, ++c
			)
		{

			cmd.Format(_T("function=update_line;name=line%d;"
				"lat0=%.7lf;lon0=%.7lf;lat1=%.7lf;lon1=%.7lf;"
				"style_pen=1;color_pen=255,0,0,%d;width_pen=%d;"
				),
				c,
				lastlat, lastlon, *latp, *lonp,
				int((1 - c * 1.0/ maxsz)*255),
				int((1 - c * 1.0 / maxsz) * 6)+1
				);
			m_map.osm_layer_call_function(_T("geomarker1"), cmd);

			lastlat = *latp, lastlon = *lonp;

		}
		m_map.osm_set_center_pos(m_currentLat, m_currentLon);

		if (rand()%10==1)
			m_direct += rand()%100/10.0-5.0;
		this->SetTimer(m_nTimer, 100, 0);
	}

	CDialog::OnTimer(nIDEvent);
}


//----------------------------------

std::unordered_map<_tstring, _tstring> string2map(LPCTSTR str)
{
	std::unordered_map<_tstring, _tstring> res_map;
	size_t szLen = _tcslen(str);
	std::vector<TCHAR> pArrBuf;
	pArrBuf.resize(szLen + 1);
	_tcscpy_s(pArrBuf.data(), szLen + 1, str);

	std::vector<size_t> vec_pairs;
	//record each pairs' begin
	vec_pairs.push_back(0);
	for (int i = 0; i < szLen; ++i)
	{
		if (pArrBuf[i] == ';')
		{
			vec_pairs.push_back(i + 1);
			pArrBuf[i] = 0;
		}
	}
	//for each pairs, split by '='
	size_t pairs = vec_pairs.size();
	for (size_t i = 0; i < pairs; ++i)
	{
		size_t cur_pair = vec_pairs[i];
		if (pArrBuf[cur_pair] == 0)
			continue;
		int pec = cur_pair;
		while (pec < szLen)
		{
			if (pArrBuf[pec] == 0)
				break;
			if (pArrBuf[pec] == _T('='))
			{
				pArrBuf[pec] = 0;
				_tstring key = &pArrBuf[cur_pair];
				_tstring val = &pArrBuf[pec + 1];
				res_map[key] = val;
			}
			else
				++pec;

		}
	}
	return std::move(res_map);
}

_tstring export_res()
{
	_tstring fm;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(IDB_PNG_MARK), _T("PNG")); // type
	if (!hRsrc)
		return std::move(fm);

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return std::move(fm);

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);

	// save to file
	const DWORD dwSize = MAX_PATH;
	TCHAR tempPath[dwSize];

	if (GetTempPath(dwSize, tempPath) < dwSize)
	{
		_tstring filename = tempPath;
		filename += _T("\\qplanetosm_mfc_png.png");
		CFile file;
		if (file.Open(filename.c_str(), CFile::modeCreate | CFile::modeWrite))
		{
			file.Write(pmem, len);
			file.Close();
			fm = filename;
		}
	}


	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	return std::move(fm);
}