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

#include <imgcut/utils/point_shift.hpp>

using std::stack;

namespace cvts {
    namespace img_cut {
        
        /// <summary>
        /// 从种子点开始进行生长
        /// </summary>
        /// <typeparam name="TYPE">点数据类型,uchar/cv::Vec3b</typeparam>
        /// <param name="src">原图</param>
        /// <param name="dest">已生长结果</param>
        /// <param name="mask">本次生长结果</param>
        /// <param name="seed">种子点</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="use8">使用8领域</param>
        template <typename TYPE> static void grow(
                const cv::Mat& src, const cv::Mat& dest, 
                cv::Mat& mask,
                const cv::Point& seed,
                const int& threshold,
                int (*distance)(const TYPE&, const TYPE&),
    			const bool& use8 = true
        ) {
            static stack<cv::Point> point_stack;
            point_stack.push(seed);
            static uchar* mask_row_data = mask.ptr<uchar>(seed.y);
            mask_row_data[seed.x] = 1;

            static const int rows = src.rows;
            static const int cols = src.cols;

            static unsigned int lenPonitShift2D;;
            if (use8) {
                const cv::Point(&PointShift2D)[8] = cvts::img_cut::PointShift2D8;
                lenPonitShift2D = 8;
            } else {
                const cv::Point(&PointShift2D)[4] = cvts::img_cut::PointShift2D4;
                lenPonitShift2D = 4;
            }

            cv::Point center;
            while (!point_stack.empty()) {
                center = point_stack.top();
                point_stack.pop();
                const TYPE center_data = src.ptr<TYPE>(center.y)[center.x];
                for (int i = 0; i < lenPonitShift2D; i++) {
                    cv::Point estimating_point = center + PointShift2D[i];
                    if (
                            estimating_point.x < 0
                            || estimating_point.x > src.cols - 1
                            || estimating_point.y < 0
                            || estimating_point.y > src.rows - 1) {
                        // 判断邻接点是否越过图像边界
                        continue;
                    }

					uchar* estimating_point_mask_row_data = mask.ptr<uchar>(estimating_point.y);
					uchar estimating_point_mask_data = estimating_point_mask_row_data[estimating_point.x];
                    const uchar estimating_point_dest_data = dest.ptr<uchar>(estimating_point.y)[estimating_point.x];
                    if (0 != estimating_point_dest_data || 1 == estimating_point_mask_data) { continue; }  // 如果已经在搜过了就不搜了,

			        const TYPE estimating_point_src_data = src.ptr<TYPE>(estimating_point.y)[estimating_point.x];
                    int delta = distance(center_data, estimating_point_src_data);
                    if (delta < threshold) {
                        estimating_point_mask_row_data[estimating_point.x] = 1;
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
        /// <param name="src">待求解图片</param>
        /// <param name="seedMask">种子掩模</param>
        /// <param name="dest">结果</param>
        /// <param name="threshold">阈值</param>
        /// <param name="distance">距离函数</param>
        /// <param name="min_region_area_factor">最小面积占全图百分比</param>
        /// <param name="use_8">使用8领域</param>
        template <typename TYPE> void mask_region_grow(
            const cv::Mat& src,
            const cv::Mat seedMask,
            cv::Mat& dest,
            const int& threshold,
            int (*distance)(const TYPE&, const TYPE&),
            //const cv::Point PointShift2D[8],
            const double& min_region_area_factor = 0.01,
            const bool& use_8 = true
        ) {
            const int& rows = src.rows;
            const int& cols = src.cols;
            dest = cv::Mat::zeros(rows, cols, CV_8UC1);
			cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
			const int min_region_area = int(min_region_area_factor * cols * rows);
            uchar* dest_row;
            const uchar* seedMask_row;
			static int mask_area;

            for (int k = 0; k < rows; k++) {
                dest_row = dest.ptr<uchar>(k);
		        seedMask_row = seedMask.ptr<uchar>(k);
                for (int i = 0; i < cols; i++) {
                    if ((
        		    		0 == dest_row[i]
		        	    	&& 255 == seedMask_row[i]
				    )) {
					    cvts::img_cut::grow(src, dest, mask, cv::Point(i, k), threshold, distance, use_8);
					    mask_area = (unsigned int)cv::sum(mask).val[0];
					    if (mask_area > min_region_area) {
						    dest = dest + mask;
					    }
					    mask = mask - mask;     // zero mask, ready for next "seed grow"

                    }
                }
            }
            dest *= 255;
            return;
        }
    }
}

#endif //IMF_CUT__REGION_GROW