// ToolTipMark.cpp: implementation of the CToolTipMark class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolTipMark.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolTipMark::CToolTipMark(CWnd *pWnd)
{
	m_pOwnerWnd = pWnd;
}

CToolTipMark::~CToolTipMark()
{
	m_RegedTip.clear();
}
//////////////////////////////////////////////////////////////////////////
BOOL	CToolTipMark::InitTool()
{
	//tool
	if(!m_pOwnerWnd) return FALSE;
	BOOL bOK = m_ToolTip.Create(m_pOwnerWnd,TTS_ALWAYSTIP);
	if(bOK)
	{
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SetMaxTipWidth(440);
		m_ToolTip.SetStyle(TTS_WITHTITLE);
		m_ToolTip.SetDelayTime(TTDT_INITIAL,500);
		return m_ToolTip.AddTool(m_pOwnerWnd,"");
	}
	return FALSE;
}

BOOL	CToolTipMark::AddTool(CWnd* pWnd, LPCTSTR lpszText, LPCRECT lpRectTool, UINT nIDTool)
{
	return m_ToolTip.AddTool(pWnd, lpszText, lpRectTool, nIDTool);
}

void	CToolTipMark::RelayEvent(LPMSG lpMsg)
{
	m_ToolTip.RelayEvent(lpMsg);
}

void	CToolTipMark::RegTipInfo(CRect rc, MemStkInfo *pStock, CWnd *pWnd)
{
	if(pStock)
	{
		RegedTipInfo rti;
		rti.rc.CopyRect(rc);
		rti.pStock = pStock;
		rti.pWnd = pWnd;
		m_RegedTip.push_back(rti);
	}
}

void	CToolTipMark::ShowTool(CPoint pt, CWnd *pWnd)
{
	static CPoint s_LastToolTipPt=CPoint(0,0);
	if(abs(pt.x-s_LastToolTipPt.x)<5 && abs(pt.y-s_LastToolTipPt.y)<5)
		return;
	s_LastToolTipPt=pt;
	for(int i=0;i<m_RegedTip.size();i++)
	{
		if(m_RegedTip[i].pWnd&&pWnd!=m_RegedTip[i].pWnd) continue;
		CRect rc = m_RegedTip[i].rc;
		if(rc.PtInRect(pt))
		{
			CString lpString="";
			BOOL bHasTip = m_RegedTip[i].pStock&&GetToolText(lpString, m_RegedTip[i].pStock);
			if(bHasTip) 
			{
				m_ToolTip.UpdateTipText(lpString, pWnd);
				m_ToolTip.Update();
				m_ToolTip.Activate(TRUE);
				return;
			}
		}
	}
	m_ToolTip.UpdateTipText("", pWnd);
	m_ToolTip.Update();
	m_ToolTip.Pop();
}

void	CToolTipMark::GetToolTextEx(CString &strRes, CPoint pt, CWnd *pWnd)
{
	for(int i=0;i<m_RegedTip.size();i++)
	{
		if(m_RegedTip[i].pWnd&&pWnd!=m_RegedTip[i].pWnd) continue;
		CRect rc = m_RegedTip[i].rc;
		if(rc.PtInRect(pt))
		{
			BOOL bHasTip = GetToolText(strRes, m_RegedTip[i].pStock);
			if(bHasTip) return;
		}
	}
}

void	CToolTipMark::ClearToolTipInfo(CWnd *pWnd)
{
	vector<RegedTipInfo>::iterator iter = m_RegedTip.begin();
	for(;!m_RegedTip.empty()&&iter!=m_RegedTip.end();)
	{
		RegedTipInfo rti = (*iter);
		if(rti.pWnd==pWnd||!rti.pWnd)
		{
			m_RegedTip.erase(iter);
			if(!m_RegedTip.empty()) 
				iter = m_RegedTip.begin();
			else iter++;
		}
		else iter++;
	}
}