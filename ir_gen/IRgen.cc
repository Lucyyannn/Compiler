#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

IRgenTable irgen_table;    // 中间代码生成的辅助变量
LLVMIR llvmIR;             // 我们需要在这个变量中生成中间代码
int maxReg = -1; //寄存器索引的最大值
int nowLabel = -1;
int maxLabel = -1; //最大的block索引
FuncDefInstruction funcNow; //当前的函数，方便建立新的block
BasicInstruction::LLVMType funcRetType = BasicInstruction::VOID;
int funcPaLabel = 0; //函数形参位置

int loopStartLabel = -1;
int loopEndLabel = -1;

//bool globalDefFlag = false;
Type::ty condType = Type::ty::BOOL;

BasicInstruction::LLVMType ty2llvmty[6] = {BasicInstruction::VOID, BasicInstruction::I32, BasicInstruction::FLOAT32, BasicInstruction::I1, BasicInstruction::PTR, BasicInstruction::DOUBLE};

void AddLibFunctionDeclare();

// 在基本块B末尾生成一条新指令
void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg);
void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg);
void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg);
void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg);

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg);
void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg);

void IRgenFptosi(LLVMBlock B, int src, int dst);
void IRgenSitofp(LLVMBlock B, int src, int dst);
void IRgenZextI1toI32(LLVMBlock B, int src, int dst);
//void IRgenTransFptrToPtr(LLVMBlock B, int src, int dst);

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr);

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);
void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name);
void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name);

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val);
void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val);
void IRgenRetVoid(LLVMBlock B);

void IRgenBRUnCond(LLVMBlock B, int dst_label);
void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label);

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims);

RegOperand *GetNewRegOperand(int RegNo);



// generate TypeConverse Instructions from type_src to type_dst
// eg. you can use fptosi instruction to converse float to int 
// eg. you can use zext instruction to converse bool to int
void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src, int dst) {
    if(type_src==Type::ty::INT && type_dst==Type::ty::BOOL){
        IRgenIcmpImmRight(B,IcmpInstruction::IcmpCond::ne,src,0,dst); 
    }else if(type_src==Type::ty::FLOAT && type_dst==Type::ty::BOOL){
        IRgenFcmpImmRight(B,IcmpInstruction::FcmpCond::ONE,src,0,dst);
    }
}

void BasicBlock::InsertInstruction(int pos, Instruction Ins) {
    assert(pos == 0 || pos == 1);
    if (pos == 0) {
        Instruction_list.push_front(Ins);
    } else if (pos == 1) {
        Instruction_list.push_back(Ins);
    }
}

/*
二元运算指令生成的伪代码：
    假设现在的语法树节点是：AddExp_plus
    该语法树表示 addexp + mulexp

    addexp->codeIR()
    mulexp->codeIR()
    假设mulexp生成完后，我们应该在基本块B0继续插入指令。
    addexp的结果存储在r0寄存器中，mulexp的结果存储在r1寄存器中
    生成一条指令r2 = r0 + r1，并将该指令插入基本块B0末尾。
    标注后续应该在基本块B0插入指令，当前节点的结果寄存器为r2。
    (如果考虑支持浮点数，需要查看语法树节点的类型来判断此时是否需要隐式类型转换)
*/

/*
while语句指令生成的伪代码：
    while的语法树节点为while(cond)stmt

    假设当前我们应该在B0基本块开始插入指令
    新建三个基本块Bcond，Bbody，Bend
    在B0基本块末尾插入一条无条件跳转指令，跳转到Bcond

    设置当前我们应该在Bcond开始插入指令
    cond->codeIR()    //在调用该函数前你可能需要设置真假值出口
    假设cond生成完后，我们应该在B1基本块继续插入指令，Bcond的结果为r0
    如果r0的类型不为bool，在B1末尾生成一条比较语句，比较r0是否为真。
    在B1末尾生成一条条件跳转语句，如果为真，跳转到Bbody，如果为假，跳转到Bend

    设置当前我们应该在Bbody开始插入指令
    stmt->codeIR()
    假设当stmt生成完后，我们应该在B2基本块继续插入指令
    在B2末尾生成一条无条件跳转语句，跳转到Bcond

    设置当前我们应该在Bend开始插入指令
*/

void __Program::codeIR() {
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() { addexp->codeIR(); }

void AddExp_plus::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    addexp->codeIR();
    int reg1 = maxReg;
    mulexp->codeIR();
    int reg2 = maxReg;
    Type::ty addtype = addexp->attribute.T.type;
    Type::ty multype = mulexp->attribute.T.type;

    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; addtype=Type::INT;
    }
    if(multype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; multype=Type::INT;
    }

    // 检查子表达式类型并选择指令操作符号opcode
    if(addtype==Type::FLOAT || multype==Type::FLOAT){
        if(addtype==Type::INT){/* 显式类型转换：LLVM IR中，浮点运算指令要求两操作数都是float*/
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(multype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenArithmeticF32(nowBlock,BasicInstruction::LLVMIROpcode::FADD,reg1,reg2,++maxReg);
    }
    else{
        IRgenArithmeticI32(nowBlock,BasicInstruction::LLVMIROpcode::ADD,reg1,reg2,++maxReg);
    }
}

void AddExp_sub::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    addexp->codeIR();
    int reg1 = maxReg;
    mulexp->codeIR();
    int reg2 = maxReg;
    Type::ty addtype = addexp->attribute.T.type;
    Type::ty multype = mulexp->attribute.T.type;

    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; addtype=Type::INT;
    }
    if(multype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; multype=Type::INT;
    }

    if(addtype==Type::FLOAT || multype==Type::FLOAT){
       if(addtype==Type::INT){/* 显式类型转换：LLVM IR中，浮点运算指令要求两操作数都是float*/
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(multype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenArithmeticF32(nowBlock,BasicInstruction::LLVMIROpcode::FSUB,reg1,reg2,++maxReg);
    }
    else{
        IRgenArithmeticI32(nowBlock,BasicInstruction::LLVMIROpcode::SUB,reg1,reg2,++maxReg);
    }
}

void MulExp_mul::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    mulexp->codeIR();
    int reg1 = maxReg;
    unary_exp->codeIR();
    int reg2 = maxReg;
    Type::ty multype = mulexp->attribute.T.type;
    Type::ty unarytype = unary_exp->attribute.T.type;

    if(multype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; multype=Type::INT;
    }
    if(unarytype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; unarytype=Type::INT;
    }

    if(multype==Type::FLOAT || unarytype==Type::FLOAT){
        if(multype==Type::INT){/* 显式类型转换：LLVM IR中，浮点运算指令要求两操作数都是float*/
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(unarytype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenArithmeticF32(nowBlock,BasicInstruction::LLVMIROpcode::FMUL,reg1,reg2,++maxReg);
    }
    else{
        IRgenArithmeticI32(nowBlock,BasicInstruction::LLVMIROpcode::MUL,reg1,reg2,++maxReg);
    }
}

void MulExp_div::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    mulexp->codeIR();
    int reg1 = maxReg;
    unary_exp->codeIR();
    int reg2 = maxReg;
    Type::ty multype = mulexp->attribute.T.type;
    Type::ty unarytype = unary_exp->attribute.T.type;

    if(multype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; multype=Type::INT;
    }
    if(unarytype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; unarytype=Type::INT;
    }

    if(multype==Type::FLOAT || unarytype==Type::FLOAT){
        if(multype==Type::INT){/* 显式类型转换：LLVM IR中，浮点运算指令要求两操作数都是float*/
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(unarytype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenArithmeticF32(nowBlock,BasicInstruction::LLVMIROpcode::FDIV,reg1,reg2,++maxReg);
    }
    else{
        IRgenArithmeticI32(nowBlock,BasicInstruction::LLVMIROpcode::DIV,reg1,reg2,++maxReg);
    }
}

void MulExp_mod::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    mulexp->codeIR();
    int reg1 = maxReg;
    unary_exp->codeIR();
    int reg2 = maxReg;
    Type::ty multype = mulexp->attribute.T.type;
    Type::ty unarytype = unary_exp->attribute.T.type;

    if(multype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; multype=Type::INT;
    }
    if(unarytype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; unarytype=Type::INT;
    }

    // if(multype==Type::FLOAT || unarytype==Type::FLOAT){
    //     IRgenArithmeticF32(nowBlock,BasicInstruction::LLVMIROpcode::MOD,reg1,reg2,++maxReg);
    // }
    // else{
        //经过类型检查，取模运算的操作数一定不是float
        IRgenArithmeticI32(nowBlock,BasicInstruction::LLVMIROpcode::MOD,reg1,reg2,++maxReg);
    //}
}

void RelExp_leq::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    relexp->codeIR();
    int reg1 = maxReg;
    addexp->codeIR();
    int reg2 = maxReg;
    Type::ty reltype = relexp->attribute.T.type;
    Type::ty addtype = addexp->attribute.T.type;

    if(reltype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; reltype=Type::INT;
    }
    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; addtype=Type::INT;
    }

    if(reltype==Type::FLOAT || addtype==Type::FLOAT){
        if(reltype==Type::INT){         /* 显式类型转换：LLVM IR中，fcmp要求两操作数都是float*/
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(addtype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::OLE,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::sle,reg1,reg2,++maxReg);
    }
}

void RelExp_lt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    relexp->codeIR();
    int reg1 = maxReg;
    addexp->codeIR();
    int reg2 = maxReg;
    Type::ty reltype = relexp->attribute.T.type;
    Type::ty addtype = addexp->attribute.T.type;

    if(reltype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; reltype=Type::INT;
    }
    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; addtype=Type::INT;
    }

    if(reltype==Type::FLOAT || addtype==Type::FLOAT){
        if(reltype==Type::INT){
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(addtype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::OLT,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::slt,reg1,reg2,++maxReg);
    }
}

