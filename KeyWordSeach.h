  // KeyWordSeach.h: interface for the KeyWordSeach class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYWORDSEACH_H__D0979340_1D31_4B48_B199_CBD9B7F0B3FB__INCLUDED_)
#define AFX_KEYWORDSEACH_H__D0979340_1D31_4B48_B199_CBD9B7F0B3FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_CODE_SIZE	21

enum
{
	STK_NUMCODE_KEY = 0x0001,
	STK_NAME_KEY = 0x0002,
	STK_CHARCODE_KEY = 0x0004,
};
//////////////////////////////////////////////////////////////////////////

struct KeyCodePack
{
	BYTE SetCode;
	short CodeLen;
	char Code[MAX_CODE_SIZE];
};

struct ContentPack
{
	long nIDIndex;
	BYTE Reserved;
};

struct KeyCodeCmp
{
	BOOL operator()(const KeyCodePack &c1, const KeyCodePack &c2) const
	{
		if(c1.SetCode!=c2.SetCode)
			return c1.SetCode < c2.SetCode ;
		if(c1.CodeLen!=c1.CodeLen)
			return c1.CodeLen < c2.CodeLen ;
		return strcmp(c1.Code, c2.Code) < 0;
	}
};

class KeyWordSeach  
{
public:
	KeyWordSeach();
	virtual ~KeyWordSeach();

	void	RegiseterStk(short nIDIndex, BOOL bNumCode=TRUE, BOOL bRegName=FALSE);
	char*	SearchKeyWord(DWORD nSearchType, char *pStart, char *pEnd);
	void	InitResult(void);

public:
	vector<ContentPack> m_KeyWordOK;

private:
	map<KeyCodePack, ContentPack, KeyCodeCmp> m_KeyWordSearchMap;
	vector<short> m_KeyCodeLen;
};

#endif // !defined(AFX_KEYWORDSEACH_H__D0979340_1D31_4B48_B199_CBD9B7F0B3FB__INCLUDED_)
