#include <opencv2/opencv.hpp>
#include"IO.h"
class Draw{
    public:
    cv::Mat window;
    Draw(){
        window=cv::Mat(1000,1000,CV_8UC3,cv::Scalar(220,220,220));
    }
    std::pair<int,int> convert(const Vertex&v){
        return std::make_pair(v.x*300+50,900-v.y*300+50);
    }
    std::pair<int,int> convert(float x,float y){
        return std::make_pair(x*150+75,450-y*150+75);
    }
    void draw_line(const Vertex&v1,const Vertex&v2,char c='b'){

        cv::Point start(convert(v1).first,convert(v1).second);
        cv::Point end(convert(v2).first,convert(v2).second);
        cv::line(window,start,end,c=='b'?cv::Scalar(155,155,104):cv::Scalar(204,255,204),2);
    }
    void draw_point(const Vertex&v,bool if_num=false,char c='r'){
        cv::Scalar scr,scb,scp;
        scr=cv::Scalar(0,0,255);
        scb=cv::Scalar(255,0,0);
        scp=cv::Scalar(0,255,0);
        cv::Point point(convert(v).first,convert(v).second);
        if(c=='r')
        cv::circle(window, point, 3,scr , -1);
        else if(c=='b')
        cv::circle(window, point, 3,scb , -1);
        else
        cv::circle(window, point, 3,scp , -1);
        if(if_num)
        draw_number(convert(v).first,convert(v).second,v.id);
    }
    void draw_point(const float&x,const float&y,char c='r'){
        cv::Scalar scr,scb,scp;
        scr=cv::Scalar(0,0,255);
        scb=cv::Scalar(255,0,0);
        scp=cv::Scalar(0,255,0);
        cv::Point point(convert(x,y).first,convert(x,y).second);
        if(c=='r')
        cv::circle(window, point, 3,scr , -1);
        else if(c=='b')
        cv::circle(window, point, 3,scb , -1);
        else
        cv::circle(window, point, 3,scp , -1);
    }
    void draw_number(const float&x,const float&y,const int&id){
        cv::Point textPosition(x, y); // 文本位置
        int fontFace = cv::FONT_HERSHEY_SIMPLEX; // 字体
        double fontScale = 0.8; // 字体大小
        cv::Scalar color(0, 0, 0); // 文本颜色，白色
        int thickness = 2; // 文本粗细
        cv::putText(window, std::to_string(id), textPosition, fontFace, fontScale, color, thickness);
    }
    void show(){
        cv::imshow("1412",window);
        cv::waitKey(0);
    }
    void clear(){
        window.setTo(cv::Scalar(200, 200, 200));
    }
};