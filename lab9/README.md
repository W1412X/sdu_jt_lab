# 计算机图形学实验9 RayBounding Valume求交和 BVH查找  
## [Github地址](https://github.com/W1412X/sdu_jt_lab/tree/main/lab9)
## [RBV,BVH参考文章](https://blog.csdn.net/qq_35312463/article/details/108419276)
## 前言  
### 光线追踪的基本流程  
- 像素网格生成：首先，我们需要为最终的图像建立一个二维像素网格。每个像素代表屏幕上的一个点，我们的目标是计算出这个点应该显示为什么颜色。
- 发射光线：对于每个像素，从相机的位置出发，沿着该像素指向场景的方向发射一条光线。这条光线是我们想象中的从观察者的眼睛到屏幕这一段空间中的延续。
- 光线与物体的交点检测：我们计算这条光线是否与场景中的任何物体相交，以及交点的位置。在这个例子中，就是判断光线是否击中了立方体的一个面。这通常涉及到射线与几何体求交的数学运算。
- 着色计算：
- 漫反射：如果光线击中了立方体，我们会计算光线击中面后的基本颜色（比如立方体表面的颜色）。这是最简单的光照模型，假设光线均匀地散射到各个方向。
  - 光照计算：点光源对这个交点的影响。计算光线从光源到交点的入射角，使用兰伯特余弦定律来确定光照强度。简单来说，光线垂直于表面时最亮，斜射时逐渐变暗。
  - 环境光与反射
- 累积颜色：根据上述计算得到的光照信息，为当前像素分配一个颜色值。如果光线没有击中任何物体，则可能根据背景色或环境光来着色。
- 重复步骤：对图像中的每个像素重复执行上述过程，直到所有像素的颜色都被计算出来，形成最终的图像。
## 实验部分
### 修改`Render`
- 和之前的实验8一样，修改一些东西
  - `dir`为对应的像素视锥体的方向向量
  - `ray`光线
  - 调用`scene`类中的`castRay`来返回光线与物体交点的颜色  
```cpp
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(-1, 5, 10);
    int m = 0;
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            Vector3f dir = normalize(Vector3f(x, y, -1)); // Don't forget to normalize this direction!
            // ori = eye_pos
            Ray ray(eye_pos, dir, 0);
            framebuffer[m++] = scene.castRay(ray, 0);
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
```
### 修改`Traingle.cpp`的`getInsection`函数
- 修改为返回一个`insection`对象，包含了交点的一些信息  
- 修改后的代码
```cpp
inline Intersection Triangle::getIntersection(Ray ray)
{
    Intersection inter;//交点信息结构体

    if (dotProduct(ray.direction, normal) > 0)//两者点积大于0，光线与三角形法向量方向一致，不会有交点
        return inter;
    double u, v, t_tmp = 0;
    Vector3f pvec = crossProduct(ray.direction, e2);
    double det = dotProduct(e1, pvec);
    if (fabs(det) < EPSILON)
        return inter;

    double det_inv = 1. / det;
    Vector3f tvec = ray.origin - v0;
    u = dotProduct(tvec, pvec) * det_inv;
    if (u < 0 || u > 1)
        return inter;
    Vector3f qvec = crossProduct(tvec, e1);
    v = dotProduct(ray.direction, qvec) * det_inv;
    if (v < 0 || u + v > 1)
        return inter;
    t_tmp = dotProduct(e2, qvec) * det_inv;//交点到沿光线到摄像机的距离  
    // TODO find ray triangle intersection
    //有交点，更新inter属性  
    inter.happened=true;
    inter.coords=ray(t_tmp);//计算对应的交点坐标(Ray重载了那个()操作符)
    inter.normal=normal;//法向量  
    inter.distance=t_tmp;//距离
    inter.obj=this;//交点所在的三角形  
    inter.m=m;//材质，类的成语阿
    return inter;
}
```

### RayBoundding Valume  
> 上一个实验我们直接计算从射线与每个三角形的交点，这里首先在场景中生成立方体，检测光线是否与立方体有交点，如果没有的话就直接跳过。有的话按照原来的方法算  
- Bounds3.hpp  
  > 这个是对应的包围盒的类，需要实现的是其中的  `IntersectP`  
  - 成员变量
    - pMax,p_Min就是立方体最大和最小的点(前边的左下方的点和后边的右上方的点)
  - 函数
    - Diagonal()：返回包围盒的对角线向量
    - maxExtent()：返回包围盒最大尺寸的方向索引（0对应X轴，1对应Y轴，2对应Z轴）
    - SurfaceArea()：计算并返回包围盒的表面积
    - Centroid()：计算并返回包围盒的中心点
    - Intersect(const Bounds3& b)：返回与另一个包围盒b相交部分的新包围盒
    - Offset(const Vector3f& p)：给定一个点p，计算该点在包围盒内的归一化偏移坐标
    - Overlaps(const Bounds3& b1, const Bounds3& b2)：判断两个包围盒是否重叠
    - Inside(const Vector3f& p, const Bounds3& b)：判断点p是否在包围盒b内部
    - 括号运算符重载operator[]：允许通过索引访问pMin，pMax
    - Union(const Bounds3& b1, const Bounds3& b2)：计算并返回包含两个包围盒的最小包围盒
    - Union(const Bounds3& b, const Vector3f& p)：计算并返回同时包含包围盒b和点p的最小包围盒
