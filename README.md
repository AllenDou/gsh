GSH
===

gsh是什么?
-------------------------------------------
gsh 是一套基于redis网络层开发的具有开放性的计算平台,它的理念是可以自由化的开发计算公式(formula),并把公式嵌入到计算平台中,以达到统一计算的目的.

gsh中的'计算'不同于传统意义的数学计算,当然,对于数学计算,它完全可以胜任,不仅如此,开发者也可以把存储当作一种计算,开发出模块并嵌入到gsh中,默认formula中的[cache]就是一个具有存储功能的计算公式.

gsh可以同时挂载多个formula,由于是沿用redis的单线程体系架构,gsh在接受到命令后会逐条执行,这就要求开发者在开发formula时,不要添加数据处理时间过长的逻辑,避免影响其他formula,当然,如果你的gsh只有一个formula,那就ok了.

gsh支持在线安装formula即OFI(Online Formula Install)机制. OFI准许在不重启gsh的情况下安装一个新的formula,这样做的好处很显然,就是添加新功能的同时不影响线上服务.

编译 & 执行:
-------------------------------------------

	% cd gsh
	% make
	% ./bin/gsh-server etc/gsh.conf

formula示例:
-------------------------------------------
[bc]

用lex,yacc制作的数据计算器,支持加减乘除括号等运算符.开发者可以阅读此formula代码,不仅可以学习gsh的体系结构,也可以对lex,yacc词法分析器等有所了解.不了解lex,yacc的可以试图google其与C++的关系.

[cache]

通用内存存储formula,采用连续内存存储空间进行数据存储,极大限度的避免了内存碎片,并配备rbtree结构化的内存回收站,使内存存储变得更加高效,耗能更低.	

[carsvm]

新浪(sina.com)博客于2012年7月开始开发的垂直数据挖掘系统,其中使用了svm机制,为了能减少svm接口调用,carsvm应运而生,通过carsvm搭建svm数据处理平台,可有效的减少资源消耗,开发者在进行数据迁移时不再需要移动svm代码和训练样本,直接通过redis协议调用carsvm接口即可,不仅减少了很大的工作量,而且易于维护,从而降低开发成本.

[sample]

sample是一个示例,开发者可以直接修改sample来扩展formula,而无需用gen.sh脚本来生成.

怎样开发formula?
-------------------------------------------
很简单,执行formula文件夹中的gen.sh脚本即可,如:

	% cd formula
	% sh gen.sh sina
	% make 

通过以上两条命令就可以生成一个叫sina的formula,并自动配置此formula到gsh.conf文件中,gsh启动过程中会自动加载gsh.conf中的formula.


客户端 & 命令格式
-------------------------------------------
gsh只采用了redis的网络层,对于原strings,hashes,lists,sets,sorted sets等数据结构匀不再支持,只保留了info命令,grun命令为保留命令,同set命令一样.
命令格式 如下:
	
	% grun carsrvm json
	
carsvm 是formula名称 json是数据,为了更好的使gsh应用的实际生产环境,json数据采用如下格式:
	
	%{
	%	"time": 1349688926,		/*时间戳*/
	%	"ip": "127.0.0.1",		/*ip地址*/
	%	"script": "test.php",	/*客户端脚本名称*/
	%	"formula": "cache",		/*指定哪个fomula处理数据.*/
	%	"programmer": "Allen",	/*程序员名称.*/
	%	"data": {}				/*data数据会被传送给相应formula内部*/
	%}
以上字段必须存在,否则被视为错误数据.


-------------------

注:
tag v1.8.5 同svn的v1.8.5 一样.


Enjoy!




