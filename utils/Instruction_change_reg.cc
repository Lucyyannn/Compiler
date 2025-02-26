#include "../include/Instruction.h"
#include "../include/basic_block.h"

void LoadInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(pointer->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)pointer)->GetRegNo();
        if(store_map.find(regno)!=store_map.end()){
            int new_regno = store_map.at(regno);
            pointer = GetNewRegOperand(new_regno);
        }
    }
}

void StoreInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void ArithmeticInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void IcmpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void FcmpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(op1->GetOperandType()==BasicOperand::REG){
        int regno1 = ((RegOperand*)op1)->GetRegNo();
        if(use_map.find(regno1)!=use_map.end()){
            int new_regno1 = use_map.at(regno1);
            op1 = GetNewRegOperand(new_regno1);
        }
    }
    if(op2->GetOperandType()==BasicOperand::REG){
        int regno2 = ((RegOperand*)op2)->GetRegNo();
        if(use_map.find(regno2)!=use_map.end()){
            int new_regno2 = use_map.at(regno2);
            op2 = GetNewRegOperand(new_regno2);
        }
    }
}

void PhiInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    
}

void AllocaInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void BrCondInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(cond->GetOperandType()==BasicOperand::REG){    
        int regno = ((RegOperand*)cond)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            cond = GetNewRegOperand(new_regno);
        }
    }
}

void BrUncondInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void GlobalVarDefineInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void GlobalStringConstInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void CallInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    for(auto &arg: args){
        if(arg.second->GetOperandType()==BasicOperand::REG){
            int regno = ((RegOperand*)arg.second)->GetRegNo();
            if(use_map.find(regno)!=use_map.end()){
                int new_regno = use_map.at(regno);
                arg.second = GetNewRegOperand(new_regno);
            }
        }
    }
}

void RetInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(ret_type==BasicInstruction::LLVMType::VOID) return;
    if(ret_val->GetOperandType()==BasicOperand::REG){        
        int regno = ((RegOperand*)ret_val)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            ret_val = GetNewRegOperand(new_regno);
        }
    }
}

void GetElementptrInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    for(int i=0; i<indexes.size(); i++){
        if(indexes[i]->GetOperandType()==BasicOperand::REG){
            int regno = ((RegOperand*)indexes[i])->GetRegNo();
            if(use_map.find(regno)!=use_map.end()){
                int new_regno = use_map.at(regno);
                indexes[i] = GetNewRegOperand(new_regno);
            }
        }
    }
}

void FunctionDefineInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void FunctionDeclareInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){}

void FptosiInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void SitofpInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}

void ZextInstruction::ChangeReg(const std::map<int, int> &store_map, const std::map<int, int> &use_map){
    if(value->GetOperandType()==BasicOperand::REG){
        int regno = ((RegOperand*)value)->GetRegNo();
        if(use_map.find(regno)!=use_map.end()){
            int new_regno = use_map.at(regno);
            value = GetNewRegOperand(new_regno);
        }
    }
}