void RelExp_geq::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    relexp->codeIR();
    int reg1 = maxReg;
    addexp->codeIR();
    int reg2 = maxReg;
    Type::ty reltype = relexp->attribute.T.type;
    Type::ty addtype = addexp->attribute.T.type;

    if(reltype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; reltype=Type::INT;
    }
    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; addtype=Type::INT;
    }

    if(reltype==Type::FLOAT || addtype==Type::FLOAT){
        if(reltype==Type::INT){
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(addtype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::OGE,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::sge,reg1,reg2,++maxReg);
    }
}

void RelExp_gt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    relexp->codeIR();
    int reg1 = maxReg;
    addexp->codeIR();
    int reg2 = maxReg;
    Type::ty reltype = relexp->attribute.T.type;
    Type::ty addtype = addexp->attribute.T.type;

    if(reltype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; reltype=Type::INT;
    }
    if(addtype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; addtype=Type::INT;
    }

    if(reltype==Type::FLOAT || addtype==Type::FLOAT){
        if(reltype==Type::INT){
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(addtype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::OGT,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::sgt,reg1,reg2,++maxReg);
    }
}

void EqExp_eq::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    eqexp->codeIR();
    int reg1 = maxReg;
    relexp->codeIR();
    int reg2 = maxReg;
    Type::ty eqtype = eqexp->attribute.T.type;
    Type::ty reltype = relexp->attribute.T.type;

    if(eqtype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; eqtype=Type::INT;
    }
    if(reltype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; reltype=Type::INT;
    }

    if(reltype==Type::FLOAT || eqtype==Type::FLOAT){
        if(eqtype==Type::INT){
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(reltype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::OEQ,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::eq,reg1,reg2,++maxReg);
    }
}

void EqExp_neq::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    eqexp->codeIR();
    int reg1 = maxReg;
    relexp->codeIR();
    int reg2 = maxReg;
    Type::ty eqtype = eqexp->attribute.T.type;
    Type::ty reltype = relexp->attribute.T.type;

    if(eqtype==Type::BOOL){   /*显式类型转换：bool-->int */
        IRgenZextI1toI32(nowBlock,reg1,++maxReg);
        reg1 = maxReg; eqtype=Type::INT;
    }
    if(reltype==Type::BOOL){
        IRgenZextI1toI32(nowBlock,reg2,++maxReg);
        reg2 = maxReg; reltype=Type::INT;
    }

    if(reltype==Type::FLOAT || eqtype==Type::FLOAT){
        if(eqtype==Type::INT){
            IRgenSitofp(nowBlock,reg1,++maxReg);
            reg1=maxReg;
        }if(reltype==Type::INT){
            IRgenSitofp(nowBlock,reg2,++maxReg);
            reg2=maxReg;
        }
        IRgenFcmp(nowBlock,BasicInstruction::FcmpCond::ONE,reg1,reg2,++maxReg);
    }
    else{
        IRgenIcmp(nowBlock,BasicInstruction::IcmpCond::ne,reg1,reg2,++maxReg);
    }
}

