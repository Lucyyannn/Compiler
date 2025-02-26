#include "riscv64_instSelect.h"
#include <sstream>

//根据ir寄存器号获取相应寄存器，没有则新建
Register RiscV64Selector::GetllvmReg(int ir_reg, MachineDataType type) {
    if (llvm_rv_regtable.find(ir_reg) == llvm_rv_regtable.end()) {
        llvm_rv_regtable[ir_reg] = GetNewReg(type);
    }
    return llvm_rv_regtable[ir_reg];
}
//新建寄存器
Register RiscV64Selector::GetNewReg(MachineDataType type) {
    return cur_func->GetNewRegister(type.data_type, type.data_length);
}

Register RiscV64Selector::IImmToReg(int iimm){
    Register result=GetNewReg(INT64);
    if(iimm>=-2048&&iimm<2047){
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,result,iimm));
    }else{
        auto mid_reg=GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LUI,mid_reg,((unsigned int)((iimm+0x800)>>12))));
        cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI,result,mid_reg,(iimm<<12)>>12));
    }
    return result;
}
Register RiscV64Selector::FImmToReg(float fimm){
    Register result=GetNewReg(FLOAT64);
    auto int_reg=IImmToReg(fimm);
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X,result,int_reg));
    return result;
}
void RiscV64Selector::FImmToRReg(Register reg,float fimm){
    auto result=GetNewReg(FLOAT64);
    auto int_reg=IImmToReg(fimm);
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X,reg,int_reg));
    return ;
}

template <> void RiscV64Selector::ConvertAndAppend<LoadInstruction *>(LoadInstruction *ins) {
    //【0.1】DataType-->RiscVinst的映射
    std::map<BasicInstruction::LLVMType,riscvOpcode>opcode;
    opcode[BasicInstruction::LLVMType::I32]=RISCV_LW;
    opcode[BasicInstruction::LLVMType::FLOAT32]=RISCV_FLW;
    opcode[BasicInstruction::LLVMType::PTR]=RISCV_LD;
    //【0.2】取result寄存器
    Register result_reg;
    int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
    if(ins->GetDataType()==BasicInstruction::LLVMType::FLOAT32){
        result_reg=GetllvmReg(result_regno,FLOAT64);
    }else{
        result_reg=GetllvmReg(result_regno,INT64);
    }
    //【1】load局部变量（reg，栈上）
    //lw/flw/ld  rd, offset(sp)   /  load rd,0(ptr)
    if(ins->GetPointer()->GetOperandType()==BasicOperand::operand_type::REG){
        int pointer_regno=((RegOperand*)ins->GetPointer())->GetRegNo();
        if(llvm_rv_allocas.find(pointer_regno)!=llvm_rv_allocas.end()){
            int offset=llvm_rv_allocas[pointer_regno];
            auto instr=rvconstructor->ConstructIImm(opcode[ins->GetDataType()],result_reg,GetPhysicalReg(RISCV_sp),offset);
            cur_block->push_back(instr);
        }else{
            auto instr=rvconstructor->ConstructIImm(opcode[ins->GetDataType()],result_reg,GetllvmReg(pointer_regno,INT64),0);
            cur_block->push_back(instr);
        }
    //【2】load全局变量（global,静态存储区）
    // lui rd1, %hi(ptr)     load rd, %lo(ptr)(rd1)
    }else if(ins->GetPointer()->GetOperandType()==BasicOperand::operand_type::GLOBAL){
        auto ptr_name=((GlobalOperand*)ins->GetPointer())->GetName();
        auto ptr_hi=RiscVLabel(ptr_name,true);
        auto ptr_lo=RiscVLabel(ptr_name,false);

        Register simi_result_reg=GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
        cur_block->push_back(rvconstructor->ConstructILabel(opcode[ins->GetDataType()],result_reg,simi_result_reg,ptr_lo));
    }
}

template <> void RiscV64Selector::ConvertAndAppend<StoreInstruction *>(StoreInstruction *ins) {
    //【0】
    std::map<BasicInstruction::LLVMType,riscvOpcode>opcode;
    opcode[BasicInstruction::LLVMType::I32]=RISCV_SW;
    opcode[BasicInstruction::LLVMType::FLOAT32]=RISCV_FSW;

    //【1】处理value，--->reg
    Register value;
    switch(ins->GetValue()->GetOperandType()){
        case BasicOperand::IMMI32:{
            auto immint=((ImmI32Operand*)ins->GetValue())->GetIntImmVal();
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,value,immint));
            break;
        }
        case BasicOperand::IMMF32:{
            auto immfloat=((ImmF32Operand*)ins->GetValue())->GetFloatVal();
            value=FImmToReg(immfloat);
            break;
        }
        case BasicOperand::REG:{
            int reg_no=((RegOperand*)ins->GetValue())->GetRegNo();
            if(ins->GetDataType()==BasicInstruction::LLVMType::I32){
                value=GetllvmReg(reg_no,INT64);
            }else if(ins->GetDataType()==BasicInstruction::LLVMType::FLOAT32){
                value=GetllvmReg(reg_no,FLOAT64);
            }
        }
        default:
            break;
    }
    //【2】处理pointer：reg/global
    switch(ins->GetPointer()->GetOperandType()){
        //sw value, offset(sp)  /sw value, 0(ptr)
        case BasicOperand::REG:{
            int reg_no=((RegOperand*)ins->GetPointer())->GetRegNo();
            if(llvm_rv_allocas.find(reg_no)!=llvm_rv_allocas.end()){
                int offset=llvm_rv_allocas[reg_no];
                cur_block->push_back(rvconstructor->ConstructSImm(opcode[ins->GetDataType()],value,GetPhysicalReg(RISCV_sp),offset));
            }else{
                cur_block->push_back(rvconstructor->ConstructSImm(opcode[ins->GetDataType()],value,GetllvmReg(reg_no,INT64),0));
            }
            break;
        }
        //lui ptr, %hi(pointer)  sw value, %lo(pointer)(ptr)
        case BasicOperand::GLOBAL:{
            auto ptr_name=((GlobalOperand*)ins->GetPointer())->GetName();
            auto ptr_hi=RiscVLabel(ptr_name,true);
            auto ptr_lo=RiscVLabel(ptr_name,false);

            Register simi_result_reg=GetNewReg(INT64);
            cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
            cur_block->push_back(rvconstructor->ConstructSLabel(opcode[ins->GetDataType()],value,simi_result_reg,ptr_lo));
            break;
        }
        default:
            break;
    }
}

