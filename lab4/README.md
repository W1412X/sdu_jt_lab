## IO.h

### 定义

本文件定义了用于表示简单几何模型（如多面体）的数据结构和相关操作。主要包含以下类：

- `Vertex`：表示一个顶点，包含顶点的度、编号、细分后的编号、所在面的编号以及与之相连的边的编号列表。
- `Edge`：表示一条边，包含边的编号、中心点坐标、中心点编号、连接的两个顶点编号以及相邻面的编号列表。
- `Face`：表示一个面，包含面的边数、编号、中心点坐标、中心点编号、构成面的顶点列表以及边列表。
- `IO`：提供对模型数据的输入/输出功能，包括从文件中读取模型数据、构建内部数据结构，并提供查询是否存在特定边的方法。

### Vertex

#### 成员变量

- `int degree`: 顶点的度，即与该顶点相连的边数。
- `int id`: 顶点的唯一编号。
- `int updated_id`: 细分后顶点的编号。
- `int face`: 顶点所在的面的编号。
- `std::vector<int> connect_edges`: 存放与该顶点相连的边的编号列表。
- `float x, y`: 顶点的二维坐标。

#### 构造函数

- `Vertex(const float &a, const float &b, const int &i)`: 使用给定的二维坐标 `(a, b)` 和编号 `i` 创建顶点，初始化连接边列表为空。
- `Vertex()`: 默认构造函数，创建一个无坐标、编号为默认值且连接边列表为空的顶点。

### Edge

#### 成员变量

- `int id`: 边的唯一编号。
- `float cen_x, cen_y`: 边的中心点坐标。
- `int cen_id`: 边的中心点编号。
- `int v1, v2`: 边连接的两个顶点的编号。
- `std::vector<int> near_face`: 存放与该边相邻的面的编号列表。

#### 构造函数

- `Edge(int &a, int &b, std::vector<Vertex> &vertex_se, int &id2)`: 使用给定的顶点编号 `a`、`b`、顶点集引用 `vertex_se` 和边编号 `id2` 创建边。同时细分关联顶点的度和连接边列表，初始化相邻面列表为空。
- `Edge()`: 默认构造函数，创建一个无关联顶点、编号和相邻面列表为空的边。

### Face

#### 成员变量

- `int edge_num`: 面的边数。
- `int id`: 面的唯一编号。
- `float cen_x, cen_y`: 面的中心点坐标。
- `int cen_id`: 面的中心点编号。
- `std::vector<int> vertex`: 存放构成面的顶点编号列表。
- `std::vector<int> edge`: 存放构成面的边编号列表。

#### 构造函数

- `Face(int i)`: 使用给定的编号 `i` 创建面，初始化顶点和边列表为空。
- `Face()`: 默认构造函数，创建一个无编号、顶点和边列表为空的面。

### IO

#### 成员变量

- `std::string file_name`: 存储模型数据文件名。
- `std::vector<Vertex> vertex_set`: 存放所有顶点的集合。
- `std::vector<Edge> edge_set`: 存放所有边的集合。
- `std::vector<Face> face_set`: 存放所有面的集合。

#### 成员函数

- `int if_exists_edge(const int &a, const int &b)`: 查询是否存在连接顶点 `a` 和 `b` 的边。如果存在，返回边的编号；否则返回 -1。

#### 构造函数

- `IO()`: 默认构造函数，创建一个空的模型数据管理对象。
- `IO(std::string file_n)`: 使用给定的文件名 `file_n` 构造模型数据管理对象。读取指定文件中的模型数据，解析并填充 `vertex_set`、`edge_set` 和 `face_set`。同时，计算每个面的边列表和每个边的相邻面列表。

解析文件格式约定：
- 文件以字符 `'v'` 开始的行表示顶点，格式为：`v x y`
- 文件以字符 `'e'` 开始的行表示边，格式为：`e v1 v2`
- 文件以字符 `'f'` 开始的行表示面，格式为：`f v1 v2 ... vn`，其中 `vn` 是面最后一个顶点的编号


## Catmull.h
### 定义
本文件是算法的实现部分，定义了一个Catmull类
### Catmull类
#### 成员变量
- `last_vertex_set` 存储上一轮的细分的顶点
- `last_edget_set`  存储上一轮细分的边
- `last_face_set`  存储上一轮细分的面
- `updated_vertex_set`  用于存储细分中细分的顶点
- `updated_edge_set`  用于存储细分中细分的边
- `updated_face_set` 用于存储细分中细分的面
- `face_vertex` 用于存储细分中计算得到的面中心的点
- `vertex_update_by_last_vertex` 用于存储上一轮顶点细分后的顶点
- `vertex_id` `face_id` `edge_id` 用于为本轮细分产生的顶点，边，面进行编号
#### 构造函数
- 利用IO.h类读取数据，并存储读取到的数据作为上一轮的数据
#### prepare()
- 计算面中心点和边中心点
#### update_vertex()
- 根据面中心点以及边中心点以及上一轮的顶点这一轮由上一轮的顶点细分得到的顶点的位置
#### save_new_vertex()
- 存储新的顶点到updated_vertex_set
- 新的顶点来源于三种
  - 由原来的顶点变换而来的
  - 生成的边点
  - 生成的面点
- 存储这一轮细分得到的顶点的位置，为了方便后续新一轮的边和面以及边，面，点三者关系的细分与记录，需要将此时的细分后的顶点的id一并存入上一轮的变量中。具体的
  - 在Vertex中的updated_id中存储由这个顶点变换得到的新顶点的位置
  - 在Egde中的cen_id记录这个边边点对应的在新的顶点集中位置
  - 在Face中的cen_id记录这个面的面点对应的在新的顶点集中的位置
#### save_new_edge_face()
- 存储新的边以及面到updated_face_set,updated_edge_set
- 边和面的生成规则
  - 以面点为核心，每个面点与一个这个面内的顶点以及对应的两个边点构成一个面

- 两层遍历
  - 第一层遍历上一轮的面
  - 第二层遍历这个面对应的顶点
- 由于在存储新的顶点的时候将新的顶点的编号存在了上一轮的面，边以及顶点中，所以可以直接在存储新的边和面时使用新的顶点编号
> 具体实现自己看代码吧，感觉很难说清楚
#### generator()
- 对细分的过程进行了整合并为下一轮细分的变量做初始化
#### out_re()
- 调试的时候用，输出了点信息，边信息，以及面信息
## Draw.h
### 定义
本文件是实现Opencv画图的类
### Draw 类
#### 成员变量
- `window` 画图的窗口
#### std::pair<int,int> convert(const Vertex&v)
- 用于进行坐标转换，将输入的.obj文件中坐标转换为显示在窗口的坐标
#### draw_line(const Vertex&v1,const Vertex&v2,char c='b')
- 传入边顶点以及颜色画边
#### draw_point(const Vertex&v,bool if_num=false,char c='r')
- 传入顶点以及顶点颜色画点以及可选是否显示顶点编号
- 颜色
  - 主要是调试的时候用来区分边点，面点，顶点
#### draw_point(const float&x,const float&y,char c='r')
- 画点另一种实现
- 调试用
#### draw_number(const float&x,const float&y,const int&id)
- 在指定的位置显示数字
- 在画点函数中使用
#### show()
- 显示窗口
#### clear()
- 清除窗口已经画出来的内容
- 具体是直接用背景覆盖了原来的内容

## Solve.cpp
### 定义
- 在这里将上述的三个类统一使用
- 实现了用户交互逻辑