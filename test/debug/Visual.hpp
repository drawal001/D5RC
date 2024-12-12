/**
 * @file Visual.hpp
 * @author drawal (2581478521@qq.com)
 * @brief 视觉debug函数库，开发中
 * @version 0.1
 * @date 2024-12-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "CameraBot.h"
#include "CameraTop.h"
#include <time.h>

std::string win_name = "test";
static time_t t;

/**
 * @brief 读取和保存图像，重载：topC
 *
 * @param topCamera
 */
void Test_GetAndSaveImg(D5R::CameraTop *topCamera) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1295, 1024));
    cv::Mat img_top;
    while (topCamera->Read(img_top)) {
        // cv::line(img_top, cv::Point(100, 1620), cv::Point(1800, 1620), cv::Scalar(0), 2);
        cv::imshow(win_name, img_top);
        if (cv::waitKey(1) == 27) {
            break;
        }
        if (cv::waitKey(1) == 32) {
            std::string filename =
                "../image/12_12/topC_" + std::to_string(time(&t)) + ".png";
            cv::imwrite(filename, img_top);
            // std::cout << count++ << std::endl;
            continue;
        }
    }
    cv::waitKey(0);
}

/**
 * @brief 读取和保存图像，重载：botC
 *
 * @param botCamera
 */
void Test_GetAndSaveImg(D5R::CameraBot *botCamera) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1295, 1024));
    cv::Mat img_bot;
    while (botCamera->Read(img_bot)) {

        // cv::line(img_bot, cv::Point(100, 1600), cv::Point(1800, 1600), cv::Scalar(0), 2);
        cv::imshow(win_name, img_bot);
        if (cv::waitKey(1) == 27) {
            break;
        }
        if (cv::waitKey(1) == 32) {
            std::string filename =
                "../image/12_12/botC_" + std::to_string(time(&t)) + ".png";
            cv::imwrite(filename, img_bot);
            // std::cout << count++ << std::endl;
            continue;
        }
    }
    cv::waitKey(0);
}

/**
 * @brief 获取夹钳库定位模板并保存
 *
 * @param img 输入图像
 */
void Test_GetPosTemplate(cv::Mat img) {
    cv::Rect roi(790, 130, 280, 280);
    cv::Mat ROI = img(roi).clone();
    cv::rectangle(img, roi, cv::Scalar(0, 0, 255), 4);
    cv::imshow(win_name, img);
    cv::waitKey(0);
    cv::imshow(win_name, ROI);
    cv::waitKey(0);
    cv::imwrite("../test/debug/image/PosTemple.png", ROI);
}

/**
 * @brief 根据夹钳库定位模板对图像进行切割，获取切割后的定位点
 *
 * @param img
 * @param temp
 */
void Test_GetROI(cv::Mat img, cv::Mat temp) {
    cv::Mat result;
    cv::matchTemplate(img, temp, result, cv::TM_SQDIFF_NORMED);
    cv::Point minLoc, maxLoc;
    double minVal, maxVal;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    cv::Point2f roiPos = cv::Point2f(minLoc.x - 300, minLoc.y + 300);
    std::cout << roiPos << std::endl;
    cv::Rect roi = cv::Rect(roiPos, cv::Size(850, 2046 - roiPos.y));
    cv::rectangle(img, roi, cv::Scalar(0), 4);
    cv::circle(img, roiPos + cv::Point2f(435, 1050), 4, cv::Scalar(0), 4);
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1250, 1025));
    cv::imshow("test", img);
    cv::waitKey(0);
}

/**
 * @brief 获取并保存钳口的模板，并进行图像处理确定定位信息
 *
 * @param img
 */
