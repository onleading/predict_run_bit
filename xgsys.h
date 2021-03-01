#ifndef XGSYS_H_
#define XGSYS_H_

//////////////////////////////////////////////////////////////

//单根K线形态(Odd K Line)
#define KLINEO_VSMALL			1			//极小K线(类十字星)
#define	KLINEO_NSMALL           2           //普通小K线
#define KLINEO_SMALL            3           //小K线
#define KLINEO_VBIG				4			//极小K线(类十字星)
#define KLINEO_NBIG             8          	//普通大K线
#define KLINEO_BIG              12         	//大K线
#define KLINEO_NORMAL           16          //中K线
#define KLINEO_NOHEADFOOT   	1           //秃头光脚K线
#define KLINEO_NOHEAD           2           //秃头K线
#define KLINEO_NOFOOT           3           //光脚K线
#define KLINEO_UVLSHADOW    	4           //上极长K线
#define KLINEO_DVLSHADOW        5           //下极长K线
#define KLINEO_ULSHADOW     	6           //上长K线
#define KLINEO_DLSHADOW         7           //下长K线
#define KLINEO_ELSHADOW         8           //等长K线
#define KLINEO_UP               9           //阳线
#define KLINEO_DOWN             10          //阴线
#define KLINEO_LEVEL            11          //平盘线(十字线)

//双根,三根K线形态(Twin K Line)
#define KLINET_ASCENT           1			//攀援线
#define KLINET_SLIDE            2           //滑行线
#define KLINET_JMPUP            3           //跳高线
#define KLINET_JMPDOWN          4           //跳水线
#define KLINET_CRACK            5           //压迫线
#define KLINET_FLATTER          6           //奉承线
#define KLINET_ABANDON          7           //弃子线
#define KLINET_COVER        	8			//覆盖线
#define KLINET_CUTIN        	9           //切入线
#define KLINET_DRAGUP           10          //上拉线
#define KLINET_DRAGDOWN         11          //下拖线
#define KLINET_SURRENDER        12          //归顺线
#define KLINET_REBELL           13          //反叛线
#define KLINET_FEIGN        	14          //伪K线(*)
#define KLINET_FEIGNUP          15          //伪阳线
#define KLINET_FEIGNDOWN        16          //伪阴线
#define KLINET_COORD            17          //并列线(*)
#define KLINET_COORDUP          18          //并列阳线
#define KLINET_COORDDOWN        19          //并列阴线
#define KLINET_CONTAIN          20          //包容线(*)
#define KLINET_CUU              21          //阳包阳
#define KLINET_CUD              22          //阳包阴
#define KLINET_CUL              23          //阳包星
#define KLINET_CDU              24          //阴包阳
#define KLINET_CDD              25          //阴包阴
#define KLINET_CDL              26          //阴包星
#define KLINET_GRAVID           27          //孕妊线(*)
#define KLINET_GUU              28          //阳孕阳
#define KLINET_GUD              29          //阳孕阴
#define KLINET_GUL              30          //阳孕星
#define KLINET_GDU              31          //阴孕阳
#define KLINET_GDD              32          //阴孕阴
#define KLINET_GDL              33          //阴孕星
#define KLINET_NIP              34          //中夹线(*)
#define KLINET_NDU              35          //阴夹阳
#define KLINET_NUD              36          //阳夹阴
#define KLINET_FAKECOVER        37          //类覆盖线
#define KLINET_FAKECUTIN        38          //类切入线

