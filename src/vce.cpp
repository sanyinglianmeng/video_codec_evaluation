#include <iostream>
#include <map>
#include <stdio.h>
#include <string>

#include "lib/command1.h"

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

int main(int argc, const char *argv[]) {
    vce::Base *p = NULL;

    p = (vce::Base *)vce::VceCommandsFactory::getInstance().CreateObjectByName("Command1");
    p->print();

    return 0;
}
