// VideoPanel.cpp : implementation of the CVideoPanel class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "define.h"

#include "VideoPanel.h"
#include "VideoWnd.h"

#define PANEL_TIMER_ID	1
int i = 0;
LRESULT CVideoPanel::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CTrackBarCtrl trackVolume;
	CStatic stcTimeFrame;
	::SkinSE_SubclassWindow(m_hWnd, _T("dlg.videopanel"));
	::SkinSE_SetLayoutMainFrame(m_hWnd);

	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STARTPAUSE), _T("panel.btn.play"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STOP), _T("panel.btn.stop"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_SLD_PROGRESS), _T("panel.sld.progress1"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_SLD_AUDIO), _T("panel.sld.audio"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_CHK_AUDIO), _T("panel.chk.audio"));
	::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_STC_TIMEFRAME), _T("panel.stc.timeframe"));

	stcTimeFrame = GetDlgItem(IDC_PANEL_STC_TIMEFRAME);
	::SetWindowLong(stcTimeFrame, GWL_STYLE, stcTimeFrame.GetStyle() | SS_NOTIFY);

	trackVolume = GetDlgItem(IDC_PANEL_SLD_AUDIO);
	trackVolume.SetRangeMin(0);
	trackVolume.SetRangeMax(100);
	trackVolume.SetPos(100);

	SetTimer(PANEL_TIMER_ID, 1000);

	return TRUE;
}

LRESULT CVideoPanel::OnLBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//::SendMessage(GetParent(), uMsg, wParam, lParam);
	//if(NULL != m_pwndParent)
	//	m_pwndParent->OnLBtnDown(uMsg, wParam, lParam, bHandled);
	return 0;
}


LRESULT CVideoPanel::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TCHAR msg[256] = {0};
	int current, total;

	if(PANEL_TIMER_ID != wParam) return 0;

	if(NULL == m_pFlashObject) return 0;
	if(!m_pFlashObject->HavingMovie()) return 0;
	if(1 != m_pFlashObject->IsPlaying()) return 0;

	total = m_pFlashObject->GetTotalFrames();
	if(0 > total) return 0;

	current = m_pFlashObject->GetCurrentFrame();
	if(0 > current) return 0;

	CTrackBarCtrl trackProgress = GetDlgItem(IDC_PANEL_SLD_PROGRESS);
	trackProgress.SetPos(current);


	CStatic stcTimeFrame = GetDlgItem(IDC_PANEL_STC_TIMEFRAME);
	_stprintf_s(msg, 256, _T("%d / %d"), current, total);
	stcTimeFrame.SetWindowText(msg);

	return 0;
}

BOOL CVideoPanel::WindowMove(UINT x, UINT y, UINT cx, UINT cy)
{
	if(NULL == m_hWnd) return FALSE;

	return (::SetWindowPos(m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE));
}


LRESULT CVideoPanel::OnCheckAudio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//MessageBox(_T("Ohooo"));
	/*
	SFPCGetTotalFrames info1;
	SFPCGetFrameNum info2;
	SFPCCurrentFrame info3;

	if(NULL == m_hWndFlash) return FALSE;

	::SendMessage(m_hWndFlash, FPCM_GET_TOTALFRAMES, 0, (LPARAM)&info1);
	if(FAILED(info1.hr))
	{
		// error
		return FALSE;
	}

	::SendMessage(m_hWndFlash, FPCM_GET_FRAMENUM, 0, (LPARAM)&info2);
	if(FAILED(info2.hr))
	{
		// error
		return FALSE;
	}

	::SendMessage(m_hWndFlash, FPCM_CURRENTFRAME, 0, (LPARAM)&info3);
	if(FAILED(info3.hr))
	{
		// error
		return FALSE;
	}
	return TRUE;
	*/
	CButton btnAudio;

	if(NULL == m_pFlashObject) return FALSE;
	btnAudio = GetDlgItem(IDC_PANEL_CHK_AUDIO);
	
	return m_pFlashObject->EnableSound(btnAudio.GetCheck());

	return 0;
}

