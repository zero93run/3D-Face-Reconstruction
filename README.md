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

### 5.1 Algorithm

一般的基于图像的三维人脸建模方法可以大致分为图像数据采集、三维信息提取、三维模型构建和纹理映射等四个阶段。而其中的关键所在，就是如何把从图像中获取的这些原始的、可能含有噪声的三维信息，转化为一个特定的真实感的三维人脸模型，即所谓的三维模型构建。  
一种简单方法是在输入图像中提取特征点， 并计算出特征点的三维坐标，然后利用特征点的坐标对一个标准的人脸模型进行修改变形来得到照片中特定的人脸。

### 5.2 Asm

ASM是基于统计学习模型的特征点提取的一种方法。这个方法是95年就提出来的，不过至今仍是比较好的人脸特征点提取的方案。方法的提出人Tim Cootes后来还提出了aam算法，也很有名，但如果简单是特征点的定位的话，asm的效果可能更好一些。ASM跟大多数统计学习方法一样，也包括train和test（or fit）两部分，也就是形状建模build和形状匹配fit。

1.  Asm训练：搜集n个训练样本，树洞记录下每个训练样本中的k个关键点，构建训练集的形状向量，对形状向量进行归一化，对形状向量进行PCA处理，为每个特征点构建局部特征；
2.  Asm搜索：计算每个特征点的新位置，仿射变换中的参数更新。

### 5.3 DirectX

DirectX（Direct eXtension，简称DX）是由微软公司创建的多媒体编程接口。由C++编程语言实现，遵循COM。被广泛使用于Microsoft Windows、Microsoft XBOX、Microsoft XBOX 360和Microsoft XBOX ONE电子游戏开发，并且只能支持这些平台。DirectX是由很多API组成的，按照性质分类，可以分为四大部分，显示部分、声音部分、输入部分和网络部分。


## 6. Solution  

### 6.1 Algorithm 

1.  读入人脸模型、harr检测器和图像；
2.  利用opencv harr算法检测图像中人脸矩形区域；
3.  基于人脸矩形区域，利用asm算法提取人脸特征点；
4.  纹理映射。

### 6.2 Require

1. Directx9
2. Opencv
3. Asmlibrary

### 6.3 Result