// short circuit &&
void LAndExp_and::codeIR() {
    LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++maxLabel);
    //newBlock->comment="与操作新增块";
    int tempLabel = maxLabel;//tempLabel暂存新增块的label

    landexp->true_label = maxLabel;
    landexp->false_label = false_label;
    landexp->codeIR();
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    nowLabel = tempLabel; // 当前label设置为逻辑符号的新块

    if(landexp->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(nowBlock,landexp->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }

    IRgenBrCond(nowBlock,maxReg,tempLabel,false_label); // 在前一个块的最后加上跳转指令
    eqexp->codeIR();
    LLVMBlock nowBlock2 = llvmIR.GetBlock(funcNow,nowLabel); //新增块：进行类型转换
    if(eqexp->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(nowBlock2,eqexp->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }
    //condType = eqexp->attribute.T.type; // 记录最后的类型，便于转换
}

// short circuit ||
void LOrExp_or::codeIR() {
    LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++maxLabel);
    //newBlock->comment="或操作新增块";
    int tempLabel = maxLabel;

    lorexp->true_label = true_label;
    lorexp->false_label = maxLabel;
    lorexp->codeIR();
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    nowLabel = tempLabel;

    if(lorexp->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(nowBlock,lorexp->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }

    IRgenBrCond(nowBlock,maxReg,true_label,tempLabel);
    landexp->true_label = true_label;
    landexp->false_label = false_label;
    landexp->codeIR();
    LLVMBlock nowBlock2 = llvmIR.GetBlock(funcNow,nowLabel);
    if(landexp->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(nowBlock2,landexp->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }
    //condType = landexp->attribute.T.type;
}

void ConstExp::codeIR() { addexp->codeIR(); }

/*
* 1. 用于在计算时加载单个数据 
*  %r8 = load i32, ptr @a
*  %r9 = load i32, ptr %r3
* 2. 用于给变量的声明赋值
*/
void Lval::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    Operand operand = nullptr;
    BasicInstruction::LLVMType type;
    int reg = irgen_table.symbol_table.lookup(name);
    // 处理非数组
    //if(dims==nullptr){   ////////////////// lval的dims存储的是调用时的索引，如int a[10][2]={}；putarray(a);或int x=3;int b=x; a和x的dims都是空的
                           /////////////////  区分该表达式本身是数组还是变量，看类型检查时登记在symbol_table中的dims，
                           /////////////////  经类型检查后，该dims已经标记在此语法树节点dimsDeclined中
    if(dimsDeclined.size()==0){
        // 找不到同名变量会返回-1，代表是全局变量
        if(reg==-1){
            operand = GetNewGlobalOperand(name->get_string());
            type = ty2llvmty[semant_table.GlobalTable[name].type];
        }
        else{
            operand = GetNewRegOperand(reg);
            type = ty2llvmty[irgen_table.reg_ty_map[reg]];
        }
        // 非左值需要load指令（右值）
        if(!is_left){
            IRgenLoad(nowBlock,type,++maxReg,operand);
        }
        // 左值直接获取对应的寄存器值，用于后面的store
        else{
            op_ptr = operand;
        }
    }
    // 处理数组
    /*
    对于局部变量的数组： 若为普通数组，具有完整的维度声明，从AST节点的dimsDeclined获取维度的定义（semant阶段收集)。计算索引地址时前置偏移为0；
                       若为函数形参，声明时第一维省略，从irgen_table.ArrayAttribute获取缺省的维度定义(funcf->codeIR收集）。计算索引地址时前置偏移为第一维索引。
    
    数组维度定义dims与lval索引数量关系:   
                        若为普通数组，dimsInt.size()==dims.size()==indexs.size()-1 【前置偏移0】-->具体元素
                                     dimsInt.size()>dims.size()==indexs.size()-1  【前置偏移0】-->一段数组
                        若为函数形参，dimsInt.size()==dims.size()-1==indexs.size()=1【前置偏移为indexs[0]/dims[0]】-->一段数组
                                     若为具体元素，不以ptr形式声明
    */
    else{
        if(reg==-1){//是全局变量
            operand = GetNewGlobalOperand(name->get_string());//operand存储数组基址，RealAddr存储索引后的元素地址
            type = ty2llvmty[semant_table.GlobalTable[name].type];//type指示int/float
        }
        else{//是局部变量
            operand = GetNewRegOperand(reg);
            type = ty2llvmty[irgen_table.reg_ty_map[reg]];
        }
        //取值: 数组本身的维度声明dimsInt，访问时的索引列表indexOperands-->取偏移地址-->取元素值(rignt)/给地址(left)
        std::vector<int>dimsInt{}; std::vector<Operand> indexOperands{};
        //auto it=irgen_table.ArrayAttribute.find(reg);
        VarAttribute var=irgen_table.symbol_table.lookup_val(name);
        if(var.type!=Type::VOID){//若是函数形参
            dimsInt=var.dims;
        }else{//若是普通数组
            dimsInt=dimsDeclined;
            indexOperands.push_back(new ImmI32Operand(0));
        }
        if(dims!=nullptr){
            for(auto dim:*dims){
                dim->codeIR();
                if(dim->attribute.V.ConstTag){//索引是常量，直接收集
                    indexOperands.push_back(new ImmI32Operand(dim->attribute.V.val.IntVal));
                }else{//索引是表达式，经过load，结果已存在maxReg--> Bool->Int隐式转换
                    if(dim->attribute.T.type==Type::BOOL){
                        int preReg=maxReg;
                        IRgenZextI1toI32(nowBlock,preReg,++maxReg);
                    }
                    indexOperands.push_back(GetNewRegOperand(maxReg));
                }
            }
        }
        IRgenGetElementptrIndexI32(nowBlock,type,++maxReg,operand,dimsInt,indexOperands);
        Operand RealAddr=GetNewRegOperand(maxReg);
        if(dims==nullptr||(dims!=nullptr&&dims->size()<dimsDeclined.size())){//若仍是数组
            assert(!is_left);//数组的不能是左值，因为是地址
            op_ptr = RealAddr;

        }else{//若是数组的某个元素
            //若非左值，将其值加载到新寄存器中；若是左值，在原地址上使用
            if(!is_left){
                IRgenLoad(nowBlock,type,++maxReg,RealAddr);
            }else{
                op_ptr = RealAddr;
            }
        }
    }
}

void FuncRParams::codeIR() {
    //LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    if(params->size()==0)
        return;
    for(int i=0; i<params->size(); i++){
        Expression param = (*params)[i];
        param->codeIR(); // 表达式计算完成后，，最大寄存器就是存有目标数据的寄存器
        args->push_back({ty2llvmty[param->attribute.T.type], GetNewRegOperand(maxReg)});
    }
}

void Func_call::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    
    /* 从FunctionTable中取出函数定义*/
    FuncDef funcDef = semant_table.FunctionTable[name];
    BasicInstruction::LLVMType returnType = ty2llvmty[funcDef->return_type];
    std::vector<FuncFParam>* def_formals=funcDef->formals;

    // 无参数函数
    if(funcr_params==nullptr){
        if(returnType==BasicInstruction::LLVMType::VOID)
            IRgenCallVoidNoArgs(nowBlock,returnType,name->get_string());
        else
            IRgenCallNoArgs(nowBlock,returnType,++maxReg,name->get_string());
    }
    // 有参数函数
    else{
        /* 对每对实参、形参进行类型比较，对实参进行类型转换*/
        std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> _args{};
        for(int i=0;i<(*def_formals).size();i++){
            Expression param=(*((FuncRParams*)funcr_params)->params)[i];//对实参进行codeIR()
            param->codeIR();//对实参检查类型、隐式转换   
            BasicInstruction::LLVMType Rtype=ty2llvmty[param->attribute.T.type]; 
            BasicInstruction::LLVMType Ftype= (*def_formals)[i]->dims!=nullptr?
                                                BasicInstruction::LLVMType::PTR:
                                                ty2llvmty[(*def_formals)[i]->type_decl];

            if(Rtype==BasicInstruction::LLVMType::I1){
                int srcReg=maxReg;
                IRgenZextI1toI32(nowBlock,srcReg,++maxReg);
                Rtype=BasicInstruction::LLVMType::I32;
            }
            if(Ftype==BasicInstruction::LLVMType::FLOAT32 && Rtype==BasicInstruction::LLVMType::I32){
                int srcReg=maxReg;
                IRgenSitofp(nowBlock,srcReg,++maxReg);
            }else if(Ftype==BasicInstruction::LLVMType::I32 && Rtype==BasicInstruction::LLVMType::FLOAT32){
                int srcReg=maxReg;
                IRgenFptosi(nowBlock,srcReg,++maxReg);
            }
            _args.push_back({Ftype,GetNewRegOperand(maxReg)});
        }
        /* 进行函数调用 */
        if(returnType==BasicInstruction::LLVMType::VOID)
            IRgenCallVoid(nowBlock,returnType,_args,name->get_string());
        else
            IRgenCall(nowBlock,returnType,++maxReg,_args,name->get_string());

        //((FuncRParams*)funcr_params)->args = nullptr;
    }
}

void UnaryExp_plus::codeIR() {
    unary_exp->codeIR();
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    int unaryReg = maxReg;
    if(unary_exp->attribute.T.type==Type::BOOL){
        IRgenZextI1toI32(nowBlock,unaryReg,++maxReg);
    }
}

void UnaryExp_neg::codeIR() {
    unary_exp->codeIR();
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    int unaryReg = maxReg;

    if(unary_exp->attribute.T.type==Type::INT){
        IRgenArithmeticI32ImmLeft(nowBlock,BasicInstruction::SUB,0,unaryReg,++maxReg);
    }
    else if(unary_exp->attribute.T.type==Type::BOOL){
        IRgenZextI1toI32(nowBlock,unaryReg,++maxReg);
        unaryReg = maxReg;
        IRgenArithmeticI32ImmLeft(nowBlock,BasicInstruction::SUB,0,unaryReg,++maxReg);
    }else{
        IRgenArithmeticF32ImmLeft(nowBlock,BasicInstruction::FSUB,0,unaryReg,++maxReg);
    }
}

void UnaryExp_not::codeIR() {
    unary_exp->codeIR();
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    int unaryReg = maxReg;

    if(unary_exp->attribute.T.type==Type::INT){
        IRgenIcmpImmRight(nowBlock,BasicInstruction::IcmpCond::eq,unaryReg,0,++maxReg);
    }
    else if(unary_exp->attribute.T.type==Type::BOOL){
        IRgenZextI1toI32(nowBlock,unaryReg,++maxReg);
        unaryReg = maxReg;
        IRgenIcmpImmRight(nowBlock,BasicInstruction::IcmpCond::eq,unaryReg,0,++maxReg);
    }else{
        IRgenFcmpImmRight(nowBlock,BasicInstruction::FcmpCond::OEQ,unaryReg,0,++maxReg);
    }
}

void IntConst::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenArithmeticI32ImmAll(nowBlock,BasicInstruction::ADD,0,val,++maxReg);
}

void FloatConst::codeIR() {    
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenArithmeticF32ImmAll(nowBlock,BasicInstruction::FADD,0,val,++maxReg);
}

void StringConst::codeIR() { //TODO("StringConst CodeIR"); 
}

void PrimaryExp_branch::codeIR() { exp->codeIR(); }

void assign_stmt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    ((Lval*)lval)->is_left=true;
    lval->codeIR();
    exp->codeIR();
    int expReg=maxReg;
    //int <--> float 显式类型转换
    if(lval->attribute.T.type==Type::INT&&exp->attribute.T.type==Type::FLOAT){
        IRgenFptosi(nowBlock,expReg,++maxReg);
    }else if(lval->attribute.T.type==Type::FLOAT&&exp->attribute.T.type==Type::INT){
        IRgenSitofp(nowBlock,expReg,++maxReg);
    }
    IRgenStore(nowBlock,ty2llvmty[lval->attribute.T.type],GetNewRegOperand(maxReg),((Lval*)lval)->op_ptr);
}