void Test_GetJawTemplate(cv::Mat img) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1250, 1025));

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Point2f roiP(600, 630);
    cv::Rect roi = cv::Rect(roiP, cv::Size(600, 750));
    cv::Mat roiImg = gray(roi).clone();
    // cv::rectangle(gray, roi, cv::Scalar(0), 4);
    // cv::imshow(win_name, gray);
    // cv::waitKey(0);
    // cv::imwrite("../test/debug/image/output/jaw.png", roiImg);
    // return;

    // 图像处理
    // cv::Mat bulr;
    // cv::medianBlur(roiImg, bulr, 5);
    cv::Mat jaw_binary;
    cv::threshold(roiImg, jaw_binary, 101, 255, cv::THRESH_BINARY);
    cv::Mat jaw_Gauss;
    cv::GaussianBlur(jaw_binary, jaw_Gauss, cv::Size(7, 7), 0, 0, cv::BORDER_DEFAULT);
    // Canny边缘检测
    cv::Mat edges;
    cv::Canny(jaw_Gauss, edges, 50, 150);
    // 轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // 压缩
    std::vector<cv::Point> contours_1;
    for (const auto &contour : contours) {
        // 丢弃小的轮廓
        if (contour.size() < 200) {
            continue;
        }
        contours_1.insert(contours_1.end(), contour.begin(), contour.end());
    }
    // 凸包
    std::vector<cv::Point> hull;
    cv::convexHull(cv::Mat(contours_1), hull);

    cv::Mat black = cv::Mat(roiImg.size(), roiImg.type(), cv::Scalar::all(0));

    if (hull.size() > 1) {
        for (int i = 0; i < hull.size() - 1; i++) {
            cv::line(black, hull[i], hull[i + 1], cv::Scalar(255), 2);
        }
        cv::line(black, hull[hull.size() - 1], hull[0], cv::Scalar(255), 2);
    }
    // 获取最小外接矩形
    cv::RotatedRect rect = cv::minAreaRect(hull);
    cv::Point2f rectPoints[4];
    rect.points(rectPoints);

    int shortindex = (rect.size.width < rect.size.height) ? 1 : 0;
    cv::Point2f midPoint_up =
        0.5 * (rectPoints[shortindex] + rectPoints[(shortindex + 1) % 4]);

    cv::line(roiImg, midPoint_up, rect.center, cv::Scalar(0), 2);
    for (int i = 0; i < 4; i++) {
        cv::line(roiImg, rectPoints[i], rectPoints[(i + 1) % 4], cv::Scalar(0), 2);
    }
    std::cout << rect.center << std::endl;
    std::cout << midPoint_up << std::endl;

    cv::imshow("sdf", edges);
    cv::waitKey(0);
    cv::imshow("sdf", black);
    cv::waitKey(0);
    cv::imshow("sdf", roiImg);
    cv::waitKey(0);
}

/**
 * @brief 获取钳口模板中的钳口圆心点位点
 *
 * @param img
 */
void Test_GetJawCircleCenter(cv::Mat img) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Mat bin;
    cv::threshold(gray, bin, 60, 255, cv::THRESH_BINARY);
    cv::Rect roi(205, 25, 190, 85);
    cv::Mat black = cv::Mat(img.size(), gray.type(), cv::Scalar::all(0));
    bin(roi).copyTo(black(roi));
    // cv::rectangle(img, roi, cv::Scalar(0, 0, 255), 2);
    cv::imshow(win_name, black);
    cv::waitKey(0);
    // return;
    cv::Mat inv_bin;
    cv::bitwise_not(black, inv_bin);

    cv::Mat edge;
    cv::Canny(inv_bin, edge, 50, 150);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edge, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point> contours_;
    for (auto &contour : contours) {
        contours_.insert(contours_.end(), contour.begin(), contour.end());
    }
    cv::Point2f center;
    float r;
    cv::minEnclosingCircle(contours_, center, r);
    cv::circle(img, center, r, cv::Scalar(0, 0, 255), 2);
    cv::imshow(win_name, img);
    cv::waitKey(0);
    std::cout << center << std::endl;
}

/**
 * @brief 获取SIFT关键点与匹配符，保存成yml格式文件
 *
 * @param model
 * @param m ModelType：-JAW -CLAMP
 */
