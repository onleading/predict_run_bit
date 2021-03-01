#ifndef XGSYS_H_
#define XGSYS_H_

//////////////////////////////////////////////////////////////

//����K����̬(Odd K Line)
#define KLINEO_VSMALL			1			//��СK��(��ʮ����)
#define	KLINEO_NSMALL           2           //��ͨСK��
#define KLINEO_SMALL            3           //СK��
#define KLINEO_VBIG				4			//��СK��(��ʮ����)
#define KLINEO_NBIG             8          	//��ͨ��K��
#define KLINEO_BIG              12         	//��K��
#define KLINEO_NORMAL           16          //��K��
#define KLINEO_NOHEADFOOT   	1           //ͺͷ���K��
#define KLINEO_NOHEAD           2           //ͺͷK��
#define KLINEO_NOFOOT           3           //���K��
#define KLINEO_UVLSHADOW    	4           //�ϼ���K��
#define KLINEO_DVLSHADOW        5           //�¼���K��
#define KLINEO_ULSHADOW     	6           //�ϳ�K��
#define KLINEO_DLSHADOW         7           //�³�K��
#define KLINEO_ELSHADOW         8           //�ȳ�K��
#define KLINEO_UP               9           //����
#define KLINEO_DOWN             10          //����
#define KLINEO_LEVEL            11          //ƽ����(ʮ����)

//˫��,����K����̬(Twin K Line)
#define KLINET_ASCENT           1			//��Ԯ��
#define KLINET_SLIDE            2           //������
#define KLINET_JMPUP            3           //������
#define KLINET_JMPDOWN          4           //��ˮ��
#define KLINET_CRACK            5           //ѹ����
#define KLINET_FLATTER          6           //�����
#define KLINET_ABANDON          7           //������
#define KLINET_COVER        	8			//������
#define KLINET_CUTIN        	9           //������
#define KLINET_DRAGUP           10          //������
#define KLINET_DRAGDOWN         11          //������
#define KLINET_SURRENDER        12          //��˳��
#define KLINET_REBELL           13          //������
#define KLINET_FEIGN        	14          //αK��(*)
#define KLINET_FEIGNUP          15          //α����
#define KLINET_FEIGNDOWN        16          //α����
#define KLINET_COORD            17          //������(*)
#define KLINET_COORDUP          18          //��������
#define KLINET_COORDDOWN        19          //��������
#define KLINET_CONTAIN          20          //������(*)
#define KLINET_CUU              21          //������
#define KLINET_CUD              22          //������
#define KLINET_CUL              23          //������
#define KLINET_CDU              24          //������
#define KLINET_CDD              25          //������
#define KLINET_CDL              26          //������
#define KLINET_GRAVID           27          //������(*)
#define KLINET_GUU              28          //������
#define KLINET_GUD              29          //������
#define KLINET_GUL              30          //������
#define KLINET_GDU              31          //������
#define KLINET_GDD              32          //������
#define KLINET_GDL              33          //������
#define KLINET_NIP              34          //�м���(*)
#define KLINET_NDU              35          //������
#define KLINET_NUD              36          //������
#define KLINET_FAKECOVER        37          //�า����
#define KLINET_FAKECUTIN        38          //��������

//K��������̬(Trend K Line)
#define KLINETL_HIGH            1           //�ߵ�(*)
#define KLINETL_LOW             2           //�͵�(*)
#define KLINETL_TOSS            3           //��(*)
#define KLINETL_DULL            4           //����(*)
#define KLINETL_UP              5           //����;��(*)
#define KLINETL_DOWN            6           //�µ�;��(*)
#define KLINETL_HIGHTOSS        7           //�ߵ���(*)
#define KLINETL_LOWTOSS         8           //�͵���(*)
#define KLINETL_HIGHDULL        9           //�ߵ�����(*)
#define KLINETL_LOWDULL         10          //�͵�����(*)
#define KLINETL_FASTUP          11          //����(*)
#define KLINETL_FASTDOWN        12          //����(*)
#define KLINETL_SLOWUP          13          //����(*)
#define KLINETL_SLOWDOWN        14          //����(*)
#define KLINETL_HIGHTOSS1       15          //�ߵ��𵴳���
#define KLINETL_LOWTOSS1        16          //�͵��𵴳���
#define KLINETL_HIGHTOSS2       17          //�ߵ�������
#define KLINETL_LOWTOSS2        18          //�͵�������
#define KLINETL_HIGHTOSS3       19          //�ߵ���ĩ��
#define KLINETL_LOWTOSS3        20          //�͵���ĩ��
#define KLINETL_HIGHDULL1       21          //�ߵ����̳���
#define KLINETL_LOWDULL1        22          //�͵����̳���
#define KLINETL_HIGHDULL2       23          //�ߵ���������
#define KLINETL_LOWDULL2        24          //�͵���������
#define KLINETL_HIGHDULL3       25          //�ߵ�����ĩ��
#define KLINETL_LOWDULL3        26          //�͵�����ĩ��
#define KLINETL_HIGHFASTUP      27          //�ߵ�����
#define KLINETL_HIGHFASTDOWN    28          //�ߵ�����
#define KLINETL_HIGHSLOWUP      29          //�ߵ�����
#define KLINETL_HIGHSLOWDOWN    30          //�ߵ�����
#define KLINETL_LOWFASTUP       31          //�͵�����
#define KLINETL_LOWFASTDOWN     32          //�͵�����
#define KLINETL_LOWSLOWUP       33          //�͵�����
#define KLINETL_LOWSLOWDOWN     34          //�͵�����

#define KLINE_1001  4   //�����ƻ���--�͵�������������       {"�͵�������������",         2, 0,0},
#define KLINE_1002  5   //����������--�͵�������������       {"�͵�������������",         2, 0,0},
#define KLINE_1003  6   //����������--�͵������н�����       {"�͵������н�����",         2, 0,0},
#define KLINE_1004  7   //�����Ĵ���--�͵������й�����       {"�͵������й�����",         2, 0,0},
#define KLINE_1005  8   //����һ����--�͵�����������������   {"�͵�����������������",     2, 0,0},
#define KLINE_1006  9   //������ӿ��--�͵�����������������   {"�͵�����������������",     2, 0,0},
#define KLINE_1007  10   //���������--�͵�����ĩ�ڹ�����     {"�͵�����ĩ�ڹ�����",       3, 0,0},
#define KLINE_1008  11  //����ˮů��--�͵����̸��ڽ�����     {"�͵����̸��ڽ�����",       3, 0,0},
#define KLINE_1009  12  //�����籡��--�͵�����ĩ�ڴ���ͻ��   {"�͵�����ĩ�ڴ���ͻ��",     3, 0,0},
#define KLINE_1010  13  //����������--С��������������     {"С��������������",       3, 0,0},
#define KLINE_1011  14  //������̩��--�͵�����ĩ�ں�����     {"�͵�����ĩ�ں�����",       3, 0,0},

#define KLINE_1012  16  //ǧ�������--�͵�����������         {"�͵�����������",           15,0,0},
#define KLINE_1013  17  //����̽����--��������               {"��������",                 15,0,0},
#define KLINE_1014  18  //����������--�͵�����������ʮ����   {"�͵�����������ʮ����",     15,0,0},
#define KLINE_1015  19  //��ü������--�͵�����������������   {"�͵�����������������",     15,0,0},
#define KLINE_1016  20  //�乭�����--�͵�����������������   {"�͵�����������������",     15,0,0},
#define KLINE_1017  21  //�к�������--�͵�������ͷ����     {"�͵�������ͷ����",       15,0,0},
#define KLINE_1018  22  //�𼦱�����--�͵������������       {"�͵������������",         15,0,0},
#define KLINE_1019  23  //��Ӱ������--�͵������������       {"�͵������������",         15,0,0},
#define KLINE_1020  24  //����������--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1021  25  //����������--�͵����������������� {"�͵�����������������",   15,0,0},
#define KLINE_1022  26  //��ɫ�ƿ���--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1023  27  //���������--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1024  28  //��÷������--�͵�����������ʮ����   {"�͵�����������ʮ����",     15,0,0},
#define KLINE_1025  29  //�ݴ������--�͵����������й�����   {"�͵����������й�����",     15,0,0},
#define KLINE_1026  30  //����ҹ����--�͵����������н�����   {"�͵����������н�����",     15,0,0},
#define KLINE_1027  31  //�����¶��--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1028  32  //��ǵͨ;��--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1029  33  //����Ͷ����--�͵�������������       {"�͵�������������",         15,0,0},
#define KLINE_1030  34  //˫��̽����--�͵�����������������   {"�͵�����������������",     15,0,0},
#define KLINE_1031  35 //÷��������--�͵����ξѻ�������     {"�͵����ξѻ�������",       15,0,0},

