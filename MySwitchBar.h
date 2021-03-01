/*
*	����,��,��,��,���ַ��
*	ˮƽ���ʱֻ��һ����ť,��ť�����ص�,��ֱ���ʱ��������ť,�μ���ť��������ʾ
*	ˮƽ���Ĭ����߰�ť��ס�ұ߰�ť
*	WM_USER+1001 �� WM_USER+1003 �ε���Ϣid��ʹ��
*	0xfffe,0xffff���ط��ͺ󷭰�ťռ��,��ǩ����ʹ��������ID
*	����ǩ���л�ʱ,�����ڻ��UM_MYSWITCHBAR_CLICK��Ϣ,LPARAM������MySwitchBarMsg�ṹ��ָ��
*
*
*	����/ɾ������:
*	����/ɾ��item��m_vData
*	���¼���page,����Ҫ��ı�m_nCurPage
*	������ʾ�ı�ǩ��Rgn
*	��ʾ/���ط�ҳ��ť,���Ʒ�ҳ��ť��״̬(enable)
*	����m_nCurPage�ƶ����б�ǩ,������ʾ/����
*
*	��������/ɾ������:
*	�ⲿ����SetMyRedraw(FALSE)
*	����/ɾ��item��m_vData
*	�ⲿ����SetMyRedraw(TRUE)
*	��SetMyRedraw(TRUE)�н���
*	���¼���page,����Ҫ��ı�m_nCurPage
*	������ʾ�ı�ǩ��Rgn
*	��ʾ/���ط�ҳ��ť,���Ʒ�ҳ��ť��״̬(enable)
*	����m_nCurPage�ƶ����б�ǩ,������ʾ/����
* 
*	��ҳ����:
*	�ı�m_nCurPage
*	��ʾ/���ط�ҳ��ť,���Ʒ�ҳ��ť��״̬(enable)
*	������ʾ�ı�ǩ��Rgn
*	����m_nCurPage�ƶ����б�ǩ,������ʾ/����
*	
*	�л�����:
*	�ı�m_nIndex_sel
*	1.�����:
*	������ʾ�ı�ǩ��Rgn
*	�������ڷ���Ϣ
*	2.�ⲿ����SetSel:
*	��ԭʼѡ�к͵�ǰ����ѡ�ж�������ʾ��ǩ����
*	nothing
*	����
*	������ʾ�ı�ǩ��Rgn
*	
*	�ߴ�ı�:
*	���¼��㲢�ƶ���ҳ��ť
*	���¼���page,����Ҫ��ı�m_nCurPage
*	��ʾ/���ط�ҳ��ť,���Ʒ�ҳ��ť��״̬(enable)
*	����m_nCurPage�ƶ����б�ǩ,������ʾ/����
*	������ʾ�ı�ǩ��Rgn
*			
*	��ղ���:
*	���m_vData
*	���m_vPage
*	����m_nIndex_sel=-1
*	����m_nCurPage=-1
*	���ط�ҳ��ť
*	
*	ˢ�±���
*	���㵱ǰҳ�б�ǩռ�õ�rgn�Լ���ҳ��ť��rgn,����ˢ��������ȥ��
*
*/

#ifndef _TDX_MYSWITCHBAR_
#define _TDX_MYSWITCHBAR_

#include "MyBitmapBt.h"
#include <vector>

const UINT BT_PAGE_UP	= 0xfffe;
const UINT BT_PAGE_DOWN	= 0xffff;

const UINT UM_MYSWITCHBAR_CLICK = WM_USER+1003;	//����ǩ�л�ʱ���ʹ���Ϣ

typedef MBB_OwnerDraw MSB_OwnerDraw;			//ÿ����ǩ�Ļ����޸ĺ���

typedef struct tagMySwitchBarMsg	//����ǩ������л�ʱ,��Ϣ�ĵڶ��������Ǵ˽ṹ��ָ��
{
	UINT id;				//SwitchBar��ID
	UINT last_sel;			//ǰһ����ѡ�еı�ǩ��������
	UINT last_id;			//ǰһ����ѡ�еı�ǩ��ID
	UINT cur_sel;			//��ǰ��ѡ�еı�ǩ��������
	UINT cur_id;			//��ǰ��ѡ�еı�ǩ��ID
}MySwitchBarMsg,*pMySwitchBarMsg;

enum MSB_BorderState_ToRight		//����ť�����ұ߰�ť���ص�״̬
{	
	MSBBS_OVER=0,			//���ұ߰�ť֮��,��ס�ұ߰�ť
	MSBBS_UNDER				//���ұ߰�ť֮��,���ұ߰�ť��ס
};

