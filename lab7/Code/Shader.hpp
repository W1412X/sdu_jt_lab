//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <eigen3/Eigen/Eigen>
#include "Texture.hpp"


struct fragment_shader_payload
{
    fragment_shader_payload()

        : texture(nullptr), u_right(0.f), v_top(0.f) {}


    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,

                       const Eigen::Vector2f& tc, Texture* tex,

                       float u_right_val, float v_top_val)

        : color(col), normal(nor), tex_coords(tc), texture(tex),

          u_right(u_right_val), v_top(v_top_val) {}
    float u_right; // 新增: 右侧纹理坐标
    float v_top;  // 新增: 顶部纹理坐标
    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coords;
    Texture* texture;
};

struct vertex_shader_payload
{
    Eigen::Vector3f position;
};

#endif //RASTERIZER_SHADER_H