void expr_stmt::codeIR() { exp->codeIR(); }

void block_stmt::codeIR() {
    //irgen_table.symbol_table.enter_scope(); //进入作用域
    b->codeIR();
    //irgen_table.symbol_table.exit_scope(); //离开作用域
}

void ifelse_stmt::codeIR() {
    maxLabel = nowLabel; // 在条件语句中，maxLabel要代替nowLabel开辟最新的块

    // if的语句部分指令块
    LLVMBlock ifBlock = llvmIR.NewBlock(funcNow,++maxLabel); 
    //ifBlock->comment="if_else语句的if块";
    int ifLabel = maxLabel;
    // else的语句部分指令块
    LLVMBlock elseBlock = llvmIR.NewBlock(funcNow,++maxLabel); 
    //elseBlock->comment="if_else语句的else块";
    int elseLabel = maxLabel;
    // 后面的新指令块
    LLVMBlock endBlock = llvmIR.NewBlock(funcNow,++maxLabel); 
    //endBlock->comment="if_else语句结束后的新块";
    int endLabel = maxLabel;

    Cond->true_label = ifLabel;
    Cond->false_label = elseLabel;
    condType = Type::ty::BOOL;
    Cond->codeIR();
    LLVMBlock condEndBlock = llvmIR.GetBlock(funcNow,nowLabel);

    if(Cond->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,Cond->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }else if(condType != Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,condType,Type::ty::BOOL,condReg,++maxReg);
    }

    IRgenBrCond(condEndBlock,maxReg,ifLabel,elseLabel); // 条件语句结束块设置跳转
    nowLabel = maxLabel;
    
    LLVMBlock trueIfBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 实际的if语句块
    IRgenBRUnCond(ifBlock,nowLabel); //从if标志语句块跳到实际语句块
    ifstmt->codeIR();
    LLVMBlock ifEndBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(ifEndBlock,endLabel); // if语句块之后需要跳到else块之后的新块

    LLVMBlock trueElseBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 实际的else语句块
    IRgenBRUnCond(elseBlock,nowLabel); //从else标志语句块跳到实际语句块
    elsestmt->codeIR();

    LLVMBlock elseEndBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(elseEndBlock,endLabel); // 防止有不进行跳转的块
    
    LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 后面的新指令块
    IRgenBRUnCond(endBlock,nowLabel);
}


void if_stmt::codeIR() {
    maxLabel = nowLabel; // 在条件语句中，maxLabel要代替nowLabel开辟最新的块

    LLVMBlock stmtBlock = llvmIR.NewBlock(funcNow,++maxLabel); // if的语句部分标志块
    //stmtBlock->comment="if语句";
    int ifLabel = maxLabel;
    LLVMBlock endBlock = llvmIR.NewBlock(funcNow,++maxLabel); // 语句结束标志块
    //endBlock->comment="if语句结束后的新块";
    int endLabel = maxLabel;

    Cond->true_label = ifLabel;
    Cond->false_label = endLabel;
    condType = Type::ty::BOOL;
    Cond->codeIR();
    LLVMBlock condEndBlock = llvmIR.GetBlock(funcNow,nowLabel);

    if(Cond->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,Cond->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }else if(condType != Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,condType,Type::ty::BOOL,condReg,++maxReg);
    }

    IRgenBrCond(condEndBlock,maxReg,ifLabel,endLabel); // 条件语句结束块设置跳转
    nowLabel = maxLabel;

    LLVMBlock trueStmtBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 实际的if语句块
    IRgenBRUnCond(stmtBlock,nowLabel);
    ifstmt->codeIR();

    LLVMBlock stmtEndBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(stmtEndBlock,endLabel); // 防止有不进行跳转的块

    LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 后面的新指令块
    IRgenBRUnCond(endBlock,nowLabel);
}

// void while_stmt::codeIR() {
//     // 保证block末尾是跳转指令
//     LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
//     IRgenBRUnCond(nowBlock,nowLabel+1);

//     LLVMBlock condBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 循环条件需要跳转回来，要新建块
//     maxLabel = nowLabel; // 在条件语句中，maxLabel要代替nowLabel开辟最新的块
//     condBlock->comment="while循环的条件语句开始块";
//     int condStartLabel = maxLabel; // 条件语句的开始块

//     LLVMBlock stmtBlock = llvmIR.NewBlock(funcNow,++maxLabel); // while循环语句指令块
//     stmtBlock->comment="while循环";
//     int stmtLabel = maxLabel;
//     LLVMBlock endBlock = llvmIR.NewBlock(funcNow,++maxLabel); // 后面的新指令块
//     endBlock->comment="while循环结束后的新块";
//     int endLabel = maxLabel;

//     // 进行语句块指令生成，需要记录循环的跳转块下标
//     int temp_loopStartLabel = loopStartLabel;
//     int temp_loopEndLabel = loopEndLabel;
//     loopStartLabel = condStartLabel;
//     loopEndLabel = endLabel;

//     Cond->true_label = stmtLabel;
//     Cond->false_label = endLabel;
//     condType = Type::ty::BOOL;
//     Cond->codeIR();
//     LLVMBlock condEndBlock = llvmIR.GetBlock(funcNow,nowLabel);

//     if(Cond->attribute.T.type!=Type::ty::BOOL){
//         int condReg = maxReg;
//         IRgenTypeConverse(condEndBlock,Cond->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
//     }else if(condType != Type::ty::BOOL){
//         int condReg = maxReg;
//         IRgenTypeConverse(condEndBlock,condType,Type::ty::BOOL,condReg,++maxReg);
//     }

//     IRgenBrCond(condEndBlock,maxReg,stmtLabel,endLabel); // 条件语句结束块设置跳转
//     nowLabel = maxLabel;

//     LLVMBlock trueStmtBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 实际的while语句块
//     trueStmtBlock->comment="实际的while语句块";
//     IRgenBRUnCond(stmtBlock,nowLabel);

//     body->codeIR();

//     LLVMBlock stmtEndBlock = llvmIR.GetBlock(funcNow,nowLabel);
//     IRgenBRUnCond(stmtEndBlock,condStartLabel); // while语句块的最末尾需要无条件跳回条件语句处
    
//     LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 后面的新指令块
//     newBlock->comment="while后面的实际新块";
//     IRgenBRUnCond(endBlock,nowLabel);

//     // 为了防止循环嵌套导致跳转块被替换，需用临时变量记录并在语句块生成之后恢复
//     loopStartLabel = temp_loopStartLabel;
//     loopEndLabel = temp_loopEndLabel;
// }

void while_stmt::codeIR() {
    // 保证block末尾是跳转指令
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(nowBlock,nowLabel+1);

    LLVMBlock condBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 循环条件需要跳转回来，要新建块
    maxLabel = nowLabel; // 在条件语句中，maxLabel要代替nowLabel开辟最新的块
    //condBlock->comment="while循环的条件语句开始块";
    int condStartLabel = maxLabel; // 条件语句的开始块

    LLVMBlock stmtBlock = llvmIR.NewBlock(funcNow,++maxLabel); // while循环语句指令块
    //stmtBlock->comment="while循环";
    int stmtLabel = maxLabel;
    LLVMBlock endBlock = llvmIR.NewBlock(funcNow,++maxLabel); // 后面的新指令块
    //endBlock->comment="while循环结束后的新块";
    int endLabel = maxLabel;

    Cond->true_label = stmtLabel;
    Cond->false_label = endLabel;
    condType = Type::ty::BOOL;
    Cond->codeIR();
    LLVMBlock condEndBlock = llvmIR.GetBlock(funcNow,nowLabel);

    if(Cond->attribute.T.type!=Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,Cond->attribute.T.type,Type::ty::BOOL,condReg,++maxReg);
    }else if(condType != Type::ty::BOOL){
        int condReg = maxReg;
        IRgenTypeConverse(condEndBlock,condType,Type::ty::BOOL,condReg,++maxReg);
    }

    IRgenBrCond(condEndBlock,maxReg,stmtLabel,endLabel); // 条件语句结束块设置跳转
    nowLabel = maxLabel;

    // 进行语句块指令生成，需要记录循环的跳转块下标
    int temp_loopStartLabel = loopStartLabel;
    int temp_loopEndLabel = loopEndLabel;
    loopStartLabel = condStartLabel;
    loopEndLabel = endLabel;

    LLVMBlock trueStmtBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 实际的while语句块
    //trueStmtBlock->comment="实际的while语句块";
    IRgenBRUnCond(stmtBlock,nowLabel);

    body->codeIR();

    // 为了防止循环嵌套导致跳转块被替换，需用临时变量记录并在语句块生成之后恢复
    loopStartLabel = temp_loopStartLabel;
    loopEndLabel = temp_loopEndLabel;

    LLVMBlock stmtEndBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(stmtEndBlock,condStartLabel); // while语句块的最末尾需要无条件跳回条件语句处
    
    LLVMBlock newBlock = llvmIR.NewBlock(funcNow,++nowLabel); // 后面的新指令块
    //newBlock->comment="while后面的实际新块";
    IRgenBRUnCond(endBlock,nowLabel);
}





