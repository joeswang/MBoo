// DlgOption.cpp : implementation of the CDlgOption class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "DlgOption.h"

LRESULT CDlgOption::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(g_configInfo.url);
	GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(g_configInfo.videodir);
	return TRUE;
}

LRESULT CDlgOption::OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
sqlite3 *db;
sqlite3_stmt *stmt = NULL;
char *pcol = 0;
int rc;
char sql[SQL_STMT_MAX_LEN] = {0};
char buf[SQL_STMT_MAX_LEN] = {0};
TCHAR msg[MAX_PATH] = {0};

	StringCchCat(msg, MAX_PATH, g_configInfo.maindir);
	StringCchCat(msg, MAX_PATH, _T("\\"));
	StringCchCat(msg, MAX_PATH, _T(DEFAULT_VIDEO_DB));
	rc = sqlite3_open(CT2A(msg), &db);
	if( rc )
	{
		//_stprintf_s(msg, 256, _T("无法打开数据库文件: %s\n"), DEFAULT_VIDEO_DB);
		//MessageBox(msg);
		sqlite3_close(db);
		EndDialog(IDOK);
		return FALSE;
	}

	GetDlgItem(IDC_OPTION_EDIT_URL).GetWindowText(msg, MAX_PATH);
	g_configInfo.url[0] = 0x00;
	StringCchCat(g_configInfo.url, MAX_PATH, msg);
	buf[0] = 0;
	sprintf_s(buf, SQL_STMT_MAX_LEN, "%s", CT2A(msg));
	sprintf_s(sql, SQL_STMT_MAX_LEN, "UPDATE config SET charval ='%s' WHERE param = %d", buf, PARAM_QUERY_URL);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);

	GetDlgItem(IDC_OPTION_EDIT_DIR).GetWindowText(msg, MAX_PATH);
	g_configInfo.videodir[0] = 0x00;
	StringCchCat(g_configInfo.videodir, MAX_PATH, msg);
	buf[0] = 0;
	sprintf_s(buf, SQL_STMT_MAX_LEN, "%s", CT2A(msg));
	sprintf_s(sql, SQL_STMT_MAX_LEN, "UPDATE config SET charval ='%s' WHERE param = %d", buf, PARAM_VIDEO_DIR);
	if(SQLITE_OK == sqlite3_prepare_v2(db, sql, -1, &stmt,NULL))
	{
		rc = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);

	sqlite3_close(db);

	EndDialog(IDOK);
	return TRUE;
}

LRESULT CDlgOption::OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CDlgOption::OnBtnBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR   szDefaultDir[MAX_PATH] = {0};  
	TCHAR szPath[MAX_PATH] = {0};  
    BROWSEINFO  bf;  

	StringCchCat(szDefaultDir, MAX_PATH, g_configInfo.maindir);

	ZeroMemory(&bf,sizeof(BROWSEINFO));  
    bf.lParam=(LPARAM)szDefaultDir;  
    bf.lpfn=NULL;  
    bf.lpszTitle=_T("请选择视频存放的文件夹");  
    bf.ulFlags=BIF_STATUSTEXT| BIF_USENEWUI | BIF_RETURNONLYFSDIRS;  
  
    LPITEMIDLIST lpitem=SHBrowseForFolder(&bf);  
    if (lpitem!=NULL)  
    {  
        if(SHGetPathFromIDList(lpitem,szPath) == TRUE)  
        {  
			GetDlgItem(IDC_OPTION_EDIT_DIR).SetWindowText(szPath);
            //MessageBox(szPath);
        }  
    }
	return TRUE;
}