void Test_GetSIFTParam(cv::Mat model, D5R::ModelType m) {
    std::string filename_keypoint, filename_descriptors;
    if (m == D5R::JAW) {
        filename_keypoint = "../test/debug/yml/KeyPoints_Jaw.yml";
        filename_descriptors = "../test/debug/yml/Descriptors_Jaw.yml";
    } else {
        filename_keypoint = "../test/debug/yml/KeyPoints_Clamp.yml";
        filename_descriptors = "../test/debug/yml/Descriptors_Clamp.yml";
    }
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> keyPoints_Model;
    sift->detect(model, keyPoints_Model);
    cv::FileStorage fs1(filename_keypoint, cv::FileStorage::WRITE);
    fs1 << "keypoints" << keyPoints_Model;
    fs1.release();

    cv::Mat descriptors_model;
    sift->compute(model, keyPoints_Model, descriptors_model);
    cv::FileStorage fs2(filename_descriptors, cv::FileStorage::WRITE);
    fs2 << "descriptors" << descriptors_model;
    fs2.release();
}

/**
 * @brief 测试SIFT匹配效果
 *
 * @param image
 * @param m ModelType：-JAW -CLAMP
 */
void Test_Match(cv::Mat image, D5R::ModelType m) {
    cv::Mat model;
    std::string filename_keypoint, filename_descriptors;
    if (m == D5R::JAW) {
        model = cv::imread("../test/debug/image/output/jaw.png", 0);
        filename_keypoint = "../test/debug/yml/KeyPoints_Jaw.yml";
        filename_descriptors = "../test/debug/yml/Descriptors_Jaw.yml";
    } else {
        model = cv::imread("../test/debug/image/output/clamp.png", 0);
        filename_keypoint = "../test/debug/yml/KeyPoints_Clamp.yml";
        filename_descriptors = "../test/debug/yml/Descriptors_Clamp.yml";
    }

    cv::Point2f roiPos(462, 468);
    cv::Rect roi = cv::Rect(roiPos, cv::Size(850, 2046 - roiPos.y));
    cv::Mat ROI = image(roi).clone();

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

    std::vector<cv::KeyPoint> keyPoints_Model;
    cv::FileStorage fs1(filename_keypoint, cv::FileStorage::READ);
    fs1["keypoints"] >> keyPoints_Model;
    fs1.release();

    cv::Mat descriptors_model;
    cv::FileStorage fs2(filename_descriptors, cv::FileStorage::READ);
    fs2["descriptors"] >> descriptors_model;
    fs2.release();

    // int64 start = cv::getTickCount();
    std::vector<cv::KeyPoint> keyPoints_Img;
    sift->detect(ROI, keyPoints_Img);
    cv::Mat descriptors_Img;
    sift->compute(ROI, keyPoints_Img, descriptors_Img);

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> goodMatches;
    matcher->knnMatch(descriptors_model, descriptors_Img, knn_matches, 2);
    for (auto &knn_matche : knn_matches) {
        if (knn_matche[0].distance < ratio_thresh * knn_matche[1].distance) {
            goodMatches.push_back(knn_matche[0]);
        }
    }
    std::cout << "good match size " << goodMatches.size() << std::endl;

    cv::Mat img_matches_knn;

    cv::drawMatches(model, keyPoints_Model, ROI, keyPoints_Img, goodMatches, img_matches_knn, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    cv::imwrite("../test/debug/image/res_clamp_match_knn_0452.png", img_matches_knn);
    std::string windowname2 = "Match res";
    cv::namedWindow(windowname2, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowname2, cv::Size(1295, 1024));
    cv::imshow(windowname2, image);
    cv::waitKey(0);
}

/**
 * @brief 获取并保存夹钳模板，并获取其定位信息
 *
 * @param img
 */
void Test_GetClampTemplate(cv::Mat img) {
    // cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    // cv::resizeWindow(win_name, cv::Size(1250, 1025));

    // cv::Point2f roiP(1300, 900);
    // cv::Rect roi = cv::Rect(roiP, cv::Size(650, 1000));
    // cv::Mat roiImg = img(roi).clone();
    // cv::rectangle(img, roi, cv::Scalar(0), 4);
    // cv::imshow(win_name, img);
    // cv::waitKey(0);
    // cv::imwrite("../test/debug/image/output/clamp.png", roiImg);
    // return;

    cv::Rect roi(0, 0, 650, 700);
    cv::Mat roiImg = img(roi).clone();
    // cv::imshow("test",roiImg);
    // cv::waitKey(0);
    // return;

    cv::Mat gray;
    cv::cvtColor(roiImg, gray, cv::COLOR_BGR2GRAY);
    cv::Mat Imgblur;

    cv::medianBlur(gray, Imgblur, 3);
    // 左右分批处理
    cv::Point roiPos_left(0, 0);
    cv::Rect roi_left = cv::Rect(roiPos_left, cv::Size(200, 450));
    cv::Mat imgRoi_left = Imgblur(roi_left).clone();
    cv::Point roiPos_right(450, 0);
    cv::Rect roi_right = cv::Rect(roiPos_right, cv::Size(200, 450));
    cv::Mat imgRoi_right = Imgblur(roi_right).clone();
    // cv::imshow("test", imgRoi_left);
    // cv::waitKey(0);
    // cv::imshow("test", imgRoi_right);
    // cv::waitKey(0);
    // return;
    // 提取轮廓
    cv::Mat edges_left;
    cv::Canny(imgRoi_left, edges_left, 50, 150);
    std::vector<std::vector<cv::Point>> contours_left;
    cv::findContours(edges_left, contours_left, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point> contour_left;
    for (auto &contour : contours_left) {
        if (contour.size() < 50) {
            continue;
        }
        contour_left.insert(contour_left.end(), contour.begin(), contour.end());
    }

    cv::Mat edges_right;
    cv::Canny(imgRoi_right, edges_right, 50, 150);
    std::vector<std::vector<cv::Point>> contours_right;
    cv::findContours(edges_right, contours_right, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point> contour_right;
    for (auto &contour : contours_right) {
        if (contour.size() < 50) {
            continue;
        }
        contour_right.insert(contour_right.end(), contour.begin(), contour.end());
    }

    std::cout << contour_left.size() << std::endl;
    std::cout << contour_right.size() << std::endl;
    // cv::imshow("test", edges_left);
    // cv::waitKey(0);
    // cv::imshow("test", edges_right);
    // cv::waitKey(0);
    // return;

    cv::Mat black_left = cv::Mat(imgRoi_left.size(), imgRoi_left.type(), cv::Scalar::all(0));
    cv::Mat black_right = cv::Mat(imgRoi_right.size(), imgRoi_right.type(), cv::Scalar::all(0));

    // 凸包
    std::vector<cv::Point> hull_left;
    cv::convexHull(cv::Mat(contour_left), hull_left);
    std::vector<cv::Point> hull_right;
    cv::convexHull(cv::Mat(contour_right), hull_right);

    // 绘制
    if (hull_left.size() > 1) {
        for (int i = 0; i < hull_left.size() - 1; i++) {
            cv::line(black_left, hull_left[i], hull_left[i + 1], cv::Scalar(255), 2);
        }
        cv::line(black_left, hull_left[hull_left.size() - 1], hull_left[0], cv::Scalar(255), 2);
    }

    if (hull_right.size() > 1) {
        for (int i = 0; i < hull_right.size() - 1; i++) {
            cv::line(black_right, hull_right[i], hull_right[i + 1], cv::Scalar(255), 2);
        }
        cv::line(black_right, hull_right[hull_right.size() - 1], hull_right[0], cv::Scalar(255), 2);
    }
    // cv::imshow("test", black_left);
    // cv::waitKey(0);
    // cv::imshow("test", black_right);
    // cv::waitKey(0);
    // return;

    // 霍夫直线拟合
    std::vector<cv::Vec4i> lines_left;
    cv::HoughLinesP(black_left, lines_left, 1, CV_PI / 180, 100, 375, 20);
    std::cout << lines_left.size() << std::endl;
    std::vector<cv::Vec4i> lines_right;
    cv::HoughLinesP(black_right, lines_right, 1, CV_PI / 180, 100, 375, 20);
    std::cout << lines_right.size() << std::endl;
    // return;
    // 绘制直线
    // 2,3
    for (auto &line : lines_left) {
        cv::line(img, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 0, 255), 2);
        float angle_left = atan2f((line[3] - line[1]), (line[2] - line[0])) * (-180) / CV_PI;
        cv::putText(img, std::to_string(angle_left), cv::Point(line[2], line[3]), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    }
    for (auto &line : lines_right) {
        cv::line(img, cv::Point(line[0], line[1]) + roiPos_right, cv::Point(line[2] + 2, line[3]) + roiPos_right, cv::Scalar(0, 0, 255), 2);
        float angle_right = atan2f((line[1] - line[3]), (line[0] - line[2] - 2)) * (-180) / CV_PI;
        cv::putText(img, std::to_string(angle_right), cv::Point(line[0], line[1]) + roiPos_right, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
    }
    cv::Point2f up = 0.5 * (cv::Point(lines_right[0][0], lines_right[0][1]) + cv::Point(lines_left[0][2], lines_left[0][3]) + roiPos_right);
    cv::Point2f down = 0.5 * (cv::Point(lines_right[0][2] + 2, lines_right[0][3]) + cv::Point(lines_left[0][0], lines_left[0][1]) + roiPos_right);
    down = up - 0.3 * (up - down);
    cv::line(img, up, down, cv::Scalar(0, 0, 255), 2);
    float angle = atan2f((up.y - down.y), (up.x - down.x)) * (-180) / CV_PI;
    cv::putText(img, std::to_string(angle), up, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));

    std::cout << up << std::endl;
    std::cout << down << std::endl;
    cv::imshow(win_name, img);
    cv::waitKey(0);

    cv::destroyAllWindows();
}

/**
 * @brief 获取钳口库水平定位线，用的是最小二乘拟合，反正最后位置也看不到，那就调FailingHeight这个函数吧
 * 
 * @param img 
 */
void Test_GetBotCameraPosLine(cv::Mat img) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1295, 275));
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Point2f roiPos(200, 1700);
    cv::Rect roi = cv::Rect(roiPos, cv::Size(2200, 348));
    cv::Mat ROI = gray(roi).clone();

    cv::imshow(win_name, ROI);
    cv::waitKey(0);
    // return;
    cv::Mat bin;
    cv::threshold(ROI, bin, 30, 255, cv::THRESH_BINARY);
    cv::Mat gauss;
    cv::GaussianBlur(bin, gauss, cv::Size(5, 5), 25);

    cv::imshow(win_name, bin);
    cv::waitKey(0);
    cv::imshow(win_name, gauss);
    cv::waitKey(0);
    // return;

    cv::Mat dst, edge;
    cv::Scharr(gauss, dst, CV_32F, 1, 0);
    cv::convertScaleAbs(dst, edge);

    cv::imshow(win_name, edge);
    cv::waitKey(0);
    // return;

    std::vector<cv::Vec4f> lines;
    cv::HoughLinesP(edge, lines, 1, CV_PI / 180, 200, 500, 300);
    std::cout << lines.size() << std::endl;
    // return;
    // 最小二乘拟合
    cv::resizeWindow(win_name, cv::Size(1295, 1048));
    int n = lines.size() * 2;
    float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    for (auto &line : lines) {
        // cv::line(img, cv::Point2f(line[0], line[1]) + roiPos, cv::Point2f(line[2], line[3]) + roiPos, cv::Scalar(0, 255, 0), 2);
        // cv::imshow(win_name, img);
        // cv::waitKey(0);
        sum_x += (line[0] + line[2] + 2 * roiPos.x);
        sum_y += (line[1] + line[3] + 2 * roiPos.y);
        sum_xy += ((line[0] + roiPos.x) * (line[1] + roiPos.y) + (line[2] + roiPos.x) * (line[3] + roiPos.y));
        sum_x2 += ((line[0] + roiPos.x) * (line[0] + roiPos.x) + (line[2] + roiPos.x) * (line[2] + roiPos.x));
    }
    float mean_x = sum_x / n;
    float mean_y = sum_y / n;

    float a = (sum_xy - n * mean_x * mean_y) / (sum_x2 - n * mean_x * mean_x);
    float b = (mean_y - a * mean_x);
    std::cout << "y = " << a << "x + " << b << std::endl;
    cv::line(img, cv::Point2f(200, 200 * a + b), cv::Point2f(2000, 2000 * a + b), cv::Scalar(0, 0, 255), 2);
    cv::imshow(win_name, img);
    cv::waitKey(0);
}