//K线趋势形态(Trend K Line)
#define KLINETL_HIGH            1           //高档(*)
#define KLINETL_LOW             2           //低档(*)
#define KLINETL_TOSS            3           //震荡(*)
#define KLINETL_DULL            4           //横盘(*)
#define KLINETL_UP              5           //上升途中(*)
#define KLINETL_DOWN            6           //下跌途中(*)
#define KLINETL_HIGHTOSS        7           //高档震荡(*)
#define KLINETL_LOWTOSS         8           //低档震荡(*)
#define KLINETL_HIGHDULL        9           //高档横盘(*)
#define KLINETL_LOWDULL         10          //低档横盘(*)
#define KLINETL_FASTUP          11          //急升(*)
#define KLINETL_FASTDOWN        12          //急跌(*)
#define KLINETL_SLOWUP          13          //缓升(*)
#define KLINETL_SLOWDOWN        14          //缓跌(*)
#define KLINETL_HIGHTOSS1       15          //高档震荡初期
#define KLINETL_LOWTOSS1        16          //低档震荡初期
#define KLINETL_HIGHTOSS2       17          //高档震荡中期
#define KLINETL_LOWTOSS2        18          //低档震荡中期
#define KLINETL_HIGHTOSS3       19          //高档震荡末期
#define KLINETL_LOWTOSS3        20          //低档震荡末期
#define KLINETL_HIGHDULL1       21          //高档横盘初期
#define KLINETL_LOWDULL1        22          //低档横盘初期
#define KLINETL_HIGHDULL2       23          //高档横盘中期
#define KLINETL_LOWDULL2        24          //低档横盘中期
#define KLINETL_HIGHDULL3       25          //高档横盘末期
#define KLINETL_LOWDULL3        26          //低档横盘末期
#define KLINETL_HIGHFASTUP      27          //高档急升
#define KLINETL_HIGHFASTDOWN    28          //高档急跌
#define KLINETL_HIGHSLOWUP      29          //高档缓升
#define KLINETL_HIGHSLOWDOWN    30          //高档缓跌
#define KLINETL_LOWFASTUP       31          //低档急升
#define KLINETL_LOWFASTDOWN     32          //低档急跌
#define KLINETL_LOWSLOWUP       33          //低档缓升
#define KLINETL_LOWSLOWDOWN     34          //低档缓跌

#define KLINE_1001  4   //江花似火谱--低档多条并列阳线       {"低档多条并列阳线",         2, 0,0},
#define KLINE_1002  5   //春潮带雨谱--低档多条并列阴线       {"低档多条并列阴线",         2, 0,0},
#define KLINE_1003  6   //寒江独钓谱--低档横盘中剑形线       {"低档横盘中剑形线",         2, 0,0},
#define KLINE_1004  7   //新燕啄春谱--低档横盘中弓形线       {"低档横盘中弓形线",         2, 0,0},
#define KLINE_1005  8   //南天一柱谱--低档横盘中下跳大阳线   {"低档横盘中下跳大阳线",     2, 0,0},
#define KLINE_1006  9   //春潮暗涌谱--低档横盘中上跳大阴线   {"低档横盘中上跳大阴线",     2, 0,0},
#define KLINE_1007  10   //如箭在弦谱--低档横盘末期弓形线     {"低档横盘末期弓形线",       3, 0,0},
#define KLINE_1008  11  //春江水暖谱--低档横盘该期剑形线     {"低档横盘该期剑形线",       3, 0,0},
#define KLINE_1009  12  //红日喷薄谱--低档横盘末期大阳突破   {"低档横盘末期大阳突破",     3, 0,0},
#define KLINE_1010  13  //气吞万里谱--小幅盘落后大阳向上     {"小幅盘落后大阳向上",       3, 0,0},
#define KLINE_1011  14  //三阳开泰谱--低档横盘末期红三兵     {"低档横盘末期红三兵",       3, 0,0},

