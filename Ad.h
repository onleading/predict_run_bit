#ifndef AD_H_
#define AD_H_

#define AD_MAXNUM		20
#define AD_MAXNAME		64
#define AD_MAXCONTENT	1000
#define AD_MAXURL		200

typedef struct tagADINFOSET
{
	char	 m_szADName[AD_MAXNAME];
	char	 m_szADCont[AD_MAXCONTENT];
	char	 m_szADUrl[AD_MAXURL];
	
	COLORREF m_rgbBack1;
	COLORREF m_rgbBack2;
	COLORREF m_rgbTxtBk;
	COLORREF m_rgbFront;

	long	 m_nStaySeconds;	
	long	 m_lExpireDate;

	UINT	 m_nWidth;
	UINT	 m_nHeight;
	UINT	 m_nStyle;
}ADINFOSET;

typedef struct tagADHEAD
{
	short	m_nADNum;
	short	m_nADMinRandPopTime;
	short	m_nADMaxRandPopTime;
	DWORD	m_nADGUID;
	tagADHEAD()
	{
		memset(this,0,sizeof(tagADHEAD));
	}
}ADHEAD;

extern void GetNeedNoteInfo();
extern void GetNeedAdInfo();

extern ADHEAD		g_AdHead;
extern ADINFOSET*	g_pAdList;

#endif
