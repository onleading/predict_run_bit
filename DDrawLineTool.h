#ifndef __DDRAW__DRAWLINETOOL__H__
#define	__DDRAW__DRAWLINETOOL__H__

//使用方法:
//本类维护画线存盘信息,并且在定位点和画图区确定的情况下绘制指定类型的图形/文字
//判断选中时,将画线信息读出,按照区块的实际情况进行定位点坐标生成,然后送入判断
//画线时,将画线信息读出,按照区块的实际情况进行定位点坐标生成,然后送入画线
//正在调整的线和未调整的线在横坐标上有差别,未调整的线完全定位在K线中轴上,正在调整的线以定位点坐标为准
//画线或者修改时,应该将画线信息在类外维护,并实时调整.
//画新线时,直接在外处理完,然后添加保存,不在类中处理,此类仅作为一个维护和表现类,不管控制,只是一个画线的对象,控制由外部解决.
//本类非线程安全,只能在单线程中使用.

//几个前提:
//1.画线为单线程操作
//2.每一条画线从被加入序列开始,序列中的位置不会被改变,但是会有标志变化,通过lLineNo反映,所以可以直接引出指针
class DDrawLineTool
{
protected:
	CString		m_FilePath;
	BOOL		m_bLoaded;
	//
	LPDRAWLINE	m_aDrawLine;				//画线信息缓存
	int			m_nDrawLine;				//画线个数
	int			m_nDrawSpace;				//画线信息缓存大小
	//
	CFont		m_DrawTextFont;				//描述文字
	//
	BOOL		m_bShowSelfLine;			//是否显示画线
	//
	long		m_xPos[MAX_LINEPOINTNUM*6];	//辅助定位点坐标
	long		m_yPos[MAX_LINEPOINTNUM*6];	//辅助定位点坐标
public:
	DDrawLineTool();
	~DDrawLineTool();
public:
	//载入存盘的画线信息(这是必须调用的)
	void LoadLine();
	//保存存盘的画线信息
	void SaveLine();
	//检查画线信息缓存空间
	BOOL CheckSpace(long lReqSpc);
public:
	//添加一条新画线
	BOOL AddLine(LPDRAWLINE pDrawLine);
	//删除一条画线,lineno小于0时,全部删除;在编号范围内,删除指定;否则不删除
	void DeleteLine(long lineno = -1);
	//存盘前清除已经删除的画线
	void CleanDel();
	//退出前删除分时画线
	void CleanZstLine();
	//取得画线条数
	long GetLineNum();
	//取得全部画线信息(参数为空指针,返回画线个数和画线信息指针)
	long GetAllLine(LPDRAWLINE &pDrawLine);
	//重新设置全部画线
	BOOL SetAllLine(LPDRAWLINE pDrawLine,long lLineNum);
public:
	//判断是否选中(参数1为该线的原始信息,参数2为定位点在背景中的位置,参数3为选中点,参数4为画线区,返回值为选择结果,0-5:定位点号,10:非定位点,20:未选中)
	long CheckSel(LPDRAWLINE pDrawLine,CPoint *linepoint,long pointnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist=NULL,long lAssist=0);
	//画指定线(参数2为该线的原始信息,参数3为定位点在背景中的位置,参数4为固定点个数(新增画线有效),参数5为画线区,参数6为画线方式,覆盖时颜色为反色)
	//如果是画新线,没有变动的部分不需要重画,变动的部分反色填充老的,画新的
	void DrawOne(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist=NULL,long lAssist=0);
public:
	//获取显隐标志
	BOOL GetShowFlag();
	//设置显隐标志
	void SetShowFlag(BOOL bShowSelfLine);
	//添加新线时,鼠标有效动作的最大计数次数
	long GetMaxSetPtNum(DWORD dwway);
protected:
	//直线
	BOOL CalcBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawBeeLine(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//线段
	BOOL CalcSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSegment(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//射线
	BOOL CalcRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//箭头
	BOOL CalcArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawArrow(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//价格通道
	BOOL CalcPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawPriceBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//管道线
	BOOL CalcParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawParallel(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//矩形
	BOOL CalcRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//圆弧线
	BOOL CalcArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawArc(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//价格黄金分割
	BOOL CalcGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGold(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//价格百分比
	BOOL CalcPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawPercent(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//波段(价格等分)
	BOOL CalcSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSameSect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//黄金价格线
	BOOL CalcGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGoldPrice(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//黄金目标线
	BOOL CalcGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGoldAim(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//周期线(时间等分)
	BOOL CalcSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSameSpan(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//时间斐波那契
	BOOL CalcFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawFibonacci(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//甘氏时间
	BOOL CalcGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGannTime(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//线性回归带
	BOOL CalcLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinearBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//线性回归延长带
	BOOL CalcLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinearRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//线性回归线
	BOOL CalcLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinear(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//阻速线
	BOOL CalcHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawHoldFast(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//角度线
	BOOL CalcAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawAngle(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//上升箭头
	BOOL CalcRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRise(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//下跌箭头
	BOOL CalcDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawDive(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//文本
	BOOL CalcText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawText(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
protected:
	BOOL ExtendLine(long &xPos1,long &yPos1,long &xPos2,long &yPos2,long nExtendType,CRect rc,long lLeft,long lTop);
	//
	void DrawBar(CDC *pDC,long xp,long yp,CRect rc);
};

#endif
