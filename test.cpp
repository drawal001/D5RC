#include "D5Robot.h"
#include "DllApi.h"
#include "KineHelper.hpp"
#include "RobotException.hpp"
#include "Visual.hpp"

const std::string port = "\\\\.\\COM16";

int main() {
    std::string root = "E:/WYL_workspace/D5RC/";
    std::cout << "Hello Robot!" << std::endl;
    D5R::D5Robot robot;

    // try {
    //     robot.InitNator();
    // } catch (D5R::RobotException &e) {
    //     std::cout << e.what() << std::endl;
    // }

    // try {
    //     robot.InitRMD(port.c_str());
    // } catch (D5R::RobotException &e) {
    //     std::cout << e.what() << std::endl;
    // }

    // try {
    //     robot.InitTopCamera();
    // } catch (D5R::RobotException &e) {
    //     std::cout << e.what() << std::endl;
    // }

    // try {
    //     robot.InitBotCamera();
    // } catch (D5R::RobotException &e) {
    //     std::cout << e.what() << std::endl;
    // }

    // robot.JointsMoveAbsolute({0, 0, 8000000, 8300000, 0});
    // robot.JointsMoveAbsolute({0, 0, 8000000, 0, 0});
    cv::Mat img = cv::imread("../test/debug/image/output/clamp_bot.png");
    Test_GetClampTemplate_BotC(img);
    // Test_GetSIFTParam(temp, CLAMP);
    // Test_GetAndSaveImg(robot.topCamera);

    // robot.VCJawChange();

    return 0;
}

// cv::waitKey(0);
// cv::Point2f d(567, 475);
// cv::Mat img;
// robot.topCamera->Read(img);
// robot.topCamera->GetROI(img);
// // 获取一次钳口位置信息
// std::vector<cv::Point2f> jawPos;
// robot.topCamera->SIFT(img, JAW, jawPos);
// float jawAngle = atan2f(jawPos[1].y - jawPos[0].y, jawPos[1].x - jawPos[0].x) * (-180) / CV_PI;
// std::vector<cv::Point2f> clampPos;
// robot.topCamera->SIFT(img, CLAMP, clampPos);
// // cv::line(img, jawPos[0] + d, jawPos[1] + d, cv::Scalar(0), 2);
// cv::line(img, clampPos[0] + d, clampPos[1] + d, cv::Scalar(0), 10);
// std::string winname = "test";
// cv::namedWindow(winname, cv::WINDOW_NORMAL);
// cv::resizeWindow(winname, cv::Size(1295, 1024));
// cv::imshow(winname, img);
// cv::waitKey(0);

// cv::Mat img = cv::imread("E:/WYL_workspace/D5RC/image/12_7/topC_0.png", 0);
// robot.topCamera->GetROI(img);
// std::vector<cv::Point2f> jawPos;
// robot.topCamera->SIFT(img, JAW, jawPos);
// float jawAngle = atan2f(jawPos[1].y - jawPos[0].y, jawPos[1].x - jawPos[0].x) * (-180) / CV_PI;
// std::vector<cv::Point2f> clampPos;
// robot.topCamera->SIFT(img, CLAMP, clampPos);
// float clampAngle = atan2f(clampPos[0].y - clampPos[1].y, clampPos[0].x - clampPos[1].x) * (-180) / CV_PI;
// // cv::line(img, jawPos[0] + d, jawPos[1] + d, cv::Scalar(0), 2);
// std::cout << "sdfa  " << jawAngle << "  sdf   " << clampAngle << std::endl;