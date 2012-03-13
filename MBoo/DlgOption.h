// DlgOption.h : interface of the CDlgOption class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CDlgOption : public CDialogImpl<CDlgOption>
{
public:
    enum { IDD = IDD_DLG_OPTION };

public: 

    BEGIN_MSG_MAP(CDlgOption)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_OPTION_BTN_BROWSE, OnBtnBrowse)
		COMMAND_ID_HANDLER(IDOK, OnBtnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnBtnCancel)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