typedef struct tagMySwitchBarData
{
	MyBitmapBt *pBt;
	CRect		rect;
	UINT		nID;		//��ťID

	LONG		level;		//0.һ����ǩ 1.������ǩ
	CRgn		rgn;		//��ť���ڱ߽�

	MSB_BorderState_ToRight borderState;	
							//����ť�����ұ߰�ť���ص�״̬,ˮƽ���ʱ��Ч

	void*		pItemData;	//��չ��Ϣ
	
	tagMySwitchBarData()
	{
		level = 0;
		nID	= 0;
		pBt = NULL;
		borderState = MSBBS_OVER;
		pItemData = NULL;
		rect.SetRectEmpty();
	}
	~tagMySwitchBarData()
	{
		if(pBt && pBt->m_hWnd && ::IsWindow(pBt->m_hWnd))
			pBt->DestroyWindow();
		TDEL(pBt);
	}
}MySwitchBarData,*pMySwitchBarData;

typedef std::vector<pMySwitchBarData> vMySwitchBarData;

typedef struct tagMySwitchBarPage	//һ���ɼ�ҳ�Ŀ�ʼ�ͽ�����ǩ��������
{
	UINT	nShowFirst;
	UINT	nShowLast;
	int		nPageSize;	//ˮƽ���ʱ��ʾ���,��ֱ���ʱ��ʾ�߶�
}MySwitchBarPage;

typedef std::vector<MySwitchBarPage> vMySwitchBarPage;

enum MySwitchBarStyle	//���
{
	MSBS_H_T=0,
	MSBS_H_B,
	MSBS_V_L,
	MSBS_V_R
};

enum MSB_EraseBkgndStyle	//�ػ汳���ķ��
{
	MSB_EBS_BITMAP=0,		//��λͼ���
	MSB_EBS_GRADIENT,		//�ý���ɫ
	MSB_EBS_OWNERDRAW,		//ͨ�����������Ի�
};

/////////////////////////////////////////////////////////////////////////////
// CMySwitchBar window

class CMySwitchBar : public CWnd
{
// Construction
public:
	CMySwitchBar();
	CMySwitchBar(UINT id,MySwitchBarStyle style=MSBS_H_T,int marge_left=0,int marge_top=0);
// Attributes
protected:
	vMySwitchBarData m_vData;
	MySwitchBarStyle m_style;		//���

	UINT			m_nID;
	CBitmap *		m_BK[12];		//0-8Ϊ��ǩ��״̬��������,9-11Ϊ��������������

	MyBitmapBt		m_pageBt[2];	//�ط���ť�ͺ󷭰�ť
	MyBitmapBt *	ptest;
	CRect			m_bt_rect[2];
	CRect			m_btsRect;		//����ťһ��ռ������
	vMySwitchBarPage m_vPage;		//��ҳ��Ϣ,ÿ����ԱΪһҳ,û���κα�ǩʱΪ��
	int				m_nCurPage;		//��ǰ��ʾ��ҳ��,-1��ʾ��ǰû���κο���ʾҳ

	MSB_OwnerDraw	m_pfOwnerDraw;	//���˺���ָ�벻Ϊ��,���ڻ���ÿ����ť��������ô˺����Ի�
	MSB_EraseBkgndStyle m_EraseBkgndStyle;	
									//���Ʊ����ķ��
	//���ý���ɫ����ʱ�Ĳ���
	COLORREF		m_startColor,m_endColor,m_cor_bk;
	BOOL			m_bHorizontal;

	//�ı�����
	COLORREF		m_cor_text_n,m_cor_text_d,m_cor_text_o;
	TextStyle		m_text_style_n,m_text_style_d,m_text_style_o;
	int				m_text_height_n,m_text_height_d,m_text_height_o;
	CPoint			m_offset[3];	//�����ڲ�ͬ״̬ʱ��ʾ��ƫ����

