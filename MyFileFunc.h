#if !defined(AFX_MYFILEFUNC_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_)
#define AFX_MYFILEFUNC_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyFileFunc.h : header file
//
#include <vector>
typedef std::vector<CString> PATHLIST;

// #define SUPPORT_ZIP		//֧��Zip�ĵ��Ĳ���,����ʹ�ÿ�ע�͵�����
#define SUPPORT_TEA		//֧��TEA����/�����㷨

#ifndef TDEL
#define TDEL(a) if(a){delete a;a=NULL;}
#endif

/////////////////////////////////////////////////////////////////////////////
//class CMyFileFunc

class CMyFileFunc
{
	// Construction
public:
	CMyFileFunc();
	
	// Attributes
public:
	
	// Operations
public:
	//hasTΪTRUEʱ��֤·��ĩβ��"\\",����֤û��
	void FormatPath(CString &path,BOOL hasT);
	//ȷ��ָ����Ŀ¼����
	BOOL MakeSureDirectoryPathExists(const char * lpPath);
	//�ļ���Ŀ¼�Ƿ����
	BOOL FileExist(const char *file);
	//���ļ��������ļ����õ�·��,��:��"c:\a\b.txt"����"c:\a\"
	CString GetPathName(LPCSTR file);
	//����ѡ��·���Ի���
	void SelectFolder(LPCSTR Caption,CString &path,HWND hParentWnd);
	//ɾ��һ��Ŀ¼,������Ŀ¼�µ������ļ����ļ���
	BOOL DeleteDirectory(LPCSTR DirName);
	//��mainpath�������ļ��ľ���/���·������pathlist��
	void GetAllFilePath(CString mainpath,PATHLIST &pathlist,BOOL bAbsolute=TRUE);

protected:
	void GetAllFilePath2(CString mainpath,PATHLIST &pathlist);


#ifdef SUPPORT_ZIP
public:
	//��ѹzip�ļ��е������ļ���ָ��Ŀ¼
	BOOL CMyFileFunc::UnZipAllFilesToPath(const char *filename,const char *path);
	//��������ļ���zipѹ����
	//pathlistΪ���·����,��ʽ��:"aaa.txt","zip\\bbb.txt"
	//mainpathΪ���·������Ŀ¼,��ʽ��:"c:\\zip\\"
	BOOL ZipFiles(const char *mainpath,PATHLIST pathlist,const char *zipfilename);
#endif

#ifdef SUPPORT_TEA
public:
	//���鴦��
	//�ڴ�����֮��ļ�/����
	BOOL TEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	BOOL TEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);

	//�ֿ鴦��,���������鴦��������
	//�ļ�֮��ļ�/����
	BOOL fTEA_encipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key);
	BOOL fTEA_decipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key);
	//�ڴ�����֮��ļ�/����
	BOOL mTEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	BOOL mTEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	
/**********************************************************
   TEA�����㷨

   Input values: 
			long v[2]    64-bit plaintext block
			long k[4]	128-bit key
			long n		��������
   Output values:
			long v[2]    64-bit ciphertext block 
 **********************************************************/
protected:
	void _TEA_encipher(unsigned long * v, const unsigned long * k ,long n );
	void _TEA_decipher(unsigned long * v, const unsigned long * k ,long n );

#endif
	
	// Implementation
public:
	virtual ~CMyFileFunc();
};

extern CMyFileFunc g_FF;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYFILEFUNC_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_)