void continue_stmt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(nowBlock,loopStartLabel);
}

void break_stmt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenBRUnCond(nowBlock,loopEndLabel);
}

void return_stmt::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    return_exp->codeIR();
    BasicInstruction::LLVMType returnType=ty2llvmty[return_exp->attribute.T.type];
    int returnReg=maxReg; 

    /* 显式类型转换*/
    if(returnType==BasicInstruction::LLVMType::I1){
        IRgenZextI1toI32(nowBlock,returnReg,++maxReg);
        returnReg=maxReg; returnType=BasicInstruction::LLVMType::I32;
    }
    if(funcRetType==BasicInstruction::LLVMType::I32){
        if(returnType==BasicInstruction::LLVMType::FLOAT32){
            IRgenFptosi(nowBlock,returnReg,++maxReg);
        }
    }else if(funcRetType==BasicInstruction::LLVMType::FLOAT32){
        if(returnType==BasicInstruction::LLVMType::I32){
            IRgenSitofp(nowBlock,returnReg,++maxReg);
        }
    }
    IRgenRetReg(nowBlock,funcRetType,maxReg);
}

void return_stmt_void::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    IRgenRetVoid(nowBlock);
}

void ConstInitVal::codeIR() {
    if(initval!=nullptr){
        for(auto Initval:*initval){
            Initval->codeIR();
        }
    }
}

void ConstInitVal_exp::codeIR() {
    exp->codeIR();
}

void VarInitVal::codeIR() {
    if(initval!=nullptr){
        for(auto Initval:*initval){
            Initval->codeIR();
        }
    }
}

void VarInitVal_exp::codeIR() {
    exp->codeIR();
}

/*  *************************************** 数组处理逻辑 ***************************************/
/*
@ 函数功能：
    IR_Gen需要将用户初始化列表中的值显式初始化【访问、存值】-->索引、数值
@ 变量含义：
    - semant_list: 经过语义分析得到的完整初始化列表，一维
    - init_list:   用户定义的原始初始化列表，展开为一维

    - sip: semant_list的遍历指针
    - iip：init_list的遍历指针
@ 实现思路：
    -定位post：逐个比对init_list中的各值value在semant_list中的位置pos
        若value!=0 , -->pos √
        若value==0 , pos需要匹配semant_list中最近的用户初始化的0，而非semant填充的0
                    pos in Initval0Pos -->pos √
                    pos not in Initval0Pos -->sip++
    -pos转换：根据一维pos值，计算该value在数组各维度的索引值indexs
        std::vector< > indexs; size=d
        pos, array[k][m]...[n],(d维): 
            indexs[i]=pos/该维长度 。pos=pos%该维长度
            indexs[i+1]=...
*/
/*
    优化-->直接在semant遍历时直接记录所有显示初始化值的pos。[0,size()-1]
           将semant自动填充的0的pos设为0，以此区分。
           thisDimNum 6,2,1
           a[4][3][2]  pos=19 (20)
           a[3][0][1]
*/
/*
补充-->对于调用表达式作为初始化值的initval
    通过isLval==true判断；
    非const exp和数组引用 需要显式load值 ： initval->codeIR()内部完成

关键-->将load得到的变量值store入数组中正确的位置（pos-->indexs）
    一边递归遍历initval（同时调用其codeIR加载变量值-->varReg）
    一边对exp检查isLval, isLval==true :
                                expPos --> indexs,store入相应位置

*/

//根据pos计算indexs
std::vector<Operand> PosToIndexs(std::vector<int> thisDimNum, int pos){
    std::vector<Operand>indexs{new ImmI32Operand(0)};//计算indexs
    for(int j=0;j<thisDimNum.size();j++){
        indexs.push_back(new ImmI32Operand(pos/thisDimNum[j]));
        pos%=thisDimNum[j];
    }
    return indexs;
}
//递归显式初始化
void RecursionGen(LLVMBlock nowBlock,int varReg, int& ip, InitVal& init, bool ConstTag, VarAttribute& DimAttr,std::vector<int>& thisDimNum,Type::ty type){
    if(init!=nullptr){
        //数组声明为常量Const
        if(ConstTag){
            std::vector<InitVal> initvals={};
            initvals=*(((ConstInitVal*)init)->initval);
            for(int i=0;i<initvals.size();i++){
                    //对于单值，直接处理
                    if(initvals[i]->is_exp()){
                        initvals[i]->codeIR();//maxReg应存储着InitVal的计算结果变量-->initvalReg
                        int initvalReg=maxReg;
                        auto ConstInitExp=(ConstInitVal_exp*)initvals[i];
                        
                        while(DimAttr.RealInitvalPos[ip]==0){ip++;}//只处理用户显式初始化的值
                        std::vector<Operand> indexs=PosToIndexs(thisDimNum,ip);//计算索引位置index
                    
                        //调用getelementptr,获取该位置指针reg(默认index_type是I32)
                        IRgenGetElementptrIndexI32(nowBlock,ty2llvmty[type],++maxReg,GetNewRegOperand(varReg),DimAttr.dims,indexs);
                        int ptrReg=maxReg;
                        //显式初始化值 store->maxReg
                        if(type==Type::INT){
                            if(initvals[i]->attribute.T.type==Type::FLOAT){
                                IRgenFptosi(nowBlock,initvalReg,++maxReg);
                                initvalReg=maxReg;
                            }
                            //f(ConstInitExp->isLval){//初始值是表达式，需要加载该变量
                            IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,initvalReg,GetNewRegOperand(ptrReg));
                            //}else{//初始值是立即数，直接存入
                            //    IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,new ImmI32Operand(DimAttr.IntInitVals[ip]),GetNewRegOperand(maxReg));
                            //}
                        }else if(type==Type::FLOAT){
                            if(initvals[i]->attribute.T.type==Type::INT){
                                IRgenSitofp(nowBlock,initvalReg,++maxReg);
                                initvalReg=maxReg;
                            }
                            // if(ConstInitExp->isLval){//初始值是表达式，需要加载该变量
                            IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,initvalReg,GetNewRegOperand(ptrReg));
                            // }else{//初始值是立即数，直接存入
                            //     IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,new ImmF32Operand(DimAttr.IntInitVals[ip]),GetNewRegOperand(maxReg));
                            // }
                        }
                        ip++;
                    }//仍为列表，继续递归分解
                    else{
                        RecursionGen(nowBlock,varReg,ip,initvals[i],ConstTag,DimAttr,thisDimNum,type);
                    }
                
            }
        }//数组声明非常量Var
        else{
            std::vector<InitVal> initvals={};
            initvals=*(((ConstInitVal*)init)->initval);
            for(int i=0;i<initvals.size();i++){
                    //对于单值，直接处理
                    if(initvals[i]->is_exp()){
                        initvals[i]->codeIR();//maxReg应存储着InitVal的计算结果变量-->initvalReg
                        int initvalReg=maxReg;
                        auto VarInitExp=(VarInitVal_exp*)initvals[i];
                        
                        while(DimAttr.RealInitvalPos[ip]==0){ip++;}//只处理用户显式初始化的值
                        std::vector<Operand> indexs=PosToIndexs(thisDimNum,ip);//计算索引位置index

                        //调用getelementptr,获取该位置指针reg(默认index_type是I32)
                        IRgenGetElementptrIndexI32(nowBlock,ty2llvmty[type],++maxReg,GetNewRegOperand(varReg),DimAttr.dims,indexs);
                        int ptrReg=maxReg;
                        //显式初始化值 store->maxReg
                        if(type==Type::INT){
                            if(initvals[i]->attribute.T.type==Type::FLOAT){
                                IRgenFptosi(nowBlock,initvalReg,++maxReg);
                                initvalReg=maxReg;
                            }
                            //f(ConstInitExp->isLval){//初始值是表达式，需要加载该变量
                            IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,initvalReg,GetNewRegOperand(ptrReg));
                            //}else{//初始值是立即数，直接存入
                            //    IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,new ImmI32Operand(DimAttr.IntInitVals[ip]),GetNewRegOperand(maxReg));
                            //}
                        }else if(type==Type::FLOAT){
                            if(initvals[i]->attribute.T.type==Type::INT){
                                IRgenSitofp(nowBlock,initvalReg,++maxReg);
                                initvalReg=maxReg;
                            }
                            // if(ConstInitExp->isLval){//初始值是表达式，需要加载该变量
                            IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,initvalReg,GetNewRegOperand(ptrReg));
                            // }else{//初始值是立即数，直接存入
                            //     IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,new ImmF32Operand(DimAttr.IntInitVals[ip]),GetNewRegOperand(maxReg));
                            // }
                        }
                        ip++;
                    }//仍为列表，继续递归分解
                    else{
                        RecursionGen(nowBlock,varReg,ip,initvals[i],ConstTag,DimAttr,thisDimNum,type);
                    }
            }
        }
    }
}