bool overflow12(int imm){
    return (imm<-2048 || imm>2047);
}
template <> void RiscV64Selector::ConvertAndAppend<ArithmeticInstruction *>(ArithmeticInstruction *ins) {
    Operand op1=ins->GetOperand1();
    Operand op2=ins->GetOperand2();
    Operand result=ins->GetResult();
    
    switch(ins->GetOpcode()){
    //【1】ADD
    case (BasicInstruction::LLVMIROpcode::ADD):{
        switch(op1->GetOperandType()){
            case BasicOperand::IMMI32:{
                switch(op2->GetOperandType()){
                    //【1.1】imm+imm
                    case BasicOperand::IMMI32:{
                        //【1.1.1】获取结果寄存器
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        //【1.1.2】处理op1
                        int op1_imm=((ImmI32Operand*)op1)->GetIntImmVal();
                        auto op1_reg=GetNewReg(INT64);
                        auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op1_reg,op1_imm);
                        cur_block->push_back(instr1);
                        //【1.1.3】处理op2,生成最终指令
                        int op2_imm=((ImmI32Operand*)op2)->GetIntImmVal();
                        if(overflow12(op2_imm)){//op2超范围，则li rt,op2; addw rd,rs,rt
                            auto op2_reg=GetNewReg(INT64);
                            auto instr2=rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm);
                            cur_block->push_back(instr2);

                            auto instr3=rvconstructor->ConstructR(RISCV_ADDW,rd,op1_reg,op2_reg);
                            cur_block->push_back(instr3);
                        }else{//op2未超范围，则addiw rd,rs,op2_imm
                            auto instr4=rvconstructor->ConstructIImm(RISCV_ADDIW,rd,op1_reg,op2_imm);
                            cur_block->push_back(instr4);
                        }
                        break;
                    }
                    //【1.2】imm+reg
                    case BasicOperand::REG:{
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        int op1_imm=((ImmI32Operand*)op1)->GetIntImmVal();
                        Register op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),INT64);

                        if(overflow12(op1_imm)){
                            auto op1_reg=GetNewReg(INT64);
                            auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op1_reg,op1_imm);
                            cur_block->push_back(instr1);

                            auto instr2=rvconstructor->ConstructR(RISCV_ADDW,rd,op1_reg,op2_reg);
                            cur_block->push_back(instr2);
                        }else{
                            auto instr3=rvconstructor->ConstructIImm(RISCV_ADDIW,rd,op2_reg,op1_imm);
                            cur_block->push_back(instr3);
                        }
                    }
                    default:
                        break;
                }
                break;
            }
            case BasicOperand::REG:{
                switch(op2->GetOperandType()){
                    //【1.3】reg+imm
                    case BasicOperand::IMMI32:{
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        Register op1_reg=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        int op2_imm=((ImmI32Operand*)op1)->GetIntImmVal();
                        
                        if(overflow12(op2_imm)){
                            auto op2_reg=GetNewReg(INT64);
                            auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm);
                            cur_block->push_back(instr1);

                            auto instr2=rvconstructor->ConstructR(RISCV_ADDW,rd,op1_reg,op2_reg);
                            cur_block->push_back(instr2);
                        }else{
                            auto instr3=rvconstructor->ConstructIImm(RISCV_ADDIW,rd,op1_reg,op2_imm);
                            cur_block->push_back(instr3);
                        }
                        break;
                    }
                    //【1.4】reg+reg
                    case BasicOperand::REG:{
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        Register op1_reg=GetllvmReg(((RegOperand*)op1)->GetRegNo(),INT64);
                        Register op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),INT64);
                
                        auto instr=rvconstructor->ConstructR(RISCV_ADDW,rd,op1_reg,op2_reg);
                        cur_block->push_back(instr);
                    }
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
        break;
    }
    case (BasicInstruction::LLVMIROpcode::SUB):{
        switch(op1->GetOperandType()){
            case BasicOperand::IMMI32:{//第一个操作数一定转化为reg
                //【2.1.1】获取结果寄存器
                Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                //【2.1.2】处理op1：imm->reg
                int op1_imm=((ImmI32Operand*)op1)->GetIntImmVal();
                auto op1_reg=GetNewReg(INT64);
                auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op1_reg,op1_imm);
                cur_block->push_back(instr1);
                //【2.1.3】处理op2
                switch(op2->GetOperandType()){
                    case BasicOperand::IMMI32:{
                        int op2_imm=((ImmI32Operand*)op2)->GetIntImmVal();
                        if(overflow12(op2_imm)){//op2超范围，则li rt,op2; addw rd,rs,rt
                            auto op2_reg=GetNewReg(INT64);
                            auto instr2=rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm);
                            cur_block->push_back(instr2);

                            auto instr3=rvconstructor->ConstructR(RISCV_SUBW,rd,op1_reg,op2_reg);
                            cur_block->push_back(instr3);
                        }else{//op2未超范围，则addiw rd,rs,op2_imm
                            auto instr4=rvconstructor->ConstructIImm(RISCV_ADDIW,rd,op1_reg,-op2_imm);
                            cur_block->push_back(instr4);
                        }
                        break;
                    }

                    case BasicOperand::REG:{
                        Register op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),INT64);

                        auto instr=rvconstructor->ConstructR(RISCV_SUBW,rd,op1_reg,op2_reg);
                        cur_block->push_back(instr);
                    }
                    default:
                        break;
                }
                break;
            }
            case BasicOperand::REG:{
                switch(op2->GetOperandType()){
                    //【2.2】reg-imm
                    case BasicOperand::IMMI32:{
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        Register op1_reg=GetllvmReg(((RegOperand*)op1)->GetRegNo(),INT64);
                        int op2_imm=((ImmI32Operand*)op2)->GetIntImmVal();
                        
                        if(overflow12(op2_imm)){
                            auto op2_reg=GetNewReg(INT64);
                            auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm);
                            cur_block->push_back(instr1);

                            auto instr2=rvconstructor->ConstructR(RISCV_SUBW,rd,op1_reg,op2_reg);
                            cur_block->push_back(instr2);
                        }else{
                            auto instr3=rvconstructor->ConstructIImm(RISCV_ADDIW,rd,op1_reg,-op2_imm);
                            cur_block->push_back(instr3);
                        }
                        break;
                    }
                    //【2.3】reg-reg
                    case BasicOperand::REG:{
                        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
                        Register op1_reg=GetllvmReg(((RegOperand*)op1)->GetRegNo(),INT64);
                        Register op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),INT64);
                
                        auto instr=rvconstructor->ConstructR(RISCV_SUBW,rd,op1_reg,op2_reg);
                        cur_block->push_back(instr);
                    }
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
        break;
    }
    case (BasicInstruction::LLVMIROpcode::MUL):
    case (BasicInstruction::LLVMIROpcode::DIV):
    case (BasicInstruction::LLVMIROpcode::MOD):{
        //【3.1】结果寄存器
        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),INT64);
        //【3.2】op1-->reg
        Register op1_reg;
        if(op1->GetOperandType()==BasicOperand::IMMI32){
            int op1_imm=((ImmI32Operand*)op1)->GetIntImmVal();
            op1_reg=GetNewReg(INT64);
            auto instr1=rvconstructor->ConstructUImm(RISCV_LI,op1_reg,op1_imm);
            cur_block->push_back(instr1);
        }else if(op1->GetOperandType()==BasicOperand::REG){
            op1_reg=GetllvmReg(((RegOperand*)op1)->GetRegNo(),INT64);
        }
        //【3.3】op2-->reg
        Register op2_reg;
        if(op2->GetOperandType()==BasicOperand::IMMI32){
            int op2_imm=((ImmI32Operand*)op2)->GetIntImmVal();
            op2_reg=GetNewReg(INT64);
            auto instr2=rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm);
            cur_block->push_back(instr2);
        }else if(op2->GetOperandType()==BasicOperand::REG){
            op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),INT64);
        }
        //【3.4】生成指令
        if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::MUL){
            auto instr3=rvconstructor->ConstructR(RISCV_MULW,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::DIV){
            auto instr3=rvconstructor->ConstructR(RISCV_DIVW,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::MOD){
            auto instr3=rvconstructor->ConstructR(RISCV_REMW,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else{}
        break;
    }
    case (BasicInstruction::LLVMIROpcode::FADD):
    case (BasicInstruction::LLVMIROpcode::FSUB):
    case (BasicInstruction::LLVMIROpcode::FDIV):
    case (BasicInstruction::LLVMIROpcode::FMUL):{
        //【3.1】结果寄存器
        Register rd=GetllvmReg(((RegOperand*)result)->GetRegNo(),FLOAT64);
        //【3.2】op1-->reg
        Register op1_reg;
        if(op1->GetOperandType()==BasicOperand::IMMF32){
            int op1_imm=((ImmF32Operand*)op1)->GetFloatVal();
            op1_reg=FImmToReg(op1_imm);

        }else if(op1->GetOperandType()==BasicOperand::REG){
            op1_reg=GetllvmReg(((RegOperand*)op1)->GetRegNo(),FLOAT64);
        }
        //【3.3】op2-->reg
        Register op2_reg;
        if(op2->GetOperandType()==BasicOperand::IMMF32){
            int op2_imm=((ImmF32Operand*)op2)->GetFloatVal();
            op2_reg=FImmToReg(op2_imm);

        }else if(op2->GetOperandType()==BasicOperand::REG){
            op2_reg=GetllvmReg(((RegOperand*)op2)->GetRegNo(),FLOAT64);
        }
        //【3.4】生成指令
        if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FMUL){
            auto instr3=rvconstructor->ConstructR(RISCV_FMUL_S,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FDIV){
            auto instr3=rvconstructor->ConstructR(RISCV_FDIV_S,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FADD){
            auto instr3=rvconstructor->ConstructR(RISCV_FADD_S,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else if(ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FSUB){
            auto instr3=rvconstructor->ConstructR(RISCV_FSUB_S,rd,op1_reg,op2_reg);
            cur_block->push_back(instr3);
        }else{}
        break;
    }
    default:
        break;
    }
}

template <> void RiscV64Selector::ConvertAndAppend<IcmpInstruction *>(IcmpInstruction *ins) {
    int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
    br_context[result_regno]=ins;
}

template <> void RiscV64Selector::ConvertAndAppend<FcmpInstruction *>(FcmpInstruction *ins) {
    int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
    br_context[result_regno]=ins;
}

//计算分配的内存空间（字节数，栈空间）
//维护栈偏移
template <> void RiscV64Selector::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *ins) {
    int reg_no = ((RegOperand*)ins->GetResult())->GetRegNo();
    int size = 4;//一个i32/float32的字节数
    if(ins->GetDims().size()!=0){
        for(auto dim:ins->GetDims()){
            size *=dim;
        }
    }
    //记录当前起始地址对应的栈偏移
    llvm_rv_allocas[reg_no]=cur_offset;
    cur_offset+=size;
}

template <> void RiscV64Selector::ConvertAndAppend<BrCondInstruction *>(BrCondInstruction *ins) {
    Register br_op1,br_op2;
    riscvOpcode br_opcode;
    //【1】处理cmp_inst,取操作数
    int result_regno=((RegOperand*)ins->GetCond())->GetRegNo();
    Instruction ori_ins = br_context[result_regno];
    if(ori_ins->GetOpcode()==BasicInstruction::LLVMIROpcode::ICMP){
        auto ins=(IcmpInstruction*)ori_ins;
        Register op1,op2;
        //【1】操作数1
        if(ins->GetOp1()->GetOperandType()==BasicOperand::REG){
            int op1_regno=((RegOperand*)ins->GetOp1())->GetRegNo();
            op1 = GetllvmReg(op1_regno,INT64);
        }else if(ins->GetOp1()->GetOperandType()==BasicOperand::IMMI32){
            int imm=((ImmI32Operand*)ins->GetOp1())->GetIntImmVal();
            op1 = GetNewReg(INT64);
            auto immtoreg_instr = rvconstructor->ConstructUImm(RISCV_LI, op1, imm);
            cur_block->push_back(immtoreg_instr);
        }
        //【2】操作数2
        if(ins->GetOp2()->GetOperandType()==BasicOperand::REG){
            int op2_regno=((RegOperand*)ins->GetOp2())->GetRegNo();
            op2 = GetllvmReg(op2_regno,INT64);
        }else if(ins->GetOp2()->GetOperandType()==BasicOperand::IMMI32){
            int imm=((ImmI32Operand*)ins->GetOp2())->GetIntImmVal();
            op2 = GetNewReg(INT64);
            auto immtoreg_instr = rvconstructor->ConstructUImm(RISCV_LI, op2, imm);
            cur_block->push_back(immtoreg_instr);
        }
        //【3】opcode
        br_opcode = IcmpCond_to_opcode[ins->GetCond()];
        br_op1=op1; br_op2=op2;
    }else if(ori_ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FCMP){
        auto ins=(FcmpInstruction*)ori_ins;
        Register op1,op2;
        //【1】操作数1
        if(ins->GetOp1()->GetOperandType()==BasicOperand::REG){
            int op1_regno=((RegOperand*)ins->GetOp1())->GetRegNo();
            op1 = GetllvmReg(op1_regno,FLOAT64);
        }else if(ins->GetOp1()->GetOperandType()==BasicOperand::IMMF32){
            float imm=((ImmF32Operand*)ins->GetOp1())->GetFloatVal();
            op1=FImmToReg(imm);
        }
        //【2】操作数2
        if(ins->GetOp2()->GetOperandType()==BasicOperand::REG){
            int op2_regno=((RegOperand*)ins->GetOp2())->GetRegNo();
            op2 = GetllvmReg(op2_regno,FLOAT64);
        }else if(ins->GetOp2()->GetOperandType()==BasicOperand::IMMF32){
            float imm=((ImmF32Operand*)ins->GetOp2())->GetFloatVal();
            op2=FImmToReg(imm);
        }
        //【3.1】opcode
        auto cond=ins->GetCond();
        auto opcode = FcmpCond_to_opcode[cond];
        //【3.2】将浮点比较转化为整数比较
        Register cmp_result = GetNewReg(INT64);
        auto instcode = FcmpCond_to_instcode[cond];
        if(cond==BasicInstruction::FcmpCond::OGT||cond==BasicInstruction::FcmpCond::UGT||
            cond== BasicInstruction::FcmpCond::OGE||cond== BasicInstruction::FcmpCond::UGE){
            auto cmp_instr=rvconstructor->ConstructR(instcode, cmp_result, op2, op1);
            cur_block->push_back(cmp_instr);
        }else{
            auto cmp_instr=rvconstructor->ConstructR(instcode, cmp_result, op1, op2);
            cur_block->push_back(cmp_instr);
        }
        
        br_opcode=opcode;
        br_op1=cmp_result; br_op2=GetPhysicalReg(RISCV_x0);
    }

    //【2】生成指令
    RiscVLabel label= RiscVLabel(((LabelOperand *)ins->GetTrueLabel())->GetLabelNo());
    auto true_instr=rvconstructor->ConstructBLabel(br_opcode,br_op1,br_op2,label);
    auto false_instr=rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0),
                       RiscVLabel(((LabelOperand*)ins->GetFalseLabel())->GetLabelNo()));
    cur_block->push_back(true_instr);
    cur_block->push_back(false_instr);
}
//根据IR时的标签号，创建riscV标签，生成jar指令
//jal x0, label :无条件跳转不需要保存返回地址
template <> void RiscV64Selector::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *ins) {
    int label_no = ((LabelOperand*)ins->GetDestLabel())->GetLabelNo();
    RiscVLabel label = RiscVLabel(label_no);

    auto instr=rvconstructor->ConstructJLabel(RISCV_JAL,GetPhysicalReg(RISCV_x0),label);
    cur_block->push_back(instr);
}


