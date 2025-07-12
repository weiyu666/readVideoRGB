# readVideoRGB
readVideoRGB是基于ffmpeg实现的编解码，读取视频每一帧中指定的坐标（x,y）的RGB值的demo小工具，也算是对ffmpeg的一种学习练习。

## 程序使用说明

**文件结构如下**：

![fileDirectory](E:\readVideoRGB\readme\fileDirectory.jpg) 

**程序**：readvideoRGB.exe 可以使用双击或者cmd下执行；

**配置文件**：readvideoRGB.config使用文本编辑器打开或记事本，修改成你需要读取的文件名称与XY坐标（XY的坐标范围是0到width-1,0到height-1），特别注意配置文件是ASCII编码只支持英文+符号+数字；

![config](E:\readVideoRGB\readme\config.jpg)

Video.mp4是测试视频，记得跟配置文件相对应，跟exe放在同一层级目录；

Reslut.txt 获取的每一帧对应XY坐标的RGB888数据：

特别注意Reslut.txt 不用手动清空再生成，程序自动会清空重新生成；

![result](E:\readVideoRGB\readme\result.png)

