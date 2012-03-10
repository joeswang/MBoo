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

	CBrush brush;
	brush.CreateSolidBrush(RGB(130,135,144));
	dc.FillRect(&rc, brush);
/*
	TCHAR msg[256] = {0};
	_stprintf_s(msg, 256, _T("%d:%d / %d:%d"), rc.left, rc.right, rc.top, rc.bottom);
	dc.TextOut(100,100, msg);
*/
/*
	rc.left = rc.top = 0;
	rc.right = m_videoWnd.GetVideoWidth();
	rc.bottom = m_videoWnd.GetVideoHeight();
*/
	SIZE sz;
	CBitmap bmpBK;
	bmpBK.LoadBitmap(IDR_BITMAP_BKGROUND);
	bmpBK.GetSize(sz);
	if(m_videoRect.right - m_videoRect.left < sz.cx || m_videoRect.bottom - m_videoRect.top < sz.cy) return 0;

	int x = m_videoRect.left + (m_videoRect.right - m_videoRect.left - sz.cx) / 2;
	int y = m_videoRect.top + (m_videoRect.bottom - m_videoRect.top - sz.cy) / 2;

	CDC dcMem;
	dcMem.CreateCompatibleDC(dc);
	HBITMAP hbmpOld = dcMem.SelectBitmap(bmpBK);
	dc.BitBlt(x, y, sz.cx, sz.cy, dcMem, 0, 0, SRCCOPY);
	dcMem.SelectBitmap(hbmpOld);
	//dcMem.DeleteDC();
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
	UINT marginV, marginH, x, y, width, height;

	RECT rc;
	GetClientRect(&rc);

	cw = rc.right - rc.left;
	ch = rc.bottom - rc.top;
	vw = m_videoWnd.GetVideoWidth();
	vh = m_videoWnd.GetVideoHeight();

	if((cw  < vw + DIALOG_LIST_MIN_WIDTH) || (ch < vh + DIALOG_PANEL_HEIGHT))
	{
		return 0;
	}

	// move the Dialog Panel
	height = DIALOG_PANEL_HEIGHT;
	//if(height > DIALOG_PANEL_MAX_HEIGHT) height = DIALOG_PANEL_MAX_HEIGHT; 

	marginV =  (ch - height - vh ) / 2;
	marginH = marginV;
	if( cw - vw - marginH * 2 < DIALOG_LIST_MIN_WIDTH) 
	{
		marginH = (cw -vw - DIALOG_LIST_MIN_WIDTH) / 2;
	}
	x = rc.right - marginH - vw;
	y = rc.top + marginV;
	m_videoWnd.WindowMove(x, y, vw, vh);
	//m_advWnd.WindowMove(x, y, vw, vh);
	m_videoRect.left = x;
	m_videoRect.top = y;
	m_videoRect.right = x + vw;
	m_videoRect.bottom = y + vh;

	width = cw - 2 * marginH - vw;
	m_videoList.WindowMove(0, 0, width, rc.bottom - rc.top);

	x = rc.left + width;
	y = rc.bottom - height;
//	width = cw - width;
	m_videoPanel.WindowMove(x, y, cw - width, height);

	return 0;
}