template <> void RiscV64Selector::ConvertAndAppend<CallInstruction *>(CallInstruction *ins) {
    //【1】函数参数--->reg(a0~a7)/stack   (全局变量存的不是值，而是标签)
    int iregnum=0,fregnum=0;
    int stack_offset=0;
    for(int i=0;i<ins->GetParameterList().size();i++){
        auto para_pair=ins->GetParameterList()[i];//[type,Operand]
        Operand param=para_pair.second;
        switch(para_pair.first){
            case BasicInstruction::LLVMType::I32:
            case BasicInstruction::LLVMType::PTR:{
                //【1.1】reg未满，存入reg
                if(iregnum<8){
                    switch(param->GetOperandType()){
                        case BasicOperand::REG:{
                            Register src_reg=GetllvmReg(((RegOperand*)param)->GetRegNo(),INT64);
                            cur_block->push_back(rvconstructor->ConstructR(RISCV_ADDW,GetPhysicalReg(RISCV_a0+iregnum),src_reg,GetPhysicalReg(RISCV_x0)));
                            break;
                        }
                        case BasicOperand::IMMI32:{
                            int src_imm=((ImmI32Operand*)param)->GetIntImmVal();
                            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,GetPhysicalReg(RISCV_a0+iregnum),src_imm));
                            break;
                        }
                        case BasicOperand::GLOBAL:{
                            auto ptr_name=((GlobalOperand*)param)->GetName();
                            auto ptr_hi=RiscVLabel(ptr_name,true);
                            auto ptr_lo=RiscVLabel(ptr_name,false);

                            Register simi_result_reg=GetNewReg(INT64);
                            cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
                            cur_block->push_back(rvconstructor->ConstructILabel(RISCV_ADDIW,GetPhysicalReg(RISCV_a0+iregnum),simi_result_reg,ptr_lo));
                            // if(para_pair.first==BasicInstruction::LLVMType::I32){
                            //     cur_block->push_back(rvconstructor->ConstructILabel(RISCV_LW,GetPhysicalReg(RISCV_a0+iregnum),simi_result_reg,ptr_lo));
                            // }else{
                            //     cur_block->push_back(rvconstructor->ConstructILabel(RISCV_LD,GetPhysicalReg(RISCV_a0+iregnum),simi_result_reg,ptr_lo));
                            // }
                            break;
                        }
                        default:
                            break;
                    }
                }
                //【1.2】reg已满，存入stack
                else{
                    switch(param->GetOperandType()){
                        case BasicOperand::REG:{
                            Register src_reg=GetllvmReg(((RegOperand*)param)->GetRegNo(),INT64);
                            cur_block->push_back(rvconstructor->ConstructSImm(RISCV_SD,src_reg,GetPhysicalReg(RISCV_sp),stack_offset));
                            break;
                        }
                        case BasicOperand::IMMI32:{
                            int src_imm=((ImmI32Operand*)param)->GetIntImmVal();
                            Register src_reg=GetNewReg(INT64);
                            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,src_reg,src_imm));
                            cur_block->push_back(rvconstructor->ConstructSImm(RISCV_SD,src_reg,GetPhysicalReg(RISCV_sp),stack_offset));
                            break;
                        }
                        case BasicOperand::GLOBAL:{
                            auto ptr_name=((GlobalOperand*)param)->GetName();
                            auto ptr_hi=RiscVLabel(ptr_name,true);
                            auto ptr_lo=RiscVLabel(ptr_name,false);

                            Register simi_result_reg=GetNewReg(INT64);
                            Register label_reg=GetNewReg(INT64);
                            cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
                            cur_block->push_back(rvconstructor->ConstructILabel(RISCV_ADDIW,label_reg,simi_result_reg,ptr_lo));
                            cur_block->push_back(rvconstructor->ConstructSImm(RISCV_SD,label_reg,GetPhysicalReg(RISCV_sp),stack_offset));
                            break;
                        }
                        default:
                            break;
                    }
                    stack_offset+=8;
                }
                iregnum++;
                break;
            }
            case BasicInstruction::LLVMType::FLOAT32:{
                //【1.1】reg未满，存入reg
                if(iregnum<8){
                    switch(param->GetOperandType()){
                        case BasicOperand::REG:{
                            Register src_reg=GetllvmReg(((RegOperand*)param)->GetRegNo(),FLOAT64);
                            auto fzero=FImmToReg(0.0);
                            cur_block->push_back(rvconstructor->ConstructR(RISCV_FADD_S,GetPhysicalReg(RISCV_fa0+fregnum),src_reg,fzero));
                            break;
                        }
                        case BasicOperand::IMMF32:{
                            float src_imm=((ImmF32Operand*)param)->GetFloatVal();
                            //cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,GetPhysicalReg(RISCV_fa0+fregnum),src_imm));
                            FImmToRReg(GetPhysicalReg(RISCV_fa0+fregnum),src_imm);
                            break;
                        }
                        // case BasicOperand::GLOBAL:{
                        //     auto ptr_name=((GlobalOperand*)param)->GetName();
                        //     auto ptr_hi=RiscVLabel(ptr_name,true);
                        //     auto ptr_lo=RiscVLabel(ptr_name,false);

                        //     Register simi_result_reg=GetNewReg(INT64);
                        //     cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
                        //     cur_block->push_back(rvconstructor->ConstructILabel(RISCV_FLW,GetPhysicalReg(RISCV_fa0+fregnum),simi_result_reg,ptr_lo));
                        //     break;
                        // }
                        default:
                            break;
                    }
                }
                //【1.2】reg已满，存入stack
                else{
                    switch(param->GetOperandType()){
                        case BasicOperand::REG:{
                            Register src_reg=GetllvmReg(((RegOperand*)param)->GetRegNo(),FLOAT64);
                            cur_block->push_back(rvconstructor->ConstructSImm(RISCV_FSD,src_reg,GetPhysicalReg(RISCV_sp),stack_offset));
                            break;
                        }
                        case BasicOperand::IMMF32:{
                            float src_imm=((ImmF32Operand*)param)->GetFloatVal();
                            auto src_reg=FImmToReg(src_imm);
                            cur_block->push_back(rvconstructor->ConstructSImm(RISCV_FSD,src_reg,GetPhysicalReg(RISCV_sp),stack_offset));
                            break;
                        }
                        default:
                            break;
                    }
                }
                fregnum++;
            }
            default:
                break;
        }
    }

    //【2】生成call指令
    if(iregnum>8){iregnum=8;}
    if(fregnum>8){fregnum=8;}
    auto function_name=ins->GetFunctionName();
    cur_block->push_back(rvconstructor->ConstructCall(RISCV_CALL,function_name, iregnum, fregnum));
    cur_func->UpdateParaSize(stack_offset);

    //【3】返回值处理:a0/fa0--->result_reg
    if(ins->GetReturnType()==BasicInstruction::LLVMType::I32){
        int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
        auto result_reg=GetllvmReg(result_regno,INT64);
        cur_block->push_back(rvconstructor->ConstructR(RISCV_ADDW,result_reg,GetPhysicalReg(RISCV_a0),GetPhysicalReg(RISCV_x0)));
    }else if(ins->GetReturnType()==BasicInstruction::LLVMType::FLOAT32){
        int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
        auto result_reg=GetllvmReg(result_regno,FLOAT64);
        auto fzero=FImmToReg(0.0);
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FADD_S,result_reg,GetPhysicalReg(RISCV_fa0),fzero));
    }
}


