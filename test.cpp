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

    robot.InitNator();

    robot.InitRMD(port.c_str());

    // robot.InitTopCamera();

    // robot.InitBotCamera();

    robot.JointsMoveAbsolute({0, 0, 7500000, 5000000, 0});
    // robot.JointsMoveRelative({0, 0, 1000000, 0, 0});
    // robot.JointsMoveAbsolute({0, 0, 7500000, 0, 0});

    // cv::Mat img = cv::imread("../image/12_10/botC_1.png");
    // Test_GetBotCameraPosLine(img);
    // Sleep(4000);
    // cv::Mat img = cv::imread("../image/12_10/botC_2.png", 0);
    // double h = Test_GetFallingHeight(img);
    // TaskSpace pError{0, 0, h, 0, 0};
    // JointSpace jError = KineHelper::InverseDifferential(pError, robot.GetCurrentPose());
    // std::cout << h << std::endl;
    // std::cout << std::endl;
    // std::cout << jError.R1 << std::endl;
    // std::cout << jError.P2 << std::endl;
    // std::cout << jError.P3 << std::endl;
    // std::cout << jError.P4 << std::endl;
    // std::cout << jError.R5 << std::endl;

    // robot.VCJawChange();

    // Test_GetAndSaveImg(robot.topCamera);
    // Test_GetAndSaveImg(robot.botCamera);

    return 0;
}