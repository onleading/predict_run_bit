#ifndef __DDRAWLINESTRUCT__H__
#define __DDRAWLINESTRUCT__H__

//
#define DRAW_PI					3.1415926535898
//
#define MAX_LINEPOINTNUM		5				//自画线最大定位点数
#define BARSIZE_SELFLINE		3				//选中时,自画线选中块区域半边长
#define SELECT_ON				5				//选中范围
#define SPACE_ICONOFFSET		16				//图标必要偏移
//内存管理
#define SELFLINEMAX		30000
#define	SELFLINESTEP	200
//
#define CHECKSELFLINESPACE(bRet,pt,tmppt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		(tmppt) = new type[(req)+(step)];\
		if((tmppt)==NULL) (bRet) = FALSE;\
		else\
		{\
			memset((tmppt),0,((req)+(step))*sizeof(type));\
			if((spc)>0 && (pt)!=NULL) memcpy((tmppt),(pt),(spc)*sizeof(type));\
			TDELA(pt);\
			(pt) = (tmppt);\
			(spc) = (req)+(step);\
		}\
	}\
}
//走势图指标(注意,因创始人的原因,绑定指标的画线,指标简称不能超过9个字节)
#define	ZST_STANDK				"ZST_PX"		//价格
#define	ZST_STANDV				"ZST_VOL"		//成交量
#define	ZST_STANDLB				"ZST_LB"		//量比
#define	ZST_STANDLD				"ZST_MMLD"		//买卖力道
#define	ZST_STANDTO				"ZST_TBSO"		//总买总卖
#define	ZST_STANDTOC			"ZST_TOC"		//净挂净撤
#define	ZST_STANDCS				"ZST_CS"		//撤单累计
#define	ZST_STANDTN				"ZST_TN"		//成交笔数
#define	ZST_STANDLT				"ZST_LTN"		//大单成交笔数
#define	ZST_STANDIO				"ZST_IOV"		//内外盘统计
#define	ZST_STANDLV				"ZST_LVOL"		//大单成交总量
//
#define	ZST_PARTI				1
#define	ZST_PARTII				2
#define	ZST_PARTIII				3
//画线周期
#define LINEPERIOD_MIN5			0				//5分钟数据
#define LINEPERIOD_MIN15		1				//15分钟数据
#define LINEPERIOD_MIN30		2				//30分钟数据
#define LINEPERIOD_HOUR			3				//1小时数据
#define LINEPERIOD_DAY			4				//日线数据
#define LINEPERIOD_WEEK			5				//周线数据
#define LINEPERIOD_MONTH		6				//月线数据
/////////////////////
#define LINEPERIOD_MIN1			7				//1分钟
#define LINEPERIOD_MINN			8				//多分钟
#define LINEPERIOD_DAYN			9				//多日线
#define LINEPERIOD_SEASON		10				//季线
#define LINEPERIOD_YEAR			11				//年线
/////////////////////
#define	LINEPERIOD_ZST			100				//分时走势图
//线延长类型
#define LINEEXTENDTYPE_NOEXTEND	0				//不延长
#define LINEEXTENDTYPE_ONE		1				//延长一端
#define LINEEXTENDTYPE_BOTH		2				//双向延长
//自画线绘制类型
#define LINESTAGE_NORMAL		0				//普通
#define LINESTAGE_DWCOVER		1				//覆盖移动痕迹
#define LINESTAGE_MVCOVER		2				//移动覆盖痕迹
#define LINESTAGE_MOVING		3				//当前移动最新状态
#define LINESTAGE_ORIGIN		4				//移动画线时的原始画线
//选中情况
#define LINESEL_SELNOTPOINT		10
#define	LINESEL_SELNONE			20
//新增画线的标志
#define	NEWLINE_ID				-10
#define	SELNONE_ID				-1

#define	INITLINEPROPERTY()	{\
		switch(pDrawLine->lversion)\
		{\
		case 1:\
			nStyle = pDrawLine->lLineType;\
			nColor = pDrawLine->lcrf;\
			nWidth = pDrawLine->lLineWidth;\
			break;\
		default:\
			break;\
		}\
		if(bCover) nDrawMode = R2_XORPEN;\
	}

//画价位分隔线
#define HORIZON(nPointNum) {\
		for(int i=0;i<nPointNum;i++)\
		{\
			long nTmpYPos = long((m_yPos[1]-m_yPos[0])*fRatio[i]+m_yPos[0]+0.5);\
			g_d.ClipLine(pDC,rc.left,nTmpYPos,rc.right,nTmpYPos,rc,nColor,nDrawMode,nDotLine[i]?PS_DOT:PS_SOLID,nWidth);\
			if(!bCover && nTmpYPos>rc.top+20 && nTmpYPos<rc.bottom)\
				g_d.DisplayText(pDC,rc.left,nTmpYPos-14,nColor,0,szInfo[i]);\
			if(bSel)\
			{\
				g_d.Bar(pDC,m_xPos[0]-BARSIZE_SELFLINE,m_yPos[0]-BARSIZE_SELFLINE,m_xPos[0]+BARSIZE_SELFLINE,m_yPos[0]+BARSIZE_SELFLINE,LEVEL_BRUSH);\
				g_d.Bar(pDC,m_xPos[1]-BARSIZE_SELFLINE,m_yPos[1]-BARSIZE_SELFLINE,m_xPos[1]+BARSIZE_SELFLINE,m_yPos[1]+BARSIZE_SELFLINE,LEVEL_BRUSH);\
			}\
		}\
	}

//选中价位分隔线
#define SELHORIZON(nPointNum,fRatio) {\
		if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)\
			return 0;\
		else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)\
			return 1;\
		else\
		{\
			long nTmpYPos = 0;\
			for(int i=0;i<nPointNum;i++)\
			{\
				nTmpYPos = long((m_yPos[1]-m_yPos[0])*fRatio[i]+m_yPos[0]+0.5);\
				if(abs(point.y-nTmpYPos)<SELECT_ON) return LINESEL_SELNOTPOINT;\
			}\
		}\
	}

#endif