//ret:将函数返回值存入a0/fa0寄存器 ,跳转指令生成
template <> void RiscV64Selector::ConvertAndAppend<RetInstruction *>(RetInstruction *ins) {
    if (ins->GetRetVal() != NULL) {
        if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMI32) {
            auto retimm_op = (ImmI32Operand *)ins->GetRetVal();
            auto imm = retimm_op->GetIntImmVal();

            auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a0), imm);
            cur_block->push_back(retcopy_instr);
        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMF32) {
            // auto retimm_op = (ImmF32Operand*)ins->GetRetVal();
            // auto imm = retimm_op->GetFloatVal();

            // auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI,GetPhysicalReg(RISCV_fa0),imm);
            // cur_block->push_back(retcopy_instr);
            int imm=((ImmF32Operand*)ins->GetRetVal())->GetFloatVal();
            FImmToRReg(GetPhysicalReg(RISCV_fa0),imm);

        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::REG) {
            auto retreg_op = (RegOperand*)ins->GetRetVal();
            auto reg_no = retreg_op->GetRegNo();
            //区分int和float
            if(ins->GetType()==BasicInstruction::LLVMType::I32){
                auto reg = GetllvmReg(reg_no,INT64);
                auto retcopy_instr = rvconstructor->ConstructR(RISCV_ADD,GetPhysicalReg(RISCV_a0),GetPhysicalReg(RISCV_x0),reg);//通过加0移动寄存器
                cur_block->push_back(retcopy_instr);
            }else if(ins->GetType()==BasicInstruction::LLVMType::I32){
                auto reg = GetllvmReg(reg_no,FLOAT64);
                auto reg_f0 = GetNewReg(FLOAT64);
                auto retchange_instr = rvconstructor->ConstructR2(RISCV_FMV_W_X,reg_f0,GetPhysicalReg(RISCV_x0));//FMV.W.X ft0, x0 先将x0的0转化为0.0
                auto retcopy_instr = rvconstructor->ConstructR(RISCV_FADD_S,GetPhysicalReg(RISCV_fa0),reg_f0,reg);//FADD_S a0,ft0,reg
                cur_block->push_back(retchange_instr);
                cur_block->push_back(retcopy_instr);
            }
        }
    }
    //JALR x0, ra, 0 : 跳转到ra保存的函数返回地址处
    auto ret_instr = rvconstructor->ConstructIImm(RISCV_JALR, GetPhysicalReg(RISCV_x0), GetPhysicalReg(RISCV_ra), 0);
    if (ins->GetType() == BasicInstruction::I32) {
        ret_instr->setRetType(1);
    } else if (ins->GetType() == BasicInstruction::FLOAT32) {
        ret_instr->setRetType(2);
    } else {
        ret_instr->setRetType(0);
    }
    cur_block->push_back(ret_instr);
}