#define KLINE_1032  38  //���ϲ�¥��--�ϵ����������ͻ��     {"�ϵ����������ͻ��",       37,0,0},
#define KLINE_1033  39  //һƬ������--����һ���ߺ󲻲�ȱ     {"����һ���ߺ󲻲�ȱ",       37,0,0},
#define KLINE_1034  40  //ǧ��������--���θ߿�����           {"���θ߿�����",             37,0,0},
#define KLINE_1035  41  //�Ƹ�������--������Ԯ��             {"������Ԯ��",               37,0,0},
#define KLINE_1036  42  //��׷�����--����������             {"����������",               37,0,0},
#define KLINE_1037  43  //��η������--������;����ʮ����     {"������;����ʮ����",       37,0,0},
#define KLINE_1038  44  //�̽�Զ����--������;���߽�����     {"������;���߽�����",       37,0,0},
#define KLINE_1039  45  //����������--������;���߹�����     {"������;���߹�����",       37,0,0},
#define KLINE_1040  46  //����������--������;Сѹ����       {"������;Сѹ����",         37,0,0},
#define KLINE_1041  47  //��ݺ�����--������;α����         {"������;α����",           37,0,0},
#define KLINE_1042  48  //��������--������;������         {"������;������",           37,0,0},
#define KLINE_1043  49  //����������--������;��˳��         {"������;��˳��",           37,0,0},
#define KLINE_1044  50  //����������--������;������         {"������;������",           37,0,0},
#define KLINE_1045  51  //�򳪸�����--������;������         {"������;������",           37,0,0},

#define KLINE_1046  53  //��������--������;������         {"������;������",           52,0,0},
#define KLINE_1047  54  //����ǰ����--������;������         {"������;������",           52,0,0},
#define KLINE_1048  55  //��Ŀ�ƿ���--������;������         {"������;������",           52,0,0},
#define KLINE_1049  56  //����Ϸ����--������;������         {"������;������",           52,0,0},
#define KLINE_1050  57  //���ʸ߷���--����������             {"����������",               52,0,0},
#define KLINE_1051  58  //����������--��������               {"��������",                 52,0,0},
#define KLINE_1052  59  //����Ϊ����--������������           {"������������",             52,0,0},
#define KLINE_1053  60  //ƾ��Զ����--��Խ�����߻�ѹ����     {"��Խ�����߻�ѹ����",       52,0,0},
#define KLINE_1054  61  //����������--������; ���������    {"������;�������������",   52,0,0},
#define KLINE_1055  62  //հǰ�˺���--������; ˼��������    {"������;��˼����������",   52,0,0},
#define KLINE_1056  63  //�����ܵ���--������;������         {"������;������",           52,0,0},
#define KLINE_1057  64  //�Ǹ߻�Զ��--������;������         {"������;������",           52,0,0},
#define KLINE_1058  65  //�˱�������--������;α����         {"������;α����",           52,0,0},

#define KLINE_1059  68  //���־�Ⱦ��--�ߵ�������������       {"�ߵ�������������",         67,0,0},
#define KLINE_1060  69  //ɽ��������--�ߵ������������       {"�ߵ������������",         67,0,0},
#define KLINE_1061  70  //����¥̨��--����������             {"����������",               67,0,0},
#define KLINE_1062  71  //���ճ�ͷ��--�ߵ�����������������   {"�ߵ�����������������",     67,0,0},
#define KLINE_1063  72  //��ڣ������--�ߵ������й�����       {"�ߵ������й�����",         67,0,0},
#define KLINE_1064  73  //��¶ʼ����--�ߵ������н�����       {"�ߵ������н�����",         67,0,0},
#define KLINE_1065  74  //����ѹ����--�����߽ӻ�����         {"�����߽ӻ�����",           67,0,0},
#define KLINE_1066  75  //��ѻ������--������ѻ               {"������ѻ",                 67,0,0},

