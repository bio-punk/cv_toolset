/*
 * @Author: bio-punk
 * @Date: 2021-01-14 14:13:58
 * @LastEditTime: 2021-01-14 17:32:11
 * @LastEditors: Please set LastEditors
 * @Description: 核心文件
 * @FilePath: \code\include\cvtoolset\include\cvtoolset.hpp
 */

#ifndef __cplusplus
#  error cvtoolset.hpp header must be compiled as C++
#endif

#ifndef CVTOOLSET_HPP
#define CVTOOLSET_HPP

#include <stack>
#include <array>

#include <cassert>

#include <opencv2/opencv.hpp>

#include <imgcut/region_grow.hpp>
#include <imgcut/centers_cut.hpp>

//#include <remove_noise/binary_graph.hpp>
#include <remove_noise/naive_remove.hpp>
#include <remove_noise/contours_remove.hpp>

#include <utils/point_shift.hpp>

#endif
