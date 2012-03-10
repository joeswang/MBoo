// MBooView.h : interface of the CMBooView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
//#include "AdvWnd.h"
#include "VideoWnd.h"
#include "VideoList.h"
#include "VideoPanel.h"

class CMBooView : public CWindowImpl<CMBooView>
{
public:
	DECLARE_WND_CLASS(NULL)

	//CAdvWnd		m_advWnd;
	RECT		m_videoRect;
	CVideoWnd	m_videoWnd;
	CVideoList	m_videoList;
	CVideoPanel	m_videoPanel;

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL PlayFlashVideo(LPCTSTR lpszURL);

	BEGIN_MSG_MAP(CMBooView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLBtnDown)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