#define KLINE_1012  16  //千里冰封谱--低档急跌三大阴         {"低档急跌三大阴",           15,0,0},
#define KLINE_1013  17  //龙宫探宝谱--三空阴线               {"三空阴线",                 15,0,0},
#define KLINE_1014  18  //晨星启明谱--低档急跌后下跳十字星   {"低档急跌后下跳十字星",     15,0,0},
#define KLINE_1015  19  //扬眉出鞘谱--低档急跌后下跳剑形线   {"低档急跌后下跳剑形线",     15,0,0},
#define KLINE_1016  20  //弯弓射雕谱--低档急跌后下跳弓形线   {"低档急跌后下跳弓形线",     15,0,0},
#define KLINE_1017  21  //残寒消尽谱--低档急跌后尽头阴线     {"低档急跌后尽头阴线",       15,0,0},
#define KLINE_1018  22  //金鸡报晓谱--低档急跌后独阳线       {"低档急跌后独阳线",         15,0,0},
#define KLINE_1019  23  //顾影自怜谱--低档急跌后独阴线       {"低档急跌后独阴线",         15,0,0},
#define KLINE_1020  24  //朝阳掩月谱--低档急跌后阳包阴       {"低档急跌后阳包阴",         15,0,0},
#define KLINE_1021  25  //春寒倒灌谱--低档急跌后最后包容阴线 {"低档急跌后最后包容阴线",   15,0,0},
#define KLINE_1022  26  //晓色云开谱--低档急跌后阴孕阳       {"低档急跌后阴孕阳",         15,0,0},
#define KLINE_1023  27  //珠帘半卷谱--低档急跌后阴孕阴       {"低档急跌后阴孕阴",         15,0,0},
#define KLINE_1024  28  //寒梅报春谱--低档急跌后阴孕十字星   {"低档急跌后阴孕十字星",     15,0,0},
#define KLINE_1025  29  //草船借箭谱--低档急跌后阴孕弓形线   {"低档急跌后阴孕弓形线",     15,0,0},
#define KLINE_1026  30  //宝剑夜鸣谱--低档急跌后阴孕剑形线   {"低档急跌后阴孕剑形线",     15,0,0},
#define KLINE_1027  31  //春光半露谱--低档急跌后切入线       {"低档急跌后切入线",         15,0,0},
#define KLINE_1028  32  //天堑通途谱--低档急跌后阴夹阳       {"低档急跌后阴夹阳",         15,0,0},
#define KLINE_1029  33  //弃暗投明谱--低档急跌后上拉线       {"低档急跌后上拉线",         15,0,0},
#define KLINE_1030  34  //双针探底谱--低档急跌后连拉剑形线   {"低档急跌后连拉剑形线",     15,0,0},
#define KLINE_1031  35 //梅开二度谱--低档二次狙击性阳线     {"低档二次狙击性阳线",       15,0,0},

#define KLINE_1032  38  //再上层楼谱--上档盘整后大阳突破     {"上档盘整后大阳突破",       37,0,0},
#define KLINE_1033  39  //一片冰心谱--跳高一字线后不补缺     {"跳高一字线后不补缺",       37,0,0},
#define KLINE_1034  40  //千帆竞渡谱--二次高开阳线           {"二次高开阳线",             37,0,0},
#define KLINE_1035  41  //破釜沉舟谱--跳空攀援线             {"跳空攀援线",               37,0,0},
#define KLINE_1036  42  //勇追穷寇谱--渐大三连阳             {"渐大三连阳",               37,0,0},
#define KLINE_1037  43  //不畏浮云谱--上升中途跳高十字星     {"上升中途跳高十字星",       37,0,0},
#define KLINE_1038  44  //仗剑远游谱--上升中途跳高剑形线     {"上升中途跳高剑形线",       37,0,0},
#define KLINE_1039  45  //后羿射日谱--上升中途跳高弓形线     {"上升中途跳高弓形线",       37,0,0},
#define KLINE_1040  46  //望断南雁谱--上升中途小压迫线       {"上升中途小压迫线",         37,0,0},
#define KLINE_1041  47  //蚍蜉撼树谱--上升中途伪阴线         {"上升中途伪阴线",           37,0,0},
#define KLINE_1042  48  //风卷残云谱--上升中途阳包阴         {"上升中途阳包阴",           37,0,0},
#define KLINE_1043  49  //从善如流谱--上升中途归顺线         {"上升中途归顺线",           37,0,0},
#define KLINE_1044  50  //葵花向日谱--上升中途上拉线         {"上升中途上拉线",           37,0,0},
#define KLINE_1045  51  //夫唱妇随谱--上升中途阳孕阴         {"上升中途阳孕阴",           37,0,0},

