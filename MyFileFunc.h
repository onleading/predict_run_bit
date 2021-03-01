#if !defined(AFX_MYFILEFUNC_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_)
#define AFX_MYFILEFUNC_H__A03B588E_47C1_4C62_958C_6D83630F3F64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyFileFunc.h : header file
//
#include <vector>
typedef std::vector<CString> PATHLIST;

// #define SUPPORT_ZIP		//支持Zip文档的操作,若不使用可注释掉此行
#define SUPPORT_TEA		//支持TEA加密/解密算法

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
	//hasT为TRUE时保证路径末尾有"\\",否则保证没有
	void FormatPath(CString &path,BOOL hasT);
	//确保指定的目录存在
	BOOL MakeSureDirectoryPathExists(const char * lpPath);
	//文件或目录是否存在
	BOOL FileExist(const char *file);
	//由文件的完整文件名得到路径,如:由"c:\a\b.txt"返回"c:\a\"
	CString GetPathName(LPCSTR file);
	//弹出选择路径对话框
	void SelectFolder(LPCSTR Caption,CString &path,HWND hParentWnd);
	//删除一个目录,包括该目录下的所有文件和文件夹
	BOOL DeleteDirectory(LPCSTR DirName);
	//将mainpath下所有文件的绝对/相对路径存入pathlist中
	void GetAllFilePath(CString mainpath,PATHLIST &pathlist,BOOL bAbsolute=TRUE);

protected:
	void GetAllFilePath2(CString mainpath,PATHLIST &pathlist);


#ifdef SUPPORT_ZIP
public:
	//解压zip文件中的所有文件到指定目录
	BOOL CMyFileFunc::UnZipAllFilesToPath(const char *filename,const char *path);
	//创建多个文件的zip压缩包
	//pathlist为相对路径集,格式如:"aaa.txt","zip\\bbb.txt"
	//mainpath为相对路径的主目录,格式如:"c:\\zip\\"
	BOOL ZipFiles(const char *mainpath,PATHLIST pathlist,const char *zipfilename);
#endif

#ifdef SUPPORT_TEA
public:
	//整块处理
	//内存数据之间的加/解密
	BOOL TEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	BOOL TEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);

	//分块处理,不能于整块处理函数混用
	//文件之间的加/解密
	BOOL fTEA_encipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key);
	BOOL fTEA_decipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key);
	//内存数据之间的加/解密
	BOOL mTEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	BOOL mTEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k);
	
/**********************************************************
   TEA加密算法

   Input values: 
			long v[2]    64-bit plaintext block
			long k[4]	128-bit key
			long n		加密轮数
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
