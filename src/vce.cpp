#include <iostream>
#include <map>
#include <set>
#include <stdio.h>
#include <string>

#include "lib/cmd_ssim.h"
#include "lib/command1.h"
#include "utils/cmdlineutils.h"

namespace vce {

typedef void *(*PCreateObject)(void);

#define REGISTER(className)                                                                                                                \
    className *objectCreator##className() {                                                                                                \
        return new className;                                                                                                              \
    }                                                                                                                                      \
    RegisterAction g_creatorRegister##className(#className, (PCreateObject)objectCreator##className)

class VceCommandsFactory {
private:
    std::map<std::string, PCreateObject> m_classMap;
    VceCommandsFactory(){};

public:
    void *CreateObjectByName(std::string className) {
        std::map<std::string, PCreateObject>::const_iterator iter;
        iter = m_classMap.find(className);
        if (iter == m_classMap.end())
            return NULL;
        else
            return iter->second(); //函数指针的调用
    }
    void registClass(std::string name, PCreateObject method) {
        m_classMap.insert(std::pair<std::string, PCreateObject>(name, method));
    }
    static VceCommandsFactory &getInstance() {
        {
            static VceCommandsFactory cf;
            return cf;
        }
    }
};

class RegisterAction {
public:
    RegisterAction(std::string className, PCreateObject ptrCreateFn) {
        VceCommandsFactory::getInstance().registClass(className, ptrCreateFn);
    }
};

// 新加的工具在此注册
REGISTER(Command1);
REGISTER(ssim);

} // namespace vce

int main(int argc, char *argv[]) {

    // vce支持的命令集合, 新加的工具在此也要注册下
    // std::set<std::string> set_vce_all_orders = {"vpsnr", "checkdropframe", "ssim", "msssim", "siti"};
    std::set<std::string> set_vce_all_orders = {"ssim"};
    // 避免初始化语句太长了，如果要新加的工具，也可以在这里insert
    // vce_orders.insert("siti");

    // set转string set_vce_all_orders->str_vce_all_orders
    std::string str_vce_all_orders = "";
    std::string seperator = ", ";
    for (auto &order : set_vce_all_orders) {
        str_vce_all_orders += order + seperator;
    }

    // 根据str_vce_all_orders构造str_vce_cmdhelp
    std::string str_vce_cmdhelp = "vce [command options]: ";
    str_vce_cmdhelp += str_vce_all_orders;

    // str_command合法性检查，要在set_vce_all_orders中注册
    std::string str_command = "";
    if (argc > 1) {
        str_command = argv[1];
    }
    else {
        std::cerr << "too little params been got. please use: \n" << str_vce_cmdhelp << "as option " << std::endl;
        return 0;
    }

    if (set_vce_all_orders.find(str_command) == set_vce_all_orders.end()) {
        std::cerr << "cmd option is not valid. please use: \n" << str_vce_cmdhelp << "as option " << std::endl;
        return 0;
    }

    vce::Base *p = NULL;

    p = (vce::Base *)vce::VceCommandsFactory::getInstance().CreateObjectByName(str_command);
// run函数作为command执行的统一入口
#ifdef DEBUG
    std::cout << "begin command " << str_command << std::endl;
#endif
    p->run(argc, argv);

    return 0;
}
