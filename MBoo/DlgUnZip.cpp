#include "stdafx.h"
#include "resource.h"
#include "define.h"
#include "VideoData.h"

#include "DlgUnZip.h"

LRESULT CDlgUnZip::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
//CEdit editDIR;
//	editDIR = GetDlgItem(IDC_OPTION_EDIT_DIR);
	GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_HIDE);
	GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_HIDE);


	//::SkinSE_SubclassWindow(m_hWnd, _T("dlg.videounzip"));
	::SkinSE_SetLayoutMainFrame(m_hWnd);

	::SkinSE_SubclassWindow(GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP), _T("unzip.pgs.video"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_UNZIP_PGS_PERCENT), _T("unzip.pgs.total"));

	CProgressBarCtrl pgs1 = GetDlgItem(IDC_UNZIP_PGS_PERCENT);
	CProgressBarCtrl pgs2 = GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP);
	pgs1.SetRange(0, 100);
	pgs1.SetPos(0);
	pgs2.SetRange(0, 100);
	pgs2.SetPos(0);

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

	_beginthread(thread_unzip_videofile, 0, m_hWnd);

	return TRUE;
}

LRESULT CDlgUnZip::OnUpdatePGSUnzip(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(100 == wParam && 100 == lParam)
	{
		GetDlgItem(IDC_UNZIP_STATIC_VIDEPZIP).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_STATIC_PERCENT).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_PGS_PERCENT).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UNZIP_BTN_UNCOMPRESS).EnableWindow();
		GetDlgItem(IDOK).EnableWindow();
		GetDlgItem(IDCANCEL).EnableWindow();
		return 0;
	}
	CProgressBarCtrl pgs1 = GetDlgItem(IDC_UNZIP_PGS_PERCENT);
	CProgressBarCtrl pgs2 = GetDlgItem(IDC_UNZIP_PGS_VIDEOZIP);
	//pgs1.SetRange(0, 100);
	pgs1.SetPos(lParam);
	//pgs2.SetRange(0, 100);
	pgs2.SetPos(wParam);

	return 0;
}

