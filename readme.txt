20090713
1.支持总在前端显示
2.就是在切换个股的K线的时候，如果突然出现新股，K线就会放大， （改为至少20根)

20090709
1.支持重点突出的配置项(CheckBox)
2.闪电图的修改
3.扩展行情分时右上角没有图标


20090706
合并新代码
目前闪电手的行情背景固定，能支持自定义面板或者其他行情界面
Ctrl+Home,End逻辑的修改
个人理财功能中新建帐号的问题
用户板块管理中删除多个板块优化处理
龙虎统计的修改

20090617
支持广发证券在登录界面上放置空闲图片的需求
进入即发送扩展状态栏行情，用于计算大盘的领先线
综合排名中有些歧义的处理
时间段分时区间统计的BUG修改
系统状态中加入更多的信息提示

20090611
支持不显示大盘领先线
支持定制版面中亦锁定初始K线数
修正初始化期间有时退出的BUG

20090609
扩展行情多股同列死机的问题

20090607
修正平盘K线的显示颜色的精度问题 Close+COMPREE<OPEN之类的

20090602
SYSINFO接口中加入扩展行情在线状态及是否是实时港股状态

20090601

自定义板块数量的改进,支持超过100个 修改自定义板块个数设置为无限制 (达400个)
6.05版本分时图按TBA键切换半场走势，到6.06此功能和快捷键被取消 

20090531

1.解决自动查找最快造成对行情主站SYN冲击的问题


////////////////////////////////////

利润同比和收入同比
自填(有争议)每股收益的处理

简单的财务分析功能:
////////////////////////////////////
考虑将这6个字段换成

    float       J_qtly;         //其它利益 -> 应收帐款 yszk
    float       J_btsy;         //补贴收入 -> 经营现金流 jyxjl
    float       J_yywsz;        //营业外收支 -> 总现金流 zxjl
    float       J_snsytz;       //上年损益调整 -> 存货 ch

存货 应收帐款 主营成本 股东人数 (营业费用 管理费用 财务费用)

	流动比率=流动资产/流动负债
	速动比率=（流动资产-存货）/流动负债
////////////////////////////////////

解决公式参数精灵汉字双字节乱码的问题
综合选股指定日期选股的问题

完整分时表的显示问题
20050722 by iwld:走势图信息显示区和光标的相对/绝对坐标问题(在定制版面中会造成显示混乱)
行情表自定义分类显示tab
K线Tooltip信息扩充
增加周期的快捷键 9X
移动筹码分布集中度
大盘分时多空指标加上零轴
市净率
FXT五彩K线交易指示的锁定
改叠加时叠加股ToolTip显示错误的问题
叠加时用百分比坐标时Y轴数字不显示

#支持只下载A股数据
#支持证监会行业
#利润同比和收入同比
#自填(有争议)每股收益的处理