#define KLINE_1046  53  //红旗半卷谱--上升中途切入线         {"上升中途切入线",           52,0,0},
#define KLINE_1047  54  //负重前行谱--上升中途阳孕阳         {"上升中途阳孕阳",           52,0,0},
#define KLINE_1048  55  //极目云空谱--上升中途并阳线         {"上升中途并阳线",           52,0,0},
#define KLINE_1049  56  //春燕戏花谱--上升中途滑行线         {"上升中途滑行线",           52,0,0},
#define KLINE_1050  57  //勇攀高峰谱--上升三连击             {"上升三连击",               52,0,0},
#define KLINE_1051  58  //柳暗花明谱--上升三法               {"上升三法",                 52,0,0},
#define KLINE_1052  59  //以退为进谱--上升跳空三法           {"上升跳空三法",             52,0,0},
#define KLINE_1053  60  //凭高远眺谱--超越覆盖线或压迫线     {"超越覆盖线或压迫线",       52,0,0},
#define KLINE_1054  61  //近乡情怯谱--上升中途 受阻红三兵    {"上升中途“受阻红三兵”",   52,0,0},
#define KLINE_1055  62  //瞻前顾后谱--上升中途 思考红三兵    {"上升中途“思考红三兵”",   52,0,0},
#define KLINE_1056  63  //腹背受敌谱--上升中途阴夹阳         {"上升中途阴夹阳",           52,0,0},
#define KLINE_1057  64  //登高怀远谱--上升中途阳包阳         {"上升中途阳包阳",           52,0,0},
#define KLINE_1058  65  //退避三舍谱--上升中途伪阳线         {"上升中途伪阳线",           52,0,0},

#define KLINE_1059  68  //层林尽染谱--高档多条并列阳线       {"高档多条并列阳线",         67,0,0},
#define KLINE_1060  69  //山雨欲来谱--高档多根并列阴线       {"高档多根并列阴线",         67,0,0},
#define KLINE_1061  70  //雾锁楼台谱--反击滑行线             {"反击滑行线",               67,0,0},
#define KLINE_1062  71  //月照城头谱--高档横盘中上跳大阴线   {"高档横盘中上跳大阴线",     67,0,0},
#define KLINE_1063  72  //青冢向晚谱--高档横盘中弓形线       {"高档横盘中弓形线",         67,0,0},
#define KLINE_1064  73  //秋露始白谱--高档横盘中剑形线       {"高档横盘中剑形线",         67,0,0},
#define KLINE_1065  74  //黑云压城谱--滑行线接滑行线         {"滑行线接滑行线",           67,0,0},
#define KLINE_1066  75  //乱鸦啼螟谱--见顶三鸦               {"见顶三鸦",                 67,0,0},

#define KLINE_1067  77  //千钧一发谱--高档盘整末期剑形线     {"高档盘整末期剑形线",       76,0,0},
#define KLINE_1068  78  //飞流直下谱--高档盘整末期大阴向下   {"高档盘整末期大阴向下",     76,0,0},
#define KLINE_1069  79  //风急天高谱--高档盘整末期黑三兵     {"高档盘整末期黑三兵",       76,0,0},
#define KLINE_1070  80  //山河变色谱--小幅盘升后大阴向下     {"小幅盘升后大阴向下",       76,0,0},
#define KLINE_1071  81  //雨横风狂谱--暴跌三鸦               {"暴跌三鸦",                 76,0,0},
#define KLINE_1072  82  //绝顶揽胜谱--炮楼顶                 {"炮楼顶",                   76,0,0},

