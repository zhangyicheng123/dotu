# .u
.u语言是一款自定义性极强的语言，欢迎加入开发！
# 语法
output=变量名（可以用来输出变量的内容）
define(变量名)=number（把变量定义为数字类型）
define(变量名)=string（把变量定义为字符串类型）
comp(算式)=新变量名（计算算式，可以用数字变量，输出到右侧的新变量）
output=comp(算式)（计算算式，可以用数字变量并直接输出）
input(变量名)（将具体内容输入到变量）
这个程序打开后会提示输入.u文件的地址，输入地址后就会运行.u文件
import 库名（导入库，库的格式为.uh，需要输入.uh文件的地址，除了自带的这些代码，其他代码，哪怕是判断语句都需要从库中导入）
V(变量名)=内容（定义一个变量的内容）
