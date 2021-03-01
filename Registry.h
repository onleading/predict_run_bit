#ifndef __REGISTRY__H__
#define __REGISTRY__H__

typedef struct
{	char	strBlockName[9];
	short	nStockNum;
	short   nContent[USER_SIZE];
}MEM_BLOCK;

typedef struct
{	char    strBlockName[9];
	short   nStockNum;
	short	BlockAttr;
	char    strContent[USER_SIZE][7];
}DSK_BLOCK;

//////////////////////////////////////////

#define FREE			-1
#define USED			0
#define EMPTY			-1

#define RT_KEY			0
#define RT_VALUE		64
#define RT_LIMIT		128
#define RT_ANY          256

#define RT_CHAR         1
#define RT_CARRAY		2
#define RT_CHARSTRING	3
#define RT_SHORT		4
#define RT_INT			5
#define RT_LONG			6
#define RT_USHORT		7
#define RT_UINT			8
#define RT_ULONG		9
#define RT_FLOAT		10
#define RT_DOUBLE		11
#define RT_LDOUBLE      12
#define RT_PTR          13
#define RT_USRDEF		14

#define MAXNAMELEN      64

typedef struct	tagRegHead
{	char	strVersion[MAXNAMELEN];     //ע���İ汾���
	long    lIndexOffset;				//�������ƫ��		(�ֽ�ƫ��)
	long    lDataOffset;        		//��������ƫ��		(�ֽ�ƫ��)
	long    lIndexSize;         		//�������Ĵ�С		(�ֽ���)
	long    lDataSize;					//�������Ĵ�С		(�ֽ���)
	long    lIndexRecNum;				//����������
}REGHEAD;


//����,ֵ��������
//      ����			����
//						ֵ��
//      ����			���� RT_KEY  +��������(��ΪRT_KEY    ��ֵΪ��)
//						ֵ�� RT_VALUE+��������(��ΪRT_VALUSE ��ֵΪ��)
//      ����            RT_KEY...  ��������
//						RT_VALUE...ֵ������
//      ����			RT_KEY...  �������ݵĳ���
//						RT_VALUE...ֵ�����ݵĳ���
//      ƫ��			RT_KEY...  �������ݵĳ���(���������)
//						RT_VALUE...ֵ�����ݵĳ���(���������)
//		ǰ����ֵ����	��ֵ���ϼ���������//��Ϊ����Ϊ�׽ڵ���ΪEMPTY
//		�μ���ֵ����	��ֵ��μ���������//��Ϊ����Ϊβ�ڵ���ΪEMPTY
//      ��������		����ֵ�����ļ�(��Ϊ����ΪEMPTY)
//		���Ӽ���ֵ����	��ֵ���׼���������

typedef struct	tagRegIndex
{	char    strName[MAXNAMELEN];
	long    lType;
	char*   pData;
	long    lLength;
	long    lOffset;
	long    lPrevIndex;
	long    lNextIndex;
	long    lParentIndex;
	long    lChildIndex;
	long    bUsed;
}REGINDEX;

class CRegistry
{
public:
	CRegistry(char * strVersion);
	virtual	~CRegistry();
	long	OpenReg(char* lpzRegDBPath,short bCreate=FALSE);
	long    SaveReg(char* lpzRegDBPath);
	long    RegWrite(char*lpzBranchPath,long lType=RT_KEY,void*pData=NULL,long lSize=-1);
	long    RegRead(char*lpzBranchPath,void* pData,long nMaxBytes);
	LPSTR	Peekstr(char*lpzBranchPath);
private:
	REGHEAD		m_RegHead;
	REGINDEX	*m_pRegIndex;
	short       m_bRegSafe;
	char        m_strVersion[MAXNAMELEN];
private:
	long    FindBranch(char*lpzBranchPath);
	long    LocateChild(char*lpzChildName,long lParentIndex,long lType);
	long    NewIndex(char*lpzName,long lType,void*pData,long lLength,long lParentIndex);
	long	SetIndexData(char*lpzName,long lType,void*pData,long lLength,long lTargetIndex);
};
#endif