#define KLINE_1073  84  //花近高楼谱--高档急升三大阳         {"高档急升三大阳",           83,0,0},
#define KLINE_1074  85  //九天揽月谱--三空阳线               {"三空阳线",                 83,0,0},
#define KLINE_1075  86  //月明星稀谱--高档弃十字星           {"高档弃十字星",             83,0,0},
#define KLINE_1076  87  //出师未捷谱--高档弃弓形线           {"高档弃弓形线",             83,0,0},
#define KLINE_1077  88  //太公钓鱼谱--高档弃吊劲线           {"高档弃吊劲线",             83,0,0},
#define KLINE_1078  89  //秋菊竞艳谱--高档尽头阳线           {"高档尽头阳线",             83,0,0},
#define KLINE_1079  90  //高不胜寒谱--绝顶独阴线             {"绝顶独阴线",               83,0,0},
#define KLINE_1080  91  //一览众山谱--绝顶独阳线             {"绝顶独阳线",               83,0,0},
#define KLINE_1081  92  //双雁南归谱--高档跳空滑行线         {"高档跳空滑行线",           83,0,0},
#define KLINE_1082  93  //偷天换日谱--高档阴包阳             {"高档阴包阳",               83,0,0},
#define KLINE_1083  94  //血色黄昏谱--高档最后包容阳线       {"高档最后包容阳线",         83,0,0},
#define KLINE_1084  95  //天狗食月谱--高档阴包阴             {"高档阴包阴",               83,0,0},
#define KLINE_1085  96  //冰河洗剑谱--高档阴包剑形线         {"高档阴包剑形线",           83,0,0},
#define KLINE_1086  97  //一叶惊秋谱--高档阳孕阴             {"高档阳孕阴",               83,0,0},
#define KLINE_1087  98  //落日残霞谱--高档阳孕阳             {"高档阳孕阳",               83,0,0},
#define KLINE_1088  99  //晚晴风歇谱--高档阳孕十字星         {"高档阳孕十字星",           83,0,0},
#define KLINE_1089 100  //解甲归田谱--高档阳孕吊颈线         {"高档阳孕吊颈线",           83,0,0},
#define KLINE_1090 101  //风摧秀木谱--高档覆盖线             {"高档覆盖线",               83,0,0},
#define KLINE_1091 102  //魂断蓝桥谱--高档阳夹阴             {"高档阳夹阴",               83,0,0},
#define KLINE_1092 103  //夕阳寒鸦谱--高档下拖线             {"高档下拖线",               83,0,0},
#define KLINE_1093 104  //卷土重来谱--高档二次狙击性阴线     {"高档二次狙击性阴线",       83,0,0},

#define KLINE_1094 108  //泉咽危石谱--下档盘整后大阴突破     {"下档盘整后大阴突破",       106,0,0},
#define KLINE_1095 109  //一泻千里谱--跳水一字线后不补缺     {"跳水一字线后不补缺",       106,0,0},
#define KLINE_1096 110  //夜雨频滴谱--二次跳空阴线           {"二次跳空阴线",             106,0,0},
#define KLINE_1097 111  //一江春水谱--跳空滑行线             {"跳空滑行线",               106,0,0},
#define KLINE_1098 112  //大浪淘沙谱--渐大三连阴             {"渐大三连阴",               106,0,0},
#define KLINE_1099 113  //落日浮云谱--下跌中途跳水十字星     {"下跌中途跳水十字星",       106,0,0},
#define KLINE_1100 114  //落花坠楼谱--下跌中途跳水弓形线     {"下跌中途跳水弓形线",       106,0,0},
#define KLINE_1101 115  //碧溪垂钓谱--下跌中途跳水剑形线     {"下跌中途跳水剑形线",       106,0,0},
#define KLINE_1102 116  //落红难缀谱--下跌中途小奉承线       {"下跌中途小奉承线",         106,0,0},
#define KLINE_1103 117  //桃花流水谱--下跌中途伪阳线         {"下跌中途伪阳线",           106,0,0},
#define KLINE_1104 118  //水阔鱼沉谱--下跌中途阴包阳         {"下跌中途阴包阳",           106,0,0},
#define KLINE_1105 119 //绿波依旧谱--下跌中途反叛线         {"下跌中途反叛线",           106,0,0},
#define KLINE_1106 120 //零落成泥谱--下跌中途下拖线         {"下跌中途下拖线",           106,0,0},
#define KLINE_1107 121 //帘卷秋霁谱--下跌中途阴包阳         {"下跌中途阴包阳",           106,0,0},

