#include<bits/stdc++.h>
#include <opencv2/opencv.hpp>
class Point {
    public:
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
};
class Draw {
public:
    Draw() {
        image = cv::Mat::zeros(1000, 1000, CV_8UC3);
        cv::rectangle(image, cv::Point(200, 200), cv::Point(800, 800), cv::Scalar(255, 255, 255), 2);
        for (int i = 1; i < 60; ++i) {
            cv::line(image, cv::Point(200 + i * 10, 200), cv::Point(200 + i * 10, 800), cv::Scalar(200, 200, 200), 1);
            cv::line(image, cv::Point(200, 200 + i * 10), cv::Point(800, 200 + i * 10), cv::Scalar(200, 200, 200), 1);
        }
    }
    void init(){
        image = cv::Mat::zeros(1000, 1000, CV_8UC3);
        cv::rectangle(image, cv::Point(200, 200), cv::Point(800, 800), cv::Scalar(255, 255, 255), 2);
        for (int i = 1; i < 60; ++i) {
            cv::line(image, cv::Point(200 + i * 10, 200), cv::Point(200 + i * 10, 800), cv::Scalar(200, 200, 200), 1);
            cv::line(image, cv::Point(200, 200 + i * 10), cv::Point(800, 200 + i * 10), cv::Scalar(200, 200, 200), 1);
        }
    }
    cv::Point convert_point(const Point&p){
        return cv::Point(int((p.x-1)*10+5),int(1000-((p.y)*10-5)));
    }
    void draw_line(const Point& p1, const Point& p2, const char&c='w'){
        cv::Scalar color = cv::Scalar(255, 255, 255);
        if(c=='w'){}
        else if(c=='r')
        color=cv::Scalar(255, 0, 0);
        cv::line(image, convert_point(p1), convert_point(p2), color,1);
    }
    //染色格子
    void draw_pixel(const Point&p) {
        int pixel_id=(((p.x-20))+(60-(p.y-19))*60);
        if (pixel_id >= 0 && pixel_id < 3600) {
            int row = pixel_id / 60;
            int col = pixel_id % 60;
            int startX = 200;
            int startY = 200;
            int cellSize = 10;
            int xPos = startX + col * cellSize + 1;
            int yPos = startY + row * cellSize + 1;
            cv::rectangle(image, cv::Point(xPos, yPos), cv::Point(xPos + cellSize - 1, yPos + cellSize - 1), cv::Scalar(128, 128, 128), -1);
        } else {
            std::cout << "Invalid cell number." << std::endl;
        }
    }
    void show(){
        cv::imshow("1412", image);
        cv::waitKey(0);
    }

private:
    cv::Mat image;
};
std::pair<Point, Point> liangBarskyClip(const Point& p1, const Point& p2, const Point& min, const Point& max) {
    float t0 = 0.0, t1 = 1.0;
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float p[4] = {-dx, dx, -dy, dy};
    float q[4] = {p1.x - min.x, max.x - p1.x, p1.y - min.y, max.y - p1.y};
    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0) {
            if (q[i] < 0) {
                return std::make_pair(Point(0, 0), Point(0, 0));
            }
        } else {
            float t = q[i] / p[i];
            if (p[i] < 0) {
                if (t > t1) return std::make_pair(Point(0, 0), Point(0, 0));
                if (t > t0) t0 = t;
            } else {
                if (t < t0) return std::make_pair(Point(0, 0), Point(0, 0));
                if (t < t1) t1 = t;
            }
        }
    }
    int xx=int(p1.x + t0 * dx),yy=int(p1.y + t0 * dy);
    if(p1.x + t0 * dx>int(p1.x + t0 * dx)){
        xx=int(p1.x + t0 * dx)+1;
    }
    if(p1.y + t0 * dy>int(p1.y + t0 * dy)){
        yy=int(p1.y + t0 * dy)+1;
    }
    Point re_p1(xx, yy);

    Point re_p2(int(p1.x + t1 * dx),int(p1.y + t1 * dy));
    return std::make_pair(re_p1, re_p2);
}
int compute_decide(const Point&last_pixel,const Point&p1,const Point&p2){
    return 2*(p1.y-p2.y)*(last_pixel.x+1)+(p2.x-p1.x)+2*(p2.x-p1.x)*last_pixel.y+2*p1.x*p2.y-2*p2.x*p1.y;
}
std::vector<Point> mid_alg(const Point&p1,const Point&p2)
{
    float k=(p2.y-p1.y-0.0)/(p2.x-p1.x+0.0);
    std::vector<Point>line_points;
    if(k>0){
        line_points.push_back(p1);
        int x = p1.x;
        Point last_pixel = p1;
        int d;
        if (k <= 1)
            while (last_pixel.x <= p2.x){
                d = compute_decide(last_pixel, p1, p2);
                if (d <= 0){
                    line_points.push_back(Point(last_pixel.x + 1, last_pixel.y + 1));
                }
                else{
                    line_points.push_back(Point(last_pixel.x + 1, last_pixel.y));
                }
                last_pixel = line_points[line_points.size() - 1];
            }
        else
            while (last_pixel.y <= p2.y){
                d = compute_decide(last_pixel, p1, p2);
                if (d <= 0){
                    line_points.push_back(Point(last_pixel.x, last_pixel.y + 1));
                }
                else{
                    line_points.push_back(Point(last_pixel.x + 1, last_pixel.y + 1));
                }
                last_pixel = line_points[line_points.size() - 1];
            }
    }
    else{
        line_points.push_back(p2);
        int x = p2.x;
        Point last_pixel = p2;
        int d;
        if (k >= -1)
            while (last_pixel.x >= p1.x){
                d = compute_decide(last_pixel, p1, p2);
                if (d <= 0){
                    line_points.push_back(Point(last_pixel.x - 1, last_pixel.y + 1));
                }
                else{
                    line_points.push_back(Point(last_pixel.x - 1, last_pixel.y));
                }
                last_pixel = line_points[line_points.size() - 1];
            }
        else
            while (last_pixel.y <= p1.y){
                d = compute_decide(last_pixel, p1, p2);
                if (d <= 0){
                    line_points.push_back(Point(last_pixel.x, last_pixel.y + 1));
                }
                else{
                    line_points.push_back(Point(last_pixel.x - 1, last_pixel.y + 1));
                }
                last_pixel = line_points[line_points.size() - 1];
            }
    }
    return line_points;
}
int main() {
    Draw dw=Draw();
    Point min(20, 20);
    Point max(80, 80);
    int x,y;
    while(1){
        std::cout<<"enter the point 1(0-100):";
        std::cin>>x>>y;
        Point p1(x, y);
        std::cout<<"enter the point 2(0-100):";
        std::cin>>x>>y;
        Point p2(x, y);
        std::pair<Point, Point> result = liangBarskyClip(p1, p2, min, max);
        std::cout << "Clipped Point 1: (" << result.first.x << ", " << result.first.y << ")" << std::endl;
        std::cout << "Clipped Point 2: (" << result.second.x << ", " << result.second.y << ")" << std::endl;
        dw.draw_line(p1,p2);
        dw.draw_line(result.first,result.second,'r');
        std::vector<Point>line_points=mid_alg(result.first,result.second);
        for(int i=0;i<line_points.size();i++){
            std::cout<<line_points[i].x<<' '<<line_points[i].y<<std::endl;
            dw.draw_pixel(line_points[i]);
        }
        dw.show();
        dw.init();
    }
    return 0;
}
