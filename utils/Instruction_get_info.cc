#include "../include/Instruction.h"
#include "../include/basic_block.h"

// 下面是获取指令结果寄存器的函数定义
int LoadInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int StoreInstruction::GetDefRegno(){
    if(pointer->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)pointer)->GetRegNo();
    }
    return -1;
}

int ArithmeticInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int IcmpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int FcmpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int PhiInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int AllocaInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}


int BrCondInstruction::GetDefRegno(){
    return -1;
}


int BrUncondInstruction::GetDefRegno(){
    return -1;
}


int GlobalVarDefineInstruction::GetDefRegno(){
    return -1;
}


int GlobalStringConstInstruction::GetDefRegno(){
    return -1;
}


int CallInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int RetInstruction::GetDefRegno(){
    return -1;
}

int GetElementptrInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int FunctionDefineInstruction::GetDefRegno(){
    return -1;
}

int FunctionDeclareInstruction::GetDefRegno(){
    return -1;
}

int FptosiInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int SitofpInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}

int ZextInstruction::GetDefRegno(){
    if(result->GetOperandType()==BasicOperand::REG){
        return ((RegOperand*)result)->GetRegNo();
    }
    return -1;
}







std::set<int> LoadInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(pointer->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)pointer)->GetRegNo());
    }
    return regno_set;
}

std::set<int> StoreInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    if(pointer->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)pointer)->GetRegNo());
    }
    return regno_set;
}

std::set<int> ArithmeticInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> IcmpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> FcmpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(op1->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op1)->GetRegNo());
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)op2)->GetRegNo());
    }
    return regno_set;
}

std::set<int> PhiInstruction::GetUseRegno(){
    std::set<int> regno_set;
    for(auto& phi_pair: phi_list){
        if(phi_pair.second->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)phi_pair.second)->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> AllocaInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> BrCondInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(cond->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)cond)->GetRegNo());
    }
    return regno_set;
}

std::set<int> BrUncondInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> GlobalVarDefineInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> GlobalStringConstInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> CallInstruction::GetUseRegno(){
    std::set<int> regno_set;
    for(int i=0; i<args.size(); i++){
        if(args[i].second->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)args[i].second)->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> RetInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(ret_val==nullptr)
        return regno_set;
    if(ret_val->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)ret_val)->GetRegNo());
    }
    return regno_set;
}

std::set<int> GetElementptrInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(ptrval->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)ptrval)->GetRegNo());
    }
    for(int i=0; i<indexes.size(); i++){
        if(indexes[i]->GetOperandType()==BasicOperand::REG){
            regno_set.insert(((RegOperand*)indexes[i])->GetRegNo());
        }
    }
    return regno_set;
}

std::set<int> FunctionDefineInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> FunctionDeclareInstruction::GetUseRegno(){
    std::set<int> regno_set;
    return regno_set;
}

std::set<int> FptosiInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
}

std::set<int> SitofpInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
}

std::set<int> ZextInstruction::GetUseRegno(){
    std::set<int> regno_set;
    if(value->GetOperandType()==BasicOperand::REG){
        regno_set.insert(((RegOperand*)value)->GetRegNo());
    }
    return regno_set;
}