template <> void RiscV64Selector::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *ins) {
    //【1】获取源操作数reg
    Register src;
    if(ins->GetSrc()->GetOperandType()==BasicOperand::REG){
        src=GetllvmReg(((RegOperand*)ins->GetSrc())->GetRegNo(),FLOAT64);
    }else if(ins->GetSrc()->GetOperandType()==BasicOperand::IMMF32){//先imm->reg 
        // src=GetNewReg(FLOAT64);
        // float src_imm=((ImmF32Operand*)ins->GetSrc())->GetFloatVal();
        // cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,src,src_imm));
        int src_imm=((ImmF32Operand*)ins->GetSrc())->GetFloatVal();
        auto src=FImmToReg(src_imm);
    }
    //[2]生成指令
    Register dst=GetllvmReg(((RegOperand*)ins->GetResult())->GetRegNo(),INT64);
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FCVT_W_S,dst,src));
}

template <> void RiscV64Selector::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *ins) {
    //【1】获取源操作数reg
    Register src;
    if(ins->GetSrc()->GetOperandType()==BasicOperand::REG){
        src=GetllvmReg(((RegOperand*)ins->GetSrc())->GetRegNo(),INT64);
    }else if(ins->GetSrc()->GetOperandType()==BasicOperand::IMMI32){//先imm->reg 
        src=GetNewReg(INT64);
        int src_imm=((ImmI32Operand*)ins->GetSrc())->GetIntImmVal();
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,src,src_imm));
    }
    //[2]生成指令
    Register dst=GetllvmReg(((RegOperand*)ins->GetResult())->GetRegNo(),FLOAT64);
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FCVT_S_W,dst,src));
}



