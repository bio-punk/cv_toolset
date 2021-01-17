
#ifndef IMG_CUT__UTILS__POINT_SHIFT
#define IMG_CUT__UTILS__POINT_SHIFT

#include <cvtoolset.hpp>

using std::array;

namespace cvts {
    namespace img_cut {
        /// <summary>
        /// 8领域
        /// </summary>
        const cv::Point PointShift2D8[8] = { 
            cv::Point(-1, -1),
            cv::Point(-1, 0),
            cv::Point(-1, 1),
            cv::Point(0, -1),
            cv::Point(0, 1),
            cv::Point(1, -1),
            cv::Point(1, 0),
            cv::Point(1, 1)
        };

        /// <summary>
        /// 4领域
        /// </summary>
        const cv::Point PointShift2D4[4] = { 
            cv::Point(-1, 0),
            cv::Point(0, -1),
            cv::Point(0, 1),
            cv::Point(1, 0)
        };
    }
}

#endif //IMG_CUT__UTILS__POINT_SHIFT