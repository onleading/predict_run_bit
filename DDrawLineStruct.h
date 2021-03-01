#ifndef __DDRAWLINESTRUCT__H__
#define __DDRAWLINESTRUCT__H__

//
#define DRAW_PI					3.1415926535898
//
#define MAX_LINEPOINTNUM		5				//�Ի������λ����
#define BARSIZE_SELFLINE		3				//ѡ��ʱ,�Ի���ѡ�п������߳�
#define SELECT_ON				5				//ѡ�з�Χ
#define SPACE_ICONOFFSET		16				//ͼ���Ҫƫ��
//�ڴ����
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
//����ͼָ��(ע��,��ʼ�˵�ԭ��,��ָ��Ļ���,ָ���Ʋ��ܳ���9���ֽ�)
#define	ZST_STANDK				"ZST_PX"		//�۸�
#define	ZST_STANDV				"ZST_VOL"		//�ɽ���
#define	ZST_STANDLB				"ZST_LB"		//����
#define	ZST_STANDLD				"ZST_MMLD"		//��������
#define	ZST_STANDTO				"ZST_TBSO"		//��������
#define	ZST_STANDTOC			"ZST_TOC"		//���Ҿ���
#define	ZST_STANDCS				"ZST_CS"		//�����ۼ�
#define	ZST_STANDTN				"ZST_TN"		//�ɽ�����
#define	ZST_STANDLT				"ZST_LTN"		//�󵥳ɽ�����
#define	ZST_STANDIO				"ZST_IOV"		//������ͳ��
#define	ZST_STANDLV				"ZST_LVOL"		//�󵥳ɽ�����
//
#define	ZST_PARTI				1
#define	ZST_PARTII				2
#define	ZST_PARTIII				3
//��������
#define LINEPERIOD_MIN5			0				//5��������
#define LINEPERIOD_MIN15		1				//15��������
#define LINEPERIOD_MIN30		2				//30��������
#define LINEPERIOD_HOUR			3				//1Сʱ����
#define LINEPERIOD_DAY			4				//��������
#define LINEPERIOD_WEEK			5				//��������
#define LINEPERIOD_MONTH		6				//��������
/////////////////////
#define LINEPERIOD_MIN1			7				//1����
#define LINEPERIOD_MINN			8				//�����
#define LINEPERIOD_DAYN			9				//������
#define LINEPERIOD_SEASON		10				//����
#define LINEPERIOD_YEAR			11				//����
/////////////////////
#define	LINEPERIOD_ZST			100				//��ʱ����ͼ
//���ӳ�����
#define LINEEXTENDTYPE_NOEXTEND	0				//���ӳ�
#define LINEEXTENDTYPE_ONE		1				//�ӳ�һ��
#define LINEEXTENDTYPE_BOTH		2				//˫���ӳ�
//�Ի��߻�������
#define LINESTAGE_NORMAL		0				//��ͨ
#define LINESTAGE_DWCOVER		1				//�����ƶ��ۼ�
#define LINESTAGE_MVCOVER		2				//�ƶ����Ǻۼ�
#define LINESTAGE_MOVING		3				//��ǰ�ƶ�����״̬
#define LINESTAGE_ORIGIN		4				//�ƶ�����ʱ��ԭʼ����
//ѡ�����
#define LINESEL_SELNOTPOINT		10
#define	LINESEL_SELNONE			20
//�������ߵı�־
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

//����λ�ָ���
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

//ѡ�м�λ�ָ���
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