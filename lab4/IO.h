#ifndef EXAMPLE_H
#define EXAMPLE_H
#include<bits/stdc++.h>
class Vertex{
    public:
    int degree;//度
    int id;//编号
    int updated_id;//更新后点的id
    int face;//对于面点所在的面的编号,
    std::vector<int>connect_edges;//存放的是连接到此点的边的编号
    float x,y;
    Vertex(const float&a,const float&b,const int&i):x(a),y(b),id(i){
        connect_edges=std::vector<int>(0);
    };
    Vertex(){
connect_edges=std::vector<int>(0);
    };
};
class Edge{
    public:
    int id;
    float cen_x,cen_y;
    int cen_id;
    int v1,v2;
    std::vector<int>near_face;//存放相邻面的编号
    Edge(int&a,int&b,std::vector<Vertex>&vertex_se,int&id2){
        id=id2;
        v1=a;
        v2=b;
        vertex_se[a-1].degree++;
        vertex_se[b-1].degree++;
        vertex_se[a-1].connect_edges.push_back(id);
        vertex_se[b-1].connect_edges.push_back(id);
        near_face=std::vector<int>(0);
    }
    Edge(){
        near_face=std::vector<int>(0);
    };
};
class Face{
    public:
    int edge_num=0;
    int id;
    float cen_x;
    float cen_y;
    int cen_id;
    std::vector<int>vertex;
    std::vector<int>edge;
    Face(int i):id(i){
        vertex=std::vector<int>(0);
        edge=std::vector<int>(0);
    };
    Face(){
        vertex=std::vector<int>(0);
        edge=std::vector<int>(0);
    }
};
class IO{
    public:
    std::string file_name;
    std::vector<Vertex>vertex_set;
    std::vector<Edge>edge_set;
    std::vector<Face>face_set;
    int if_exists_edge(const int&a,const int&b){
        for(int i=0;i<edge_set.size();i++){
            if((a==edge_set[i].v1&&b==edge_set[i].v2)||(b==edge_set[i].v1&&a==edge_set[i].v2)){
                return edge_set[i].id;
            }
        }
        return -1;
    }
    IO(){};
    IO(std::string file_n):file_name(file_n){
        std::ifstream inputFile(file_name);
        std::string line;
        if (inputFile.is_open()) {
            char c;
            float a,b;
            int j,k;
            int id=0;
            int id2;
            std::string s;
            while(inputFile>>c){
                if(c=='f'){
                    break;
                }
                inputFile>>a>>b;
                if(c=='v'){
                    vertex_set.push_back(Vertex(a,b,id+1));
                }else if(c=='e'){
                    j=a,k=b;
                    id2=id-vertex_set.size()+1;
                    edge_set.push_back(Edge(j,k,vertex_set,id2));
                }
                id++;
            }
            id=1;
            int val=0;
            char last_c='f';
            Face face=Face(1);
            while(inputFile.get(c)){
                if(c==' '){
                    if(last_c!='f'){
                        face.vertex.push_back(val);
                    }
                    val=0;
                    last_c=c;
                    continue;
                }else if(c=='f'){
                    val=0;
                    last_c=c;
                }else if(c=='\n'){
                    last_c=c;
                    face.vertex.push_back(val);
                    face_set.push_back(face);
                    face=Face(++id);
                    val=0;
                    continue;
                }else{
                    last_c=c;
                    val=10*val+(c-48);
                }
            }
            face.vertex.push_back(val);
            face_set.push_back(face);
            inputFile.close();
        } else {
            std::cerr << "无法打开文件" << std::endl;
        }
        //根据面对应的点确定面的边
        for(int i=0;i<face_set.size();i++){
            for(int u=0;u<face_set[i].vertex.size();u++){
                for(int k=u+1;k<face_set[i].vertex.size();k++){
                    int tmp=if_exists_edge(face_set[i].vertex[u],face_set[i].vertex[k]);
                    if(tmp!=-1){
                        face_set[i].edge.push_back(tmp);
                        face_set[i].edge_num++;
                    }
                }
            }
        }
        //将边相邻的面进行添加
        for(int i=0;i<face_set.size();i++){
            for(int u=0;u<face_set[i].edge.size();u++){
                edge_set[face_set[i].edge[u]-1].near_face.push_back(face_set[i].id);
            }
        }
    }
};
#endif // EXAMPLE_H