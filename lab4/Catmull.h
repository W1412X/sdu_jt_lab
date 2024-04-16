#include"IO.h"
class Catmull{
    public:
    std::vector<Vertex>last_vertex_set;//存储上一轮的顶点
    std::vector<Edge>last_edge_set;//上一轮的边
    std::vector<Face>last_face_set;//上一轮的面
    //更新后的
    std::vector<Vertex>updated_vertex_set;
    std::vector<Edge>updated_edge_set;
    std::vector<Face>updated_face_set;
    /*每次更新的核心点集包括山上一轮的顶点以及这一轮更新得到的面点，一个顶点和一个面点确定一个新的面
    *以面点为核心，面点需要的信息为坐标，所在面
    */
   std::vector<Vertex>face_vertex;
   std::vector<Vertex>vertex_update_by_last_vertex;//由上一轮的顶点变换而来的这一轮的对应的地爱嗯点
   int vertex_id=0;//用于设置每一轮遍历的id    
   int face_id=0;
   int edge_id=0;
    IO io;
    Catmull(){
        io=IO("../data.obj");
        last_vertex_set=io.vertex_set;
        last_edge_set=io.edge_set;
        last_face_set=io.face_set;
    }
    void prepare(){//计算面中心点和边中心点
        for(int i=0;i<last_face_set.size();i++){//更新面点
            float sum_x=0,sum_y=0;
            for(int u=0;u<last_face_set[i].vertex.size();u++){
                sum_x+=last_vertex_set[last_face_set[i].vertex[u]-1].x;
                sum_y+=last_vertex_set[last_face_set[i].vertex[u]-1].y;
            }
            last_face_set[i].cen_x=sum_x/last_face_set[i].vertex.size();
            last_face_set[i].cen_y=sum_y/last_face_set[i].vertex.size();
        }
        for(int i=0;i<last_edge_set.size();i++){//计算边中点
            float sum=0;
            float sum_x,sum_y;
            sum_x=(last_vertex_set[last_edge_set[i].v1-1].x+last_vertex_set[last_edge_set[i].v2-1].x);
            sum_y=(last_vertex_set[last_edge_set[i].v1-1].y+last_vertex_set[last_edge_set[i].v2-1].y);
            sum+=2;
            if(last_edge_set[i].near_face.size()>=4){
                for(int u=0;u<last_edge_set[i].near_face.size();u++){
                    sum++;
                    sum_x+=last_face_set[last_edge_set[i].near_face[u]-1].cen_x;
                    sum_y+=last_face_set[last_edge_set[i].near_face[u]-1].cen_y;
                }
            }
            last_edge_set[i].cen_x=sum_x/sum;
            last_edge_set[i].cen_y=sum_y/sum;
        }
    }
    void update_vertex(){//更新上一轮顶点的变更后的位置
        for(int i=0;i<last_vertex_set.size();i++){
            if(last_vertex_set[i].connect_edges.size()<=3){
                vertex_update_by_last_vertex.push_back(Vertex(last_vertex_set[i].x,last_vertex_set[i].y,-1));
                continue;
            }
            float sum_edge_x=0,sum_edge_y=0;
            float sum_face_x=0,sum_face_y=0;
            float sum_self_x=0,sum_self_y=0;
            int sum_divide=0;
            float re_x=0,re_y=0;
            //面点
            //通过边获取其相邻的面
            std::unordered_map<int,int>_m;
            std::vector<int>faces_tmp;
            for(int u=0;u<last_vertex_set[i].connect_edges.size();u++){
                for(int k=0;k<last_edge_set[last_vertex_set[i].connect_edges[u]-1].near_face.size();k++){
                    int fac=last_edge_set[last_vertex_set[i].connect_edges[u]-1].near_face[k];
                    if(_m.find(fac)==_m.end()){
                        _m[fac]=1;
                        faces_tmp.push_back(fac);
                    }
                }
            }
            for(int u=0;u<faces_tmp.size();u++){
                sum_divide++;
                sum_face_x+=last_face_set[faces_tmp[u]-1].cen_x;
                sum_face_y+=last_face_set[faces_tmp[u]-1].cen_y;
            }
            re_x+=sum_face_x/(sum_divide*4);
            re_y+=sum_face_y/(sum_divide*4);
            //边点
            for(int u=0;u<last_vertex_set[i].connect_edges.size();u++){
                sum_edge_x+=last_edge_set[last_vertex_set[i].connect_edges[u]-1].cen_x;
                sum_edge_y+=last_edge_set[last_vertex_set[i].connect_edges[u]-1].cen_y;
            }
            re_x+=sum_edge_x*2/(sum_divide*4.0);
            re_y+=sum_edge_y*2/(sum_divide*4.0);
            //自己
            re_x+=last_vertex_set[i].x/4.0;
            re_y+=last_vertex_set[i].y/4.0;
            int id=i;
            vertex_update_by_last_vertex.push_back(Vertex(re_x,re_y,id));
        }
    }
    /*
    
    */
    void save_new_vertex(){//为了方便，在存储的时候，先存面的中心点，再存原来的顶点，再存边点
        for(int i=0;i<last_face_set.size();i++){
            vertex_id++;
            updated_vertex_set.push_back(Vertex(last_face_set[i].cen_x,last_face_set[i].cen_y,vertex_id));
            last_face_set[i].cen_id=vertex_id;//存一下中心点在新的顶点数组的id
        }
        for(int i=0;i<vertex_update_by_last_vertex.size();i++){
            vertex_id++;
            updated_vertex_set.push_back(vertex_update_by_last_vertex[i]);
            updated_vertex_set[updated_vertex_set.size()-1].id=vertex_id;
            last_vertex_set[i].updated_id=vertex_id;
        }
        for(int i=0;i<last_edge_set.size();i++){
            vertex_id++;
            updated_vertex_set.push_back(Vertex(last_edge_set[i].cen_x,last_edge_set[i].cen_y,vertex_id));
            last_edge_set[i].cen_id=vertex_id;
        }
    }
    std::vector<int> get_edge_by_vertex_face(int vid,int fid){
        std::vector<int>re;
        for(int i=0;i<last_vertex_set[vid-1].connect_edges.size();i++){
            for(int u=0;u<last_edge_set[last_vertex_set[vid-1].connect_edges[i]-1].near_face.size();u++){
                if(last_edge_set[last_vertex_set[vid-1].connect_edges[i]-1].near_face[u]==fid){
                    re.push_back(last_edge_set[last_vertex_set[vid-1].connect_edges[i]-1].id);
                }
            }
        }
        return re;
    }
    int convert_pair_to_int(const std::pair<int,int>p){
        return p.first*1000000+p.second;
    }
    void remove_repeat(std::vector<int>& nums) {
        std::sort(nums.begin(), nums.end());
        auto last = std::unique(nums.begin(), nums.end());
        nums.erase(last, nums.end());
    }
    void save_new_edge_face(){//ll
        std::unordered_map<int, int> edge_help;//保证边不会被重复添加
        for(int i=0;i<last_face_set.size();i++){
            for(int u=0;u<last_face_set[i].vertex.size();u++){
                //添加边
                std::vector<int>edge_ids=get_edge_by_vertex_face(last_face_set[i].vertex[u],last_face_set[i].id);
                std::pair<int,int>edge1=std::make_pair(last_vertex_set[last_face_set[i].vertex[u]-1].updated_id,last_edge_set[edge_ids[0]-1].cen_id);
                std::pair<int,int>edge2=std::make_pair(last_vertex_set[last_face_set[i].vertex[u]-1].updated_id,last_edge_set[edge_ids[1]-1].cen_id);
                if(edge_help.find(convert_pair_to_int(edge1))==edge_help.end()){
                    edge_id++;
                    edge_help[convert_pair_to_int(edge1)]=edge_id;
                    updated_edge_set.push_back(Edge(edge1.first,edge1.second,updated_vertex_set,edge_id));
                }
                if(edge_help.find(convert_pair_to_int(edge2))==edge_help.end()){
                    edge_id++;
                    edge_help[convert_pair_to_int(edge2)]=edge_id;
                    updated_edge_set.push_back(Edge(edge2.first,edge2.second,updated_vertex_set,edge_id));
                }
                std::pair<int,int>edge3=std::make_pair(last_face_set[i].cen_id,last_edge_set[edge_ids[0]-1].cen_id);
                std::pair<int,int>edge4=std::make_pair(last_face_set[i].cen_id,last_edge_set[edge_ids[1]-1].cen_id);
                if(edge_help.find(convert_pair_to_int(edge3))==edge_help.end()){
                    edge_id++;
                    edge_help[convert_pair_to_int(edge3)]=edge_id;
                    updated_edge_set.push_back(Edge(edge3.first,edge3.second,updated_vertex_set,edge_id));
                }
                if(edge_help.find(convert_pair_to_int(edge4))==edge_help.end()){
                    edge_id++;
                    edge_help[convert_pair_to_int(edge4)]=edge_id;
                    updated_edge_set.push_back(Edge(edge4.first,edge4.second,updated_vertex_set,edge_id));
                }
                //添加面
                Face face=Face(++face_id);
                //添加面的点信息
                face.vertex.push_back(last_face_set[i].cen_id);//面中心点
                face.vertex.push_back(last_vertex_set[last_face_set[i].vertex[u]-1].updated_id);//顶点
                face.vertex.push_back(last_edge_set[edge_ids[0]-1].cen_id);//边点
                face.vertex.push_back(last_edge_set[edge_ids[1]-1].cen_id);//边点
                //信息完善
                //添加面的邻边信息
                face.edge.push_back(edge_help[convert_pair_to_int(edge1)]);
                face.edge.push_back(edge_help[convert_pair_to_int(edge2)]);
                face.edge.push_back(edge_help[convert_pair_to_int(edge3)]);
                face.edge.push_back(edge_help[convert_pair_to_int(edge4)]);
                //添加边的邻面信息
                updated_edge_set[edge_help[convert_pair_to_int(edge1)]-1].near_face.push_back(face_id);
                updated_edge_set[edge_help[convert_pair_to_int(edge2)]-1].near_face.push_back(face_id);
                updated_edge_set[edge_help[convert_pair_to_int(edge3)]-1].near_face.push_back(face_id);
                updated_edge_set[edge_help[convert_pair_to_int(edge4)]-1].near_face.push_back(face_id);
                updated_face_set.push_back(face);
            }
        }
    }
    void generate(){
        prepare();
        update_vertex();
        save_new_vertex();
        save_new_edge_face();
        last_edge_set=updated_edge_set;
        last_face_set=updated_face_set;
        last_vertex_set=updated_vertex_set;
        face_vertex=std::vector<Vertex>(0);
        vertex_update_by_last_vertex=std::vector<Vertex>(0);
        edge_id=0;
        face_id=0;
        vertex_id=0;
        updated_vertex_set=std::vector<Vertex>(0);;
        updated_edge_set=std::vector<Edge>(0);
        updated_face_set=std::vector<Face>(0);
        //对点邻边去重
        for(int i=0;i<last_vertex_set.size();i++){
            remove_repeat(last_vertex_set[i].connect_edges);
        }
    }
    void out_re(){
        std::cout<<"#######################################################"<<std::endl;
        std::cout<<"点信息"<<std::endl;
        for(int i=0;i<last_vertex_set.size();i++){
            std::cout<<"v "<<last_vertex_set[i].x<<' '<<last_vertex_set[i].y<<std::endl;
        }
        std::cout<<"边信息"<<std::endl;
        for(int i=0;i<last_edge_set.size();i++){
            std::cout<<"e "<<last_edge_set[i].v1<<' '<<last_edge_set[i].v2<<std::endl;
            for(int u=0;u<last_edge_set[i].near_face.size();u++){
                std::cout<<"f "<<last_edge_set[i].near_face[u]<<std::endl;
            }
        }
        std::cout<<"面信息"<<std::endl;
        for(int i=0;i<last_face_set.size();i++){
            std::cout<<"f";
            for(int u=0;u<last_face_set[i].vertex.size();u++){
                std::cout<<' ';
                std::cout<<last_face_set[i].vertex[u];
            }
            std::cout<<std::endl;
            for(int u=0;u<last_face_set[i].edge.size();u++){
                std::cout<<last_edge_set[last_face_set[i].edge[u]].v1<<'-'<<last_edge_set[last_face_set[i].edge[u]].v2<<" ";
            }
            std::cout<<std::endl;
        }
    }
};