template <> void RiscV64Selector::ConvertAndAppend<ZextInstruction *>(ZextInstruction *ins) {
    int value_regno=((RegOperand*)ins->GetSrc())->GetRegNo();
    int result_regno=((RegOperand*)ins->GetResult())->GetRegNo();
    auto value_reg=GetllvmReg(value_regno,INT64);
    auto result_reg=GetllvmReg(result_regno,INT64);

    auto cmp_ins=br_context[value_regno];
    //【1】若为icmp
    if(cmp_ins->GetOpcode()==BasicInstruction::LLVMIROpcode::ICMP){
        auto icmp_ins = (IcmpInstruction *)cmp_ins;
        //【1.1】处理操作数，-->reg
        //op1
        Register op1_reg,op2_reg;
        if(icmp_ins->GetOp1()->GetOperandType()==BasicOperand::IMMI32){
            op1_reg=GetNewReg(INT64);
            int op1_imm=((ImmI32Operand*)icmp_ins->GetOp1())->GetIntImmVal();
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,op1_reg,op1_imm));
        }else if(icmp_ins->GetOp1()->GetOperandType()==BasicOperand::REG){
            int op1_regno=((RegOperand*)icmp_ins->GetOp1())->GetRegNo();
            op1_reg=GetllvmReg(op1_regno,INT64);
        }
        //op2
        if(icmp_ins->GetOp2()->GetOperandType()==BasicOperand::IMMI32){
            op2_reg=GetNewReg(INT64);
            int op2_imm=((ImmI32Operand*)icmp_ins->GetOp2())->GetIntImmVal();
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,op2_reg,op2_imm));
        }else if(icmp_ins->GetOp1()->GetOperandType()==BasicOperand::REG){
            int op2_regno=((RegOperand*)icmp_ins->GetOp2())->GetRegNo();
            op2_reg=GetllvmReg(op2_regno,INT64);
        }
        //【1.2】执行比较
        Register mid_reg=GetNewReg(INT64);
        auto cond=icmp_ins->GetCond();
        switch(cond){
            case BasicInstruction::IcmpCond::eq:// op1-op2 =0
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SUBW, mid_reg, op1_reg, op2_reg));
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLTIU, result_reg, mid_reg, 1));
                break;
            case BasicInstruction::IcmpCond::ne://op1-op2!=0
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SUBW, mid_reg, op1_reg, op2_reg));
            cur_block->push_back(rvconstructor->ConstructR(RISCV_SLTU, result_reg, GetPhysicalReg(RISCV_x0), mid_reg));
                break;
            case BasicInstruction::IcmpCond::sgt://op2<op1
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, result_reg, op2_reg, op1_reg));
                break;
            case BasicInstruction::IcmpCond::sge:// !(op1<op2)    
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, mid_reg, op1_reg, op2_reg));
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, mid_reg, 1));
                break;
            case BasicInstruction::IcmpCond::slt://op1<op2
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, result_reg, op1_reg, op2_reg));
                break;
            case BasicInstruction::IcmpCond::sle://!(op2<op1)    
                cur_block->push_back(rvconstructor->ConstructR(RISCV_SLT, mid_reg, op2_reg, op1_reg));
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_XORI, result_reg, mid_reg, 1));
                break;
            default:
                break;
        }
    }
    //【2】若为fcmp
    else if(cmp_ins->GetOpcode()==BasicInstruction::LLVMIROpcode::FCMP){
        auto fcmp_ins = (FcmpInstruction *)cmp_ins;
        //【2.1】处理操作数，-->reg
        Register op1_reg,op2_reg;
        if(fcmp_ins->GetOp1()->GetOperandType()==BasicOperand::IMMF32){
            int op1_imm=((ImmF32Operand*)fcmp_ins->GetOp1())->GetFloatVal();
            op1_reg=FImmToReg(op1_imm);
        }else if(fcmp_ins->GetOp1()->GetOperandType()==BasicOperand::REG){
            int op1_regno=((RegOperand*)fcmp_ins->GetOp1())->GetRegNo();
            op1_reg=GetllvmReg(op1_regno,FLOAT64);
        }

        if(fcmp_ins->GetOp2()->GetOperandType()==BasicOperand::IMMF32){
            int op2_imm=((ImmF32Operand*)fcmp_ins->GetOp2())->GetFloatVal();
            op2_reg=FImmToReg(op2_imm);
        }else if(fcmp_ins->GetOp2()->GetOperandType()==BasicOperand::REG){
            int op2_regno=((RegOperand*)fcmp_ins->GetOp2())->GetRegNo();
            op2_reg=GetllvmReg(op2_regno,FLOAT64);
        }
        //【2.2】执行比较
        auto cond=fcmp_ins->GetCond();
        auto instcode = FcmpCond_to_instcode[cond];
        if(cond==BasicInstruction::FcmpCond::OGT||cond==BasicInstruction::FcmpCond::UGT||
            cond== BasicInstruction::FcmpCond::OGE||cond== BasicInstruction::FcmpCond::UGE){
            auto cmp_instr=rvconstructor->ConstructR(instcode, result_reg, op2_reg, op1_reg);
            cur_block->push_back(cmp_instr);
        }else{
            auto cmp_instr=rvconstructor->ConstructR(instcode, result_reg, op1_reg, op2_reg);
            cur_block->push_back(cmp_instr);
        }
    }
}

