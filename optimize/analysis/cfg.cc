#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

int dfs_num = 0;

void LLVMIR::CFGInit() {
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;

        // 初始化邻接表大小
        int block_num = bb_map.size();
        cfg->G.resize(block_num);
        cfg->invG.resize(block_num);

        cfg->BuildCFG();
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::SearchB(LLVMBlock B){
    if(B->dfs_id!=0)return;
    B->dfs_id = ++dfs_num;

    for(auto it = B->Instruction_list.begin(); it != B->Instruction_list.end(); ++it){
        auto &intr = *it;
        if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_UNCOND){
            Operand operand = ((BrUncondInstruction*)intr)->GetDestLabel();
            int next_label = ((LabelOperand*)operand)->GetLabelNo();
            LLVMBlock next_block = (*block_map)[next_label];

            G[B->block_id].push_back(next_block);
            invG[next_label].push_back(B);
            next_block->comment += ("L" + std::to_string(B->block_id) + ", ");
            
            SearchB(next_block);

            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
        else if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::BR_COND){
            Operand operand1 = ((BrCondInstruction*)intr)->GetTrueLabel();
            Operand operand2 = ((BrCondInstruction*)intr)->GetFalseLabel();
            int true_label = ((LabelOperand*)operand1)->GetLabelNo();
            int false_label = ((LabelOperand*)operand2)->GetLabelNo();
            LLVMBlock true_block = (*block_map)[true_label];
            LLVMBlock false_block = (*block_map)[false_label];

            G[B->block_id].push_back(true_block);
            invG[true_label].push_back(B);
            true_block->comment += ("L" + std::to_string(B->block_id) + ", ");

            G[B->block_id].push_back(false_block);
            invG[false_label].push_back(B);
            false_block->comment += ("L" + std::to_string(B->block_id) + ", ");

            SearchB(true_block);
            SearchB(false_block);

            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
        else if(intr->GetOpcode()==BasicInstruction::LLVMIROpcode::RET){
            if(std::next(it) != B->Instruction_list.end())
                B->Instruction_list.erase(std::next(it), B->Instruction_list.end());
            return;
        }
    }
}

void CFG::BuildCFG() {
    // 深度优先遍历块内的所有指令，遇到跳转指令记录前置块和后缀块
    // 如果跳转/返回指令之后还有指令，全部删除
    // SearchB()进行递归调用
    LLVMBlock start_block = (*block_map)[0];
    SearchB(start_block);
}

std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }