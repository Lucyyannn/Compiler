#ifndef LivEInterval_H
#define LivEInterval_H
#include "../machine_pass.h"
#include <assert.h>
#include <queue>
class LiveInterval {
private:
    Register reg;
    // 当begin和end不同时, 活跃区间为[begin,end), 即左闭右开
    // 当begin和end相同时, 表示[begin,end], 即一个单点 (这么做的原因是方便活跃区间计算)
    // 注意特殊判断begin和end相同时的情况
    struct LiveSegment {
        int begin;
        int end;
        //判断一条指令是否在活跃区间范围内
        bool inside(int pos) const {
            if (begin == end) return begin == pos;
            return begin <= pos && pos < end; 
        }
        bool operator&(const struct LiveSegment &that) const {
            return this->inside(that.begin) || this->inside(that.end - 1 > that.begin ? that.end - 1 : that.begin) ||
                   that.inside(this->begin) || that.inside(this->end - 1 > this->begin ? this->end - 1 : this->begin);
        }
        bool operator==(const struct LiveSegment &that) const {
            return this->begin == that.begin && this->end == that.end;
        }
    };
    std::list<LiveSegment> segments{};
    int reference_count;

public:
    // 检测两个活跃区间是否重叠
    // 保证两个活跃区间各个段各自都是不降序（升序）排列的
    bool operator&(const LiveInterval &that) const {
        //【1】有空的，不重叠
        if(this->segments.empty()||that.segments.empty()){
            return false;
        }
        //【2】逐个遍历
        auto thisptr=this->segments.begin();
        auto thatptr=that.segments.begin();
        while(thisptr!=this->segments.end()&&thatptr!=that.segments.end()){
            if(thisptr->begin==thatptr->begin){
                return true;
            }
            if(thisptr->begin<thatptr->begin){
                if(thisptr->inside(thatptr->begin)){
                    return true;
                }else{
                    std::advance(thisptr,1);
                }
            }else{
                if(thatptr->inside(thisptr->begin)){
                    return true;
                }else{
                    std::advance(thatptr,1);
                }
            }
        }
        return false;
    }
    //判断两个活跃区间是否相等
    bool operator==(const LiveInterval &that) const{
        if(/*!this->segments.empty()&&!that.segments.empty()&&*/this->segments.size()==that.segments.size()){
            auto thisptr=this->segments.begin();
            auto thatptr=that.segments.begin();
            while(thisptr!=this->segments.end()){
                if(!(*thisptr==*thatptr)){
                    return false;
                }
                thisptr++;
                thatptr++;
            }
            return true;
        }
        return false;
    }
    //获取整个interval的begin和end
    int getMostBegin(){
        if(!segments.empty()){
            return segments.begin()->begin;
        }else{
            return -1;
        }
    }
    int getMostEnd(){
        if(!segments.empty()){
            return segments.back().end;
        }else{
            return -1;
        }
    }

    // 更新引用计数
    void IncreaseReferenceCount(int count) { reference_count += count; }
    int getReferenceCount() { return reference_count; }
    // 返回活跃区间长度
    int getIntervalLen() {
        int ret = 0;
        for (auto seg : segments) {
            ret += (seg.end - seg.begin + 1);
        }
        return ret;
    }
    Register getReg() { return reg; }
    LiveInterval() : reference_count(0) {}    // Temp
    LiveInterval(Register reg) : reg(reg), reference_count(0) {}

    void PushFront(int begin, int end) { segments.push_front({begin = begin, end = end}); }
    void SetMostBegin(int begin) { segments.begin()->begin = begin; }

    // 可以直接 for(auto segment : liveinterval)
    decltype(segments.begin()) begin() { return segments.begin(); }
    decltype(segments.end()) end() { return segments.end(); }
};

class Liveness {
private:
    MachineFunction *current_func;
    // 更新所有块DEF和USE集合
    void UpdateDefUse();
    // Key: Block_Number
    // 存储活跃变量分析的结果
    std::map<int, std::set<Register>> IN{}, OUT{}, DEF{}, USE{};

public:
    // 对所有块进行活跃变量分析并保存结果
    void Execute();
    Liveness(MachineFunction *mfun, bool calculate = true) : current_func(mfun) {
        if (calculate) {
            Execute();
        }
    }
    // 获取基本块的IN/OUT/DEF/USE集合
    std::set<Register> GetIN(int bid) { return IN[bid]; }
    std::set<Register> GetOUT(int bid) { return OUT[bid]; }
    std::set<Register> GetDef(int bid) { return DEF[bid]; }
    std::set<Register> GetUse(int bid) { return USE[bid]; }
};
#endif