template <> void RiscV64Selector::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *ins) {
    //【1】获取ptr中的基址
    Register ptr_reg=GetNewReg(INT64);
    //【1.1】load局部变量（reg，栈上）
    if(ins->GetPtrVal()->GetOperandType()==BasicOperand::operand_type::REG){
        int pointer_regno=((RegOperand*)ins->GetPtrVal())->GetRegNo();
        if(llvm_rv_allocas.find(pointer_regno)!=llvm_rv_allocas.end()){
            int offset=llvm_rv_allocas[pointer_regno];
            auto instr=rvconstructor->ConstructIImm(RISCV_LD,ptr_reg,GetPhysicalReg(RISCV_sp),offset);
            cur_block->push_back(instr);
        }else{
            auto instr=rvconstructor->ConstructIImm(RISCV_LD,ptr_reg,GetllvmReg(pointer_regno,INT64),0);
            cur_block->push_back(instr);
        }
    //【1.2】load全局变量（global,静态存储区）
    }else if(ins->GetPtrVal()->GetOperandType()==BasicOperand::operand_type::GLOBAL){
        auto ptr_name=((GlobalOperand*)ins->GetPtrVal())->GetName();
        auto ptr_hi=RiscVLabel(ptr_name,true);
        auto ptr_lo=RiscVLabel(ptr_name,false);

        Register simi_result_reg=GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI,simi_result_reg,ptr_hi));
        cur_block->push_back(rvconstructor->ConstructILabel(RISCV_LD,ptr_reg,simi_result_reg,ptr_lo));
    }
    //【2】根据indexs计算索引
    /*
        ref:https://github.com/yuhuifishash/SysY
        借鉴思想：将立即数和寄存器index区分开，立即数在编译器求值
    */
   //【2.1】计算每层索引的厚度
   int total_size=1;
   auto dims=ins->GetDims();
   for(auto dim:dims){
        total_size*=dim;
   }
   std::vector<int>DimWidth{};
   //array非参数
   //if(array_arg.find(ptr_reg.reg_no)==array_arg.end()){
    DimWidth.resize(ins->GetIndexes().size());
        for(int i=0;i<DimWidth.size();i++){
            DimWidth[i]=total_size;
            if(i!=DimWidth.size()-1){
                total_size/=dims[i];
            }
        }
   //}
//    else{//是参数
//     DimWidth.resize(ins->GetIndexes().size());
//         for(int i=0;i<DimWidth.size();i++){
//                 DimWidth[i]=total_size;
//                 total_size/=dims[i];
//         }
//     //DimWidth.push_back(1);
//    }
   
   //【2.2】遍历indexes列表，计算索引偏移
   //【2.2.1】index是立即数，直接累计offset（编译器求值）
   int cur_offset=0;
    for(int i=0;i<ins->GetIndexes().size();i++){
        Operand index=ins->GetIndexes()[i];
        if(index->GetOperandType()==BasicOperand::IMMI32){
            int index_imm=((ImmI32Operand*)index)->GetIntImmVal();
            cur_offset+=index_imm*DimWidth[i];
        }
    }
    Register index_sum_reg=GetNewReg(INT64);
    cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,index_sum_reg,cur_offset));
    //【2.2.2】index是reg，需要显式生成计算指令
    for(int i=0;i<ins->GetIndexes().size();i++){
        Operand index=ins->GetIndexes()[i];
        if(index->GetOperandType()==BasicOperand::REG){
            //(1)将width存入reg；（2)result=index*width ；（3）cur_offset+=result
            Register width_reg=GetNewReg(INT64);
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI,width_reg,DimWidth[i]));

            Register this_offset=GetNewReg(INT64);
            Register index_reg=GetllvmReg(((RegOperand*)index)->GetRegNo(),INT64);
            cur_block->push_back(rvconstructor->ConstructR(RISCV_MUL,this_offset,index_reg,width_reg));

            cur_block->push_back(rvconstructor->ConstructR(RISCV_ADDW,index_sum_reg,index_sum_reg,this_offset));
        }
    }
    //【3】根据索引计算目标地址
    Register result_reg=GetllvmReg(((RegOperand*)ins->GetResult())->GetRegNo(),INT64);
    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLLI,index_sum_reg,index_sum_reg,2));//32位=4字节，*4=>左移2位
    cur_block->push_back(rvconstructor->ConstructR(RISCV_ADDW,result_reg,ptr_reg,index_sum_reg));//result=ptr+index_offset
}

template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
    //【1】处理result
    Register res;
    int reg_no=((RegOperand*)ins->GetResult())->GetRegNo();
    if(ins->GetDataType()==BasicInstruction::LLVMType::FLOAT32){
        res=GetllvmReg(reg_no,FLOAT64);
    }else if(ins->GetDataType()==BasicInstruction::LLVMType::I32||
             ins->GetDataType()==BasicInstruction::LLVMType::PTR){
        res=GetllvmReg(reg_no,INT64);
    }
    //【2】处理[val,label]
    auto phi_instr = new MachinePhiInstruction(res);
    for(auto pair:ins->GetPhiList()){
        int label=((LabelOperand*)pair.second)->GetLabelNo();
        Operand val=pair.first;
        switch(val->GetOperandType()){
            case BasicOperand::IMMI32:{
                int immint=((ImmI32Operand*)val)->GetIntImmVal();
                phi_instr->pushPhiList(label,immint);
                break;
            }
            case BasicOperand::IMMF32:{
                int immfloat=((ImmF32Operand*)val)->GetFloatVal();
                phi_instr->pushPhiList(label,immfloat);
                break;
            }
            case BasicOperand::REG:{
                int reg_no=((RegOperand*)val)->GetRegNo();
                Register val_reg;
                if(ins->GetDataType()==BasicInstruction::LLVMType::FLOAT32){
                    val_reg=GetllvmReg(reg_no,FLOAT64);
                }else if(ins->GetDataType()==BasicInstruction::LLVMType::I32||
                         ins->GetDataType()==BasicInstruction::LLVMType::PTR){
                    val_reg=GetllvmReg(reg_no,INT64);
                }
                phi_instr->pushPhiList(label,val_reg);
            }
            default:
                break;
        }
    }
    cur_block->push_back(phi_instr);
}

