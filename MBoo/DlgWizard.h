// DlgWizard.h : interface of the CDlgWizard class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CDlgWizard : public CDialogImpl<CDlgWizard>
{
public:
    enum { IDD = IDD_DLG_WIZARD };

public: 

    BEGIN_MSG_MAP(CDlgWizard)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_WIZARD_BTN_BROWSE, OnBtnBrowse)
		COMMAND_ID_HANDLER(IDOK, OnBtnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnBtnCancel)
    END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