LRESULT CVideoPanel::OnStartPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	BOOL ret;
	UINT status;
	CButton btnStartPause;
	
	btnStartPause = GetDlgItem(IDC_PANEL_BTN_STARTPAUSE);

	if(NULL == m_pFlashObject) return FALSE;
	if(!m_pFlashObject->HavingMovie()) return FALSE;

	status = m_pFlashObject->IsPlaying(); 
	switch (status) {
		case 0 :   // not playing
			ret = m_pFlashObject->Resume();
			::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STARTPAUSE), _T("panel.btn.pause"));
			//if(ret) btnStartPause.SetWindowText(_T("PAUSE"));
			break;
			//return ret;
		case 1:	  // is playing
			ret =  m_pFlashObject->Pause();
			::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STARTPAUSE), _T("panel.btn.play"));
			//if(ret) btnStartPause.SetWindowText(_T("START"));
			break;
			//return ret;
		default:  // error!
			break;
			//return FALSE;
	}

	return TRUE;
}

LRESULT CVideoPanel::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	//CButton btnStartPause;
	//btnStartPause = GetDlgItem(IDC_PANEL_BTN_STOP);

	if(NULL == m_pFlashObject) return FALSE;
	
	return m_pFlashObject->Stop();
}

LRESULT CVideoPanel::OnTimeFrameMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//MessageBox(_T("Ohooo"));
	return 0;
}

LRESULT CVideoPanel::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	int pos, min, max;
	CTrackBarCtrl trackProgress, trackVolume;

	if(NULL == m_pFlashObject) return FALSE;

	trackProgress = GetDlgItem(IDC_PANEL_SLD_PROGRESS);
	if(trackProgress.m_hWnd == (HWND)lParam)
	{
		pos = trackProgress.GetPos();
		min = trackProgress.GetRangeMin();
		max = trackProgress.GetRangeMax();
		m_pFlashObject->SetProgress(min, max, pos);
	}

	trackVolume = GetDlgItem(IDC_PANEL_SLD_AUDIO);

	if(trackVolume.m_hWnd == (HWND)lParam)
	{
		pos = trackVolume.GetPos();
		min = trackVolume.GetRangeMin();
		max = trackVolume.GetRangeMax();
		m_pFlashObject->SetSoundVolume(min, max, pos);
	}

	return 0;

}

void CVideoPanel::UpdateVideoUI(VIDEOINFO* videoInfo)
{

	CButton btnStartPause;
	CButton btnStop;
	CButton btnAudio;
	CTrackBarCtrl trackProgress;

	btnStartPause = GetDlgItem(IDC_PANEL_BTN_STARTPAUSE);
	btnStop = GetDlgItem(IDC_PANEL_BTN_STOP);
	btnAudio = GetDlgItem(IDC_PANEL_CHK_AUDIO);
	trackProgress = GetDlgItem(IDC_PANEL_SLD_PROGRESS);

	if(NULL == m_pFlashObject)
	{
		btnStartPause.EnableWindow(FALSE);
		btnStop.EnableWindow(FALSE);
		btnAudio.EnableWindow(FALSE);
	}
	else
	{
		if(m_pFlashObject->HavingMovie())
		{
			trackProgress.SetRangeMin(0);
			trackProgress.SetRangeMax(videoInfo->totalframes);
			btnStartPause.EnableWindow(TRUE);
			btnStop.EnableWindow(TRUE);
			btnAudio.EnableWindow(TRUE);
			CheckDlgButton(IDC_PANEL_CHK_AUDIO, TRUE);
			if(1 == m_pFlashObject->IsPlaying())
			{
				::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STARTPAUSE), _T("panel.btn.pause"));
			}
			else
			{
				::SkinSE_SubclassWindow(GetDlgItem(IDC_PANEL_BTN_STARTPAUSE), _T("panel.btn.play"));
			}
			//btnAudio.CheckDlgButton();
		}
		else
		{
			btnStartPause.EnableWindow(FALSE);
			btnStop.EnableWindow(FALSE);
			btnAudio.EnableWindow(FALSE);
		}
	}

}

