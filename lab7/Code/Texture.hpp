//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include<bits/stdc++.h>
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    std::vector<cv::Mat> mip_list;
    std::vector<int>widths,heights;
public:
    Texture(const std::string& name)
    {
        int tmp=1024;
        while(tmp!=0){
            std::cout<<tmp<<std::endl;
            cv::Mat image_data;
            image_data = cv::imread("../../mipmap_imgs/"+std::to_string(tmp)+".png");
            cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
            width = image_data.cols;
            height = image_data.rows;
            mip_list.push_back(image_data);
            widths.push_back(width);
            heights.push_back(height);
            tmp/=2;
        }
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v,int level=0)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = mip_list[level].at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }
    Eigen::Vector3f getColorBilinear(float u, float v,int level=0){//传入纹理宽高比 u:v
        float w1 = int(u * width), h1 = int(v * height);//左下角的像素
        float w2 = w1 + 1, h2 = h1;//右下
        float w3 = w1, h3 = h1 + 1;//左上
        float w4 = w1 + 1, h4 = h1 + 1;//右上
        Eigen::Vector3f color1, color2, color3, color4, color5, color6, color;
        color1 = getColor(w1 / width, h1 / height,level);//左下
        color2 = getColor(w2 / width, h2 / height,level);//右下
        color3 = getColor(w3 / width, h3 / height,level);//左上
        color4 = getColor(w4 / width, h4 / height,level);//右上
        /*
        左上，右上插值得到一个
        左下，右下得到一个
        */
        color5 = color1 + (color2 - color1) * (u * width - w1);//下
        color6 = color3 + (color4 - color3) * (u * width - w1);//上
        color = color5 + (color6 - color5) * (v * height - h1);//在y方向插值
        return color;//结果
    }
    Eigen::Vector3f getColorTrilinear(float u, float v, float level)
    {
        int level_floor = static_cast<int>(std::floor(level));
        int level_ceil = std::min(level_floor + 1, static_cast<int>(mip_list.size()) - 1);
        Eigen::Vector3f color_floor = getColorBilinear(u, v, level_floor);
        Eigen::Vector3f color_ceil = getColorBilinear(u, v, level_ceil);
        // 在两个Mipmap等级间进行线性插值
        float alpha = level - level_floor;
        return (1 - alpha) * color_floor + alpha * color_ceil;
    }
};
#endif //RASTERIZER_TEXTURE_H
