// DlgOption.cpp : implementation of the CDlgOption class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "DlgOption.h"

LRESULT CDlgOption::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(DEFAULT_QUERY_URL);
	GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(DEFAULT_VIDEO_DIR);

	return TRUE;
}

LRESULT CDlgOption::OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return TRUE;
}

LRESULT CDlgOption::OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}
