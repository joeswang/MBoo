#include "stdafx.h"
#include "resource.h"
#include "define.h"
#include "VideoData.h"

#include "DlgUnZip.h"

LRESULT CDlgUnZip::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_HIDE);
	CProgressBarCtrl pgs1 = GetDlgItem(IDC_UNZIP_PGS_PERCENT);
	CProgressBarCtrl pgs2 = GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP);
	pgs1.SetRange(0, 100);
	pgs1.SetPos(0);
	pgs2.SetRange(0, 100);
	pgs2.SetPos(0);

	//::SkinSE_SubclassWindow(m_hWnd, _T("dlg.videounzip"));
	//::SkinSE_SetLayoutMainFrame(m_hWnd);
	//::SkinSE_SubclassWindow(GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP), _T("unzip.pgs.video"));
	//::SkinSE_SubclassWindow(GetDlgItem(IDC_UNZIP_PGS_PERCENT), _T("unzip.pgs.total"));
	CListViewCtrl lvZIPFile = GetDlgItem(IDC_UNZIP_LIST_VIDEOZIP);
	//lvZIPFile.SetExtendedListViewStyle(LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	lvZIPFile.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//lvZIPFile.InsertColumn(0, _T("解压缩"), 0, 50);
	lvZIPFile.InsertColumn(0, _T("视频ZIP压缩包"), 0, 550);
	lvZIPFile.SetViewType(LVS_REPORT);
	int item = 0;
	for(int i=0; i<VIDEOZIP_MAX_NUMBERS; i++)
	{
		if(FALSE == g_tblVZIP[i].valid) break;
		if(TRUE == g_tblVZIP[i].unzip) continue;
		lvZIPFile.InsertItem(item, g_tblVZIP[i].name);
		//lvZIPFile.SetCheckState(item, TRUE);
		item++;
	}

	CenterWindow();

	return TRUE;
}

LRESULT CDlgUnZip::OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	EndDialog(IDOK);
	return TRUE;
}

LRESULT CDlgUnZip::OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CDlgUnZip::OnBtnUncompress(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
/*
	CProgressBarCtrl pgs1 = GetDlgItem(IDC_UNZIP_PGS_PERCENT);
	CProgressBarCtrl pgs2 = GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP);
	pgs1.SetRange(0, 100);
	pgs1.SetPos(0);
	pgs2.SetRange(0, 100);
	pgs2.SetPos(0);

	GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_SHOW);
	GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_SHOW);
	GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_SHOW);
	GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_SHOW);

	GetDlgItem(IDC_UNZIP_BTN_UNCOMPRESS).EnableWindow(FALSE);
	GetDlgItem(IDOK).EnableWindow(FALSE);
	GetDlgItem(IDCANCEL).EnableWindow(FALSE);
*/
	_beginthread(thread_unzip_videofile, 0, m_hWnd);

	return TRUE;
}

LRESULT CDlgUnZip::OnUpdatePGSUnzip(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
WORD total, current;
LPCTSTR zipfilename;
TCHAR msg[MAX_PATH] = { 0 };

	zipfilename = (LPCTSTR)wParam;
	if(NULL != zipfilename)
	{
		memset(msg, 0, sizeof(msg));
		StringCchCat(msg, MAX_PATH, _T("正在解压缩 : "));
		StringCchCat(msg, MAX_PATH, zipfilename);
		StringCchCat(msg, MAX_PATH, _T(" ......"));
		GetDlgItem(IDC_UNZIP_STATIC_PERCENT).SetWindowText(msg);
	}

	total = HIWORD(lParam); current = LOWORD(lParam);
	if(0 == total && 0 == current)
	{
		GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_SHOW);

		GetDlgItem(IDC_UNZIP_BTN_UNCOMPRESS).EnableWindow(FALSE);
		GetDlgItem(IDOK).EnableWindow(FALSE);
		GetDlgItem(IDCANCEL).EnableWindow(FALSE);
		return 0;
	}

	if(100 == total && 100 == current)
	{
		GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_UNZIP_BTN_UNCOMPRESS).EnableWindow();
		GetDlgItem(IDOK).EnableWindow();
		GetDlgItem(IDCANCEL).EnableWindow();
		return 0;
	}
	CProgressBarCtrl pgs1 = GetDlgItem(IDC_UNZIP_PGS_PERCENT);
	CProgressBarCtrl pgs2 = GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP);
	//pgs1.SetRange(0, 100);
	pgs1.SetPos(current);
	//pgs2.SetRange(0, 100);
	pgs2.SetPos(total);

	return 0;
}

