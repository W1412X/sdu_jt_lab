#  山东大学计算机图形学实验8  
## [Github地址](https://github.com/W1412X/sdu_jt_lab/tree/main/lab8)

> **本文只显示公式，并未做推导**

##  参考文章  
- [**Möller–Trumbore 算法**](https://zhuanlan.zhihu.com/p/451582864)
## Render()  
### 说这个之前，先说一下这个`Scene`类  
- `Scene` 
  - `width`,`height`为场景的高和宽
  - `fov`为视场角([**具体定义参考**](https://zhuanlan.zhihu.com/p/679453452))
  - 其他的这个函数没用
### `scale`  用于确定屏幕上的单位长度在3D空间中的对应距离  
$$  

$$  
### 代码解释  (直接解释一下这个x，y是差不多)
- `float x = (2 * ((i + 0.5) / scene.width -0.5) * imageAspectRatio) * scale;`
  - `2 * ((i + 0.5) / scene.width -0.5)`
    - 计算对应的像素在场景中的位置并映射到[-1,1]的区间内  
  - ` * imageAspectRatio`  
    - 因为要保证视觉效果麻，感性理解一下就是  
      - 如果一个屏幕是比较宽的，那么对应的x就应该相对于原来的x比较大进行对应的缩放麻  
  - ` * scale;`  
    - 根据视场角调整对应的x，y。视场角越大，物体像素离观察者越近，对应的`dir`也应该越大   
- `y`的计算
  - 实践过程中我的`y`本来是没有乘 -1 的，但是图像是倒者的，我就乘了个 -1 调整一下  
## `Triangle.cpp`  
> 直接上公式   
- 一些变量
  - `ori` 起点
  - `dir` 方向向量
-  Moller-Trumbore 算法
  -  向量 S 表示从三角形的一个顶点到射线起点ori的向量  
  $$ S=ori $$
  - 三角形两个边(E1,E2)的向量  
  $$ E1=v1-v0 $$  
  $$ E2=v2-v0 $$
  - 计算向量 dir 与 E2 的叉乘结果 S1  ,  用于构造平面法向量相关的计算  
  $$ S1 = crossProduct(dir, E2) $$  
  - 计算向量 S 与 E1 的叉乘结果 S2  , 用于后续求解 t, u, v  
  $$ S2 = crossProduct(S, E1) $$  
  - 计算 S1 与 E1 的点乘，避免后边的除零错误  
  $$ S1E1 = dotProduct(S1, E1) $$  
  - 射线与三角形平面的交点对应的参数 t  
  $$ t = dotProduct(S2, E2) / S1E1 $$  
  - 计算 b1 和 b2，它们分别代表交点在 E1 方向和射线方向上的权重，用于确定交点在三角形内的 UV 坐标  
  $$ b1 = dotProduct(S1, S) / S1E1 $$  
  $$ b2 = dotProduct(S2, dir) / S1E1 $$  
### **判断**
  - 如果 t>=0，交点在射线正方向  (1)
  - 如果b1 >= 0, b2 >= 0 且 (1 - b1 - b2) >= 0 ，交点位于三角形内部    (2)
  - (1)(2)均满足，在三角形内  
  - 反之在外
