#include "../../machine_instruction_structures/machine.h"
#include "liveinterval.h"

// 为了实现方便，这里直接使用set进行活跃变量分析，如果你不满意，可以自行更换更高效的数据结构(例如bitset)
//交集
template <class T> std::set<T> SetIntersect(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret;
    for (auto x : b) {
        if (a.count(x) != 0) {
            ret.insert(x);
        }
    }
    return ret;
}

// 并集
template <class T> std::set<T> SetUnion(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.insert(x);
    }
    return ret;
}

// a-b
template <class T> std::set<T> SetDiff(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.erase(x);
    }
    return ret;
}

/* phi指令中，获取读、写寄存器 */
std::vector<Register *> MachinePhiInstruction::GetReadReg() {
    std::vector<Register *> ret;
    for (auto [label, op] : phi_list) {
        if (op->op_type == MachineBaseOperand::REG) {
            ret.push_back(&(((MachineRegister *)op)->reg));
        }
    }
    return ret;
}
std::vector<Register *> MachinePhiInstruction::GetWriteReg() { return std::vector<Register *>({&result}); }

/**
 * 遍历每个基本块，为每个块计算DEF[B]和USE[B]
 */
void Liveness::UpdateDefUse() {

    DEF.clear();
    USE.clear();

    auto mcfg = current_func->getMachineCFG();
    // 顺序遍历每个基本块
    auto seq_it = mcfg->getSeqScanIterator();
    seq_it->open();
    while (seq_it->hasNext()) {
        auto node = seq_it->next();

        // DEF[B]: 在基本块B中定义，并且定义前在B中没有被使用的变量集合
        // USE[B]: 在基本块B中使用，并且使用前在B中没有被定义的变量集合
        DEF[node->Mblock->getLabelId()].clear();
        USE[node->Mblock->getLabelId()].clear();

        auto &cur_def = DEF[node->Mblock->getLabelId()];
        auto &cur_use = USE[node->Mblock->getLabelId()];

        for (auto ins : *(node->Mblock)) {
            for (auto reg_r : ins->GetReadReg()) {
                if (cur_def.find(*reg_r) == cur_def.end()) {
                    cur_use.insert(*reg_r);
                }
            }
            for (auto reg_w : ins->GetWriteReg()) {
                if (cur_use.find(*reg_w) == cur_use.end()) {
                    cur_def.insert(*reg_w);
                }
            }
        }
    }
}

void Liveness::Execute() {
    UpdateDefUse();

    OUT.clear();
    IN.clear();

    auto mcfg = current_func->getMachineCFG();
    bool changed = 1;
    // 基于数据流分析的活跃变量分析
    while (changed) {
        changed = 0;
        // 顺序遍历每个基本块
        auto seq_it = mcfg->getSeqScanIterator();
        seq_it->open();
        while (seq_it->hasNext()) {
            auto node = seq_it->next();
            std::set<Register> out;
            int cur_id = node->Mblock->getLabelId();
            //块的out=所有后继块in的并集
            for (auto succ : mcfg->GetSuccessorsByBlockId(cur_id)) {
                out = SetUnion<Register>(out, IN[succ->Mblock->getLabelId()]);
            }
            if (out != OUT[cur_id]) {
                OUT[cur_id] = out;
            }
            //IN[B] = USE[B] ∪ (OUT[B] − DEF[B])
            std::set<Register> in = SetUnion<Register>(USE[cur_id], SetDiff<Register>(OUT[cur_id], DEF[cur_id]));
            if (in != IN[cur_id]) {
                changed = 1;//如果某块的 IN 或 OUT 被更新，置 changed = true，下一轮还要再做
                IN[cur_id] = in;
            }
        }
    }
}
