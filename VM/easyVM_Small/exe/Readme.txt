低调发布easyVM 0.2版


简介：
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
easyVM是一个简单的虚拟机。
0.1版本只支持8086指令集和一些简单的I/O设备，只支持英文文本显示方式。
0.2版本主要是在0.1版基础上加了一小部分32位指令（push eax等），使得easyVM可以运行MS-DOS 6.22自带的大部分程序。



文件说明：
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
Bios\Bios.bin		Bios程序
Bios\BiosData.bin	CMOS数据区
DOS.IMG			MS-DOS 6.22的镜像
easyVM.exe		easyVM主程序
easyVM.ini		easyVM的配置文件
Readme.txt		本说明文本
TC.IMG			含有Trubo C 2.0的软盘镜像
Dos622c.img		硬盘镜像



Bug report，Suggestion and Quastion are welcome.




								梁一信
								yinXlms@126.com
								2008年7月11日