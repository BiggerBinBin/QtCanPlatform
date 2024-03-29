#  QtCanPlatform
QtCanPlatform是一款用于测试CAN设备的可视化编辑软件，他可以根据CAN协议设置对应的字段参数，来测试设备。支持导入DBC文件（使用了ZLG的库，[链接](https://www.zlg.cn/data/upload/software/Can/DBC_demo_.rar)）和手动添加协议两种方式
# 历史版本
* V2.02.10：目前已经支持手动添加CAN协议和导致DBC功能
# 编译环境 #
* 编译环境：VS2022+QT5.14.2 
* 操作系统：Windows10
# 支持的CAN设备  
* PCAN、RCAN、ECAN、kvaser等，后续会添加更多
# 编译使用  
* 下载之后，用VS打开.sln文件，设置好QT版本
* 编译  
* 保存的数据在程序目录下的Data里面
* APPDIR程序目录图
	- ├─bearer
	- ├─canbus
	- ├─Data				//appData.json为CAN协议数据文件，如果没有，则自己新建一个，Jugde.ini为程序配置文件，如果没有，则自己新建一个
	- ├─iconengines
	- ├─imageformats
	- ├─logs				//日志文件
	- ├─PCAN-DATA		//保存的CAN接收到数据文件，600个保存一次
	- ├─platforms
	- ├─styles
	- └─translations
#界面如下  
## 主界面  
<img src="./pic/MainWindow.jpg">  
## 设置界面  
<img src="./pic/setWindows.jpg">
#  软件使用  
* 设置好CAN协议  
* 刷新设备  
* 选择波特率  
* 打开设备  
* 选择型号  
* 点击发送  