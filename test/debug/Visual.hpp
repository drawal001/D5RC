#pragma once
#include "CameraBot.h"
#include "CameraTop.h"

std::string win_name = "test";

void Test_GetAndSaveImg(D5R::CameraTop *topCamera) {
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

void Test_GetAndSaveImg(D5R::CameraBot *botCamera) {
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

void Test_GetPosTemplate(cv::Mat img, std::string root) {
    cv::Rect roi(790, 130, 280, 280);
    cv::Mat ROI = img(roi).clone();
    cv::rectangle(img, roi, cv::Scalar(0, 0, 255), 4);
    cv::imshow(win_name, img);
    cv::waitKey(0);
    cv::imshow(win_name, ROI);
    cv::waitKey(0);
    cv::imwrite(root + "test/debug/image/PosTemple.png", ROI);
}

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

void Test_GetJawTemple(cv::Mat img) {
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

void Test_GetJawCircleCenter(cv::Mat img) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Mat bin;
    cv::threshold(gray, bin, 60, 255, cv::THRESH_BINARY);
    cv::Rect roi(235, 45, 190, 85);
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

void Test_GetSIFTParam(cv::Mat model) {
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> keyPoints_Model;
    sift->detect(model, keyPoints_Model);
    cv::FileStorage fs1("../test/debug/yml/KeyPoints_Jaw.yml", cv::FileStorage::WRITE);
    fs1 << "keypoints" << keyPoints_Model;
    fs1.release();

    cv::Mat descriptors_model;
    sift->compute(model, keyPoints_Model, descriptors_model);
    cv::FileStorage fs2("../test/debug/yml/Descriptors_Jaw.yml", cv::FileStorage::WRITE);
    fs2 << "descriptors" << descriptors_model;
    fs2.release();
}

void Test_Match(cv::Mat image) {
    cv::Mat model = cv::imread("../test/debug/image/output/jaw.png", 0);

    cv::Point2f roiPos(489, 422);
    cv::Rect roi = cv::Rect(roiPos, cv::Size(850, 2046 - roiPos.y));
    cv::Mat ROI = image(roi).clone();

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

    std::vector<cv::KeyPoint> keyPoints_Model;
    cv::FileStorage fs1("../test/debug/yml/KeyPoints_Jaw.yml", cv::FileStorage::READ);
    fs1["keypoints"] >> keyPoints_Model;
    fs1.release();

    cv::Mat descriptors_model;
    cv::FileStorage fs2("../test/debug/yml/Descriptors_Jaw.yml", cv::FileStorage::READ);
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

    // std::vector<cv::Point2f> model_P, img_P;
    // for (const auto &match : goodMatches) {
    //     model_P.push_back(keyPoints_Model[match.queryIdx].pt);
    //     img_P.push_back(keyPoints_Img[match.trainIdx].pt);
    // }
    // cv::Mat homography = cv::findHomography(model_P, img_P, cv::RANSAC);

    // std::vector<cv::Point2f> modelPosition = {cv::Point2f(448, 63), cv::Point2f(445.8, 101)};
    // std::vector<cv::Point2f> ImgPosition;
    // cv::perspectiveTransform(modelPosition, ImgPosition, homography);
    // cv::line(image, ImgPosition[0] + cv::Point2f(800, 648), ImgPosition[1] + cv::Point2f(800, 648), cv::Scalar(0, 0, 255), 2);
    // float angle = atan2f((ImgPosition[0].y - ImgPosition[1].y), (ImgPosition[0].x - ImgPosition[1].x)) * (-180) / CV_PI;
    // cv::putText(image, std::to_string(angle), ImgPosition[0] + cv::Point2f(800, 648), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 2);

    // int64 end = cv::getTickCount();
    // std::cout << 1000.0 * (end - start) / cv::getTickFrequency() << std::endl;

    cv::Mat img_matches_knn;

    cv::drawMatches(model, keyPoints_Model, ROI, keyPoints_Img, goodMatches, img_matches_knn, cv::Scalar::all(-1),cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    cv::imwrite("../test/debug/image/res_clamp_match_knn_0452.png", img_matches_knn);
    std::string windowname2 = "Match res";
    cv::namedWindow(windowname2, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowname2, cv::Size(1295, 1024));
    cv::imshow(windowname2, image);
    cv::waitKey(0);
}