#define KLINE_1108 122 //月照孤舟谱--下跌中途覆盖线         {"下跌中途覆盖线",           107,0,0},
#define KLINE_1109 123 //卷帷望月谱--下跌中途阴孕阴         {"下跌中途阴孕阴",           107,0,0},
#define KLINE_1110 124 //目送芳尘谱--下跌中途并阳线         {"下跌中途并阳线",           107,0,0},
#define KLINE_1111 125 //王燕低飞谱--下跌中途并阴线         {"下跌中途并阴线",           107,0,0},
#define KLINE_1112 126 //秋色连波谱--下跌中途攀援线         {"下跌中途攀援线",           107,0,0},
#define KLINE_1113 127 //楼头残梦谱--下降三连击             {"下降三连击",               107,0,0},
#define KLINE_1114 128 //落红满径谱--下降三法               {"下降三法",                 107,0,0},
#define KLINE_1115 129 //抽刀断水谱--下降跳空三法           {"下降跳空三法",             107,0,0},
#define KLINE_1116 130 //青山难遮谱--超越切入线或奉承线     {"超越切入线或奉承线",       107,0,0},
#define KLINE_1117 131 //衰草凝绿谱--下跌中途受阻黑三兵     {"下跌中途受阻黑三兵",       107,0,0},
#define KLINE_1118 132 //前呼后拥谱--下跌中途思考黑三兵     {"下跌中途思考黑三兵",       107,0,0},
#define KLINE_1119 133 //疾风劲草谱--下跌中途阳夹阴         {"下跌中途阳夹阴",           107,0,0},
#define KLINE_1120 134 //无语东流谱--下跌中途阴包阴         {"下跌中途阴包阴",           107,0,0},
#define KLINE_1121 135 //浪萍难驻谱--下跌中途伪阴线         {"下跌中途伪阴线",           107,0,0},

#define JBMXG_1001	201	//小盘次新股
#define JBMXG_1002	202	//中盘次新股
#define JBMXG_1003	203	//大盘次新股
#define JBMXG_1004	204	//小盘绩优股
#define JBMXG_1005	205	//小盘绩平股
#define JBMXG_1006	206	//小盘绩差股
#define JBMXG_1007	207	//中盘绩优股
#define JBMXG_1008	208	//中盘绩平股
#define JBMXG_1009	209	//中盘绩差股
#define JBMXG_1010	210	//大盘绩优股
#define JBMXG_1011	211	//大盘绩平股
#define JBMXG_1012	212	//大盘绩差股
#define JBMXG_1013	213	//亏损股
#define JBMXG_1014	214	//具股本扩张能力股
#define JBMXG_1015	215	//具高送配能力股 
#define JBMXG_1016	216	//具丰厚滚存利润股

#define JSMXG_1001	301	//BIAS买入指示
#define JSMXG_1002	302	//SAR买入指示
#define JSMXG_1003	303	//KDJ买入指示
#define JSMXG_1004	304	//RSI买入指示
#define JSMXG_1005	305	//MACD买入指示
#define JSMXG_1006	306	//MTM买入指示
#define JSMXG_1007	307	//上穿年线	

//条件类型
#define CLETYPE_0	0	//震荡吸筹
#define CLETYPE_1	1	//底部突破
#define CLETYPE_2	2	//低位反弹

#define CLETYPE_3	3	//快速上攻
#define CLETYPE_4	4	//震荡盘上

#define CLETYPE_5	5	//震荡出贷
#define CLETYPE_6	6	//顶部突破
#define CLETYPE_7	7	//高档回调

#define CLETYPE_8	8	//快速下滑
#define CLETYPE_9	9	//震荡盘下

#define CLETYPE_10	10	//基本面
#define CLETYPE_11	11	//技术面

#define DPG		0  	 	//大盘股
#define ZPG		1   	//中盘股
#define XPG		2  	    //小盘股

#define JYG		0  		//绩优股
#define JPG		1 		//绩平股
#define JCG		2		//绩差股
#define KCG		3		//亏损股