#define KLINE_1067  77  //ǧ��һ����--�ߵ�����ĩ�ڽ�����     {"�ߵ�����ĩ�ڽ�����",       76,0,0},
#define KLINE_1068  78  //����ֱ����--�ߵ�����ĩ�ڴ�������   {"�ߵ�����ĩ�ڴ�������",     76,0,0},
#define KLINE_1069  79  //�缱�����--�ߵ�����ĩ�ں�����     {"�ߵ�����ĩ�ں�����",       76,0,0},
#define KLINE_1070  80  //ɽ�ӱ�ɫ��--С���������������     {"С���������������",       76,0,0},
#define KLINE_1071  81  //�������--������ѻ               {"������ѻ",                 76,0,0},
#define KLINE_1072  82  //������ʤ��--��¥��                 {"��¥��",                   76,0,0},

#define KLINE_1073  84  //������¥��--�ߵ�����������         {"�ߵ�����������",           83,0,0},
#define KLINE_1074  85  //����������--��������               {"��������",                 83,0,0},
#define KLINE_1075  86  //������ϡ��--�ߵ���ʮ����           {"�ߵ���ʮ����",             83,0,0},
#define KLINE_1076  87  //��ʦδ����--�ߵ���������           {"�ߵ���������",             83,0,0},
#define KLINE_1077  88  //̫��������--�ߵ���������           {"�ߵ���������",             83,0,0},
#define KLINE_1078  89  //��վ�����--�ߵ���ͷ����           {"�ߵ���ͷ����",             83,0,0},
#define KLINE_1079  90  //�߲�ʤ����--����������             {"����������",               83,0,0},
#define KLINE_1080  91  //һ����ɽ��--����������             {"����������",               83,0,0},
#define KLINE_1081  92  //˫���Ϲ���--�ߵ����ջ�����         {"�ߵ����ջ�����",           83,0,0},
#define KLINE_1082  93  //͵�컻����--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1083  94  //Ѫɫ�ƻ���--�ߵ�����������       {"�ߵ�����������",         83,0,0},
#define KLINE_1084  95  //�칷ʳ����--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1085  96  //����ϴ����--�ߵ�����������         {"�ߵ�����������",           83,0,0},
#define KLINE_1086  97  //һҶ������--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1087  98  //���ղ�ϼ��--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1088  99  //�����Ъ��--�ߵ�����ʮ����         {"�ߵ�����ʮ����",           83,0,0},
#define KLINE_1089 100  //��׹�����--�ߵ����е�����         {"�ߵ����е�����",           83,0,0},
#define KLINE_1090 101  //�����ľ��--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1091 102  //���������--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1092 103  //Ϧ����ѻ��--�ߵ�������             {"�ߵ�������",               83,0,0},
#define KLINE_1093 104  //����������--�ߵ����ξѻ�������     {"�ߵ����ξѻ�������",       83,0,0},

#define KLINE_1094 108  //Ȫ��Σʯ��--�µ����������ͻ��     {"�µ����������ͻ��",       106,0,0},
#define KLINE_1095 109  //һкǧ����--��ˮһ���ߺ󲻲�ȱ     {"��ˮһ���ߺ󲻲�ȱ",       106,0,0},
#define KLINE_1096 110  //ҹ��Ƶ����--������������           {"������������",             106,0,0},
#define KLINE_1097 111  //һ����ˮ��--���ջ�����             {"���ջ�����",               106,0,0},
#define KLINE_1098 112  //������ɳ��--����������             {"����������",               106,0,0},
#define KLINE_1099 113  //���ո�����--�µ���;��ˮʮ����     {"�µ���;��ˮʮ����",       106,0,0},
#define KLINE_1100 114  //�仨׹¥��--�µ���;��ˮ������     {"�µ���;��ˮ������",       106,0,0},
#define KLINE_1101 115  //��Ϫ������--�µ���;��ˮ������     {"�µ���;��ˮ������",       106,0,0},
#define KLINE_1102 116  //�����׺��--�µ���;С�����       {"�µ���;С�����",         106,0,0},
#define KLINE_1103 117  //�һ���ˮ��--�µ���;α����         {"�µ���;α����",           106,0,0},
#define KLINE_1104 118  //ˮ�������--�µ���;������         {"�µ���;������",           106,0,0},
#define KLINE_1105 119 //�̲�������--�µ���;������         {"�µ���;������",           106,0,0},
#define KLINE_1106 120 //���������--�µ���;������         {"�µ���;������",           106,0,0},
#define KLINE_1107 121 //����������--�µ���;������         {"�µ���;������",           106,0,0},

