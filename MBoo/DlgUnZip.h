// DlgUnZip.h : interface of the CDlgUnZip class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CDlgUnZip : public CDialogImpl<CDlgUnZip>
{
public:
    enum { IDD = IDD_DLG_UNZIP };

public: 

    BEGIN_MSG_MAP(CDlgUnZip)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PROGRESS_UNZIP_SHOW, OnUpdatePGSUnzip)
		COMMAND_ID_HANDLER(IDC_UNZIP_BTN_UNCOMPRESS, OnBtnUncompress)
		COMMAND_ID_HANDLER(IDOK, OnBtnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnBtnCancel)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdatePGSUnzip(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnUncompress(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