template <> void RiscV64Selector::ConvertAndAppend<Instruction>(Instruction inst) {
    switch (inst->GetOpcode()) {
    case BasicInstruction::LOAD:
        ConvertAndAppend<LoadInstruction *>((LoadInstruction *)inst);
        break;
    case BasicInstruction::STORE:
        ConvertAndAppend<StoreInstruction *>((StoreInstruction *)inst);
        break;
    case BasicInstruction::ADD:
    case BasicInstruction::SUB:
    case BasicInstruction::MUL:
    case BasicInstruction::DIV:
    case BasicInstruction::FADD:
    case BasicInstruction::FSUB:
    case BasicInstruction::FMUL:
    case BasicInstruction::FDIV:
    case BasicInstruction::MOD:
    case BasicInstruction::SHL:
    case BasicInstruction::BITXOR:
        ConvertAndAppend<ArithmeticInstruction *>((ArithmeticInstruction *)inst);
        break;
    case BasicInstruction::ICMP:
        ConvertAndAppend<IcmpInstruction *>((IcmpInstruction *)inst);
        break;
    case BasicInstruction::FCMP:
        ConvertAndAppend<FcmpInstruction *>((FcmpInstruction *)inst);
        break;
    case BasicInstruction::ALLOCA:
        ConvertAndAppend<AllocaInstruction *>((AllocaInstruction *)inst);
        break;
    case BasicInstruction::BR_COND:
        ConvertAndAppend<BrCondInstruction *>((BrCondInstruction *)inst);
        break;
    case BasicInstruction::BR_UNCOND:
        ConvertAndAppend<BrUncondInstruction *>((BrUncondInstruction *)inst);
        break;
    case BasicInstruction::RET:
        ConvertAndAppend<RetInstruction *>((RetInstruction *)inst);
        break;
    case BasicInstruction::ZEXT:
        ConvertAndAppend<ZextInstruction *>((ZextInstruction *)inst);
        break;
    case BasicInstruction::FPTOSI:
        ConvertAndAppend<FptosiInstruction *>((FptosiInstruction *)inst);
        break;
    case BasicInstruction::SITOFP:
        ConvertAndAppend<SitofpInstruction *>((SitofpInstruction *)inst);
        break;
    case BasicInstruction::GETELEMENTPTR:
        ConvertAndAppend<GetElementptrInstruction *>((GetElementptrInstruction *)inst);
        break;
    case BasicInstruction::CALL:
        ConvertAndAppend<CallInstruction *>((CallInstruction *)inst);
        break;
    case BasicInstruction::PHI:
        ConvertAndAppend<PhiInstruction *>((PhiInstruction *)inst);
        break;
    default:
        ERROR("Unknown LLVM IR instruction");
    }
}


/*
（1）遍历ir的CFG，收集信息：（全局变量定义，函数）-->（MachineUnit)dest
    对于每个ir的函数，遍历其所有block，构造一个等价的MachineCFG;
    ** 将IR指令转换为RISCV指令；维护函数栈空间大小 **
*/
void RiscV64Selector::SelectInstructionAndBuildCFG() {
    // 与中间代码生成一样, 如果你完全无从下手, 可以先看看输出是怎么写的
    // 即riscv64gc/instruction_print/*  common/machine_passes/machine_printer.h

    // 指令选择除了一些函数调用约定必须遵守的情况需要物理寄存器，其余情况必须均为虚拟寄存器
    dest->global_def = IR->global_def;
    // 遍历每个LLVM IR函数
    for (auto func_pair : IR->llvm_cfg) {
        auto defI=func_pair.first;
        auto cfg=func_pair.second;
        if(cfg == nullptr){
            ERROR("LLVMIR CFG is Empty, you should implement BuildCFG in MidEnd first");
        }
        std::string name = cfg->function_def->GetFunctionName();

        cur_func = new RiscV64Function(name);
        cur_func->SetParent(dest);

        dest->functions.push_back(cur_func);

        auto cur_mcfg = new MachineCFG;
        cur_func->SetMachineCFG(cur_mcfg);

        // 清空指令选择状态(可能需要自行添加初始化操作)
        ClearFunctionSelectState();

        //添加函数参数（形参寄存器）
        for(int i=0;i<defI->GetFormalTypes().size();i++){
            switch (defI->GetFormalTypes()[i]){
                case BasicInstruction::LLVMType::I32:{
                    int regno= ((RegOperand*)defI->formals_reg[i])->GetRegNo();
                    Register para=GetllvmReg(regno,INT64);
                    cur_func->AddParameter(para);
                    break;
                }
                case BasicInstruction::LLVMType::PTR:{
                    int regno= ((RegOperand*)defI->formals_reg[i])->GetRegNo();
                    Register para=GetllvmReg(regno,INT64);
                    cur_func->AddParameter(para);
                    array_arg[para.reg_no]=true;//这个数组是参数
                    break;
                }
                case BasicInstruction::LLVMType::FLOAT32:{
                    int regno= ((RegOperand*)defI->formals_reg[i])->GetRegNo();
                    Register para=GetllvmReg(regno,FLOAT64);
                    cur_func->AddParameter(para);
                }
                default:
                    break;
            }
        }

        // 遍历每个LLVM IR基本块
        for (auto block_pair: *(cfg->block_map)) {
            auto id=block_pair.first;
            auto block=block_pair.second;
            cur_block = new RiscV64Block(id);
            // 将新块添加到Machine CFG中
            cur_mcfg->AssignEmptyNode(id, cur_block);
            cur_func->UpdateMaxLabel(id);

            cur_block->setParent(cur_func);
            cur_func->blocks.push_back(cur_block);

            // 指令选择主要函数, 请注意指令选择时需要维护变量cur_offset
            for (auto instruction : block->Instruction_list) {
                
                ConvertAndAppend<Instruction>(instruction);//指令选择函数入口:分发和执行
            }
        }

        // RISCV 8字节对齐（）
        if (cur_offset % 8 != 0) {
            cur_offset = ((cur_offset + 7) / 8) * 8;
        }
        cur_func->SetStackSize(cur_offset + cur_func->GetParaSize());//函数的栈大小：函数变量+参数

        // 控制流图连边
        for (int i = 0; i < cfg->G.size(); i++) {
            const auto &arcs = cfg->G[i];
            for (auto arc : arcs) {
                cur_mcfg->MakeEdge(i, arc->block_id);
            }
        }
    }
}

void RiscV64Selector::ClearFunctionSelectState() { 
    cur_offset = 0; 
    br_context.clear();
    llvm_rv_allocas.clear();
    llvm_rv_regtable.clear();
    array_arg.clear();
}

