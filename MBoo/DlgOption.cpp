// DlgOption.cpp : implementation of the CDlgOption class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "DlgOption.h"

LRESULT CDlgOption::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
/*
	sqlite3 *db;
	sqlite3_stmt stmt = NULL;
    char *zErrMsg = 0;
	int rc;
	
	TCHAR msg[256] = {0};

	rc = sqlite3_open(DEFAULT_VIDEO_DB, &db);
	if( rc )
	{
		_stprintf_s(msg, 256, _T("无法打开数据库文件: %s\n"), DEFAULT_VIDEO_DB);
		MessageBox(msg);
		sqlite3_close(db);
		GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(DEFAULT_QUERY_URL);
		GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(DEFAULT_VIDEO_DIR);
		return FALSE;
	}

	sqlite3_prepare
	rc = sqlite3_exec(db, "insert into t values(8)", callback, 0, &zErrMsg);
	if(SQLITE_OK != rc)
	{
		MessageBox(_T("INSERT err!"));
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	*/
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
