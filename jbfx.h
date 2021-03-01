#ifndef JBFX_H_
#define JBFX_H_

#include "VList.h"

/////////////////////////////////////////////////////////////////////////
//
#define JBFX_MAX_MENUGN		20		//主菜单功能最大数
#define JBFX_MAX_MAINGN		400		//主功能最大数
#define JBFX_MAX_SUBGN		50		//子功能最大数

#define MAX_GUIDE_NAMELEN	100
struct MenuGN_Cfg
{
	int			MenuGNNum;
	char		MenuGNCode[JBFX_MAX_MENUGN];
	char		MenuGNName[JBFX_MAX_MENUGN][31];
	char		MenuGuide[JBFX_MAX_MENUGN][MAX_GUIDE_NAMELEN];
	BOOL		UseIt[JBFX_MAX_MENUGN];
};

struct MainGN_Cfg
{
	int			GNNum;
	char		GNCode[JBFX_MAX_MAINGN][20];
	char		GNName[JBFX_MAX_MAINGN][30];
	char		GNDesc[JBFX_MAX_MAINGN][100];
	char		GNGuide[JBFX_MAX_MAINGN][MAX_GUIDE_NAMELEN];
	COLORREF	SwitchColor[JBFX_MAX_MAINGN];
};

struct GN_Cfg
{
	int					GNNum;
	struct One_GN_Cfg  *GNList;
};

struct One_GN_Cfg
{
	char		GNCode[20];
	char		GNName[30];
	char		GNDesc[100];
	COLORREF	SwitchColor;
	char		Indent;
	char		Type;
};

struct JBFX_Scheme
{
	COLORREF	FrameColor;
	COLORREF	BkColor;
	COLORREF	DataBkColor;
	COLORREF	DataBkColor2;
	COLORREF	DataTextColor;
	COLORREF	ListLineColor;
	COLORREF	SelectedColor;
	COLORREF	HeadTextColor;
	COLORREF	HeadBkColor;
	COLORREF	ScrollBarColor;
	COLORREF	CodeNameColor;
	COLORREF	VolumeColor;
	COLORREF	UpColor;
	COLORREF	DownColor;
	COLORREF	StateColor[5];
	LOGFONT		ListFont;
	LOGFONT		TitleFont;
};

class JBFX
{
public:
	JBFX();
	virtual ~JBFX();

	BOOL	InitJBFX();
	void	UnInitJBFX();
	
	void	GetCwCfg();
	void	EmptyCwCfg();
private:
	BOOL	GetJBScheme();
	void	GetOneGNCfg(struct GN_Cfg *pGN,const char *GNIni);
};


#endif
