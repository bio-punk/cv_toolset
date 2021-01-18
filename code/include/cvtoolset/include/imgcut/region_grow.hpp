/*
 * @Author: bio-punk
 * @Date: 2021-01-14 14:21:02
 * @LastEditTime: 2021-01-14 17:55:58
 * @LastEditors: Please set LastEditors
 * @Description: 区域生长进行图片分割
 * @FilePath: \code\include\cvtoolset\include\imgcut\region_grow.hpp
 */

#ifndef IMG_CUT__REGION_GROW
#define IMG_CUT__REGION_GROW

#include <cvtoolset.hpp>

using std::stack;

namespace cvts {
    namespace img_cut {
        

        /// <summary>
        /// 从种子点开始进行区域生长
        /// </summary>
        /// <typeparam name="TYPE">点数据类型,uchar/cv::Vec3b</typeparam>
        /// <typeparam name="PSTYPE">领域点转换值数组类型,交给编译器自动推导,不用写,STL也是这样写的</typeparam>
        /// <param name="src">原图</param>
        /// <param name="visited">已经搜索过的点集合</param>
        /// <param name="mask">本次生长结果</param>
        /// <param name="seed">种子点</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="PointShift2Dfirst">领域点转换数组首元素指针</param>
        /// <param name="PointShift2Dlast">领域点转换数组尾元素指针</param>
        template <typename TYPE, typename PSTYPE>
        static void __singel_seed_region_grow(
                const cv::Mat& src, 
                cv::Mat& visited, cv::Mat& mask,
                const cv::Point& seed,
                const int& threshold,
                int (*distance)(const TYPE&, const TYPE&),
                PSTYPE PointShift2Dfirst, PSTYPE PointShift2Dlast
        ) {
            static stack<cv::Point> point_stack;
            point_stack.push(seed);
            uchar& mask_seed_data = mask.ptr<uchar>(seed.y)[seed.x] = 1;
            uchar& visited_seed_data = visited.ptr<uchar>(seed.y)[seed.x] = 255;

            const int& rows = src.rows;
            const int& cols = src.cols;

            static cv::Point center;
            while (!point_stack.empty()) {
                center = point_stack.top();
                point_stack.pop();
                const TYPE& center_data = src.ptr<TYPE>(center.y)[center.x];
                for (PSTYPE PointShift2D = PointShift2Dfirst; PointShift2D != PointShift2Dlast; PointShift2D++) {
                    cv::Point estimating_point = center + *PointShift2D;
                    if (
                            estimating_point.x < 0
                            || estimating_point.x > cols - 1
                            || estimating_point.y < 0
                            || estimating_point.y > rows - 1) {
                            // 判断邻接点是否越过图像边界
                        continue;
                    }

                    uchar& estimating_point_mask_data = mask.ptr<uchar>(estimating_point.y)[estimating_point.x];
                    uchar& estimating_point_visited_data = visited.ptr<uchar>(estimating_point.y)[estimating_point.x];
                    if (0 != estimating_point_visited_data || 1 == estimating_point_mask_data) { continue; }  // 如果已经在搜过了就不搜了,
                    estimating_point_visited_data = 255;

                    const TYPE& estimating_point_src_data = src.ptr<TYPE>(estimating_point.y)[estimating_point.x];
                    int delta = distance(center_data, estimating_point_src_data);
                    if (delta < threshold) {
                        estimating_point_mask_data = 1;
                        point_stack.push(estimating_point);
                    }
                }
            }
        }
    }
}

namespace cvts {
    namespace img_cut { 
        

