// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "aboutdlg.h"
#include "DlgOption.h"
#include "MBooView.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}
LRESULT CMainFrame::OnChangeTile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR msg[MAX_PATH + 1] = {0};

	RECVIDEO* p = (RECVIDEO*)lParam;
	if(NULL == p) return 0;

	_stprintf_s(msg, MAX_PATH, _T("�󱦲����� - ��ǰ��Ƶ : [%s] %s"), p->name, p->title);

	SetWindowText(msg);

	return 0;
}

LRESULT CMainFrame::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

    lpMMI->ptMinTrackSize.x = MBOO_MIN_WIDTH; 
    lpMMI->ptMinTrackSize.y = MBOO_MIN_HEIGHT; 

	return 0;
}

LRESULT CMainFrame::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(WA_ACTIVE == LOWORD(wParam) || WA_CLICKACTIVE == LOWORD(wParam))
	{
		MessageBox(_T("Good"));
	}
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document
	TCHAR lpszURL[_MAX_PATH + 256] = { 0 };
	CFileDialog cFilePath( TRUE,NULL,NULL,OFN_SHAREAWARE,_T("BBK Video Files (*.swf)\0*.swf\0"));

	cFilePath.m_ofn.lpstrFile = lpszURL;
	cFilePath.m_ofn.nMaxFile = _MAX_PATH + 256;
	if( IDOK  != cFilePath.DoModal() ) return 0;

	m_view.PlayFlashVideo(lpszURL);

	return 0;
}

LRESULT CMainFrame::OnFileOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDlgOption dlgOption;

	if(IDOK != dlgOption.DoModal()) return 0;
	
	m_view.UpdateVideoTree();
	
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

BOOL CMainFrame::PlayFlashVideo(LPCTSTR lpszURL)
{
	return m_view.PlayFlashVideo(lpszURL);
}