void InitCodeGen(VarAttribute& DimAttr, InitVal init, int varReg, LLVMBlock nowBlock,Type::ty type){
    //assert(DimAttr.IntInitVals.size()==DimAttr.RealInitvalPos.size());
    /*
    数组维度:DimAttr.dims
    完整的初始化列表：DimAttr.IntInitVals
    完整的位置列表:DimAttr.RealInitvalPos
    该数组的基址寄存器编号：varReg
    */
    //计算thisDimNum:该维度每层元素个数
    int TotalLength=0;//数组的理论总长度
    if(type==Type::INT){
        TotalLength=DimAttr.IntInitVals.size();
    }else{
        TotalLength=DimAttr.FloatInitVals.size();
    }
    std::vector<int> thisDimNum{};
    thisDimNum.push_back(TotalLength/DimAttr.dims[0]);
    for(int i=1;i<DimAttr.dims.size();i++){
        thisDimNum.push_back(thisDimNum[i-1]/DimAttr.dims[i]);
    }
    int ip=0;  
    RecursionGen(nowBlock,varReg,ip,init,DimAttr.ConstTag,DimAttr,thisDimNum,type);

    /* 对于用户没有显式初始化的部分，自动初始化存入0*/
    for(int i=0;i<TotalLength;i++){
        if(DimAttr.RealInitvalPos[i]==0){
            std::vector<Operand> indexs=PosToIndexs(thisDimNum,i);//计算索引位置index
            //调用getelementptr,获取该位置指针reg(默认index_type是I32)
            IRgenGetElementptrIndexI32(nowBlock,ty2llvmty[type],++maxReg,GetNewRegOperand(varReg),DimAttr.dims,indexs);
            if(type==Type::INT){
                IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,new ImmI32Operand(0),GetNewRegOperand(maxReg));
            }else if(type==Type::FLOAT){
                IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,new ImmF32Operand(0),GetNewRegOperand(maxReg));
            }
        }
    }
    return;
}

//无初始化值的直接全赋值0
void InitCode_no_init(VarAttribute& DimAttr,LLVMBlock nowBlock,int varReg,Type::ty type){
    int TotalLength=DimAttr.dims[0];//数组总长度
    for(int i=1;i<DimAttr.dims.size();i++){
        TotalLength*=DimAttr.dims[i];
    }
    std::vector<int> thisDimNum{};//每维度数组长度
    thisDimNum.push_back(TotalLength/DimAttr.dims[0]);
    for(int i=1;i<DimAttr.dims.size();i++){
        thisDimNum.push_back(thisDimNum[i-1]/DimAttr.dims[i]);
    }
    if(type==Type::INT){
        for(int i=0;i<TotalLength;i++){
            std::vector<Operand> indexs=PosToIndexs(thisDimNum,i);//计算索引位置index
            //调用getelementptr,获取该位置指针reg(默认index_type是I32)
            IRgenGetElementptrIndexI32(nowBlock,BasicInstruction::LLVMType::I32,++maxReg,GetNewRegOperand(varReg),DimAttr.dims,indexs);
            //向该位置存入0
            IRgenStore(nowBlock,BasicInstruction::LLVMType::I32,new ImmI32Operand(0),GetNewRegOperand(maxReg));
        }
    }else if(type==Type::FLOAT){
        for(int i=0;i<TotalLength;i++){
            std::vector<Operand> indexs=PosToIndexs(thisDimNum,i);//计算索引位置index
            //调用getelementptr,获取该位置指针reg(默认index_type是I32)
            IRgenGetElementptrIndexI32(nowBlock,BasicInstruction::LLVMType::FLOAT32,++maxReg,GetNewRegOperand(varReg),DimAttr.dims,indexs);
            //向该位置存入0
            IRgenStore(nowBlock,BasicInstruction::LLVMType::FLOAT32,new ImmF32Operand(0),GetNewRegOperand(maxReg));
        }
    }
}
    
/*  *************************************** 数组处理结束 ***************************************/

void VarDef_no_init::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    LLVMBlock allocBlock = llvmIR.GetBlock(funcNow,0);
    //处理非数组
    if(!DimsExist()){
        IRgenAlloca(allocBlock,ty2llvmty[type_decl],++maxReg);
        int varReg = maxReg;
        irgen_table.symbol_table.add_Symbol(name,varReg);
        irgen_table.reg_ty_map[varReg]=type_decl; //记录新变量的寄存器对应的类型
        // 没有初始化，默认赋值0
        if(type_decl==Type::FLOAT){
            IRgenArithmeticF32ImmAll(nowBlock,BasicInstruction::LLVMIROpcode::FADD,0,0,++maxReg);
        }
        else{
            IRgenArithmeticI32ImmAll(nowBlock,BasicInstruction::LLVMIROpcode::ADD,0,0,++maxReg);
        }
        IRgenStore(nowBlock,ty2llvmty[type_decl],GetNewRegOperand(maxReg),GetNewRegOperand(varReg)); //存储0到新的变量空间中
    }
    //处理数组
    else{
        VarAttribute DimAttr = attribute.VarAttr;//获取semant_table的symbol_table中存储过的VarAttribute属性
        IRgenAllocaArray(allocBlock,ty2llvmty[type_decl],++maxReg,DimAttr.dims);
        irgen_table.symbol_table.add_Symbol(name,maxReg);//将变量加入到irgen的symbol_table中
        int varReg=maxReg;
        irgen_table.reg_ty_map[maxReg]=type_decl;
        //所有位置初始化为0
        InitCode_no_init( DimAttr,nowBlock,varReg,type_decl);

        //没有初始化，调用@llvm.memset.p0.i32函数，所有位置填充0
        // int length=DimAttr.dims[0];
        // for(int i=1;i<DimAttr.dims.size();i++){
        //     length*=DimAttr.dims[i];
        // }
        //分配内存空间

        // std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args{};
        // // if(type_decl==Type::FLOAT){//float*指针类型转换为I8*
        // //     int srcReg=maxReg;
        // //     IRgenTransF32toI8Ptr(nowBlock,srcReg,++maxReg);
        // // }
        // args.push_back(std::make_pair(ty2llvmty[4],GetNewRegOperand(maxReg)));//起始地址
        // args.push_back(std::make_pair(BasicInstruction::I8,new ImmI8Operand(0)));//初始值：0
        // args.push_back(std::make_pair(BasicInstruction::I32,new ImmI32Operand(length*4)));//空间大小：数组长度*4
        // args.push_back(std::make_pair(BasicInstruction::I1,new ImmI1Operand(false)));//0:可以被优化
        //IRgenCallVoid(nowBlock,BasicInstruction::LLVMType::VOID,args,"llvm.memset.p0.i32");
    }
}

