> **本文只显示公式，并未做推导**
- 本文的所有代码均基于以下四篇文章中的算法实现

##  参考文章
[**Blinn-Phong原理与推导参考文章**](https://zhuanlan.zhihu.com/p/442023993)  
[**双线性插值原理与推导参考文章**](https://blog.csdn.net/eurus_/article/details/102755898)  
[**坐标插值与透视矫正原理与推导参考文章**](https://zhuanlan.zhihu.com/p/448575965)  
[**bump phong原理与推导参考文章**](https://blog.csdn.net/yjr3426619/article/details/81022781)  
[****]

## Github 文件结构
- Code文件夹下为实验要求的原始实现
  - 对应的rock.png为双线性插值的结果，与code_rock中的rock.png做对比
  - rock.png文件的生成需要修改main函数的中的路径
- code_rock文件夹下为不使用双线性插值的版本
  - 因为实验要求选取更小的纹理，所以这里选择了rock对比，**比较明显**
  - code_rock文件夹下的rock.png为采用纹理插值的结果

## 尽量试试其他的模型做对比，那个bunny模型太小了，我用blender打开几乎都看不见

## 标注了对每一个 文件/函数 的改动
### `rasterize_triangle`

#### 重心坐标插值(对应`computeBarycentric2D`函数)
$$
\beta = \frac{(y-y_A)(x_C-x_A)-(x-x_A)(y_C-y_A)}{(y_B-y_A)(x_B-x_A)-(x_C-x_A)(y_B-y_A)}
$$
$$
\gamma = \frac{(y-y_A)(x_B-x_A)-(x-x_A)(y_B-y_A)} {(y_C-y_A)(x_B-x_A)-(x_C-x_A)(y_B-y_A)}
$$
$$
\alpha = 1- \beta - \gamma
$$  
#### 透视矫正
$$
\frac {1}{Z} = {\alpha}_` \frac{1}{Z_A}+ {\beta}_` \frac{1}{Z_B}+ {\gamma}_` \frac{1}{Z_C}
$$ 
```cpp
void rst::rasterizer::rasterize_triangle(const Triangle &t, const std::array<Eigen::Vector3f, 3> &view_pos)
{
    auto v = t.toVector4();
    Vector3f color;
    float alpha, beta, gamma, lmin = INT_MAX, rmax = INT_MIN, tmax = INT_MIN, bmin = INT_MAX, id;
    for (auto &k : v){ //找到可以包围这个三角形的矩形坐标
        lmin = int(std::min(lmin, k.x()));
        rmax = std::max(rmax, k.x());
        rmax = rmax == int(rmax) ? int(rmax) - 1 : rmax;
        tmax = std::max(tmax, k.y());
        tmax = tmax == int(tmax) ? int(tmax) - 1 : tmax;
        bmin = int(std::min(bmin, k.y()));
    }
    for (float i = lmin; i <= rmax; i++)//在矩形内遍历所有点
    {
        for (float j = bmin; j <= tmax; j++){
            id = get_index(i, j);
            if (insideTriangle(i + 0.5, j + 0.5, t.v))//取像素中心作为判断
            { // 如果像素在三角形内
                // If so, use the following code to get the interpolated z value.
                std::tie(alpha, beta, gamma) = computeBarycentric2D(i + 0.5, j + 0.5, t.v);//计算得到重心
                float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;

                if (-z_interpolated < depth_buf[id])
                {                                                                                                                                                 // 如果该像素的深度更小，更新像素深度、颜色表
                    auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);                                             // 颜色插值
                    auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1).normalized();                            // 法向量插值
                    auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);                          // 纹理坐标插值
                    auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);                                  // 着色点坐标插值
                    fragment_shader_payload payload(interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr); // 将插值属性传入fragment_shader_payload
                    payload.view_pos = interpolated_shadingcoords;                                                                                                // 传入原顶点坐标
                    depth_buf[id] = -z_interpolated;
                    frame_buf[id] = fragment_shader(payload); // 使用shader计算颜色
                    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
                    set_pixel({i, j}, frame_buf[id]);
                }
            }
        }
    }
}
```

### Texture.hpp (在`texture_fragment_shader`中使用)
> 添加了双线性插值的算法`getColorBilinear`,传入的参数为从左下角开始，插值处所在的宽高比
```cpp
    Eigen::Vector3f getColorBilinear(float u, float v){//传入纹理宽高比 u:v
        float w1 = int(u * width), h1 = int(v * height);//左下角的像素
        float w2 = w1 + 1, h2 = h1;//右下
        float w3 = w1, h3 = h1 + 1;//左上
        float w4 = w1 + 1, h4 = h1 + 1;//右上
        Eigen::Vector3f color1, color2, color3, color4, color5, color6, color;
        color1 = getColor(w1 / width, h1 / height);//左下
        color2 = getColor(w2 / width, h2 / height);//右下
        color3 = getColor(w3 / width, h3 / height);//左上
        color4 = getColor(w4 / width, h4 / height);//右上
        /*
        左上，右上插值得到一个
        左下，右下得到一个
        */
        color5 = color1 + (color2 - color1) * (u * width - w1);//下
        color6 = color3 + (color4 - color3) * (u * width - w1);//上
        color = color5 + (color6 - color5) * (v * height - h1);//在y方向插值
        return color;//结果
    }
```
### `phong_fragment_shader`
- $nomal$是对应的物体被照射平面的法向量
- $light\_dir$是对应**物体**到**光源**的方向向量
- $view\_dir$是对应的**物体**到**观察者**的方向向量
- $K_d$是对应的慢射光系数，其实就是物体对应的RGB
- $half_vector$是对应的半程向量
- $r$是对应的光源到物体的距离，这里$r^2=light\_dir^2$
- $p$是对应的加快光的衰减速度的
#### 计算漫射光的公式
$$
L_d=K_d\cdot(1/r^2)\cdot max(0,normal \cdot light\_dir)
$$
#### 计算镜面反射中的高光的公式
- 半程向量
    $$
    half_vector=(light\_dir+view\_dir).normalize
    $$
- 结果
    $$
    L_s=K_s(I/r^2)\cdot max(0,half\_vector \cdot view\_dirs)^p
    $$
```cpp
Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);//环境光
    Eigen::Vector3f kd = payload.color;//传入的颜色作为漫反射系数
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);//镜面反射

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};//光源1
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};//光源2

    std::vector<light> lights = {l1, l2};//啥也不是
    Eigen::Vector3f amb_light_intensity{10, 10, 10};//基础照明水平
    Eigen::Vector3f eye_pos{0, 0, 10};//摄像机pos

    float p = 150;//随距离光的衰减

    Eigen::Vector3f color = payload.color;//原来的色
    Eigen::Vector3f point = payload.view_pos;//位置
    Eigen::Vector3f normal = payload.normal;//法线
    Eigen::Vector3f result_color = {0, 0, 0};//返回的这一点的颜色

    for (const auto& light : lights){
        //计算向量
        Eigen::Vector3f light_dir = light.position - point; //点到光源的向量
        Eigen::Vector3f light_dir_ori=light_dir;
        light_dir.normalize(); //归一
        Eigen::Vector3f view_dir = (eye_pos - point).normalized()+light_dir; //点到观察者的向量
        Eigen::Vector3f half_vec = (light_dir + view_dir).normalized(); //半角向量，即视线向量与光线方向向量的和的归一化结果
        //计算光照
        //环境光分量
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);//ka是一个常量
        //漫反射分量
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / ((light_dir_ori).dot(light_dir_ori))) * std::fmax(0, normal.dot(light_dir));//rgb*光强*物体表面的rgb系数
        //镜面反射分量
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / ((light_dir_ori).dot(light_dir_ori))) * pow(std::fmax(0, normal.dot(half_vec)), p);//rgb*闪度*系数
        // 累加到结果颜色
        result_color += (ambient + diffuse + specular);
    }
    return result_color * 255.f;
}
```

#### `texture_fragment_shader`  
- 基本上和`phong_fragment_shader`差不多，就是需要提取对应的纹理RGB
```cpp
Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{//基于纹理的计算
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)//判断是否有纹理传入
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        return_color = payload.texture->getColorBilinear(payload.tex_coords.x(), payload.tex_coords.y());//双线性插值
    }
    Eigen::Vector3f texture_color;//纹理颜色
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (const auto& light : lights){
        //计算向量
        Eigen::Vector3f light_dir = light.position - point; //点到光源的向量
        Eigen::Vector3f light_dir_ori=light_dir;
        light_dir.normalize(); //归一
        Eigen::Vector3f view_dir = (eye_pos - point).normalized()+light_dir; //点到观察者的向量
        Eigen::Vector3f half_vec = (light_dir + view_dir).normalized(); //半角向量，即视线向量与光线方向向量的和的归一化结果
        //计算光照
        //环境光分量
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);//ka是一个常量
        //漫反射分量
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / ((light_dir_ori).dot(light_dir_ori))) * std::fmax(0, normal.dot(light_dir));//rgb*光强*物体表面的rgb系数
        //镜面反射分量
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / ((light_dir_ori).dot(light_dir_ori))) * pow(std::fmax(0, normal.dot(half_vec)), p);//rgb*闪度*系数
        // 累加到结果颜色
        result_color += (ambient + diffuse + specular);
    }

    return result_color * 255.f;
}
```
#### `bump_mapping`
- 通过扰动法向量实现凹凸
- 实现思路
  - 首先算出`TBN`矩阵用于切线坐标系和世界坐标系的转换
  - 分别计算沿着`x(u)`,`y(v)`轴的颜色变化的范数(这里是L2范数)，利用k_h,k_n控制光强在范围内,得到d_U,d_V
  - 利用d_U,d_V计算扰动法向量
  - 直接利用扰动法向量(利用`TBN`矩阵转移到世界坐标系)和`Blinn-phong`算法计算
- 切线空间TBN
  - 法向量 $nomal$
  - 切线 $t$
  - 副切线 $b$
  > 副切线由法向量与切线的差成得到 $normal \times t$
- 法向量由切线空间过渡到世界空间
  - `TBN`:由切线空间过渡到世界空间的过渡矩阵
  - `TBN`矩阵
  $$
  \begin {bmatrix}
  T_x&B_x&N_x \\
  T_y&B_y&N_y \\
  T_z&B_z&N_z
  \end {bmatrix} 
  $$
  - 世界空间下的法向量
  $$
  \begin {bmatrix}
  X_w&Y_w&Z_w
  \end {bmatrix}
  $$
  - 切线空间下的法线向量
  $$
  \begin {bmatrix}
  X_t&Y_t&Z_t
  \end {bmatrix}
  $$
  - 公式
  $$
  \begin {bmatrix}
  X_t&Y_t&Z_t
  \end {bmatrix}
  \cdot
  \begin {bmatrix}
  T_x&B_x&N_x \\
  T_y&B_y&N_y \\
  T_z&B_z&N_z
  \end {bmatrix} 
  =
  \begin {bmatrix}
  X_w&Y_w&Z_w
  \end {bmatrix}
  $$
```cpp
Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;


    float kh = 0.2, kn = 0.1;//kh高度缩放因子  kn位移强度 

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)
    float x, y, z;
    Vector3f t, b;
    x = normal.x(), y = normal.y(), z = normal.z();
    t << x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z);
    b = normal.cross(t);//叉积求副切

    Matrix3f TBN;
    TBN << t.x(), b.x(), normal.x(),
           t.y(), b.y(), normal.y(),
           t.z(), b.z(), normal.z();//TBN矩阵

    float u, v, w, h;//纹理坐标+宽高
    u = payload.tex_coords.x();
    v = payload.tex_coords.y();
    w = payload.texture->width;
    h = payload.texture->height;

    float dU = kh * kn * (payload.texture->getColorBilinear(u + 1.0 / w,v).norm() - payload.texture->getColorBilinear(u,v).norm());//
    float dV = kh * kn * (payload.texture->getColorBilinear(u,v + 1.0 / h).norm() - payload.texture->getColorBilinear(u,v).norm());

    Vector3f ln;
    ln << -dU, dV, 1;

    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};
    for (const auto& light : lights){
        //计算向量
        Eigen::Vector3f light_dir = light.position - point; //点到光源的向量
        light_dir.normalize(); //归一
        Eigen::Vector3f view_dir = eye_pos - point+light_dir; //点到观察者的向量
        view_dir.normalize(); //归一
        Eigen::Vector3f half_vec = (light_dir + view_dir).normalized(); //半角向量，即视线向量与光线方向向量的和的归一化结果
        //计算光照
        //环境光分量
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);//ka是一个常量
        //漫反射分量
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / ((light_dir).dot(light_dir))) * std::fmax(0, normal.dot(l));//rgb*光强*物体表面的rgb系数
        //镜面反射分量
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / ((light_dir).dot(light_dir))) * pow(std::fmax(0, normal.dot(half_vec)), p);//rgb*闪度*系数
        // 累加到结果颜色
        result_color += ambient + diffuse + specular;
    }

    return result_color * 255.f;
}
```

#### `displacement_fragment_shader`
- 通过位移计算凹凸
- 大部分的思路和法向量扰动差不多，主要是他还直接改变了物体上的点的位置
`point += kn * normal * payload.texture->getColorBilinear(u,v).norm();//直接改变视角`  
- 实现思路
  - 首先算出`TBN`矩阵用于切线坐标系和世界坐标系的转换
  - 分别计算沿着`x(u)`,`y(v)`轴的颜色变化的范数(这里是L2范数)，利用k_h,k_n控制光强在范围内,得到d_U,d_V
  - 利用d_U,d_V计算扰动法向量,后续更新时利用扰动向量更新
  - 这里对应的需要更新物体表面点`point`
    - kn用于控制位移的程度
    - 利用`normal`法向量确定如何位移，即位移的方向
    - 最后乘上对应的双线性插值结果，就是新的点位变化量
    - 加上变化量，就是新的点位
  - 利用扰动法向量和对应的新的点位和`Blinn-phong`算法计算
```cpp
Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)
    float x, y, z;
    Vector3f t, b;
    x = normal.x(), y = normal.y(), z = normal.z();
    t << x * y / sqrt(x * x + z * z), sqrt(x * x + z * z), z * y / sqrt(x * x + z * z);
    b = normal.cross(t);

    Matrix3f TBN;
    TBN << t.x(), b.x(), normal.x(),
           t.y(), b.y(), normal.y(),
           t.z(), b.z(), normal.z();

    float u, v, w, h;
    u = payload.tex_coords.x();
    v = payload.tex_coords.y();
    w = payload.texture->width;
    h = payload.texture->height;

    float dU = kh * kn * (payload.texture->getColorBilinear(u + 1.0 / w,v).norm() - payload.texture->getColorBilinear(u,v).norm());
    float dV = kh * kn * (payload.texture->getColorBilinear(u,v + 1.0 / h).norm() - payload.texture->getColorBilinear(u,v).norm());

    Vector3f ln;
    ln << -dU, dV, 1;

    point += kn * normal * payload.texture->getColorBilinear(u,v).norm();//直接改变物体点的位置
    normal = (TBN * ln).normalized();
    Eigen::Vector3f result_color = {0, 0, 0};
    for (const auto& light : lights){
        //计算向量
        Eigen::Vector3f light_dir = light.position - point; //点到光源的向量
        light_dir.normalize(); //归一
        Eigen::Vector3f view_dir = eye_pos - point+light_dir; //点到观察者的向量
        view_dir.normalize(); //归一
        Eigen::Vector3f half_vec = (light_dir + view_dir).normalized(); //半角向量，即视线向量与光线方向向量的和的归一化结果
        //计算光照
        //环境光分量
        Eigen::Vector3f ambient = ka.cwiseProduct(amb_light_intensity);//ka是一个常量
        //漫反射分量
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / ((light_dir).dot(light_dir))) * std::fmax(0, normal.dot(l));//rgb*光强*物体表面的rgb系数
        //镜面反射分量
        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / ((light_dir).dot(light_dir))) * pow(std::fmax(0, normal.dot(half_vec)), p);//rgb*闪度*系数
        // 累加到结果颜色
        result_color += ambient + diffuse + specular;
    }
    return result_color * 255.f;
}
```