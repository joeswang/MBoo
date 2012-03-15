// DlgWizard.cpp : implementation of the CDlgWizard class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "DlgWizard.h"

LRESULT CDlgWizard::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
//CEdit editDIR;
//	editDIR = GetDlgItem(IDC_OPTION_EDIT_DIR);
	//GetDlgItem(IDC_OPTION_EDIT_URL).SetWindowText(g_configInfo.url);
	GetDlgItem(IDC_WIZARD_EDIT_DIR).SetWindowText(g_configInfo.videodir);
//	editDIR.Set;
	CenterWindow();
	return TRUE;
}

LRESULT CDlgWizard::OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
TCHAR msg[MAX_PATH] = {0};

	GetDlgItem(IDC_WIZARD_EDIT_DIR).GetWindowText(msg, MAX_PATH);
	memset(g_configInfo.videodir, 0, MAX_PATH);
	StringCchCat(g_configInfo.videodir, MAX_PATH, msg);
	EndDialog(IDOK);
	return TRUE;
}

LRESULT CDlgWizard::OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CDlgWizard::OnBtnBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR   szDefaultDir[MAX_PATH] = {0};  
	TCHAR szPath[MAX_PATH] = {0};  
    BROWSEINFO  bf;  

	StringCchCat(szDefaultDir, MAX_PATH, g_configInfo.basedir);

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
			GetDlgItem(IDC_WIZARD_EDIT_DIR).SetWindowText(szPath);
            //MessageBox(szPath);
        }  
    }
	return TRUE;
}
