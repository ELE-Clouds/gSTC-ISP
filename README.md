gSTC-ISP
====

```
gSTC-ISP是一款在Linux下的图形化的串口ISP下载工具。
目前仅支持对STC89C52RC（固件版本4.2C）的串口ISP编程，非STC官方出口，遵循GPL V3.
```



#### 软件功能限制
---
1. 仅支持STC89C52RC(固件版本4.2C），其它芯片可能造成损坏；
2. 仅支持12/11.0592/8/6/4 MHz外部晶振；
3. 使用USB转RS232串口下载可能效果较差；
4. 仅支持bin格式文件下载；
5. 下载文件不能大于8192字节。


#### 软件操作
---
1. 选择当前使用的串口，默认为ttyS0；
2. 选择下载波特率，默认为9600；
3. 选择下载文件；
4. 关闭目标电源；
5. 按下软件DownLoad按钮；
6. 启动目标板（上电），等待进度条达到100%；
7. 下载完成后 DownLoad 按钮恢复可用；
8. 重复4-7步可反复下载。


#### 安装说明
---
安装前请执行以下命令查看vte文件夹是否存在：
```
ls -l /usr/include/vte*
```

若不存在，则执行以下命令进行安装：
```
~$ sudo aptitude install libvte-dev 
```

进入gSTC-ISP目录，进行以下操作：
```
~$ sudo ./configure
~$ sudo make
~$ sudo make install
```



`备注：当前源码来自： https://sourceforge.net/projects/gstcisp/ 。
