#ifndef REMOVE_NOISE__NAIVE_REMOVE
#define REMOVE_NOISE__NAIVE_REMOVE

#include <cvtoolset.hpp>

namespace cvts {
    namespace remove_noise {


        /// <summary>
        /// 根据设定好的前景与背景进行领域去噪
        /// </summary>
        /// <typeparam name="TYPE">图片类型,uchar或者Vec3b</typeparam>
        /// <typeparam name="PSTYPE">编译器自动推导,领域</typeparam>
        /// <param name="src">待去噪原图</param>
        /// <param name="front">前景值</param>
        /// <param name="back">背景值</param>
        /// <param name="race">去噪次数</param>
        /// <param name="threshold">满足条件的领域点的个数</param>
        /// <param name="is_adjacency">判断中心点与领域点是否同属于前景/背景的函数</param>
        /// <param name="PointShift2DFirst"></param>
        /// <param name="PointShift2DLast"></param>
        template <typename TYPE, typename PSTYPE>
        void front_naive_remove(
                cv::Mat& src,
                const TYPE& front, const TYPE& back,
                unsigned short& race,
                const unsigned short& threshold,
                bool (*is_adjacency)(const TYPE&, const TYPE&),
                PSTYPE PointShift2DFirst, PSTYPE PointShift2DLast
        ) {
            const int& rows = src.rows;
            const int& cols = src.cols;
            static unsigned short nValue;
            static cv::Point estimating_point;
            static cv::Point center;

            while (race--) {
                for (int k = 1; k < rows - 1; k++) {
                    for (int i = 1; i < cols - 1; i++) {
                        TYPE& src_data = src.ptr<TYPE>(k)[i];
                        if (is_adjacency(src_data, back)) { continue; }
                        nValue = 0;
                        center = cv::Point(i, k);
                        for (PSTYPE PointShift2D = PointShift2DFirst; PointShift2D != PointShift2DLast; PointShift2D++) {
                            cv::Point estimating_point = center + *PointShift2D;
                            uchar& estimating_point_data = src.ptr<TYPE>(estimating_point.y)[estimating_point.x];
                            if (is_adjacency(estimating_point_data, front)) { nValue++; }
                        }
                        if (nValue < threshold) {
                            src_data = back;
                        }
                    }
                }
            }
        }
    }
}

#endif // !REMOVE_NOISE__NAIVE_REMOVE
