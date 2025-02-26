#include "riscv64_lowerframe.h"

/*
    假设IR中的函数定义为f(i32 %r0, i32 %r1)
    则parameters应该存放两个虚拟寄存器%0,%1

    在LowerFrame后应当为
    add %0,a0,x0  (%0 <- a0)
    add %1,a1,x0  (%1 <- a1)

    对于浮点寄存器按照类似的方法处理即可
*/
void RiscV64LowerFrame::Execute() {
    // 在每个函数的开头处插入获取参数的指令
    for (auto func : unit->functions) {
        current_func = func;
        int offset=0;
        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {    // 函数入口，需要插入获取参数的指令
                int i32_cnt = 0;
                int f32_cnt = 0;
                for (auto para : func->GetParameters()) {    // 你需要在指令选择阶段正确设置parameters的值
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {    // 插入使用寄存器传参的指令
                            b->push_front(rvconstructor->ConstructR(RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt),
                                                                    GetPhysicalReg(RISCV_x0)));
                        }
                        if (i32_cnt >= 8) {    // 插入使用内存传参的指令
                            //int offset=(i32_cnt-1)*8;
                            b->push_front(rvconstructor->ConstructIImm(RISCV_LD,para,GetPhysicalReg(RISCV_fp),offset));
                            offset+=8;
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {    // 处理浮点数
                        if (f32_cnt < 8) {    // 插入使用寄存器传参的指令
                            b->push_front(rvconstructor->ConstructR2(RISCV_FMV_S, para, GetPhysicalReg(RISCV_fa0 + f32_cnt)));
                        }
                        if (i32_cnt >= 8) {    // 插入使用内存传参的指令
                            //int offset=(f32_cnt-1)*8;
                            b->push_front(rvconstructor->ConstructIImm(RISCV_LD,para,GetPhysicalReg(RISCV_fp),offset));
                            offset+=8;
                        }
                        f32_cnt++;
                    } else {
                        ERROR("Unknown type");
                    }
                }
            }
        }
    }
}

void RiscV64LowerStack::Execute() {
    // 在函数在寄存器分配后执行
    // TODO: 在函数开头保存 函数被调者需要保存的寄存器，并开辟栈空间
    // TODO: 在函数结尾恢复 函数被调者需要保存的寄存器，并收回栈空间
    // 具体需要保存/恢复哪些可以查阅RISC-V函数调用约定
    for (auto func : unit->functions) {
        current_func = func;
        //save_res
        int save_reg_num=25;
        current_func->AddStackSize(save_reg_num*8);//保存寄存器的空间

        for (auto &b : func->blocks) {
            cur_block = b;
            //【1】在入口块前添加指令，保存函数被调者需要保存的寄存器
            if (b->getLabelId() == 0) { 
                //保存寄存器

                //（2）开辟栈空间
                //fp<-- sp，帧指针移动
                auto instr0=rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_fp),GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_x0));
                //sp - stacksize ，栈指针移动
                auto stacksz_reg = GetPhysicalReg(RISCV_t0);//此处不会再有寄存器分配为他服务，所以只能调用物理寄存器
                auto instr1=rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize());
                auto instr2=rvconstructor->ConstructR(RISCV_SUB, GetPhysicalReg(RISCV_sp),GetPhysicalReg(RISCV_sp), stacksz_reg);//栈从高地址向低地址增长
                b->push_front(instr2);
                b->push_front(instr1);
                b->push_front(instr0);
                
                //（1）store寄存器
                int offset = 0;
                for(int i=0;i<25;i++){
                    int reg_no=save_regs[i];
                    offset-=8;
                    if (reg_no >= RISCV_x0 && reg_no <= RISCV_x31) {
                        b->push_front(rvconstructor->ConstructSImm(RISCV_SD, GetPhysicalReg(reg_no),GetPhysicalReg(RISCV_sp), offset));
                    } else {
                        b->push_front(rvconstructor->ConstructSImm(RISCV_FSD, GetPhysicalReg(reg_no),GetPhysicalReg(RISCV_sp), offset));
                    }
                }
            }
            /*
                ref: https://github.com/yuhuifishash/SysY
                下面的75~79行，参考了判断函数末尾的方法
            */
           //【2】在函数末尾添加指令，恢复函数被调者需要保存的寄存器
            RiscV64Instruction * block_last_ins=(RiscV64Instruction *)(*(cur_block->ReverseBegin()));
            if (block_last_ins->getOpcode() == RISCV_JALR) {
                if (block_last_ins->getRd() == GetPhysicalReg(RISCV_x0)) {
                    if (block_last_ins->getRs1() == GetPhysicalReg(RISCV_ra)) {
                        cur_block->pop_back();//先将jar ra吐出去，加入恢复regs的指令，最后再加回来
                        //恢复寄存器

                        //（1）收回栈空间
                        auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                        b->push_back(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));//stack_size
                        b->push_back(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_sp),GetPhysicalReg(RISCV_sp), stacksz_reg));//收回栈空间

                        //（2）load 寄存器
                        int offset = 0;
                        for(int i=0;i<25;i++){
                            int reg_no=save_regs[i];
                            offset-=8;
                            if (reg_no >= RISCV_x0 && reg_no <= RISCV_x31) {
                                b->push_back(rvconstructor->ConstructIImm(RISCV_LD, GetPhysicalReg(reg_no),GetPhysicalReg(RISCV_sp), offset));
                            } else {
                                b->push_back(rvconstructor->ConstructIImm(RISCV_FLD, GetPhysicalReg(reg_no),GetPhysicalReg(RISCV_sp), offset));
                            }
                        }
                        

                        cur_block->push_back(block_last_ins);//把jar ra加回来
                    }
                }
            }

        }

    }
    // 到此我们就完成目标代码生成的所有工作了
}

