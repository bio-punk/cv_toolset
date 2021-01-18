#ifndef UTILS__POINT_SHIFT
#define UTILS__POINT_SHIFT

#include <cvtoolset.hpp>

using std::array;

namespace cvts {
    namespace utils {
        /// <summary>
        /// 8¡Ï”Ú
        /// </summary>
        const array<cv::Point, 8> PointShift2D8 {
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
        /// 4¡Ï”Ú
        /// </summary>
        const array<cv::Point, 4> PointShift2D4{
            cv::Point(-1, 0),
            cv::Point(0, -1),
            cv::Point(0, 1),
            cv::Point(1, 0)
        };
    }
}

#endif // !UTILS__POINT_SHIFT