/**
 * @brief 获取BotC钳口模板，用于计算所需的下移距离
 * 
 * @param img 
 */
void Test_GetClampTemplate_BotC(cv::Mat img) {
    // cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    // cv::resizeWindow(win_name, cv::Size(1300, 1000));
    // cv::Rect roi(1300, 1300, 700, 250);
    // // cv::rectangle(img, roi, cv::Scalar(0, 0, 255), 2);
    // cv::imshow(win_name, img);
    // cv::waitKey(0);
    // cv::Mat ROI = img(roi).clone();
    // cv::imwrite("../test/debug/image/output/clamp_bot.png", ROI);
    // return;

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Mat Imgblur;
    cv::medianBlur(gray, Imgblur, 3);

    // 提取轮廓
    cv::Mat edges;
    cv::Canny(Imgblur, edges, 80, 90);
    cv::imshow("test", edges);
    cv::waitKey(0);
    cv::Rect black(0, 0, 700, 95);
    edges(black).setTo(cv::Scalar(0));
    cv::imshow("test", edges);
    cv::waitKey(0);

    std::vector<std::vector<cv::Point>> contours_;
    cv::findContours(edges, contours_, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point> contours;
    for (auto &contour : contours_) {
        contours.insert(contours.end(), contour.begin(), contour.end());
    }

    cv::RotatedRect rect = cv::minAreaRect(contours);
    cv::Point2f rectPoints[4];
    rect.points(rectPoints);

    // 绘制最小外接矩形
    for (int i = 0; i < 4; i++) {
        cv::line(img, rectPoints[i], rectPoints[(i + 1) % 4], cv::Scalar(255), 2);
        std::cout << rectPoints[i] << std ::endl;
    }
    cv::imshow(win_name, img);
    cv::waitKey(0);
    return;
}

/**
 * @brief 获取z轴下移距离函数，使用的是模板匹配一步到位，由于botC看不到最终的位置，需要反复调试
 * 
 * @param img 
 * @return double 
 */
double Test_GetFallingHeight(cv::Mat img) {
    std::vector<cv::Point2f> _points;
    _points.push_back(cv::Point2f(93.9549, 95.2925));
    _points.push_back(cv::Point2f(513.976, 91.9765));
    float _line_a = -0.00116294;
    float _line_b = 1718.94;
    float _mapParam = 0.00943614;

    cv::Mat res;
    cv::Mat clamp = cv::imread("../test/debug/image/output/clamp_bot.png", 0);
    cv::matchTemplate(img, clamp, res, cv::TM_SQDIFF_NORMED);
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

void Test_Deformation_SURF(cv::Mat img){
    cv::Mat temp = cv::imread("../test/debug/image/output/jaw.png");
    cv::Point roiPos_L(30, 0), roiPos_R(360, 0);
    cv::Rect roi_L(roiPos_L, cv::Size(270, 800)), roi_R(roiPos_R, cv::Size(270, 800));
    cv::rectangle(temp, roi_L, cv::Scalar(0, 0, 255), 2);
    cv::rectangle(temp, roi_R, cv::Scalar(0, 0, 255), 2);
    cv::imshow(win_name, temp);
    cv::waitKey(0);
}