	int				m_item_offset;	//��������ť�ص��Ĳ���,��ֱ���ʱĬ��Ϊ1
	int				m_level_offset;	//��ֱ���ʱ,�μ���ǩ������
	int				m_marge_left,m_marge_top;	
									//��Ե���״�С
	int				m_nRedraw;		// <0��ʾ��ˢ��,>=0��ʾˢ��
	BOOL			m_bBatchCalc;	//�Ƿ����ڽ�����������
	int				m_nIndex_sel;	//��ǰѡ��״̬�İ�ť����
	std::vector<UINT>	m_vtButtons;

// Operations
public:
	//ʹ���޲������캯��ʱ,��Ҫ���ô˺�����ʼ��
	void Init(UINT id,MySwitchBarStyle style=MSBS_H_T,int marge_left=0,int marge_top=0);
	//���������ڲ�ͬ״̬�µ���ɫ,ָ��nIndexʱ����һ����ǩ,�����������б�ǩ
	void SetBkColor(COLORREF cor_bk,COLORREF startColor,COLORREF endColor);
	void SetTextColor(COLORREF cor_text_n,COLORREF cor_text_d=0,COLORREF cor_text_o=0,int nIndex=-1);
	//���������ڲ�ͬ״̬�µķ��(����,б���),ָ��nIndexʱ����һ����ǩ,�����������б�ǩ
	void SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o,int nIndex=-1);	
	//���������ڲ�ͬ״̬�µĴ�С,ָ��nIndexʱ����һ����ǩ,�����������б�ǩ
	void SetTextHeight(int height_n,int height_d=-1,int height_o=-1,int nIndex=-1);
	//���ò�ͬ״̬ʱ���ֵ�ƫ��,ָ��nIndexʱ����һ����ǩ,�����������б�ǩ
	void SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o=0,int y_o=0,int nIndex=-1);	

	//�����Ի溯��
	void SetCustomFunc(MSB_OwnerDraw pFunc);

	//��ѡ��״̬����ͨ,ѡ��״̬����ͨ,��껬��,3��״̬,ÿ��״̬��������3��λͼƴ�ӳ���ť����,���3�������屳����������
	void SetBK(long bk[12]);
	void SetBK(LPCTSTR bk[12]);
	void SetBK(CBitmap* bk[12]);
	//���÷�ҳ��ť����,0-2:�ط���ť3״̬,3-5:�󷭰�ť3״̬
	void SetButtonBK(long bk[6]);
	void SetButtonBK(LPCTSTR bk[6]);
	void SetButtonBK(CBitmap* bk[6]);

	//���ñ�ǩ�ص���
	void SetItemOffset(int offset);
	//���ô�ֱ���ʱ�μ���ǩ��������
	void SetLevelOffset(int offset);

	//��ӱ�ǩ,��nIndexǰ�����ǩ,��nIndex==-1,����ĩβ���
	BOOL InsertSwitch(int nIndex,UINT id,LPCSTR text,LPCSTR tooltip="",LONG level=0);
	//ɾ����ǩ,��nIndexΪ-1��id����,������nIndex����
	BOOL DeleteSwitch(int nIndex,UINT id=0);
	//������б�ǩ
	void Clear();

	//����ѡ��״̬,nIndex==-1ʱ��id����,����nIndex
	BOOL SetSel(int nIndex,UINT id=0);
	//����ѡ��״̬
	void GetSel(int &nIndex,UINT &id);
	//���õ�ǰ��ʾҳ��
	BOOL SetPage(UINT nPage);
	//���ص�ǰ��ʾ��ҳ��
	void GetPage(int &nCurPage,UINT &nTotalPage);
	//������Ϣ
	BOOL  SetItemData(int nIndex,UINT id,void *pItemData);
	void* GetItemData(int nIndex,UINT id=0);
	
	//�����Ƿ����¼��㲻������״��ǩ�߽�,���������밴ťʱʹ��,���Ч��
	void SetMyRedraw(BOOL bRedraw);

	//ָ������������ָ����ɫ�ý���ɫ����
	void SetEraseBackGroundParam(COLORREF StartColor,COLORREF EndColor,BOOL bHorizontal);
	//����ΪTRUEʱͨ������������ʵ���Ի�,ΪFALSEʱ��λͼ���(Ĭ�Ϸ�ʽ,���Բ��õ��ô˺���)
	void SetEraseBackGroundParam(BOOL bOwnerDraw);
	//�����ı�Label������ʱ�ķ�Ӧ��Ϊ��ť���
	void ChangeLabelBehavior(UINT nIndex);
	//�ж�һ��Label�ĵ����Ӧ�Ƿ�ť���
	BOOL IsBehaviorAsButton(UINT nIndex);
	
protected:
	MyBitmapBt* _CreateNewBt(UINT id,LPCSTR text,LPCSTR tooltip = "");
	pMySwitchBarData _CreateNewItem(UINT id,LPCSTR text,LPCSTR tooltip = "",LONG level = 0);
	BOOL _InsertSwitch(pMySwitchBarData newItem,int nIndex=-1);
	
	//�����ǩ�߽�
	BOOL _ModifyRgn(CRgn *rgn_l,int offset_l,MSB_BorderState_ToRight bs0,CRgn *rgn_m,MSB_BorderState_ToRight bs1,CRgn *rgn_r,int offset_r);
	BOOL _CalcItemRgn(UINT nIndex,UINT nStart,UINT nEnd,CRgn &rgn);
	void CalcRgn();	//���㵱ǰ��ʾҳ�ĸ���ǩ�߽�

	//����ҳ
	void _CalcPage(UINT nStart,UINT &nEnd,int &pageSize);	//������ʼ����,����һҳ����ʾ�����һ�������ʹ�ҳ�ĳߴ�
	void CalcPage();
	//����m_nCurPage�ƶ����б�ǩ,������ʾ/����
	void MoveAndShow();
	//��ʾ/���ط�ҳ��ť,�ı䷭ҳ��ť״̬
	void ShowPageButtons();

	//ͨ����ťID,���ذ�ť��������,����-1��ʾû���ҵ�
	int _GetIndexFromID(UINT id);
	//ͨ����ť������,���ذ�ťID,����-1��ʾû���ҵ�
	int _GetIDFromIndex(UINT nIndex);

	//�����Ƿ�ˢ��
	void _SetMyRedraw(BOOL bRedraw);
	
	//ˢ�±���ʱ�����ô˺���,�����Ի�����,������һ������,���ش��麯��
	virtual void MyEraseBkgnd(CDC *pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySwitchBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySwitchBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMySwitchBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnClick(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
