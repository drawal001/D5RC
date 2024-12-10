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

    // robot.JointsMoveAbsolute({0, 0, 14000000, 5000000, 0});
    // robot.JointsMoveRelative({0, 0, 0, 5000000, 0});
    // robot.JointsMoveAbsolute({0, 0, 7500000, 0, 0});

    // cv::Mat img = cv::imread("../image/12_10/botC_1.png");
    // Test_GetBotCameraPosLine(img);

    // robot.VCJawChange();

    // Test_GetAndSaveImg(robot.topCamera);
    // Test_GetAndSaveImg(robot.botCamera);

    return 0;
}