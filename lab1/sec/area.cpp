#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
bool if_point_in_triangle(const Eigen::Vector2d& p, const Eigen::Vector2d& v1, const Eigen::Vector2d& v2, const Eigen::Vector2d& v3) {
    double area_original = std::abs((v2[0] - v1[0]) * (v3[1] - v1[1]) - (v3[0] - v1[0]) * (v2[1] - v1[1]));

    double area_triangle1 = std::abs((p[0] - v1[0]) * (v2[1] - v1[1]) - (v2[0] - v1[0]) * (p[1] - v1[1]));
    double area_triangle2 = std::abs((p[0] - v2[0]) * (v3[1] - v2[1]) - (v3[0] - v2[0]) * (p[1] - v2[1]));
    double area_triangle3 = std::abs((p[0] - v3[0]) * (v1[1] - v3[1]) - (v1[0] - v3[0]) * (p[1] - v3[1]));
    double total_area = area_triangle1 + area_triangle2 + area_triangle3;
    return !std::abs(total_area - area_original) > 1e-9;
}
int main() {
    Eigen::Vector2d p(0.5, 0.5);
    Eigen::Vector2d v1,v2,v3;
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
	std::cout<<"Please enter the three vertex of the triangle:\n";
	std::cin>>v1(0)>>v1(1)>>v2(0)>>v2(1)>>v3(0)>>v3(1);
	//draw the triangle
	cv::line(image,cv::Point(origin.x+v1(0)*20,origin.y-v1(1)*20),cv::Point(origin.x+v2(0)*20,origin.y-v2(1)*20),cv::Scalar(0,0,0));
	cv::line(image,cv::Point(origin.x+v1(0)*20,origin.y-v1(1)*20),cv::Point(origin.x+v3(0)*20,origin.y-v3(1)*20),cv::Scalar(0,0,0));
	cv::line(image,cv::Point(origin.x+v3(0)*20,origin.y-v3(1)*20),cv::Point(origin.x+v2(0)*20,origin.y-v2(1)*20),cv::Scalar(0,0,0));
	//show the image
    cv::imshow("1412", image);
    cv::waitKey(0);
    while(1){
        std::cout<<"Please enter the point:\n";
	    std::cin>>p(0)>>p(1);
		cv::Point point_to_draw(origin.x + p(0) * 20, origin.y - p(1) * 20);
    	cv::circle(image, point_to_draw, 5, cv::Scalar(0, 0, 255), -1);
        if (if_point_in_triangle(p, v1, v2, v3)) {
            std::cout << "p in triangle\n" << std::endl;
        } else {
            std::cout << "p out outside the triangle\n" << std::endl;
        }  
        cv::imshow("1412",image);
        cv::waitKey(0);

    }


    return 0;
}