void VarDef::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    LLVMBlock allocBlock = llvmIR.GetBlock(funcNow,0);
    //处理非数组
    if(!DimsExist()){
        IRgenAlloca(allocBlock,ty2llvmty[type_decl],++maxReg);
        int varReg = maxReg;
        irgen_table.symbol_table.add_Symbol(name,varReg);
        irgen_table.reg_ty_map[varReg]=type_decl; //记录新变量的寄存器对应的类型

        // 有初始化
        if(init->is_exp()){
            init->codeIR();
            int initReg=maxReg; /*显式类型转换*/
            if(init->attribute.T.type==Type::INT&&type_decl==Type::FLOAT){
                IRgenSitofp(nowBlock,initReg,++maxReg);
            }else if(init->attribute.T.type==Type::FLOAT&&type_decl==Type::INT){
                IRgenFptosi(nowBlock,initReg,++maxReg);
            }
            IRgenStore(nowBlock,ty2llvmty[type_decl],GetNewRegOperand(maxReg),GetNewRegOperand(varReg)); //存储初始值到新的变量空间中
        }
    }
    //处理数组
    else{
        VarAttribute DimAttr = attribute.VarAttr;//获取semant_table的symbol_table中存储过的VarAttribute属性
        IRgenAllocaArray(allocBlock,ty2llvmty[type_decl],++maxReg,DimAttr.dims);
        irgen_table.symbol_table.add_Symbol(name,maxReg);//将变量加入到irgen的symbol_table中
        int varReg=maxReg;
        irgen_table.reg_ty_map[maxReg]=type_decl;

        /* 初始化:先调用@llvm.memset.p0.i32全部赋0，再对指定初始化的位置写入初始值 */
        //分配内存空间，初始化为0
        // int length=DimAttr.dims[0];
        // for(int i=1;i<DimAttr.dims.size();i++){
        //     length*=DimAttr.dims[i];
        // }
        // std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args{};
        // if(type_decl==Type::FLOAT){//float*指针类型转换为I8*
        //     int srcReg=maxReg;
        //     IRgenZextI1toI32(nowBlock,srcReg,++maxReg,BasicInstruction::LLVMIROpcode::BITCAST);
        // }
        // args.push_back(std::make_pair(ty2llvmty[4],GetNewRegOperand(maxReg)));//起始地址
        // args.push_back(std::make_pair(BasicInstruction::I8,new ImmI8Operand(0)));//初始值：0
        // args.push_back(std::make_pair(BasicInstruction::I32,new ImmI32Operand(length*4)));//空间大小：数组长度*4
        // args.push_back(std::make_pair(BasicInstruction::I1,new ImmI1Operand(false)));//0:可以被优化
        // IRgenCallVoid(nowBlock,BasicInstruction::LLVMType::VOID,args,"llvm.memset.p0.i32");
        //显式初始化值
        InitCodeGen(DimAttr,init,varReg,nowBlock,type_decl);
    }
}

void ConstDef::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow,nowLabel);
    LLVMBlock allocBlock = llvmIR.GetBlock(funcNow,0);
    //处理非数组
    if(!DimsExist()){
        IRgenAlloca(allocBlock,ty2llvmty[type_decl],++maxReg);
        int varReg = maxReg;
        irgen_table.symbol_table.add_Symbol(name,varReg);
        irgen_table.reg_ty_map[varReg]=type_decl; //记录新变量的寄存器对应的类型
        // // 没有初始化，默认赋值0
        // if(init==nullptr){
        //     if(type_decl==Type::FLOAT){
        //         IRgenArithmeticF32ImmAll(nowBlock,BasicInstruction::LLVMIROpcode::FADD,0,0,++maxReg);
        //     }
        //     else{
        //         IRgenArithmeticI32ImmAll(nowBlock,BasicInstruction::LLVMIROpcode::ADD,0,0,++maxReg);
        //     }
        //     IRgenStore(nowBlock,ty2llvmty[type_decl],GetNewRegOperand(maxReg),GetNewRegOperand(varReg)); //存储0到新的变量空间中
        // }
        // // 有初始化
        // else{
            // 是表达式
            if(init->is_exp()){
                init->codeIR();
                int initReg=maxReg; /*显式类型转换*/
                if(init->attribute.T.type==Type::INT&&type_decl==Type::FLOAT){
                    IRgenSitofp(nowBlock,initReg,++maxReg);
                }else if(init->attribute.T.type==Type::FLOAT&&type_decl==Type::INT){
                    IRgenFptosi(nowBlock,initReg,++maxReg);
                }
                IRgenStore(nowBlock,ty2llvmty[type_decl],GetNewRegOperand(maxReg),GetNewRegOperand(varReg)); //存储初始值到新的变量空间中
            }
        //}
    }
    //处理数组
    else{
        VarAttribute DimAttr = attribute.VarAttr;//获取semant_table的symbol_table中存储过的VarAttribute属性
        IRgenAllocaArray(allocBlock,ty2llvmty[type_decl],++maxReg,DimAttr.dims);
        irgen_table.symbol_table.add_Symbol(name,maxReg);//将变量加入到irgen的symbol_table中
        int varReg=maxReg;
        irgen_table.reg_ty_map[maxReg]=type_decl;

        /* 初始化:先调用@llvm.memset.p0.i32全部赋0，再对指定初始化的位置写入初始值 */
        // //分配内存空间，初始化为0
        // int length=DimAttr.dims[0];
        // for(int i=1;i<DimAttr.dims.size();i++){
        //     length*=DimAttr.dims[i];
        // }
        // std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args{};
        // args.push_back(std::make_pair(ty2llvmty[4],GetNewRegOperand(maxReg)));//起始地址
        // args.push_back(std::make_pair(BasicInstruction::I8,new ImmI8Operand(0)));//初始值：0
        // args.push_back(std::make_pair(BasicInstruction::I32,new ImmI32Operand(length*4)));//空间大小：数组长度*4
        // args.push_back(std::make_pair(BasicInstruction::I1,new ImmI1Operand(false)));//0:可以被优化
        // IRgenCallVoid(nowBlock,BasicInstruction::LLVMType::VOID,args,"llvm.memset.p0.i32");
        //显式初始化值
        InitCodeGen(DimAttr,init,varReg,nowBlock,type_decl);
    }
}

void VarDecl::codeIR() {
    LLVMBlock funcBlock = llvmIR.GetBlock(funcNow, nowLabel);
    for(int i=0; i<var_def_list->size();i++){
        (*var_def_list)[i]->type_decl = type_decl;
        (*var_def_list)[i]->codeIR();
    }
}

void ConstDecl::codeIR() {
    LLVMBlock funcBlock = llvmIR.GetBlock(funcNow, nowLabel);
    for(int i=0; i<var_def_list->size();i++){
        (*var_def_list)[i]->type_decl = type_decl;
        (*var_def_list)[i]->codeIR();
    }
}

void BlockItem_Decl::codeIR() {
    decl->codeIR();
}

void BlockItem_Stmt::codeIR() {
    stmt->codeIR();
}

void __Block::codeIR() {
    irgen_table.symbol_table.enter_scope(); //进入作用域
    for(int i=0; i<item_list->size(); i++){
        (*item_list)[i]->codeIR();
    }
    irgen_table.symbol_table.exit_scope(); //离开作用域
}

