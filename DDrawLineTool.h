#ifndef __DDRAW__DRAWLINETOOL__H__
#define	__DDRAW__DRAWLINETOOL__H__

//ʹ�÷���:
//����ά�����ߴ�����Ϣ,�����ڶ�λ��ͻ�ͼ��ȷ��������»���ָ�����͵�ͼ��/����
//�ж�ѡ��ʱ,��������Ϣ����,���������ʵ��������ж�λ����������,Ȼ�������ж�
//����ʱ,��������Ϣ����,���������ʵ��������ж�λ����������,Ȼ�����뻭��
//���ڵ������ߺ�δ���������ں��������в��,δ����������ȫ��λ��K��������,���ڵ��������Զ�λ������Ϊ׼
//���߻����޸�ʱ,Ӧ�ý�������Ϣ������ά��,��ʵʱ����.
//������ʱ,ֱ�����⴦����,Ȼ����ӱ���,�������д���,�������Ϊһ��ά���ͱ�����,���ܿ���,ֻ��һ�����ߵĶ���,�������ⲿ���.
//������̰߳�ȫ,ֻ���ڵ��߳���ʹ��.

//����ǰ��:
//1.����Ϊ���̲߳���
//2.ÿһ�����ߴӱ��������п�ʼ,�����е�λ�ò��ᱻ�ı�,���ǻ��б�־�仯,ͨ��lLineNo��ӳ,���Կ���ֱ������ָ��
class DDrawLineTool
{
protected:
	CString		m_FilePath;
	BOOL		m_bLoaded;
	//
	LPDRAWLINE	m_aDrawLine;				//������Ϣ����
	int			m_nDrawLine;				//���߸���
	int			m_nDrawSpace;				//������Ϣ�����С
	//
	CFont		m_DrawTextFont;				//��������
	//
	BOOL		m_bShowSelfLine;			//�Ƿ���ʾ����
	//
	long		m_xPos[MAX_LINEPOINTNUM*6];	//������λ������
	long		m_yPos[MAX_LINEPOINTNUM*6];	//������λ������
public:
	DDrawLineTool();
	~DDrawLineTool();
public:
	//������̵Ļ�����Ϣ(���Ǳ�����õ�)
	void LoadLine();
	//������̵Ļ�����Ϣ
	void SaveLine();
	//��黭����Ϣ����ռ�
	BOOL CheckSpace(long lReqSpc);
public:
	//���һ���»���
	BOOL AddLine(LPDRAWLINE pDrawLine);
	//ɾ��һ������,linenoС��0ʱ,ȫ��ɾ��;�ڱ�ŷ�Χ��,ɾ��ָ��;����ɾ��
	void DeleteLine(long lineno = -1);
	//����ǰ����Ѿ�ɾ���Ļ���
	void CleanDel();
	//�˳�ǰɾ����ʱ����
	void CleanZstLine();
	//ȡ�û�������
	long GetLineNum();
	//ȡ��ȫ��������Ϣ(����Ϊ��ָ��,���ػ��߸����ͻ�����Ϣָ��)
	long GetAllLine(LPDRAWLINE &pDrawLine);
	//��������ȫ������
	BOOL SetAllLine(LPDRAWLINE pDrawLine,long lLineNum);
public:
	//�ж��Ƿ�ѡ��(����1Ϊ���ߵ�ԭʼ��Ϣ,����2Ϊ��λ���ڱ����е�λ��,����3Ϊѡ�е�,����4Ϊ������,����ֵΪѡ����,0-5:��λ���,10:�Ƕ�λ��,20:δѡ��)
	long CheckSel(LPDRAWLINE pDrawLine,CPoint *linepoint,long pointnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist=NULL,long lAssist=0);
	//��ָ����(����2Ϊ���ߵ�ԭʼ��Ϣ,����3Ϊ��λ���ڱ����е�λ��,����4Ϊ�̶������(����������Ч),����5Ϊ������,����6Ϊ���߷�ʽ,����ʱ��ɫΪ��ɫ)
	//����ǻ�����,û�б䶯�Ĳ��ֲ���Ҫ�ػ�,�䶯�Ĳ��ַ�ɫ����ϵ�,���µ�
	void DrawOne(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist=NULL,long lAssist=0);
public:
	//��ȡ������־
	BOOL GetShowFlag();
	//����������־
	void SetShowFlag(BOOL bShowSelfLine);
	//�������ʱ,�����Ч����������������
	long GetMaxSetPtNum(DWORD dwway);
protected:
	//ֱ��
	BOOL CalcBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawBeeLine(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�߶�
	BOOL CalcSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSegment(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//����
	BOOL CalcRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//��ͷ
	BOOL CalcArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawArrow(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�۸�ͨ��
	BOOL CalcPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawPriceBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�ܵ���
	BOOL CalcParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawParallel(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//����
	BOOL CalcRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//Բ����
	BOOL CalcArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawArc(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�۸�ƽ�ָ�
	BOOL CalcGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGold(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�۸�ٷֱ�
	BOOL CalcPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawPercent(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//����(�۸�ȷ�)
	BOOL CalcSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSameSect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�ƽ�۸���
	BOOL CalcGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGoldPrice(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�ƽ�Ŀ����
	BOOL CalcGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGoldAim(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//������(ʱ��ȷ�)
	BOOL CalcSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawSameSpan(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//ʱ��쳲�����
	BOOL CalcFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawFibonacci(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//����ʱ��
	BOOL CalcGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawGannTime(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//���Իع��
	BOOL CalcLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinearBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//���Իع��ӳ���
	BOOL CalcLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinearRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//���Իع���
	BOOL CalcLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawLinear(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//������
	BOOL CalcHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawHoldFast(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�Ƕ���
	BOOL CalcAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawAngle(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//������ͷ
	BOOL CalcRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawRise(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�µ���ͷ
	BOOL CalcDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawDive(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	//�ı�
	BOOL CalcText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
	BOOL DrawText(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist);
	long SelText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist);
protected:
	BOOL ExtendLine(long &xPos1,long &yPos1,long &xPos2,long &yPos2,long nExtendType,CRect rc,long lLeft,long lTop);
	//
	void DrawBar(CDC *pDC,long xp,long yp,CRect rc);
};

#endif
