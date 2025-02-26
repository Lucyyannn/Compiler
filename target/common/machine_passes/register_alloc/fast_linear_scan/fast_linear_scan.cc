#include "fast_linear_scan.h"
bool IntervalsPrioCmp(LiveInterval a, LiveInterval b) { 
    return a.begin()->begin > b.begin()->begin; 
}

FastLinearScan::FastLinearScan(MachineUnit *unit, PhysicalRegistersAllocTools *phy, SpillCodeGen *spiller)
    : RegisterAllocation(unit, phy, spiller), unalloc_queue(IntervalsPrioCmp){}

bool FastLinearScan::DoAllocInCurrentFunc() {
    bool spilled = false;
    auto mfun = current_func;
    PRINT("FastLinearScan: %s", mfun->getFunctionName().c_str());
    // std::cerr<<"FastLinearScan: "<<mfun->getFunctionName()<<"\n";
    phy_regs_tools->clear();
    for (auto interval : intervals) {
        Assert(interval.first == interval.second.getReg());
        if (interval.first.is_virtual) {
            // 需要分配的虚拟寄存器
            unalloc_queue.push(interval.second);
        } else {
            // Log("Pre Occupy Physical Reg %d",interval.first.reg_no);
            // 预先占用已经存在的物理寄存器
            phy_regs_tools->OccupyReg(interval.first.reg_no, interval.second);
        }
    }
    // TODO: 进行线性扫描寄存器分配, 为每个虚拟寄存器选择合适的物理寄存器或者将其溢出到合适的栈地址中
    // 该函数中只需正确设置alloc_result，并不需要实际生成溢出代码
    while(!unalloc_queue.empty()){
        auto this_interval=unalloc_queue.top();

        //【1】优先找ilde reg
        int idle_regno=phy_regs_tools->getIdleReg(this_interval);
        if(idle_regno!=-1){
            phy_regs_tools->OccupyReg(idle_regno,this_interval);//(1)标记phy_reg占用情况
            // AllocResult result;
            // result.in_mem=false;
            // result.phy_reg_no=idle_regno;
            // alloc_result[mfun][this_interval.getReg()]=result;//(2)记录alloc_result
            AllocPhyReg(mfun,this_interval.getReg(),idle_regno);
        }
        //【2】全冲突，只能spill
        else{
            /*
                ref: https://github.com/yuhuifishash/SysY
                下面参考了使用weight作为spill选择依据的思想
            */
            //【2.1】先把自己spill了
            spilled=true;
            int mem_offset = phy_regs_tools->getIdleMem(this_interval);
            phy_regs_tools->OccupyMem(mem_offset, this_interval,this_interval.getReg().getDataWidth());
            // AllocResult result;
            // result.in_mem=true;
            // result.stack_offset=mem_offset;
            // alloc_result[mfun][this_interval.getReg()]=result;//记录alloc_result
            AllocStack(mfun,this_interval.getReg(),mem_offset);
            
            //【2.2】在所有冲突的reg里，寻找weight最小的替换掉
            double spill_weight = CalculateSpillWeight(this_interval);
            LiveInterval spill_interval = this_interval;
            for (auto other : phy_regs_tools->getConflictIntervals(this_interval)) {
                double other_weight = CalculateSpillWeight(other);
                if (spill_weight > other_weight && other.getReg().is_virtual) {
                    spill_weight = other_weight;
                    spill_interval = other;
                }
            }
            //【2.3】若发生替换，交换分配记录
            if (this_interval != spill_interval) {
                phy_regs_tools->swapRegspill(getAllocResultInReg(mfun, spill_interval.getReg()), spill_interval, 
                                             mem_offset,this_interval.getReg().getDataWidth(), this_interval);
                swapAllocResult(mfun, this_interval.getReg(), spill_interval.getReg());//交换分配记录
                int spill_mem = phy_regs_tools->getIdleMem(spill_interval);//被溢出的spill_mem重新存入内存
                phy_regs_tools->OccupyMem(spill_mem,spill_interval,spill_interval.getReg().getDataWidth());
                // AllocResult result;
                // result.in_mem=true;
                // result.stack_offset=spill_mem;
                // alloc_result[mfun][spill_interval.getReg()]=result;
                AllocStack(mfun,spill_interval.getReg(),spill_mem);
            }
        }

        unalloc_queue.pop();
    }

    // 返回是否发生溢出
    return spilled;
}

// 计算溢出权重
double FastLinearScan::CalculateSpillWeight(LiveInterval interval) {
    return (double)interval.getReferenceCount() / interval.getIntervalLen();
}
