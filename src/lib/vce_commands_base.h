#ifndef vce_commands_base_h
#define vce_commands_base_h

namespace vce {

class Base {
public:
    virtual void run(int argc, char *argv[]) = 0;
};

} // namespace vce

#endif /* vce_commands_base_h */
