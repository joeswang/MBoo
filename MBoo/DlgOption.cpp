// DlgOption.cpp : implementation of the CDlgOption class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "DlgOption.h"

LRESULT CDlgOption::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;
    char *pcol = 0;
	int rc;
	char sql[SQL_STMT_MAX_LEN] = {0};
	TCHAR path[2 * MAX_PATH] = {0};

	lstrcat(path, g_MainDirectory);
	lstrcat(path, _T("\\"));
	lstrcat(path, _T(DEFAULT_VIDEO_DB));

	GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(_T(DEFAULT_QUERY_URL));
	GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(_T(DEFAULT_VIDEO_DIR));

	rc = sqlite3_open(CT2A(path), &db);
	if( rc )
	{
		sqlite3_close(db);
		return FALSE;
	}

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_QUERY_URL);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szText(pcol, CP_ACP);
			GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(szText);
		}
	}
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_VIDEO_DIR);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szText(pcol, CP_ACP);
			GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(szText);
		}
	}
	sqlite3_finalize(stmt);

	sqlite3_close(db);

	return TRUE;
}

LRESULT CDlgOption::OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
/*
	sqlite3 *db;
	sqlite3_stmt *stmt = NULL;
    char *pcol = 0;
	int rc;
	char sql[SQL_STMT_MAX_LEN] = {0};
	//TCHAR msg[256] = {0};

	GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(_T(DEFAULT_QUERY_URL));
	GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(_T(DEFAULT_VIDEO_DIR));

	rc = sqlite3_open(DEFAULT_VIDEO_DB, &db);
	if( rc )
	{
		//_stprintf_s(msg, 256, _T("无法打开数据库文件: %s\n"), DEFAULT_VIDEO_DB);
		//MessageBox(msg);
		sqlite3_close(db);
		return FALSE;
	}

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_QUERY_URL);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szText(pcol, CP_ACP);
			GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(szText);
		}
	}
	sqlite3_finalize(stmt);

	sprintf_s(sql, SQL_STMT_MAX_LEN, "SELECT charval FROM config WHERE param = %d", PARAM_VIDEO_DIR);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
		if(SQLITE_ROW == rc)
		{
			pcol = (char*)sqlite3_column_text(stmt,0);
			CA2T szText(pcol, CP_ACP);
			GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(szText);
		}
	}
	sqlite3_finalize(stmt);

	sqlite3_close(db);
*/
	EndDialog(IDOK);
	return TRUE;
}

LRESULT CDlgOption::OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}
