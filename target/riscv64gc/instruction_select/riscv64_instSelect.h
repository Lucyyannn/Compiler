#ifndef RISCV64_INSTSELECT_H
#define RISCV64_INSTSELECT_H
#include "../../common/machine_passes/machine_selector.h"
#include "../riscv64.h"
#include <tuple> 
class RiscV64Selector : public MachineSelector {
private:
    int cur_offset;    // 局部变量在栈中的偏移
    //std::map<int,std::tuple<Register,Register,riscvOpcode>> br_context;//result_reg_no-->(op1,op2,opcode)
    std::map<int,Instruction>br_context;
    std::map<int,bool> array_arg;
    // 你需要保证在每个函数的指令选择结束后, cur_offset的值为局部变量所占栈空间的大小

    /*
        ref: https://github.com/yuhuifishash/SysY
        下面的14~17行，参考了两个map的维护和获取寄存器的方法
    */
    std::map<int, Register> llvm_rv_regtable;  //reg_no --> register
    std::map<int, int> llvm_rv_allocas;        //寄存器号--->栈上偏移
    Register GetllvmReg(int, MachineDataType);
    Register GetNewReg(MachineDataType);
    Register FImmToReg(float fimm);
    Register IImmToReg(int iimm);
    void FImmToRReg(Register reg,float fimm);
public:
    RiscV64Selector(MachineUnit *dest, LLVMIR *IR) : MachineSelector(dest, IR) {}
    void SelectInstructionAndBuildCFG();
    void ClearFunctionSelectState();
    template <class INSPTR> void ConvertAndAppend(INSPTR);
};
#endif