#define E_CXG       1  //次新股
#define E_XPCXG     2  //小盘次新股
#define E_ZPCXG     3  //中盘次新股
#define E_DPCXG     4  //大盘次新股
#define E_XPG       5  //小盘股
#define E_XPJYG     6  //小盘绩优股
#define E_XPJPG     7  //小盘绩平股
#define E_XPJCG     8  //小盘绩差股

#define E_ZPG       9	//中盘股
#define E_ZPJYG     10	//中盘绩优股
#define E_ZPJPG     11	//中盘绩平股
#define E_ZPJCG     12	//中盘绩差股

#define E_DPG       13	//大盘股
#define E_DPJYG     14   //大盘绩优股
#define E_DPJPG     15	//大盘绩平股
#define E_DPJCG     16	//大盘绩差股

#define E_KSG       17	//亏损股
#define E_GBKZNLG   18	//具股本扩张能力股
#define E_GSPNLG    19	//具高送配能力股
#define E_FHGDLYG   20	//具丰厚滚存利润股

#define E_NOTUSER   21
#define E_DMIKZFZ   22  //BIAS买入指示
#define E_SARBUY    23  //SAR买入指示
#define E_KDJBUY    24  //KDJ买入指示
#define E_RSIBUY    25  //RSI买入指示
#define E_MACDBUY   26  //MACD买入指示
#define E_WVADBUY   27  //MTM买入指示
#define E_SQNX      28  //上穿年线

#define	C_CUSTJBM	0
#define	C_CUSTHQ	1
#define	C_CUSTOTHER	2

#define	C_VT_FBJ	0
#define	C_VT_NBJ	1
#define	C_VT_TF		2
#define	C_VT_SEL1	3
#define	C_VT_SEL2	4

#define	C_ZGB		0	//总股本
#define	C_LTB		1	//流通股本
#define	C_BG		4	//B股
#define	C_HG		5	//H股

#define	C_ZSZ		7	//总市值
#define	C_LTSZ		8	//流通市值
#define	C_ZZC		9	//总资产
#define	C_LDZC		10	//流动资产

#define	C_wxzc		11  //无形资产
#define	C_cqtz		12  //长期投资
#define	C_ldfz		13  //流动负债
#define	C_cqfz		14 //长期负债
#define	C_zbgjj		15  //资本公积金
#define	C_yszk		16  //应收帐款
#define	C_yyly		17  //营业利润
#define	C_tzsy		18  //投资收益
#define	C_jyxjl		19  //经营现金净流量
#define	C_zxjl		20  //总现金净流量
#define	C_ch		21  //存货
#define	C_jly		22  //净利益
#define	C_wfply		23  //未分配利益

#define	C_JZC		24	//净资产
#define	C_MGJZC		25	//调整每股净资产
#define	C_MGGJJ		26	//每股公积金
#define	C_MGWFP		27	//每股未分配
#define	C_MGSY		28	//每股收益
#define	C_GDQYB		29	//股东权益比 净资产/总资产
#define	C_ZBGJJ		30	//资本公积金
#define	C_ZYSY		31	//主营收入
#define	C_LYZE		32	//利益总额
#define	C_SHLY		33	//税后利益
#define	C_JYL		34	//净资产收益率

#define	C_NOW		40	//现价
#define	C_MAX		41	//最高价
#define	C_MIN		42	//最低价
#define	C_OPEN		43	//开盘
#define	C_CLOSE		44	//收盘
#define	C_VOLUME	45	//成交量
#define	C_AMOUNT	46	//成交金额

#define	C_ZAF		47	//涨幅
#define	C_ZEF		48	//振幅
#define	C_SYL		49	//市盈率
#define	C_HSL		50	//换手率
#define	C_LIANGB	51	//量比
#define	C_WTB		52	//委比

#define	C_SSNUM		60	//交易天数
#define	C_KDJ_J		61	//KDJ中的J值
#define	C_RSI		62	//RSI1
#define	C_DMI_ADX	63	//DMI中的ADX值
#define	C_WR		64	//WR
#define	C_VR		65	//VR


#endif