#pragma once
#include "CameraBot.h"
#include "CameraTop.h"

std::string win_name = "test";

void GetAndSaveImg(D5R::CameraTop *topCamera) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1295, 1024));
    int count = 0;
    cv::Mat img_top;
    while (topCamera->Read(img_top)) {

        // cv::line(img_top, cv::Point(100, 1600), cv::Point(1800, 1600), cv::Scalar(0), 2);
        cv::imshow(win_name, img_top);
        if (cv::waitKey(1) == 27) {
            break;
        }
        if (cv::waitKey(1) == 32) {
            // topCamera.GetMapParam(img_top);
            std::string filename =
                "../image/12_7/topC_j" + std::to_string(count++) + ".png";
            cv::imwrite(filename, img_top);
            // std::cout << count++ << std::endl;
            continue;
        }
    }
    cv::waitKey(0);
}

void GetAndSaveImg(D5R::CameraBot *botCamera) {
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1295, 1024));
    int count = 0;

    cv::Mat img_bot;
    while (botCamera->Read(img_bot)) {

        // cv::line(img_bot, cv::Point(100, 1600), cv::Point(1800, 1600), cv::Scalar(0), 2);
        cv::imshow(win_name, img_bot);
        if (cv::waitKey(1) == 27) {
            break;
        }
        if (cv::waitKey(1) == 32) {
            // botCamera.GetMapParam(img_bot);
            std::string filename =
                "../image/12_7/botC_" + std::to_string(count++) + ".png";
            cv::imwrite(filename, img_bot);
            // std::cout << count++ << std::endl;
            continue;
        }
    }
    cv::waitKey(0);
}

void GetPosTemplate(cv::Mat img, std::string root) {
    cv::Rect roi(790, 130, 280, 280);
    cv::Mat ROI = img(roi).clone();
    cv::rectangle(img, roi, cv::Scalar(0, 0, 255), 4);
    cv::imshow(win_name, img);
    cv::waitKey(0);
    cv::imshow(win_name, ROI);
    cv::waitKey(0);
    cv::imwrite(root + "test/debug/image/PosTemple.png", ROI);
}

void GetROI(cv::Mat img, cv::Mat temp) {
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
    std::string win_name = "test";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1250, 1025));
    cv::imshow("test", img);
    cv::waitKey(0);
}

void GetJawTemple(cv::Mat img){
    std::string win_name = "test";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(win_name, cv::Size(1250, 1025));

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat bulr;
    cv::medianBlur(gray, bulr, 5);

    cv::Point2f roiP(600, 560);
    cv::Rect roi = cv::Rect(roiP, cv::Size(650, 800));
    cv::Mat roiImg = bulr(roi).clone();
    // cv::rectangle(gray, roi, cv::Scalar(0), 4);
    // cv::imshow(win_name, gray);
    // cv::waitKey(0);
    // cv::imwrite("../test/debug/image/output/jaw.png", roiImg);
    // return;
    // 图像处理
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