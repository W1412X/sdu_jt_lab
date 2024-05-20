# 山东大学计算机图形学实验7
## [Github地址](https://github.com/W1412X/sdu_jt_lab/tree/main/lab7)
## 前言  
> 本实验因为周日问的没有语言要求，所以用`python`写了  
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
- `texture.hpp`
  - 添加了三次插值`trilinear`的方法 
  - 修改构造函数逻辑为读取一系列使用python生成的mipmap序列图像
- `fragment_shader_payload`
  - 添加了属性`u_right`,`v_top`
  - 修改了构造函数 
- `main.cpp`
  - 添加了`CalculateLevelForTrilinear`用于计算选取mipmap序列图像的哪一个也就是`level`  
  - 仅修改`texture_fragment_shader`为使用`trilinear`插值，所以运行时运行
  ```shell
  ./Rasterizer output.png texture
  ```  
- `rasterizer.cpp`
  - 添加了计算`u_right`,`v_top`的代码，采用了新的`texture_fragment_shader`构造函数  