void __FuncFParam::codeIR() {
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow, nowLabel);
    LLVMBlock allocBlock = llvmIR.GetBlock(funcNow,0);
    //参数不为数组
    if(dims == nullptr){
        funcNow->InsertFormal(ty2llvmty[type_decl]);
        IRgenAlloca(allocBlock,ty2llvmty[type_decl],++maxReg); //为形参分配新的空间
        IRgenStore(nowBlock,ty2llvmty[type_decl],GetNewRegOperand(funcPaLabel),GetNewRegOperand(maxReg)); //存储形参到新的空间中
        irgen_table.symbol_table.add_Symbol(name,maxReg); //将参数名和其对应的寄存器记录到table中
        irgen_table.reg_ty_map[maxReg]=type_decl; //记录函数参数的寄存器对应的类型
    }
    //参数是数组
    else{
        int varReg=funcNow->formals.size();//当前形参的寄存器编号=其再formals向量中的索引
        funcNow->InsertFormal(BasicInstruction::LLVMType::PTR);
        irgen_table.symbol_table.add_Symbol(name,varReg);//添加到symbol_table中:VarAttribute
        irgen_table.reg_ty_map[varReg]=type_decl;
        VarAttribute val;
        for(int i=1;i<dims->size();i++){
            val.dims.push_back((*dims)[i]->attribute.V.val.IntVal);
        }
        val.type=type_decl;
        irgen_table.symbol_table.add_Array_Val(name,val);//添加到symbol_table中：VarAttribute
    }
}

void __FuncDef::codeIR() {
    irgen_table.symbol_table.enter_scope(); //进入作用域
    irgen_table.reg_ty_map.clear();

    //处理函数返回值和函数名
    funcRetType = ty2llvmty[return_type];
    FuncDefInstruction funcDefInst =  new FunctionDefineInstruction(funcRetType, name->get_string());
    funcNow = funcDefInst;
    llvmIR.NewFunction(funcDefInst);
    //新增一个块，入口块的编号一定为0
    nowLabel = -1;
    LLVMBlock funcBlock = llvmIR.NewBlock(funcDefInst, ++nowLabel);
    //funcBlock->comment = "Func的入口块";

    //处理函数参数
    maxReg = formals->size()-1;
    for(int i=0; i<formals->size(); i++){
        FuncFParam formal = (*formals)[i];
        funcPaLabel = i;
        formal->codeIR();
    }
    // 为了方便检查函数块内的指令，重新开辟一个块
    // LLVMBlock newBlock = llvmIR.NewBlock(funcDefInst, ++nowLabel);
    // newBlock->comment = "Func的主体块";

    block->codeIR();

    // 解决返回值为void但缺少return的情况
    LLVMBlock nowBlock = llvmIR.GetBlock(funcNow, nowLabel);
    if(nowBlock->Instruction_list.empty()){
        if(return_type==Type::ty::VOID){
            IRgenRetVoid(nowBlock);
        }
        else if(return_type==Type::ty::INT){
            IRgenRetImmInt(nowBlock,funcRetType,0);
        }else{
            IRgenRetImmFloat(nowBlock,funcRetType,0);
        }
    }
    else{
        int endOpcode = nowBlock->Instruction_list.back()->GetOpcode();
        if(endOpcode!=BasicInstruction::RET){
            if(return_type==Type::ty::VOID){
                IRgenRetVoid(nowBlock);
            }
            else if(return_type==Type::ty::INT){
                IRgenRetImmInt(nowBlock,funcRetType,0);
            }else{
                IRgenRetImmFloat(nowBlock,funcRetType,0);
            }
        }
    }

    funcNow->maxReg = maxReg;
    irgen_table.symbol_table.exit_scope(); //离开作用域
}

void CompUnit_Decl::codeIR() {
    auto def_vector = decl->get_def();
    std::map<Symbol, VarAttribute> globalVarMap = semant_table.GlobalTable;

    for(int i=0; i<def_vector.size(); i++){
        GlobalVarDefineInstruction *globalVarDefInst;
        Symbol def_name=def_vector[i]->get_name();
        VarAttribute VarAttr=globalVarMap.find(def_name)->second;
        // 处理数组
        if(def_vector[i]->DimsExist()){
            globalVarDefInst = new GlobalVarDefineInstruction(def_name->get_string(),
            ty2llvmty[decl->get_type()],VarAttr);//无论有无初始化值，都传递VarAttribute
        }
        // 处理非数组
        else{
            // 没有初始化
            if(def_vector[i]->has_initval==0){
                globalVarDefInst = new GlobalVarDefineInstruction(def_name->get_string(),
                ty2llvmty[decl->get_type()],nullptr);
            }
            // 有初始化
            else{
                Operand i_val;
                if(decl->get_type()==Type::ty::FLOAT){
                    i_val = new ImmF32Operand(VarAttr.FloatInitVals[0]);//从全局符号表中取信息，而非从节点本身（若发生隐式转换，节点本身并未被转换）
                    //i_val = new ImmF32Operand(def_vector[i]->get_init()->attribute.V.val.FloatVal);
                }
                else{
                    i_val = new ImmI32Operand(VarAttr.IntInitVals[0]);
                    //i_val = new ImmI32Operand(def_vector[i]->get_init()->attribute.V.val.IntVal);
                }
                globalVarDefInst = new GlobalVarDefineInstruction(def_vector[i]->get_name()->get_string(),
                ty2llvmty[decl->get_type()],i_val);
            }
        }
        llvmIR.global_def.push_back(globalVarDefInst);
    }

    /*
    // 一次性实现所有全局变量声明
    if(globalDefFlag)
        return;
    globalDefFlag = true;

    // 全局变量声明
    std::map<Symbol, VarAttribute> globalVarMap = semant_table.GlobalTable;
    for(auto iter=globalVarMap.begin(); iter!=globalVarMap.end(); iter++){
        VarAttribute var = iter->second;
        GlobalVarDefineInstruction *globalVarDefInst;

        // 非数组
        if(var.dims.size()==0){
            globalVarDefInst = new GlobalVarDefineInstruction(iter->first->get_string(),ty2llvmty[var.type],var);
        }
        // 数组
        else{
            TODO("GlobalVar array");
        }
        llvmIR.global_def.push_back(globalVarDefInst);
    }
    */
    
}

void CompUnit_FuncDef::codeIR() { func_def->codeIR(); }

void AddLibFunctionDeclare() {
    FunctionDeclareInstruction *getint = new FunctionDeclareInstruction(BasicInstruction::I32, "getint");
    llvmIR.function_declare.push_back(getint);

    FunctionDeclareInstruction *getchar = new FunctionDeclareInstruction(BasicInstruction::I32, "getch");
    llvmIR.function_declare.push_back(getchar);

    FunctionDeclareInstruction *getfloat = new FunctionDeclareInstruction(BasicInstruction::FLOAT32, "getfloat");
    llvmIR.function_declare.push_back(getfloat);

    FunctionDeclareInstruction *getarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getarray");
    getarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getarray);

    FunctionDeclareInstruction *getfloatarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getfarray");
    getfloatarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getfloatarray);

    FunctionDeclareInstruction *putint = new FunctionDeclareInstruction(BasicInstruction::VOID, "putint");
    putint->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putint);

    FunctionDeclareInstruction *putch = new FunctionDeclareInstruction(BasicInstruction::VOID, "putch");
    putch->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putch);

    FunctionDeclareInstruction *putfloat = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfloat");
    putfloat->InsertFormal(BasicInstruction::FLOAT32);
    llvmIR.function_declare.push_back(putfloat);

    FunctionDeclareInstruction *putarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putarray");
    putarray->InsertFormal(BasicInstruction::I32);
    putarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putarray);

    FunctionDeclareInstruction *putfarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfarray");
    putfarray->InsertFormal(BasicInstruction::I32);
    putfarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putfarray);

    FunctionDeclareInstruction *starttime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_starttime");
    starttime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(starttime);

    FunctionDeclareInstruction *stoptime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_stoptime");
    stoptime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(stoptime);

    // 一些llvm自带的函数，也许会为你的优化提供帮助
    FunctionDeclareInstruction *llvm_memset =
    new FunctionDeclareInstruction(BasicInstruction::VOID, "llvm.memset.p0.i32");
    llvm_memset->InsertFormal(BasicInstruction::PTR);
    llvm_memset->InsertFormal(BasicInstruction::I8);
    llvm_memset->InsertFormal(BasicInstruction::I32);
    llvm_memset->InsertFormal(BasicInstruction::I1);
    llvmIR.function_declare.push_back(llvm_memset);

    FunctionDeclareInstruction *llvm_umax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umax.i32");
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umax);

    FunctionDeclareInstruction *llvm_umin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umin.i32");
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umin);

    FunctionDeclareInstruction *llvm_smax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smax.i32");
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smax);

    FunctionDeclareInstruction *llvm_smin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smin.i32");
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smin);
}
