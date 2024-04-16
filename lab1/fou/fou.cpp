#include<bits/stdc++.h>
#include<Eigen/Dense>
#include <opencv2/opencv.hpp>
using namespace std;
Eigen::Vector2d get_vector_by_point(Eigen::Vector2d v1,Eigen::Vector2d v2){
	Eigen::Vector2d re;
	re<<(v2(0)-v1(0)),(v2(1)-v1(1));
	return re;
}
double crossProduct2D(Eigen::Vector2d v1, Eigen::Vector2d v2) {
    return v1(0) * v2(1) - v2(0) * v1(1);
}
bool if_in_triangle(Eigen::Vector2d v1,Eigen::Vector2d v2,Eigen::Vector2d v3,Eigen::Vector2d v4){
    Eigen::Vector2d v_1=get_vector_by_point(v2,v3);
    Eigen::Vector2d v_2=get_vector_by_point(v3,v4);
    Eigen::Vector2d v_3=get_vector_by_point(v4,v2);
    Eigen::Vector2d v1_p=get_vector_by_point(v2,v1);
    Eigen::Vector2d v2_p=get_vector_by_point(v3,v1);
    Eigen::Vector2d v3_p=get_vector_by_point(v4,v1);
    return crossProduct2D(v_1,v1_p)>=0&&crossProduct2D(v_2,v2_p)>=0&&crossProduct2D(v_3,v3_p)>=0;
}
int main(){
    Eigen::Vector2d v1,v2,v3;
	Eigen::Vector2d p;
    cv::Mat image(500, 500, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Point origin(250, 250);
    int axisLength = 400;
    cv::line(image, cv::Point(origin.x - axisLength, origin.y), cv::Point(origin.x + axisLength, origin.y), cv::Scalar(0, 0, 0), 2);
    cv::line(image, cv::Point(origin.x, origin.y - axisLength), cv::Point(origin.x, origin.y + axisLength), cv::Scalar(0, 0, 0), 2);
    for (int i = 20; i <= axisLength; i += 20) {
        cv::line(image, cv::Point(origin.x - i, origin.y - axisLength), cv::Point(origin.x - i, origin.y + axisLength), cv::Scalar(200, 200, 200));
        cv::line(image, cv::Point(origin.x + i, origin.y - axisLength), cv::Point(origin.x + i, origin.y + axisLength), cv::Scalar(200, 200, 200));
        cv::line(image, cv::Point(origin.x - axisLength, origin.y - i), cv::Point(origin.x + axisLength, origin.y - i), cv::Scalar(200, 200, 200));
        cv::line(image, cv::Point(origin.x - axisLength, origin.y + i), cv::Point(origin.x + axisLength, origin.y + i), cv::Scalar(200, 200, 200));
    }


    cout<<"Please enter the three vertex of the triangle:\n";
	cin>>v1(0)>>v1(1)>>v2(0)>>v2(1)>>v3(0)>>v3(1);
	//draw the triangle
	cv::line(image,cv::Point(origin.x+v1(0)*20,origin.y-v1(1)*20),cv::Point(origin.x+v2(0)*20,origin.y-v2(1)*20),cv::Scalar(0,0,0));
	cv::line(image,cv::Point(origin.x+v1(0)*20,origin.y-v1(1)*20),cv::Point(origin.x+v3(0)*20,origin.y-v3(1)*20),cv::Scalar(0,0,0));
	cv::line(image,cv::Point(origin.x+v3(0)*20,origin.y-v3(1)*20),cv::Point(origin.x+v2(0)*20,origin.y-v2(1)*20),cv::Scalar(0,0,0));
	//show the image
    cv::imshow("1412", image);
    cv::waitKey(0);
	while(1){
		cout<<"Please enter the point:\n";
		cin>>p(0)>>p(1);
		cv::Point point_to_draw(origin.x + p(0) * 20, origin.y - p(1) * 20);
    	cv::circle(image, point_to_draw, 5, cv::Scalar(0, 0, 255), -1);
		bool if_in=(if_in_triangle(p,v1,v2,v3));
		if(if_in){
			cout<<"p in triangle\n"<<endl;
		}else{
			cout<<"p out triangle\n"<<endl;
		}
		cv::imshow("1412",image);
		cv::waitKey(0);
	}
}