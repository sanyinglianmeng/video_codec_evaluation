#ifndef vce_commands_base_h
#define vce_commands_base_h

namespace vce {

class Base {
public:
    virtual void run(int argc, char *argv[]) = 0;

private:
    // 只做提醒实现, 不应强制cmd类进行参数检查，比如没有入参的cmd
    virtual void params_check(int argc, char *argv[]) = 0;
    // 预处理留空
    virtual void pre_action() = 0;
    // command执行
    virtual void do_action() = 0;
    // 后处理留空
    virtual void after_action() = 0;
};

} // namespace vce

#endif /* vce_commands_base_h */
