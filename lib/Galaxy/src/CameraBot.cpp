/**
 * @file CameraBot.cpp
 * @author drawal (2581478521@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-11-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "CameraBot.h"

namespace D5R {

/**
 * @brief 构造函数
 *
 * @param id 大恒相机MAC地址
 */
CameraBot::CameraBot(std::string id) : GxCamera(id) {
    // 加载模板
    std::string root(ROOT_DIR);
    _clamp = cv::imread(root + "/lib/Galaxy/image/model/botCTemplate/clamp_bot.png", 0);
    _points.push_back(cv::Point2f(93.9549, 95.2925));
    _points.push_back(cv::Point2f(513.976, 91.9765));
    _line_a = -0.00116294;
    _line_b = 1718.94;
    _mapParam = 0.00943614; // 需要重新标定，只标定高度
}

/**
 * @brief 获取平台的水平线 y = ax + b, 每次移动相机后均要重新更新
 *
 * @param img 输入图像，为灰度图
 */
void CameraBot::GetHorizontalLine(cv::Mat img) {
    // 将钳口台下半部分遮住，防止干扰，具体使用根据钳口台与相机高度而定
    cv::Point2f roiPos(200, 1700);
    cv::Rect roi = cv::Rect(roiPos, cv::Size(2200, 348));
    cv::Mat ROI = img(roi).clone();

    // 图像处理
    cv::Mat bin;
    cv::threshold(ROI, bin, 50, 255, cv::THRESH_BINARY);
    cv::Mat gauss;
    cv::GaussianBlur(bin, gauss, cv::Size(5, 5), 25);
    cv::Mat dst, edge;
    cv::Scharr(gauss, dst, CV_32F, 1, 0);
    cv::convertScaleAbs(dst, edge);
    std::vector<cv::Vec4f> lines;
    cv::HoughLinesP(edge, lines, 1, CV_PI / 180, 200, 500, 300);

    // 最小二乘拟合
    int n = lines.size() * 2;
    float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    for (auto &line : lines) {
        sum_x += (line[0] + line[2] + 2 * roiPos.x);
        sum_y += (line[1] + line[3] + 2 * roiPos.y);
        sum_xy += ((line[0] + roiPos.x) * (line[1] + roiPos.y) + (line[2] + roiPos.x) * (line[3] + roiPos.y));
        sum_x2 += ((line[0] + roiPos.x) * (line[0] + roiPos.x) + (line[2] + roiPos.x) * (line[2] + roiPos.x));
    }
    float mean_x = sum_x / n;
    float mean_y = sum_y / n;

    _line_a = (sum_xy - n * mean_x * mean_y) / (sum_x2 - n * mean_x * mean_x);
    _line_b = (mean_y - _line_a * mean_x);
}

/**
 * @brief 获取夹钳模板的定位信息，作为参考函数，在实际控制中不调用
 *
 * @param img
 * @return std::vector<cv::Point2f>
 */
std::vector<cv::Point2f> CameraBot::GetModelPoints(cv::Mat img) {
    return {};
    // 图像处理
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Mat bin;
    cv::threshold(gray, bin, 200, 255, cv::THRESH_BINARY);
    cv::Mat dst_1, dst_2;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(bin, dst_1, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(dst_1, dst_2, cv::MORPH_CLOSE, kernel);
    cv::Mat edges;
    cv::Canny(dst_2, edges, 50, 150);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // 压缩
    std::vector<cv::Point> contours_;
    for (auto &contour : contours) {
        if (contour.size() < 20) {
            continue;
        }
        contours_.insert(contours_.end(), contour.begin(), contour.end());
    }

    cv::RotatedRect rect = cv::minAreaRect(contours_);
    std::vector<cv::Point2f> rectPoints;
    rect.points(rectPoints);
    return rectPoints;
}

/**
 * @brief 返回夹钳与平台的垂直距离 + 0.3mm
 *
 * @param img 输入图像，一般为灰度图
 * @return double
 */
double CameraBot::GetDistance(cv::Mat img) {
    cv::Mat res;
    cv::matchTemplate(img, _clamp, res, cv::TM_SQDIFF_NORMED);
    cv::Point minLoc, maxLoc;
    double minVal, maxVal;
    cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);
    cv::Point2f minLoc_(minLoc.x, minLoc.y);
    double distance = 0;
    for (int i = 0; i < _points.size(); ++i) {
        distance += (abs(_line_a * (_points[i].x + minLoc_.x) - _points[i].y - minLoc_.y + _line_b) / sqrt(_line_a * _line_a + 1));
    }
    distance /= _points.size();
    return -distance * _mapParam + 0.3;
}

/**
 * @brief 获取相机映射参数，只标定z轴
 *
 * @param Calibration_board 标定板图，灰度
 */
void CameraBot::GetMapParam(cv::Mat Calibration_board) {
    std::vector<cv::Point2f> corner;
    if (!cv::findChessboardCorners(Calibration_board, cv::Size(19, 15), corner)) {
        std::cerr << "Failed to find corners in image" << std::endl;
        return;
    }
    const cv::TermCriteria criteria{cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.001};
    cv::cornerSubPix(Calibration_board, corner, cv::Size(6, 6), cv::Size(-1, -1), criteria);
    cv::drawChessboardCorners(Calibration_board, cv::Size(19, 15), corner, true);

    std::string imagename = "Calibration_board";
    cv::namedWindow(imagename, cv::WINDOW_NORMAL);
    cv::resizeWindow(imagename, cv::Size(1295, 1024));
    cv::imshow(imagename, Calibration_board);

    float sum = 0;
    cv::Point2f last_point{};
    for (int i = 0; i < 19; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (j == 0) {
                last_point = corner[j * 19 + i];
                continue;
            }
            auto a = sqrt(powf(corner[j * 19 + i].x - last_point.x, 2) + powf(corner[j * 19 + i].y - last_point.y, 2));
            sum += a;
            last_point = corner[j * 19 + i];
        }
    }
    float map_param = 14 * 19 / sum;
    std::cout << "map_param: " << map_param << " (mm/pixel)" << std::endl;
    cv::waitKey(0);
}

CameraBot::~CameraBot() {}
} // namespace D5R