#define KLINE_1108 122 //���չ�����--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1109 123 //���������--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1110 124 //Ŀ�ͷ�����--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1111 125 //����ͷ���--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1112 126 //��ɫ������--�µ���;��Ԯ��         {"�µ���;��Ԯ��",           107,0,0},
#define KLINE_1113 127 //¥ͷ������--�½�������             {"�½�������",               107,0,0},
#define KLINE_1114 128 //���������--�½�����               {"�½�����",                 107,0,0},
#define KLINE_1115 129 //�鵶��ˮ��--�½���������           {"�½���������",             107,0,0},
#define KLINE_1116 130 //��ɽ������--��Խ�����߻�����     {"��Խ�����߻�����",       107,0,0},
#define KLINE_1117 131 //˥��������--�µ���;���������     {"�µ���;���������",       107,0,0},
#define KLINE_1118 132 //ǰ����ӵ��--�µ���;˼��������     {"�µ���;˼��������",       107,0,0},
#define KLINE_1119 133 //���羢����--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1120 134 //���ﶫ����--�µ���;������         {"�µ���;������",           107,0,0},
#define KLINE_1121 135 //��Ƽ��פ��--�µ���;α����         {"�µ���;α����",           107,0,0},

#define JBMXG_1001	201	//С�̴��¹�
#define JBMXG_1002	202	//���̴��¹�
#define JBMXG_1003	203	//���̴��¹�
#define JBMXG_1004	204	//С�̼��Ź�
#define JBMXG_1005	205	//С�̼�ƽ��
#define JBMXG_1006	206	//С�̼����
#define JBMXG_1007	207	//���̼��Ź�
#define JBMXG_1008	208	//���̼�ƽ��
#define JBMXG_1009	209	//���̼����
#define JBMXG_1010	210	//���̼��Ź�
#define JBMXG_1011	211	//���̼�ƽ��
#define JBMXG_1012	212	//���̼����
#define JBMXG_1013	213	//�����
#define JBMXG_1014	214	//�߹ɱ�����������
#define JBMXG_1015	215	//�߸����������� 
#define JBMXG_1016	216	//�߷����������

#define JSMXG_1001	301	//BIAS����ָʾ
#define JSMXG_1002	302	//SAR����ָʾ
#define JSMXG_1003	303	//KDJ����ָʾ
#define JSMXG_1004	304	//RSI����ָʾ
#define JSMXG_1005	305	//MACD����ָʾ
#define JSMXG_1006	306	//MTM����ָʾ
#define JSMXG_1007	307	//�ϴ�����	

//��������
#define CLETYPE_0	0	//������
#define CLETYPE_1	1	//�ײ�ͻ��
#define CLETYPE_2	2	//��λ����

#define CLETYPE_3	3	//�����Ϲ�
#define CLETYPE_4	4	//������

#define CLETYPE_5	5	//�𵴳���
#define CLETYPE_6	6	//����ͻ��
#define CLETYPE_7	7	//�ߵ��ص�

#define CLETYPE_8	8	//�����»�
#define CLETYPE_9	9	//������

#define CLETYPE_10	10	//������
#define CLETYPE_11	11	//������

#define DPG		0  	 	//���̹�
#define ZPG		1   	//���̹�
#define XPG		2  	    //С�̹�

#define JYG		0  		//���Ź�
#define JPG		1 		//��ƽ��
#define JCG		2		//�����
#define KCG		3		//�����

#define E_CXG       1  //���¹�
#define E_XPCXG     2  //С�̴��¹�
#define E_ZPCXG     3  //���̴��¹�
#define E_DPCXG     4  //���̴��¹�
#define E_XPG       5  //С�̹�
#define E_XPJYG     6  //С�̼��Ź�
#define E_XPJPG     7  //С�̼�ƽ��
#define E_XPJCG     8  //С�̼����

#define E_ZPG       9	//���̹�
#define E_ZPJYG     10	//���̼��Ź�
#define E_ZPJPG     11	//���̼�ƽ��
#define E_ZPJCG     12	//���̼����

#define E_DPG       13	//���̹�
#define E_DPJYG     14   //���̼��Ź�
#define E_DPJPG     15	//���̼�ƽ��
#define E_DPJCG     16	//���̼����

#define E_KSG       17	//�����
#define E_GBKZNLG   18	//�߹ɱ�����������
#define E_GSPNLG    19	//�߸�����������
#define E_FHGDLYG   20	//�߷����������

#define E_NOTUSER   21
#define E_DMIKZFZ   22  //BIAS����ָʾ
#define E_SARBUY    23  //SAR����ָʾ
#define E_KDJBUY    24  //KDJ����ָʾ
#define E_RSIBUY    25  //RSI����ָʾ
#define E_MACDBUY   26  //MACD����ָʾ
#define E_WVADBUY   27  //MTM����ָʾ
#define E_SQNX      28  //�ϴ�����

#define	C_CUSTJBM	0
#define	C_CUSTHQ	1
#define	C_CUSTOTHER	2

#define	C_VT_FBJ	0
#define	C_VT_NBJ	1
#define	C_VT_TF		2
#define	C_VT_SEL1	3
#define	C_VT_SEL2	4

#define	C_ZGB		0	//�ܹɱ�
#define	C_LTB		1	//��ͨ�ɱ�
#define	C_BG		4	//B��
#define	C_HG		5	//H��

#define	C_ZSZ		7	//����ֵ
#define	C_LTSZ		8	//��ͨ��ֵ
#define	C_ZZC		9	//���ʲ�
#define	C_LDZC		10	//�����ʲ�

#define	C_wxzc		11  //�����ʲ�
#define	C_cqtz		12  //����Ͷ��
#define	C_ldfz		13  //������ծ
#define	C_cqfz		14 //���ڸ�ծ
#define	C_zbgjj		15  //�ʱ�������
#define	C_yszk		16  //Ӧ���ʿ�
#define	C_yyly		17  //Ӫҵ����
#define	C_tzsy		18  //Ͷ������
#define	C_jyxjl		19  //��Ӫ�ֽ�����
#define	C_zxjl		20  //���ֽ�����
#define	C_ch		21  //���
#define	C_jly		22  //������
#define	C_wfply		23  //δ��������

#define	C_JZC		24	//���ʲ�
#define	C_MGJZC		25	//����ÿ�ɾ��ʲ�
#define	C_MGGJJ		26	//ÿ�ɹ�����
#define	C_MGWFP		27	//ÿ��δ����
#define	C_MGSY		28	//ÿ������
#define	C_GDQYB		29	//�ɶ�Ȩ��� ���ʲ�/���ʲ�
#define	C_ZBGJJ		30	//�ʱ�������
#define	C_ZYSY		31	//��Ӫ����
#define	C_LYZE		32	//�����ܶ�
#define	C_SHLY		33	//˰������
#define	C_JYL		34	//���ʲ�������

#define	C_NOW		40	//�ּ�
#define	C_MAX		41	//��߼�
#define	C_MIN		42	//��ͼ�
#define	C_OPEN		43	//����
#define	C_CLOSE		44	//����
#define	C_VOLUME	45	//�ɽ���
#define	C_AMOUNT	46	//�ɽ����

#define	C_ZAF		47	//�Ƿ�
#define	C_ZEF		48	//���
#define	C_SYL		49	//��ӯ��
#define	C_HSL		50	//������
#define	C_LIANGB	51	//����
#define	C_WTB		52	//ί��

#define	C_SSNUM		60	//��������
#define	C_KDJ_J		61	//KDJ�е�Jֵ
#define	C_RSI		62	//RSI1
#define	C_DMI_ADX	63	//DMI�е�ADXֵ
#define	C_WR		64	//WR
#define	C_VR		65	//VR


#endif