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

    // robot.InitNator();

    // robot.InitRMD(port.c_str());

    // robot.InitTopCamera();

    // robot.InitBotCamera();

    // robot.JointsMoveAbsolute({0, 0, 7500000, 5000000, 0});
    // robot.JointsMoveRelative({0, 0, 1000000, 0, 0});
    // robot.JointsMoveAbsolute({0, 0, 7500000, 0, 0});

    cv::Mat img = cv::imread("../test/debug/image/output/jaw.png");
    Test_GetJawCircleCenter(img);

    // robot.VCJawChange();

    // Test_GetAndSaveImg(robot.topCamera);
    // Test_GetAndSaveImg(robot.botCamera);
    // cv::Mat img = cv::imread("../image/12_10/topC_404.png");
    // Test_Deformation_SURF(img);

    return 0;
}