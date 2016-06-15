# 3D Face Reconstruction

## 1. Team

 自动化学院   
 倪子涵 杨丽秦 苏伟 彭军才

## 2. Introduction

 In computer vision and computer graphics, 3D face reconstruction is the process of capturing the shape and appearance of human faces. This process can be accomplished either by active or passive methods. If the model is allowed to change its shape in time, this is referred to as non-rigid or spatiotemporal reconstruction.  
 Please refer to the following videos to better under what is 3D face reconstruction:   
 https://www.youtube.com/watch?v=fu7bTemvEKk (YouTube video)  
 https://www.youtube.com/watch?v=PhdT8MzIg_0 (YouTube video)
 
## 3. Reference

1. Total Moving Face Reconstruction, In ECCV 2014.  http://grail.cs.washington.edu/projects/totalmoving/
2. 3D Face Reconstruction from a Single Image using a Single Reference Face Shape, In TPAMI.
3. State-of-the-Art 3D facial reconstruction methods for face recognition
based on a single 2D training image per person, In Pattern Recognition Letters,2009.
4. Model-Assisted 3D Face Reconstruction from Video.

## 4. Time Line

1. 2016-05-19 —— 2016-05-31  
创建Github主页  [https://github.com/zero93run/3D-Face-Reconstruction](https://github.com/zero93run/3D-Face-Reconstruction)
2. 2016-05-31 —— 2016-06-01  
阅读参考文献  “Total Moving Face Reconstruction, In ECCV 2014”.
3. 2016-06-02 —— 2016-06-03   
阅读参考文献   “3D Face Reconstruction from a Single Image using a Single Reference Face Shape, In TPAMI”.
4. 2016-06-03 —— 2016-06-04  
阅读参考文献  "State-of-the-Art 3D facial reconstruction methods for face recognition based on a single 2D training image per person, In Pattern Recognition Letters,2009".
5. 2016-06-04 —— 2016-06-05   
确定项目解决方案：首先利用asm算法提取人脸特征点，通过特征点来构建3D模型，然后利用directX将2D纹理映射到3D人脸模型。
6. 2016-06-05 —— 2016-06-07  
 利用Asmlibrary算法提取人脸特征点，构建3D模型。
7. 2016-06-06 —— 2016-06-09   
利用directX将2D纹理映射到3D人脸模型。
8. 2016-06-10 —— 2016-06-13  
利用MFC开发项目界面。
9. 2016-06-12 —— 2016-06-14  
完善项目网站更新。 

## 5. Learning notes

### 5.1 3D Face Reconstruction

一般的基于图像的三维人脸建模方法可以大致分为图像数据采集、三维信息提取、三维模型构建和纹理映射等四个阶段。而其中的关键所在，就是如何把从图像中获取的这些原始的、可能含有噪声的三维信息，转化为一个特定的真实感的三维人脸模型，即所谓的三维模型构建。  
一种简单方法是在输入图像中提取特征点， 并计算出特征点的三维坐标，然后利用特征点的坐标对一个标准的人脸模型进行修改变形来得到照片中特定的人脸。  
三维人脸模型及建模所采用的主要方法包括人脸参数模型、生理肌肉模型、基于视频的建模、基于图像的建模以及基于形变模型的方法。  
纹理映射技术也叫纹理贴图技术，它是将图像映射到3D物体上的一种技术。纹理分为颜色纹理和凸包纹理两种，颜色纹理是通过颜色色彩或明暗度的变化体现出来的表面细节， 凸包纹理是由于不规则的细小凹凸造成的。颜色纹理贴图的一般方法是： 在一个平面区域(即纹理空间)上预先定义纹理图像，然后建立物体表面的点与纹理空间的点之间的对应——映射，当物
体表面的可见点确定之后，以纹理空间的对应点的值乘以亮度值，就可把纹理图像附到物体表面上。凸包纹理贴图的方法与颜色纹理贴图的方法类似，只是纹理值作用在法向量上，而不是颜色亮度上。人脸图像是一种颜色纹理，采用颜色纹理贴图的方法。

### 5.2 Asm

ASM是基于统计学习模型的特征点提取的一种方法。这个方法是95年就提出来的，不过至今仍是比较好的人脸特征点提取的方案。方法的提出人Tim Cootes后来还提出了aam算法，也很有名，但如果简单是特征点的定位的话，asm的效果可能更好一些。ASM跟大多数统计学习方法一样，也包括train和test（or fit）两部分，也就是形状建模build和形状匹配fit。

1.  Asm训练：搜集n个训练样本，树洞记录下每个训练样本中的k个关键点，构建训练集的形状向量，对形状向量进行归一化，对形状向量进行PCA处理，为每个特征点构建局部特征；
2.  Asm搜索：计算每个特征点的新位置，仿射变换中的参数更新。

### 5.3 DirectX

DirectX（Direct eXtension，简称DX）是由微软公司创建的多媒体编程接口。由C++编程语言实现，遵循COM。Microsoft DirectX 是这样一组技术：它们旨在使基于Windows 的计算机成为运行和显示具有丰富多媒体元素（例如全色图形、视频、3D 动画和丰富音频）的应用程序的理想平台。DirectX 包括安全和性能更新程序，以及许多涵盖所有技术的新功能。应用程序可以通过使用DirectX API 来访问这些新功能。DirectX是由很多API组成的，按照性质分类，可以分为四大部分，显示部分、声音部分、输入部分和网络部分。本项目只涉及显示部分，它是图形处理的关键，分为DirectDraw（DDraw）和 Direct3D（D3D），前者主要负责2D图像加速。它包括很多方面：我们播放mpg、DVD电影、看图、玩小游戏等等都是用的DDraw，你可以把它理解成所有划线的部分都是用的DDraw。后者则主要负责3D效果的显示，比如CS中的场景和人物、FIFA中的人物等等，都是使用了DirectX的Direct3D。  
前台缓冲区和后台缓冲区是位于系统内存或显存里的内存块，对应于将要显示的二维显示区域。前台缓冲区是显示在显示屏上的我们可以看到的内容，而后台缓冲区则主要用于图形绘制的准备工作。在后台缓冲区中的内容准备好之后，就可以和前台缓冲区进行一个交换操作，进行翻转显示，将场景显示出来。  
Direct3D初始化四步曲:  1、创建Direct3D接口对象,以便用该Direct3D对象创建Direct3D设备对象；2、获取硬件设备信息；3、填充D3DPRESENT_PARAMETERS结构体,指定我们需要在哪个窗口上进行绘制，我们通常都填hwnd；4、创建Direct3D设备接口:用CreateDevice把hwnd和Direct3D设备联系起来。  
Direct3D绘制五步曲：清屏操作， 开始场景， 正式绘制， 结束场景， 翻转显示。  
其中，纹理渲染的具体步骤如下：  1、 创建纹理并获得纹理的表面（Surface），2、 向纹理的表面渲染场景，3、 渲染纹理本身。


## 6. Solution  

### 6.1 Algorithm 

1. 对人脸图像进行滤波去噪等预处理；
2. 利用opencv harr算法检测图像中人脸矩形区域；
3. 利用asm算法在人脸矩形区域内提取人脸特征点；
4. 根据人脸特征点和标准人脸模型重建生成特定三维人脸模型；
5. 从原始图片提取人脸纹理信息，进行纹理重构和映射，利用directX生成可视化三维人脸模型。

### 6.2 Require

1. Directx9
2. Opencv
3. Asmlibrary

### 6.3 Result
![image]([https://github.com/zero93run/3D-Face-Reconstruction/blob/master/img.png](https://github.com/zero93run/3D-Face-Reconstruction/blob/master/img.png))