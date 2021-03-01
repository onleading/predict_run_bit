#if !defined(AFX_BLOCKPAGE_H__722C1226_1F72_11D3_98A5_0088CC032229__INCLUDED_)
#define AFX_BLOCKPAGE_H__722C1226_1F72_11D3_98A5_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlockPage.h : header file

#include "DragListCtrl.h"
#include "Label.h"
class CBlockPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBlockPage)

	void SaveToBlock(int nBlockIndex);
	void LoadFromBlock(int nBlockIndex);

	void StartKeyBuy(BOOL bAuto,char tempchar=0);
	void FillStock();
	void UpdateBlockGPList(int OldIndex,int NewIndex);
	void FillBlock();
	void EnableButton(BOOL bEnable);
	BOOL TestSameBlockName(int blockindex,CString blockname);

	BOOL SwapGpPos(int nNew, int nOld);
	void SwapBlockPos(BOOL bUp);
public:
	CBlockPage();
	~CBlockPage();
	// Dialog Data
	//{{AFX_DATA(CBlockPage)
	enum { IDD = IDD_BLOCK_PAGE };
	CLabel	m_BlockName;
	CDragListCtrl	m_BlockGPList;
	CListCtrl	m_BlockList;
	//}}AFX_DATA

	BOOL		KeyGuyFlag;
	CImageList	m_ImageList1,m_ImageList2;
	CString		BlockGPName[USERBLOCK_NUM+2]; //支持USERBLOCK_NUM个板块股和自选股与股票池
	int			BlockNum;
	int			nCurBlockIndex;

	int			AddedGPIndex[MAXGPNUM];
	int			AddedGPNum;

	BOOL	    bBlockGPModify;
	BOOL		bBlockModify;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBlockPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBlockPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptionRenameblock();
	afx_msg void OnOptionDeleteblock();
	afx_msg void OnOptionNewblock();
	afx_msg void OnItemchangedOptionBlockList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOptionAddgp();
	afx_msg void OnOptionDeletegp();
	afx_msg void OnOptionDeleteall();
	afx_msg void OnExportBlock();
	afx_msg void OnImportBlock();
	afx_msg void OnMoveup();
	afx_msg void OnMovedown();
	afx_msg void OnImport2block();
	afx_msg void OnBlockUp();
	afx_msg void OnBlockDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKPAGE_H__722C1226_1F72_11D3_98A5_0088CC032229__INCLUDED_)