- 待实现`IntersectP(const Ray& ray, const Vector3f& invDir, const std::array<int, 3>& dirIsNeg)`
> 判断光线是否与包围盒相交
  - 传入参数  
    - `ray` : 光线  
    - `invDir` : 光线的方向向量的到数  
    - `dirIsNeg` : 光线在x,y,z轴上的正负  
    - 后两者的传入都是为了加快速度的  
  - 计算步骤  
    - 首先计算从光线的源点到立方体的两个点`pMin,pMax`的`x,y,z`方向的参数，用于代表光线到立方体的两个点的时间  
    - 根据光线在各轴上的方向（正或负），调整最小和最大的t值。如果光线在某轴上的方向为负（即向量分量为负），则进入包围盒的临界时间实际上是与最大边界相交的时间，退出时间是与最小边界相交的时间，那就需要交换t_Min和t_Max(自己比划一下就比较好理解了)  
    - 然后就是计算`x,y,z`轴各自t_Min中的最大值就是进入立方体的时间`t_enter`，`x,y,z`轴各自的t_Max的最小值就是立方体出来立方体的时间`t_exit`  
    - 如果t_exit>=0 而且 t_enter< t_exit那就说明相交了  
```cpp
inline bool Bounds3::IntersectP(const Ray& ray, const Vector3f& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), 
    //   use this because Multiply is faster that Division
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], 
    //   use this to simplify your logic
    // TODO test if ray bound intersects
 
    float t_Min_x = (pMin.x - ray.origin.x)*invDir[0];
    float t_Min_y = (pMin.y - ray.origin.y)*invDir[1];
    float t_Min_z = (pMin.z - ray.origin.z)*invDir[2];
    float t_Max_x = (pMax.x - ray.origin.x)*invDir[0];
    float t_Max_y = (pMax.y - ray.origin.y)*invDir[1];
    float t_Max_z = (pMax.z - ray.origin.z)*invDir[2];
    
    //dirIsNeg表面光线的方向，如果是正方向则为1，pmin-O为最短路径
    //反之为负方向0，pmax-O是最短路径
    if(!dirIsNeg[0])
    {
        float t = t_Min_x;
        t_Min_x = t_Max_x;
        t_Max_x = t;
    }
    if(!dirIsNeg[1])
    {
        float t = t_Min_y;
        t_Min_y = t_Max_y;
        t_Max_y = t;
    }
    if(!dirIsNeg[2])
    {
        float t = t_Min_z;
        t_Min_z = t_Max_z;
        t_Max_z = t;
    }
 
    float t_enter = std::max(t_Min_x,std::max(t_Min_y,t_Min_z));
    float t_exit =  std::min(t_Max_x,std::min(t_Max_y,t_Max_z));
    if(t_enter<t_exit&&t_exit>=0)
        return true;
    else
        return false;
    
}
```
### BVH  
> 递归判断应用上面我们写的算法  
> 在参考文章中详细的解释了这一部分的原理，我就不解释了  
```cpp
Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    std::array<int, 3> dirIsNeg; // 和getInsectin一样光线在各坐标轴方向的正负
    dirIsNeg[0] = (ray.direction[0] > 0); //x
    dirIsNeg[1] = (ray.direction[1] > 0); //y方向
    dirIsNeg[2] = (ray.direction[2] > 0); //z方向

    Intersection inter; //初始化一个表示相交信息的对象，初始默认为未找到交点

    // 如果当前的节点其包围盒与光线没有交点，则直接返回
    if (!node->bounds.IntersectP(ray, ray.direction_inv, dirIsNeg)) {
        return inter;
    }

    // 如果当前节点是叶子节点
    if (node->left == nullptr && node->right == nullptr) {
        //直接看看这个节点的包围盒中的物体与光线的交点信息
        return node->object->getIntersection(ray);
    }

    //左
    Intersection leftInter = getIntersection(node->left, ray);
    //右
    Intersection rightInter = getIntersection(node->right, ray);

    //比较左右子树返回的交点信息，返回距离光线源更近的那个交点信息
    return leftInter.distance < rightInter.distance ? leftInter : rightInter;
}
```