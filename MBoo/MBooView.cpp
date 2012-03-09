// MBooView.cpp : implementation of the CMBooView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "MBooView.h"

BOOL CMBooView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CMBooView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//TODO: Add your drawing code here
	RECT rc;
	GetClientRect(&rc);
/*
	TCHAR msg[256] = {0};
	_stprintf_s(msg, 256, _T("%d:%d / %d:%d"), rc.left, rc.right, rc.top, rc.bottom);
	dc.TextOut(100,100, msg);
*/
	CBrush brush;
	brush.CreateSolidBrush(RGB(130,135,144));
	dc.FillRect(&rc, brush);
/*
	rc.left = rc.top = 0;
	rc.right = m_videoWnd.GetVideoWidth();
	rc.bottom = m_videoWnd.GetVideoHeight();

	CMemoryDC dcMem(dc, rc);
	//dcMem.CreateCompatibleDC(dc);
	CBitmap bmpBK;
	bmpBK.LoadBitmap(IDR_BACKGROUND);
	HBITMAP hbmpOld = dcMem.SelectBitmap(bmpBK);
	dc.BitBlt(m_videoRect.left, m_videoRect.top, 
			m_videoRect.right - m_videoRect.left, 
			m_videoRect.bottom - m_videoRect.top,
			dcMem, 0, 0, SRCCOPY);
	dcMem.SelectBitmap(hbmpOld);
*/
	return 0;
}

LRESULT CMBooView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	
	//m_advWnd.Create(m_hWnd);
	//m_advWnd.ShowWindow(SW_SHOW);
	m_videoRect.bottom = m_videoRect.left = m_videoRect.right = m_videoRect.top = 0;
	m_videoWnd.Create(m_hWnd);
	m_videoWnd.ShowWindow(SW_HIDE);

	m_videoList.Create(m_hWnd);
	m_videoList.ShowWindow(SW_SHOW);

	m_videoPanel.Create(m_hWnd);
	//m_videoPanel.UpdateUI();
	m_videoPanel.ShowWindow(SW_SHOW);

	m_videoWnd.SetPanelHandle(&m_videoPanel);
	m_videoPanel.SetFlashObject(&m_videoWnd);
	m_videoList.SetFlashObject(&m_videoWnd);

	return 0;
}

BOOL CMBooView::PlayFlashVideo(LPCTSTR lpszURL)
{
	BOOL ret;
	
	//m_advWnd.ShowWindow(SW_HIDE);
	m_videoWnd.ShowWindow(SW_SHOW);

	ret = m_videoWnd.PlayFlashVideo(lpszURL);
	//m_videoPanel.UpdateUI();

	return ret;
}

LRESULT CMBooView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UINT cw, ch;   // client area width and height
	UINT vw, vh;   // video width and height
	UINT margin, x, y, width, height;

	RECT rc;
	GetClientRect(&rc);

	cw = rc.right - rc.left;
	ch = rc.bottom - rc.top;
	vw = m_videoWnd.GetVideoWidth();
	vh = m_videoWnd.GetVideoHeight();

	if((cw - 2 < vw + DIALOG_LIST_MIN_WIDTH) || (ch - 2 < vh + DIALOG_PANEL_MIN_HEIGHT))
	{
		return 0;
	}

	// move the Dialog Panel
	height = ch - 2 - vh;
	if(height > DIALOG_PANEL_MAX_HEIGHT) height = DIALOG_PANEL_MAX_HEIGHT; 

	margin =  (ch - 2 - height - vh ) / 2;

	x = rc.right - 1 - margin - vw;
	y = rc.top + 1 + margin;
	m_videoWnd.WindowMove(x, y, vw, vh);
	//m_advWnd.WindowMove(x, y, vw, vh);
	m_videoRect.left = x;
	m_videoRect.top = y;
	m_videoRect.right = x + vw;
	m_videoRect.bottom = y + vh;

	width = cw - 2 - 2 * margin - vw;
	m_videoList.WindowMove(0, 0, width, rc.bottom - rc.top);

	x = rc.left + width;
	y = rc.bottom - height;
	width = cw - width;
	m_videoPanel.WindowMove(x, y, width, height);

	return 0;
}