        /// <summary>
        /// 基于种子掩模和领域距离的区域生长算法
        /// </summary>
        /// <typeparam name="TYPE">点类型,uchar/cv::Vec3b</typeparam>
        /// <typeparam name="PSTYPE"></typeparam>
        /// <param name="src">待求解图片</param>
        /// <param name="seedMask">种子掩模</param>
        /// <param name="dest">结果</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="PointShift2Dfirst">领域点转换数组首元素指针</param>
        /// <param name="PointShift2Dlast">领域点转换数组尾元素指针</param>
        /// <param name="min_region_area_factor">最小面积占全图百分比</param>
        template <typename TYPE, typename PSTYPE>
        void mask_region_grow(
            const cv::Mat& src,
            const cv::Mat seedMask,
            cv::Mat& dest,
            const int& threshold,
            int (*distance)(const TYPE&, const TYPE&),
            PSTYPE PointShift2Dfirst, PSTYPE PointShift2Dlast,
            const double& min_region_area_factor = 0.01
        ) {
            const int& rows = src.rows;
            const int& cols = src.cols;
            static const int min_region_area = int(min_region_area_factor * cols * rows);
            static int mask_area;

            cv::Mat mask = cv::Mat::zeros(rows, cols, CV_8UC1);
            static cv::Mat visited = cv::Mat::zeros(rows, cols, CV_8UC1);
            visited = visited - visited;
            visited.copyTo(dest);

            for (int k = 0; k < rows; k++) {
                uchar* dest_row;
                const uchar* seedMask_row;
                uchar* visited_row;

                dest_row = dest.ptr<uchar>(k);
                seedMask_row = seedMask.ptr<uchar>(k);
                visited_row = visited.ptr<uchar>(k);
                for (int i = 0; i < cols; i++) {
                    uchar& dest_data = dest.ptr<uchar>(k)[i];
                    uchar& visited_data = visited.ptr<uchar>(k)[i];
                    const uchar& seedMask_data = seedMask.ptr<uchar>(k)[i];

                    if ((
                            0 == dest_data
                            && 255 == seedMask_data
                            && 0 == visited_data
                    )) {
                        cvts::img_cut::__singel_seed_region_grow(src, visited, mask, cv::Point(i, k), threshold, distance, PointShift2Dfirst, PointShift2Dlast);
                        mask_area = (unsigned int)cv::sum(mask).val[0];
                        if (mask_area > min_region_area) {
                            dest = dest + mask;
                        }
                        mask = mask - mask;     //Faster than mask.setTo(0)
                    }
                }
            }
            dest *= 255;
            return;
        }
    }
}

namespace cvts {
    namespace img_cut {
        /// <summary>
        /// 从种子点开始进行区域生长
        /// </summary>
        /// <typeparam name="TYPE">点数据类型,uchar/cv::Vec3b</typeparam>
        /// <typeparam name="PSTYPE">领域点转换值数组类型,交给编译器自动推导,不要写,STL也是这样写的</typeparam>
        /// <param name="src">原图</param>
        /// <param name="dest">已生长结果</param>
        /// <param name="mask">本次生长结果</param>
        /// <param name="seed">种子点</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="PointShift2Dfirst">领域点转换数组首元素指针</param>
        /// <param name="PointShift2Dlast">领域点转换数组尾元素指针</param>
        template <typename TYPE, typename PSTYPE>
        static void singel_seed_region_grow(
            const cv::Mat& src, const cv::Mat& dest,
            cv::Mat& mask,
            const cv::Point& seed,
            const int& threshold,
            int (*distance)(const TYPE&, const TYPE&),
            PSTYPE PointShift2Dfirst, PSTYPE PointShift2Dlast
        ) {
            __singel_seed_region_grow(src, dest, mask, seed, threshold, distance, PointShift2Dfirst, PointShift2Dlast);
        }
    }
}

namespace cvts {
    namespace img_cut {


        /// <summary>
        /// 基于种子掩模和领域距离的区域生长算法
        /// </summary>
        /// <typeparam name="TYPE">点类型,uchar/cv::Vec3b</typeparam>
        /// <typeparam name="PSTYPE"></typeparam>
        /// <param name="src">待求解图片</param>
        /// <param name="seedMask">种子掩模</param>
        /// <param name="dest">结果</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="PointShift2Dfirst">领域点转换数组首元素指针</param>
        /// <param name="PointShift2Dlast">领域点转换数组尾元素指针</param>
        /// <param name="min_region_area_factor">最小面积占全图百分比</param>        
        /// <returns></returns>
        template <typename TYPE, typename PSTYPE>
        uchar multi_mask_region_grow(
            const cv::Mat& src,
            cv::Mat& dest,
            const int& threshold,
            int (*distance)(const TYPE&, const TYPE&),
            PSTYPE PointShift2Dfirst, PSTYPE PointShift2Dlast,
            const double& min_region_area_factor = 0.01
        ) {
            uchar padding = 1;

            return padding;
        }
    }
}

#endif //IMF_CUT__REGION_GROW
