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

    try {
        robot.InitNator();
    } catch (D5R::RobotException &e) {
        std::cout << e.what() << std::endl;
    }

    try {
        robot.InitRMD(port.c_str());
    } catch (D5R::RobotException &e) {
        std::cout << e.what() << std::endl;
    }

    try {
        robot.InitTopCamera();
    } catch (D5R::RobotException &e) {
        std::cout << e.what() << std::endl;
    }

    // try {
    //     robot.InitBotCamera();
    // } catch (D5R::RobotException &e) {
    //     std::cout << e.what() << std::endl;
    // }

    // robot.JointsMoveAbsolute({0, 0, 10000000, 8000000, 0});
    // robot.JointsMoveAbsolute({0, -8000000, 14000000, 0, -100});
    // cv::Mat img = cv::imread("../test/debug/image/output/clamp_bot.png");
    // Test_GetClampTemplate_BotC(img);
    // Test_GetSIFTParam(temp, CLAMP);
    robot.JointsMoveRelative({0, 0, 0, 5000000, 0});
    Test_GetAndSaveImg(robot.topCamera);
    // Test_GetAndSaveImg(robot.botCamera);

    // robot.VCJawChange();

    return 0;
}