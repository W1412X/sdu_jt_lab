#include<bits/stdc++.h>
#include<Eigen/Dense>
#include <opencv2/opencv.hpp>
using namespace std;
double get_angle_of_two_vector(Eigen::Vector2d v1,Eigen::Vector2d v2){
	return std::acos(v1.dot(v2)/(v1.norm()*v2.norm()));
}
Eigen::Vector2d get_vector_by_point(Eigen::Vector2d v1,Eigen::Vector2d v2){
	Eigen::Vector2d re;
	re<<(v2(0)-v1(0)),(v2(1)-v1(1));
	return re;
}
bool if_parallel(Eigen::Vector2d v1,Eigen::Vector2d v2){
	if((abs(v1(0)/v2(0))==abs(v1(1)/v2(1)))||(v1(0)==0&&v2(0)==0)||(v1(1)==0&&v2(1)==0)){
		return true;
	}
	return false;
}
Eigen::Vector2d get_center_point(Eigen::Vector2d v1,Eigen::Vector2d v2){
	Eigen::Vector2d v;
	v<<(v1(0)+v2(0))/2,(v1(1)+v2(1))/2;
	return v;
}
Eigen::Vector2d get_ray(Eigen::Vector2d v1,Eigen::Vector2d v2,Eigen::Vector2d v3){
	Eigen::Vector2d c_p=get_center_point(v2,v3);
	return get_vector_by_point(v1,c_p);
}
int if_intersect(Eigen::Vector2d v1,Eigen::Vector2d v2,Eigen::Vector2d v3,Eigen::Vector2d v4){//to compute if a ray intersecte an edge
	//v1 ray start point
	//v1 ray direction point
	//v3 edge point 1
	//v4 edge point 2
	//using the center point of the edge
	Eigen::Vector2d ray;
	ray<<v2(0)-v1(0),v2(1)-v1(1);
	Eigen::Vector2d edge;
	edge<<v4(0)-v3(0),v4(1)-v3(1);
	//compute the cross to judge if the edge parallel the ray
	if(if_parallel(ray,edge)){//return 1
		return 0;
	}
	//judge if intersect by conpute the angle use v_l v_c(ray) v_r
	Eigen::Vector2d v_l=get_vector_by_point(v1,v3);
	Eigen::Vector2d v_r=get_vector_by_point(v1,v4);
	double angle_rl=get_angle_of_two_vector(v_r,v_l);
	double angle_lc=get_angle_of_two_vector(v_r,ray);
	double angle_rc=get_angle_of_two_vector(v_l,ray);
	if(angle_lc+angle_rc>angle_rl+0.000000001){
		return 0;
	}
	return 1;
}
int how_many_ray_intersecte_triangle(Eigen::Vector2d v1,Eigen::Vector2d center_p,Eigen::Vector2d v2,Eigen::Vector2d v3,Eigen::Vector2d v4){//to how many edge in a triangle was intersect by a ray
	return if_intersect(v1,center_p,v2,v3)+if_intersect(v1,center_p,v3,v4)+if_intersect(v1,center_p,v2,v4);//return 1 is true
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
		bool if_in=(how_many_ray_intersecte_triangle(p,get_center_point(v1,v2),v1,v2,v3)==1&&how_many_ray_intersecte_triangle(p,get_center_point(v2,v3),v1,v2,v3)==1&&how_many_ray_intersecte_triangle(p,get_center_point(v1,v3),v1,v2,v3)==1);
		if(if_in){
			cout<<"p in triangle\n"<<endl;
		}else{
			cout<<"p out triangle\n"<<endl;
		}
		cv::imshow("1412",image);
		cv::waitKey(0);
	}
}
