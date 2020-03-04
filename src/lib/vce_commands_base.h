#ifndef vce_commands_base_h
#define vce_commands_base_h

namespace vce {

class Base {
public:
    virtual void run(int argc, char *argv[]) = 0;

private:
    // 只做提醒实现, 不强制应用开发者进行参数检查
    virtual void params_check(int argc, char *argv[]) = 0;
};

} // namespace vce

#endif /* vce_commands_base_h */
