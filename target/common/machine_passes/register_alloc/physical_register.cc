#include "physical_register.h"
//分配一个区间
bool PhysicalRegistersAllocTools::OccupyReg(int phy_id, LiveInterval interval) {
    phy_occupied[phy_id].push_back(interval);
    return true;
}

bool PhysicalRegistersAllocTools::ReleaseReg(int phy_id, LiveInterval interval) { 
    if(!phy_occupied[phy_id].empty()){
        for(auto it=phy_occupied[phy_id].begin();it!=phy_occupied[phy_id].end();it++){
            if(interval==*it){
                phy_occupied[phy_id].erase(it);
                return true;
            }
        }
    }
    return false;
}

/*
        ref: https://github.com/yuhuifishash/SysY
        参考了OccupyMem和ReleaseMem对内存分配的处理
    */

bool PhysicalRegistersAllocTools::OccupyMem(int offset, LiveInterval interval,int length) {
    length /= 4;
    for (int i = offset; i < offset + length; i++) {
        while (i >= mem_occupied.size()) {
            mem_occupied.push_back({});
        }
        mem_occupied[i].push_back(interval);
    }
    return true;
}
bool PhysicalRegistersAllocTools::ReleaseMem(int offset, LiveInterval interval ,int length) {

    length /= 4;
    for (int i = offset; i < offset + length; i++) {
        auto iter = mem_occupied[i].begin();
        while(iter != mem_occupied[i].end()){
            if (*iter == interval) {
                mem_occupied[i].erase(iter);
                break;
            }
            iter++;
        }
    }
    return true;
}

int PhysicalRegistersAllocTools::getIdleReg(LiveInterval interval) {
    for (auto i : getValidRegs(interval)) {//遍历所有有效寄存器
        bool conflict = false;
        for (auto interval_j : phy_occupied[i]) {
            if (interval & interval_j) { 
                conflict = true;
                break;
            } 
        }
        
        if (!conflict) {//所有interval都不冲突，就选择
            return i;  
        }
    }
    return -1;
}
int PhysicalRegistersAllocTools::getIdleMem(LiveInterval interval) { 
    //初始置为 均可分配
    std::vector<bool> flags;
    for (int i = 0; i < mem_occupied.size(); ++i) {
        flags.push_back(true);
    }
    for (int i = 0; i < mem_occupied.size(); ++i) {
        for (auto interval_j : mem_occupied[i]) {
            if (interval & interval_j) {//存在冲突的，置为false
                flags[i] = false;
                break;
            }
        }
    }
    //寻找连续的空闲区
    int free_num = 0;
    for (int offset = 0; offset < mem_occupied.size(); offset++) {
        if (flags[offset]) {
            free_num++;
        } else {
            free_num = 0;
        }
        if (free_num == interval.getReg().getDataWidth() / 8) {
            return offset - free_num + 1;
        }
    }
    //没有，则另外开辟空间
    return mem_occupied.size() - free_num;
}

int PhysicalRegistersAllocTools::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                              LiveInterval interval2) {
    ReleaseReg(p_reg1, interval1);
    ReleaseMem(offset_spill2,interval2,size);
    OccupyReg(p_reg1, interval2);
    return 0;
}

std::vector<LiveInterval> PhysicalRegistersAllocTools::getConflictIntervals(LiveInterval interval) {
    std::vector<LiveInterval> result;
    for (auto phy_intervals : phy_occupied) {
        for (auto other_interval : phy_intervals) {
            if (interval.getReg().type == other_interval.getReg().type && (interval & other_interval)) {
                result.push_back(other_interval);
            }
        }
    }
    return result;
}
