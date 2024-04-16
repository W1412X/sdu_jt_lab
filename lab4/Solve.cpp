#include"Draw.h"
#include"Catmull.h"
class Solve{
    public:
    Catmull ct;
    Draw dw;
    Solve(){
        ct=Catmull();
    }
    void generate(){
        ct.generate();
    }
    void draw_show(bool if_show_num){
        for(int i=0;i<ct.last_vertex_set.size();i++){
            dw.draw_point(ct.last_vertex_set[i],if_show_num,'b');
        }
        for(int i=0;i<ct.last_edge_set.size();i++){
            dw.draw_line(ct.last_vertex_set[ct.last_edge_set[i].v1-1],ct.last_vertex_set[ct.last_edge_set[i].v2-1]);
        }
        dw.show();
        dw.clear();
    }
};
int main(){
    while(1){
        Solve sl=Solve();
        int times=0;    
        int g;
        std::cout<<"Enter Generate Times->";
        std::cin>>times;
        std::cout<<"show vertex id?(0->n/1->y):";
        std::cin>>g;
        while(times-->0){
            sl.generate();
        }
        sl.draw_show(g==1);
    }
}