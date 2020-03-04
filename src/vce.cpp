#include <iostream>
#include <map>
#include <set>
#include <stdio.h>
#include <string>

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

} // namespace vce

int main(int argc, char *argv[]) {

    // vce支持的命令集合, 新加的工具在此也要注册下
    std::set<std::string> set_vce_all_orders = {"vpsnr", "checkdropframe", "ssim", "msssim", "siti"};
    // 避免初始化语句太长了，如果要新加的工具，也可以在这里insert
    // vce_orders.insert("siti");

    // set转string set_vce_all_orders->str_vce_all_orders
    std::string str_vce_all_orders = "";
    std::string seperator = ", ";
    for (auto &order : set_vce_all_orders) {
        str_vce_all_orders += order + seperator;
    }

    // 根据str_vce_all_orders构造str_vce_cmdhelp
    std::string str_vce_cmdhelp = "vce command: ";
    str_vce_cmdhelp += str_vce_all_orders;

    cmdline::parser cmdPara;
    cmdPara.add<std::string>("cmd", 'c', str_vce_cmdhelp, true, "");
    cmdPara.parse_check(argc, argv);

    std::string str_command = cmdPara.get<std::string>("cmd");

    vce::Base *p = NULL;

    // todo: str_command合法性检查，是否在set_vce_all_orders中
    p = (vce::Base *)vce::VceCommandsFactory::getInstance().CreateObjectByName(str_command);
    p->print();

    return 0;
}
