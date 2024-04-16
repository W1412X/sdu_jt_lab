#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(const Eigen::Vector3f& axis, float rotation_angle)
{
    Eigen::Vector3f normalized_axis = axis.normalized();//化为单位向量
    float x = normalized_axis.x();
    float y = normalized_axis.y();
    float z = normalized_axis.z();

    float cos_theta = std::cos(M_PI * rotation_angle / 180);
    float sin_theta = std::sin(M_PI * rotation_angle / 180);
    float one_mius = 1 - cos_theta;

    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    model(0, 0) = cos_theta + x * x * one_mius;
    model(0, 1) = x * y * one_mius - z * sin_theta;
    model(0, 2) = x * z * one_mius + y * sin_theta;

    model(1, 0) = y * x * one_mius + z * sin_theta;
    model(1, 1) = cos_theta + y * y * one_mius;
    model(1, 2) = y * z * one_mius - x * sin_theta;

    model(2, 0) = z * x * one_mius - y * sin_theta;
    model(2, 1) = z * y * one_mius + x * sin_theta;
    model(2, 2) = cos_theta + z * z * one_mius;

    model(3, 3) = 1;
    return model;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    float cos_theta = std::cos(M_PI*rotation_angle/180);
    float sin_theta = std::sin(M_PI*rotation_angle/180);
    //z
    model(0, 0) = cos_theta;
    model(0, 1) = -sin_theta;
    model(1,0) = sin_theta;
    model(1, 1) = cos_theta;
    model(2,2)=1;
    model(3,3)=1;
    //X
    /*
    model(0,0)=1;
    model(1,1)=cos_theta;
    model(2,1)=sin_theta;
    model(1,2)=-sin_theta;
    model(2,2)=cos_theta;
    model(3,3)=1;*/
    //Y
    /*model(0,0)=cos_theta;
    model(0,2)=-sin_theta;
    model(1,1)=1;
    model(0,2)=sin_theta;
    model(2,2)=cos_theta;
    model(3,3)=1;*/
    // Return the model matrix
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
    float tan_half_fov = std::tan(eye_fov / 2);
    float range = zNear - zFar;
    projection(0, 0) = 1 / (tan_half_fov * aspect_ratio);
    projection(1, 1) = 1 / tan_half_fov;
    projection(2, 2) = (-zNear - zFar) / range;
    projection(2, 3) = 2 * zFar * zNear / range;
    projection(3, 2) = 1;

    return projection;
}


int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;
    printf("Which Mode(0->绕z,1->绕自定义):");
    int mode=0;
    std::cin>>mode;
        float a_x,a_y,a_z;
        float angle_;
    if(mode!=0){
        std::cout<<"输入向量:";
        std::cin>>a_x>>a_y>>a_z;
        std::cout<<"输入角度:";
        std::cin>>angle_;
        Eigen::Vector3f tmp(a_x,a_y,a_z);
    }
    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        if(mode==0){
            r.set_model(get_model_matrix(angle));
        }else{
            r.set_model(get_model_matrix(tmp,angle_));
        }

        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        if(mode==0){
            r.set_model(get_model_matrix(angle));
        }else{
            r.set_model(get_model_matrix(tmp,angle_));
        }
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
