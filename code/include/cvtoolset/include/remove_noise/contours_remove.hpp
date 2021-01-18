#ifndef REMOVE_NOISE__CONTOURS_REMOVE
#define REMOVE_NOISE__CONTOURS_REMOVE

#include <cvtoolset.hpp>
namespace cvts {
    namespace remove_noise {

        /// <summary>
        /// ��ֵͼ������ͨ�����Сȥ��С��ͨ����
        /// </summary>
        /// <param name="src">��ȥ��ͼƬ</param>
        /// <param name="front">ǰ��ֵ</param>
        /// <param name="back">����ֵ</param>
        /// <param name="min_region_area_factor">��С�������</param>
        void contours_remove(
            cv::Mat& src,
            const uchar& front, const uchar& back,
            const double min_region_area_factor);
    }
}


void cvts::remove_noise::contours_remove(
        cv::Mat& src,
        const uchar& front, const uchar& back,
        const double min_region_area_factor) {
    const int& rows = src.rows;
    const int& cols = src.cols;
    static const int min_region_area = int(rows * cols * min_region_area_factor);
    cv::Mat labels, stats, centroids;
    int nccomps = cv::connectedComponentsWithStats(src, labels, stats, centroids);
    for (int i = 0; i < nccomps; i++) {
        int x = int(centroids.at<double>(i, 0));
        int y = int(centroids.at<double>(i, 1));
        //uchar* src_row_data = src.ptr<uchar>(y);
        uchar& src_data = src.ptr<uchar>(y)[x];
        if (back == src_data) { continue; }
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area >= min_region_area) { continue; }
        cv::floodFill(src, cv::Point(x, y), cv::Scalar(back));
    }
}

#endif // !REMOVE_NOISE__CONTOURS_REMOVE
