# 山东大学计算机图形学实验7
## [Github地址](https://github.com/W1412X/sdu_jt_lab/tree/main/lab7)
## 前言  
- 5.20 今天早上写实验报告发现代码有点问题，修正了一下
- 5.19 本实验因为周日问的没有语言要求，所以用`python`写了  
### `python`环境配置
- `python 3.11.5`
- `cv2`
```shell
pip install opencv-python
```
- 没有网的话换个镜像源装  
- 另外对于  使用`pyenv`管理全局环境时利用`virtualenv`创建虚拟环境出现`No module named 'pip._vendor.six'`问题 [**参见文章**](https://blog.csdn.net/m0_73802120/article/details/139033981)
## 正文  
> 本实验我利用python生成mipmap序列图像，然后在c++中读取图像并进行trilinear处理和纹理映射 (因为我周日问助教说是什么语言就可以，我也没看后面的要求，我就先用python写了，后来才发现还得用原来的c++代码)  
### python代码  
- 最好换个图像吧，看一下python代码的逻辑，换成其他的1024*1024的png格式图像生成新的mipmap序列图像，就是把lab7目录下的test.png文件换乘其他的，注意格式。图片大小转换网上有很方便的网站，也可以自己写python代码  
### c++修改部分
#### `main.cpp`
  - 添加了`CalculateLevelForTrilinear`用于计算选取mipmap序列图像的哪一个也就是`level`  
  - 仅修改`texture_fragment_shader`为使用`trilinear`插值，所以运行时运行
  ```shell
  ./Rasterizer output.png texture
  ```  
#### `texture.hpp`
  - 添加了三次插值`getColortrilinear`的方法 
    - 根据选择的`level`确定插值的两个`mipmap`序列图像
  - 修改构造函数逻辑为读取一系列使用python生成的mipmap序列图像
  - 修改了对应`getColor`，`getColorBilinear`的逻辑，主要是选择不同的mipmap序列图像作为纹理
#### `shader.cpp`
  - 对于`fragment_shader_payload`类添加了属性`u_right`,`v_top`用于存储改纹理点的上方，右侧的坐标信息
  - 修改了构造函数 
#### `rasterizer.cpp`
  - 添加了计算`u_right`,`v_top`的代码
    -通过调整三角形的顶点位置来确定对应的`u_right`,`v_top`。即分别计算三角形向右平移一个单位与向上平移一个单位的重心坐标。

