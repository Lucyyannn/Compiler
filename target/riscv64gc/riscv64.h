#ifndef RISCV64_H
#define RISCV64_H
#include "../common/MachineBaseInstruction.h"
#include "../common/machine_passes/register_alloc/basic_register_allocation.h"
#include "../common/machine_passes/register_alloc/physical_register.h"

#pragma GCC diagnostic ignored "-Wwritable-strings"
#pragma GCC diagnostic ignored "-Wc99-designator"

// 这里只提供了少量伪指令，如果你需要更多的伪值令，你需要自行添加并编写对应的输出
// 1. 在下面的枚举类型中添加指令名
// 2. 在riscv64.cc文件中正确填写OpTable

enum riscvOpcode{
    RISCV_SLL,        // SLL: 逻辑左移，按指定的立即数或寄存器值将一个寄存器的内容向左移动
    RISCV_SLLI,       // SLLI: 逻辑左移立即数，按立即数将一个寄存器的内容向左移动
    RISCV_SRL,        // SRL: 逻辑右移，按指定的立即数或寄存器值将一个寄存器的内容向右移动
    RISCV_SRLI,       // SRLI: 逻辑右移立即数，按立即数将一个寄存器的内容向右移动
    RISCV_SRA,        // SRA: 算术右移，按指定的立即数或寄存器值将一个寄存器的内容向右移动，保留符号位
    RISCV_SRAI,       // SRAI: 算术右移立即数，按立即数将一个寄存器的内容向右移动，保留符号位
    RISCV_ADD,        // ADD: 加法，将两个寄存器的值相加
    RISCV_ADDI,       // ADDI: 加法立即数，将一个寄存器的值与立即数相加
    RISCV_SUB,        // SUB: 减法，从一个寄存器的值中减去另一个寄存器的值
    RISCV_LUI,        // LUI: 加载上半立即数，将一个立即数加载到寄存器的高位部分
    RISCV_AUIPC,      // AUIPC: 加上立即数到程序计数器，将当前 PC 加上一个立即数并存储到寄存器
    RISCV_XOR,        // XOR: 按位异或，将两个寄存器的值进行按位异或操作
    RISCV_XORI,       // XORI: 按位异或立即数，将一个寄存器的值与立即数进行按位异或操作
    RISCV_OR,         // OR: 按位或，将两个寄存器的值进行按位或操作
    RISCV_ORI,        // ORI: 按位或立即数，将一个寄存器的值与立即数进行按位或操作
    RISCV_AND,        // AND: 按位与，将两个寄存器的值进行按位与操作
    RISCV_ANDI,       // ANDI: 按位与立即数，将一个寄存器的值与立即数进行按位与操作
    RISCV_SLT,        // SLT: 设置小于，如果第一个寄存器的值小于第二个寄存器的值，则设置为1，否则为0
    RISCV_SLTI,       // SLTI: 设置小于立即数，如果寄存器的值小于立即数，则设置为1，否则为0
    RISCV_SLTU,       // SLTU: 设置小于无符号，如果第一个寄存器的无符号值小于第二个寄存器的无符号值，则设置为1，否则为0
    RISCV_SLTIU,      // SLTIU: 设置小于无符号立即数，如果寄存器的无符号值小于立即数，则设置为1，否则为0
    RISCV_BEQ,        // BEQ: 分支等于，如果两个寄存器的值相等，则跳转到指定偏移地址
    RISCV_BNE,        // BNE: 分支不等于，如果两个寄存器的值不等，则跳转到指定偏移地址
    RISCV_BLT,        // BLT: 分支小于，如果第一个寄存器的值小于第二个寄存器的值，则跳转到指定偏移地址
    RISCV_BGE,        // BGE: 分支大于或等于，如果第一个寄存器的值大于或等于第二个寄存器的值，则跳转到指定偏移地址
    RISCV_BLTU,       // BLTU: 分支小于无符号，如果第一个寄存器的无符号值小于第二个寄存器的无符号值，则跳转到指定偏移地址
    RISCV_BGEU,       // BGEU: 分支大于或等于无符号，如果第一个寄存器的无符号值大于或等于第二个寄存器的无符号值，则跳转到指定偏移地址
    RISCV_JAL,        // JAL: 跳转并链接，将当前地址加上偏移量跳转，并将返回地址存储到指定寄存器
    RISCV_JALR,       // JALR: 跳转并链接寄存器，将寄存器值加上偏移量跳转，并将返回地址存储到指定寄存器
    RISCV_LB,         // LB: 加载字节，从内存中加载一个字节到寄存器，并进行符号扩展
    RISCV_LH,         // LH: 加载半字，从内存中加载一个半字（16位）到寄存器，并进行符号扩展
    RISCV_LBU,        // LBU: 加载字节无符号，从内存中加载一个字节到寄存器，不进行符号扩展
    RISCV_LHU,        // LHU: 加载半字无符号，从内存中加载一个半字（16位）到寄存器，不进行符号扩展
    RISCV_LW,         // LW: 加载字，从内存中加载一个字（32位）到寄存器
    RISCV_SB,         // SB: 存储字节，将寄存器的低字节存储到内存
    RISCV_SH,         // SH: 存储半字，将寄存器的低半字（16位）存储到内存
    RISCV_SW,         // SW: 存储字，将寄存器的低字（32位）存储到内存
    RISCV_SLLW,       // SLLW: 逻辑左移字，将寄存器的值按指定的立即数或寄存器值向左移动，适用于 32 位操作
    RISCV_SLLIW,      // SLLIW: 逻辑左移立即数字，将寄存器的值按立即数向左移动，适用于 32 位操作
    RISCV_SRLW,       // SRLW: 逻辑右移字，将寄存器的值按指定的立即数或寄存器值向右移动，适用于 32 位操作
    RISCV_SRLIW,      // SRLIW: 逻辑右移立即数字，将寄存器的值按立即数向右移动，适用于 32 位操作
    RISCV_SRAW,       // SRAW: 算术右移字，将寄存器的值按指定的立即数或寄存器值向右移动，保留符号位，适用于 32 位操作
    RISCV_SRAIW,      // SRAIW: 算术右移立即数字，将寄存器的值按立即数向右移动，保留符号位，适用于 32 位操作
    RISCV_ADDW,       // ADDW: 加法字，将两个寄存器的值相加，适用于 32 位操作
    RISCV_ADDIW,      // ADDIW: 加法立即数字，将一个寄存器的值与立即数相加，适用于 32 位操作
    RISCV_SUBW,       // SUBW: 减法字，从一个寄存器的值中减去另一个寄存器的值，适用于 32 位操作
    RISCV_LWU,        // LWU: 加载字无符号，从内存中加载一个字（32位）到寄存器，不进行符号扩展
    RISCV_LD,         // LD: 加载双字，从内存中加载一个双字（64位）到寄存器
    RISCV_SD,         // SD: 存储双字，将寄存器的值存储到内存中的双字位置

    RISCV_MUL,        // MUL: 乘法，乘两个寄存器的有符号值
    RISCV_MULH,       // MULH: 乘法高位，乘两个寄存器的有符号值并获取高位结果
    RISCV_MULHSU,     // MULHSU: 乘法高位有符号和无符号，乘有符号和无符号寄存器的值并获取高位结果
    RISCV_MULHU,      // MULHU: 乘法高位无符号，乘两个寄存器的无符号值并获取高位结果
    RISCV_DIV,        // DIV: 除法，除两个寄存器的有符号值
    RISCV_DIVU,       // DIVU: 除法无符号，除两个寄存器的无符号值
    RISCV_REM,        // REM: 取模，获取两个寄存器有符号除法的余数
    RISCV_REMU,       // REMU: 取模无符号，获取两个寄存器无符号除法的余数
    RISCV_MULW,       // MULW: 乘法字，乘两个寄存器的有符号值并截断为 32 位
    RISCV_DIVW,       // DIVW: 除法字，除两个寄存器的有符号值并截断为 32 位
    RISCV_REMW,       // REMW: 取模字，获取两个寄存器有符号除法的余数并截断为 32 位
    RISCV_REMUW,      // REMUW: 取模无符号字，获取两个寄存器无符号除法的余数并截断为 32 位

    RISCV_FMV_W_X,    // FMV.W.X: 浮点移动， 将寄存器中的整数值移动到浮点寄存器
    RISCV_FMV_X_W,    // FMV.X.W: 浮点移动， 将浮点寄存器中的值移动到整数寄存器
    RISCV_FCVT_S_W,   // FCVT.S.W: 浮点转换，将有符号整数转换为单精度浮点数
    RISCV_FCVT_D_W,   // FCVT.D.W: 浮点转换，将有符号整数转换为双精度浮点数
    RISCV_FCVT_S_WU,  // FCVT.S.WU: 浮点转换，将无符号整数转换为单精度浮点数
    RISCV_FCVT_D_WU,  // FCVT.D.WU: 浮点转换，将无符号整数转换为双精度浮点数
    RISCV_FCVT_W_S,   // FCVT.W.S: 浮点转换，将单精度浮点数转换为有符号整数
    RISCV_FCVT_W_D,   // FCVT.W.D: 浮点转换，将双精度浮点数转换为有符号整数
    RISCV_FCVT_WU_S,  // FCVT.WU.S: 浮点转换，将单精度浮点数转换为无符号整数
    RISCV_FCVT_WU_D,  // FCVT.WU.D: 浮点转换，将双精度浮点数转换为无符号整数
    RISCV_FLW,        // FLW: 加载浮点单精度，从内存中加载一个单精度浮点数到浮点寄存器
    RISCV_FLD,        // FLD: 加载浮点双精度，从内存中加载一个双精度浮点数到浮点寄存器
    RISCV_FSW,        // FSW: 存储浮点单精度，将浮点寄存器的单精度值存储到内存
    RISCV_FSD,        // FSD: 存储浮点双精度，将浮点寄存器的双精度值存储到内存
    RISCV_FADD_S,     // FADD.S: 浮点加法单精度，两个单精度浮点寄存器相加
    RISCV_FADD_D,     // FADD.D: 浮点加法双精度，两个双精度浮点寄存器相加
    RISCV_FSUB_S,     // FSUB.S: 浮点减法单精度，一个单精度浮点寄存器减去另一个
    RISCV_FSUB_D,     // FSUB.D: 浮点减法双精度，一个双精度浮点寄存器减去另一个
    RISCV_FMUL_S,     // FMUL.S: 浮点乘法单精度，两个单精度浮点寄存器相乘
    RISCV_FMUL_D,     // FMUL.D: 浮点乘法双精度，两个双精度浮点寄存器相乘
    RISCV_FDIV_S,     // FDIV.S: 浮点除法单精度，一个单精度浮点寄存器除以另一个
    RISCV_FDIV_D,     // FDIV.D: 浮点除法双精度，一个双精度浮点寄存器除以另一个
    RISCV_FSQRT_S,    // FSQRT.S: 浮点平方根单精度，计算单精度浮点寄存器的平方根
    RISCV_FSQRT_D,    // FSQRT.D: 浮点平方根双精度，计算双精度浮点寄存器的平方根
    RISCV_FMADD_S,    // FMADD.S: 浮点混合加法单精度，计算 (a * b) + c
    RISCV_FMADD_D,    // FMADD.D: 浮点混合加法双精度，计算 (a * b) + c
    RISCV_FMSUB_S,    // FMSUB.S: 浮点混合减法单精度，计算 (a * b) - c
    RISCV_FMSUB_D,    // FMSUB.D: 浮点混合减法双精度，计算 (a * b) - c
    RISCV_FNMSUB_S,   // FNMSUB.S: 浮点负混合减法单精度，计算 -(a * b) - c
    RISCV_FNMSUB_D,   // FNMSUB.D: 浮点负混合减法双精度，计算 -(a * b) - c
    RISCV_FNMADD_S,   // FNMADD.S: 浮点负混合加法单精度，计算 -(a * b) + c
    RISCV_FNMADD_D,   // FNMADD.D: 浮点负混合加法双精度，计算 -(a * b) + c
    RISCV_FSGNJ_S,    // FSGNJ.S: 浮点符号连接单精度，将一个寄存器的符号位连接到另一个寄存器
    RISCV_FSGNJ_D,    // FSGNJ.D: 浮点符号连接双精度，将一个寄存器的符号位连接到另一个寄存器
    RISCV_FSGNJN_S,   // FSGNJN.S: 浮点符号连接非单精度，将一个寄存器的符号位反转后连接到另一个寄存器
    RISCV_FSGNJN_D,   // FSGNJN.D: 浮点符号连接非双精度，将一个寄存器的符号位反转后连接到另一个寄存器
    RISCV_FSGNJX_S,   // FSGNJX.S: 浮点符号连接异或单精度，将两个寄存器的符号位异或后连接到另一个寄存器
    RISCV_FSGNJX_D,   // FSGNJX.D: 浮点符号连接异或双精度，将两个寄存器的符号位异或后连接到另一个寄存器
    RISCV_FMIN_S,     // FMIN.S: 浮点最小单精度，比较两个单精度浮点数，取较小者
    RISCV_FMIN_D,     // FMIN.D: 浮点最小双精度，比较两个双精度浮点数，取较小者
    RISCV_FMAX_S,     // FMAX.S: 浮点最大单精度，比较两个单精度浮点数，取较大者
    RISCV_FMAX_D,     // FMAX.D: 浮点最大双精度，比较两个双精度浮点数，取较大者
    RISCV_FEQ_S,      // FEQ.S: 浮点等于单精度，比较两个单精度浮点数是否相等
    RISCV_FEQ_D,      // FEQ.D: 浮点等于双精度，比较两个双精度浮点数是否相等
    RISCV_FLT_S,      // FLT.S: 浮点小于单精度，比较第一个单精度浮点数是否小于第二个
    RISCV_FLT_D,      // FLT.D: 浮点小于双精度，比较第一个双精度浮点数是否小于第二个
    RISCV_FLE_S,      // FLE.S: 浮点小于等于单精度，比较第一个单精度浮点数是否小于或等于第二个
    RISCV_FLE_D,      // FLE.D: 浮点小于等于双精度，比较第一个双精度浮点数是否小于或等于第二个
    RISCV_FCLASS_S,   // FCLASS.S: 浮点分类单精度，分类单精度浮点数的不同类别
    RISCV_FCLASS_D,   // FCLASS.D: 浮点分类双精度，分类双精度浮点数的不同类别
    RISCV_FMV_D_X,    // FMV.D.X: 浮点移动双精度，将整数寄存器的值移动到双精度浮点寄存器
    RISCV_FMV_X_D,    // FMV.X.D: 浮点移动整数，将双精度浮点寄存器的值移动到整数寄存器
    RISCV_FCVT_S_L,   // FCVT.S.L: 浮点转换，将有符号长整数转换为单精度浮点数
    RISCV_FCVT_D_L,   // FCVT.D.L: 浮点转换，将有符号长整数转换为双精度浮点数
    RISCV_FCVT_S_LU,  // FCVT.S.LU: 浮点转换，将无符号长整数转换为单精度浮点数
    RISCV_FCVT_D_LU,  // FCVT.D.LU: 浮点转换，将无符号长整数转换为双精度浮点数
    RISCV_FCVT_L_S,   // FCVT.L.S: 浮点转换，将单精度浮点数转换为有符号长整数
    RISCV_FCVT_L_D,   // FCVT.L.D: 浮点转换，将双精度浮点数转换为有符号长整数
    RISCV_FCVT_LU_S,  // FCVT.LU.S: 浮点转换，将单精度浮点数转换为无符号长整数
    RISCV_FCVT_LU_D,  // FCVT.LU.D: 浮点转换，将双精度浮点数转换为无符号长整数

    RISCV_LI,          // LI: 加载立即数，将一个立即数加载到寄存器
    RISCV_CALL,        // CALL: 调用函数，跳转到函数入口并保存返回地址

    RISCV_BGT,         // BGT: 分支大于，如果第一个寄存器的值大于第二个寄存器的值，则跳转到指定偏移地址
    RISCV_BLE,         // BLE: 分支小于或等于，如果第一个寄存器的值小于或等于第二个寄存器的值，则跳转到指定偏移地址
    RISCV_BGTU,        // BGTU: 分支大于无符号，如果第一个寄存器的无符号值大于第二个寄存器的无符号值，则跳转到指定偏移地址
    RISCV_BLEU,        // BLEU: 分支小于或等于无符号，如果第一个寄存器的无符号值小于或等于第二个寄存器的无符号值，则跳转到指定偏移地址

    RISCV_FMV_S,       // FMV.S: 浮点移动单精度，将一个寄存器的值移动到单精度浮点寄存器
    RISCV_FMV_D,       // FMV.D: 浮点移动双精度，将一个寄存器的值移动到双精度浮点寄存器

    RISCV_SH1ADD,      // SH1ADD: 半字节加法，针对特定数据宽度的加法操作
    RISCV_SH2ADD,      // SH2ADD: 双半字节加法，针对特定数据宽度的加法操作
    RISCV_SH3ADD,      // SH3ADD: 三半字节加法，针对特定数据宽度的加法操作

    RISCV_SH1ADDUW,    // SH1ADDUW: 半字节加无符号字，针对特定数据宽度的无符号加法操作
    RISCV_SH2ADDUW,    // SH2ADDUW: 双半字节加无符号字，针对特定数据宽度的无符号加法操作
    RISCV_SH3ADDUW,    // SH3ADDUW: 三半字节加无符号字，针对特定数据宽度的无符号加法操作

    RISCV_MIN,         // MIN: 最小值，比较两个寄存器的值，取较小者
    RISCV_MAX,         // MAX: 最大值，比较两个寄存器的值，取较大者
    RISCV_MINU,        // MINU: 最小值无符号，比较两个寄存器的无符号值，取较小者
    RISCV_MAXU,        // MAXU: 最大值无符号，比较两个寄存器的无符号值，取较大者

    RISCV_FCVT_D_S,    // FCVT.D.S: 浮点转换，将单精度浮点数转换为双精度浮点数
    RISCV_ZEXT_W,      // ZEXT.W: 零扩展字，将一个寄存器的值进行零扩展到更高位宽

    RISCV_FNEG_S,      // FNEG.S: 浮点取反单精度，将单精度浮点数的符号位取反
    RISCV_FNEG_D,      // FNEG.D: 浮点取反双精度，将双精度浮点数的符号位取反
};

//IcmpCond-->Opcode
const riscvOpcode IcmpCond_to_opcode[11] = {
    static_cast<riscvOpcode>(0),//占位符
    RISCV_BEQ,   // eq = 1
    RISCV_BNE,   // ne = 2
    RISCV_BGTU,  // ugt = 3
    RISCV_BGEU,  // uge = 4
    RISCV_BLTU,  // ult = 5
    RISCV_BLEU,  // ule = 6
    RISCV_BGT,   // sgt = 7
    RISCV_BGE,   // sge = 8
    RISCV_BLT,   // slt = 9
    RISCV_BLE    // sle = 10
};
//FcmpCond-->Opcode
const riscvOpcode FcmpCond_to_opcode[17] = {
    static_cast<riscvOpcode>(0), // 0: 占位符
    static_cast<riscvOpcode>(0), // 1: FALSE
    RISCV_BNE,     // 2: OEQ
    RISCV_BNE,     // 3: OGT
    RISCV_BNE,     // 4: OGE
    RISCV_BNE,     // 5: OLT
    RISCV_BNE,     // 6: OLE
    RISCV_BEQ,     // 7: ONE
    static_cast<riscvOpcode>(0), // 8: ORD
    RISCV_BNE,     // 9: UEQ
    RISCV_BNE,     // 10: UGT
    RISCV_BNE,     // 11: UGE
    RISCV_BNE,     // 12: ULT
    RISCV_BNE,     // 13: ULE
    RISCV_BEQ,     // 14: UNE
    static_cast<riscvOpcode>(0), // 15: UNO
    static_cast<riscvOpcode>(0)  // 16: TRUE
};
//FcmpCond-->instCode
const riscvOpcode FcmpCond_to_instcode[17] = {
    static_cast<riscvOpcode>(0), // 0: 占位符
    static_cast<riscvOpcode>(0), // 1: FALSE
    RISCV_FEQ_S,    // 2: OEQ
    RISCV_FLT_S,    // 3: OGT
    RISCV_FLE_S,    // 4: OGE
    RISCV_FLT_S,    // 5: OLT
    RISCV_FLE_S,    // 6: OLE
    RISCV_FEQ_S,    // 7: ONE
    static_cast<riscvOpcode>(0),  // 8: ORD
    RISCV_FEQ_S,    // 9: UEQ
    RISCV_FLT_S,    //10: UGT
    RISCV_FLE_S,    //11: UGE
    RISCV_FLT_S,    //12: ULT
    RISCV_FLE_S,    //13: ULE
    RISCV_FEQ_S,    //14: UNE
    static_cast<riscvOpcode>(0),  //15: UNO
    static_cast<riscvOpcode>(0)   //16: TRUE
};

struct RvOpInfo {
    enum {
        R_type,
        I_type,
        S_type,
        B_type,
        U_type,
        J_type,
        R2_type,
        R4_type,
        CALL_type,
    };
    int ins_formattype;    // 指令类型
    char *name;
    int latency;    // sifive-u74上的硬件指令延迟, 可以用于指令调度优化, 如果你不打算实现该优化可以忽略
};
extern RvOpInfo OpTable[];
enum regs{
    RISCV_x0,
    RISCV_x1,
    RISCV_x2,
    RISCV_x3,
    RISCV_x4,
    RISCV_x5,
    RISCV_x6,
    RISCV_x7,
    RISCV_x8,
    RISCV_x9,
    RISCV_x10,
    RISCV_x11,
    RISCV_x12,
    RISCV_x13,
    RISCV_x14,
    RISCV_x15,
    RISCV_x16,
    RISCV_x17,
    RISCV_x18,
    RISCV_x19,
    RISCV_x20,
    RISCV_x21,
    RISCV_x22,
    RISCV_x23,
    RISCV_x24,
    RISCV_x25,
    RISCV_x26,
    RISCV_x27,
    RISCV_x28,
    RISCV_x29,
    RISCV_x30,
    RISCV_x31,
    RISCV_f0,
    RISCV_f1,
    RISCV_f2,
    RISCV_f3,
    RISCV_f4,
    RISCV_f5,
    RISCV_f6,
    RISCV_f7,
    RISCV_f8,
    RISCV_f9,
    RISCV_f10,
    RISCV_f11,
    RISCV_f12,
    RISCV_f13,
    RISCV_f14,
    RISCV_f15,
    RISCV_f16,
    RISCV_f17,
    RISCV_f18,
    RISCV_f19,
    RISCV_f20,
    RISCV_f21,
    RISCV_f22,
    RISCV_f23,
    RISCV_f24,
    RISCV_f25,
    RISCV_f26,
    RISCV_f27,
    RISCV_f28,
    RISCV_f29,
    RISCV_f30,
    RISCV_f31,
    RISCV_INVALID,
    RISCV_spilled_in_memory,
};
enum intregs{
    RISCV_ra  = RISCV_x1,   // 返回地址寄存器 (Return Address Register): 存储函数调用的返回地址
    RISCV_sp  = RISCV_x2,   // 堆栈指针寄存器 (Stack Pointer): 指向当前堆栈顶，用于堆栈操作
    RISCV_gp  = RISCV_x3,   // 全局指针寄存器 (Global Pointer): 指向全局数据段，便于访问全局变量
    RISCV_tp  = RISCV_x4,   // 线程指针寄存器 (Thread Pointer): 指向线程本地存储（TLS）数据
    RISCV_t0  = RISCV_x5,   // 临时寄存器 0 (Temporary Register 0): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_t1  = RISCV_x6,   // 临时寄存器 1 (Temporary Register 1): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_t2  = RISCV_x7,   // 临时寄存器 2 (Temporary Register 2): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_s0  = RISCV_x8,   // 保存寄存器 0 / 帧指针 (Saved Register 0 / Frame Pointer): 用于保存需要在函数调用间保持的值或指向栈帧基址
    RISCV_s1  = RISCV_x9,   // 保存寄存器 1 (Saved Register 1): 用于保存需要在函数调用间保持的值
    RISCV_a0  = RISCV_x10,  // 参数寄存器 0 / 返回值寄存器 0 (Argument Register 0 / Return Value Register 0): 用于传递函数参数和返回第一个值
    RISCV_a1  = RISCV_x11,  // 参数寄存器 1 / 返回值寄存器 1 (Argument Register 1 / Return Value Register 1): 用于传递函数参数和返回第二个值
    RISCV_a2  = RISCV_x12,  // 参数寄存器 2 (Argument Register 2): 用于传递函数参数
    RISCV_a3  = RISCV_x13,  // 参数寄存器 3 (Argument Register 3): 用于传递函数参数
    RISCV_a4  = RISCV_x14,  // 参数寄存器 4 (Argument Register 4): 用于传递函数参数
    RISCV_a5  = RISCV_x15,  // 参数寄存器 5 (Argument Register 5): 用于传递函数参数
    RISCV_a6  = RISCV_x16,  // 参数寄存器 6 (Argument Register 6): 用于传递函数参数
    RISCV_a7  = RISCV_x17,  // 参数寄存器 7 (Argument Register 7): 用于传递函数参数
    RISCV_s2  = RISCV_x18,  // 保存寄存器 2 (Saved Register 2): 用于保存需要在函数调用间保持的值
    RISCV_s3  = RISCV_x19,  // 保存寄存器 3 (Saved Register 3): 用于保存需要在函数调用间保持的值
    RISCV_s4  = RISCV_x20,  // 保存寄存器 4 (Saved Register 4): 用于保存需要在函数调用间保持的值
    RISCV_s5  = RISCV_x21,  // 保存寄存器 5 (Saved Register 5): 用于保存需要在函数调用间保持的值
    RISCV_s6  = RISCV_x22,  // 保存寄存器 6 (Saved Register 6): 用于保存需要在函数调用间保持的值
    RISCV_s7  = RISCV_x23,  // 保存寄存器 7 (Saved Register 7): 用于保存需要在函数调用间保持的值
    RISCV_s8  = RISCV_x24,  // 保存寄存器 8 (Saved Register 8): 用于保存需要在函数调用间保持的值
    RISCV_s9  = RISCV_x25,  // 保存寄存器 9 (Saved Register 9): 用于保存需要在函数调用间保持的值
    RISCV_s10 = RISCV_x26,  // 保存寄存器 10 (Saved Register 10): 用于保存需要在函数调用间保持的值
    RISCV_s11 = RISCV_x27,  // 保存寄存器 11 (Saved Register 11): 用于保存需要在函数调用间保持的值
    RISCV_t3  = RISCV_x28,  // 临时寄存器 3 (Temporary Register 3): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_t4  = RISCV_x29,  // 临时寄存器 4 (Temporary Register 4): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_t5  = RISCV_x30,  // 临时寄存器 5 (Temporary Register 5): 用于临时数据存储，不需要在函数调用间保持值
    RISCV_t6  = RISCV_x31,  // 临时寄存器 6 (Temporary Register 6): 用于临时数据存储，不需要在函数调用间保持值
};

enum {
    RISCV_fp = RISCV_x8,
};

enum floatregs{
    RISCV_ft0 = RISCV_f0,
    RISCV_ft1 = RISCV_f1,
    RISCV_ft2 = RISCV_f2,
    RISCV_ft3 = RISCV_f3,
    RISCV_ft4 = RISCV_f4,
    RISCV_ft5 = RISCV_f5,
    RISCV_ft6 = RISCV_f6,
    RISCV_ft7 = RISCV_f7,
    RISCV_fs0 = RISCV_f8,
    RISCV_fs1 = RISCV_f9,
    RISCV_fa0 = RISCV_f10,
    RISCV_fa1 = RISCV_f11,
    RISCV_fa2 = RISCV_f12,
    RISCV_fa3 = RISCV_f13,
    RISCV_fa4 = RISCV_f14,
    RISCV_fa5 = RISCV_f15,
    RISCV_fa6 = RISCV_f16,
    RISCV_fa7 = RISCV_f17,
    RISCV_fs2 = RISCV_f18,
    RISCV_fs3 = RISCV_f19,
    RISCV_fs4 = RISCV_f20,
    RISCV_fs5 = RISCV_f21,
    RISCV_fs6 = RISCV_f22,
    RISCV_fs7 = RISCV_f23,
    RISCV_fs8 = RISCV_f24,
    RISCV_fs9 = RISCV_f25,
    RISCV_fs10 = RISCV_f26,
    RISCV_fs11 = RISCV_f27,
    RISCV_ft8 = RISCV_f28,
    RISCV_ft9 = RISCV_f29,
    RISCV_ft10 = RISCV_f30,
    RISCV_ft11 = RISCV_f31,
};
//被调用者保存的寄存器
const regs save_regs[25] = {
    RISCV_x1,
    RISCV_x8,
    RISCV_x9,
    RISCV_x18,
    RISCV_x19,
    RISCV_x20,
    RISCV_x21,
    RISCV_x22,
    RISCV_x23,
    RISCV_x24,
    RISCV_x25,
    RISCV_x26,
    RISCV_x27,
    RISCV_f8,
    RISCV_f9,
    RISCV_f18,
    RISCV_f19,
    RISCV_f20,
    RISCV_f21,
    RISCV_f22,
    RISCV_f23,
    RISCV_f24,
    RISCV_f25,
    RISCV_f26,
    RISCV_f27,
};

static inline MachineDataType getRVRegType(int reg_no) {
    if (reg_no >= RISCV_x0 && reg_no <= RISCV_x31) {
        return INT64;
    }
    if (reg_no >= RISCV_f0 && reg_no <= RISCV_f31) {
        return FLOAT64;
    }
    ERROR("Unknown reg_no %d", reg_no);
}

static inline Register GetPhysicalReg(int reg_no) { return Register(false, reg_no, getRVRegType(reg_no)); }

struct RiscV64RegisterInfo {
    char *name;
};
extern RiscV64RegisterInfo RiscV64Registers[];
extern Register RISCVregs[];

/*
一般情况下，可以认为中端条件跳转语句的前一条指令一定是icmp或fcmp
如果你的中端实现不是这样，需要自行探索一下条件跳转语句的翻译方式

%r3 = icmp ne i32 %r1, %r2
br i1 %r3, label %L1, label %L2
应当被翻译为:
bne %1, %2, .L1(即jmp_label_id)
j L2
*/
struct RiscVLabel :public Label{
    int jmp_label_id = 0; // 该id为跳转的基本块编号
    bool is_data_address = false; // 是否为数据段标签
    std::string name;
    bool is_hi; // 对应%hi(name) 和 %lo(name)
    RiscVLabel() :Label(0, 0){} ;
    RiscVLabel(int jmp, int seq) : Label(jmp, seq), name() {}//brcond
    RiscVLabel(std::string name, bool is_hi):name(name), is_hi(is_hi) ,Label(0, 0){ this->is_data_address = true; }
    // 添加一些你想用的构造函数
    RiscVLabel(int label_no):jmp_label_id(label_no),Label(0, 0){}
};

class RiscV64Instruction : public MachineBaseInstruction {
private:
    int op;
    Register rd, rs1, rs2, rs3;
    bool use_label;
    int imm;
    RiscVLabel label;

    // 下面两个变量的具体作用见ConstructCall函数
    int callireg_num;
    int callfreg_num;

    int ret_type; // 用于GetI_typeReadreg(), 即确定函数返回时用的是a0寄存器还是fa0寄存器, 或者没有返回值

    std::vector<Register *> GetR_typeReadreg() { return {&rs1, &rs2}; }
    std::vector<Register *> GetR2_typeReadreg() { return {&rs1}; }
    std::vector<Register *> GetR4_typeReadreg() { return {&rs1, &rs2, &rs3}; }
    std::vector<Register *> GetI_typeReadreg() {
        std::vector<Register *> ret;
        ret.push_back(&rs1);
        if (op == RISCV_JALR) { 
            // 当ret_type为1或2时, 我们认为jalr只会用于函数返回, 所以jalr会读取a0或fa0寄存器(即函数返回值)
            // 如果函数没有返回值或者你在其他地方使用到了jalr指令，将ret_type设置为0即可
            if (ret_type == 1) {
                ret.push_back(&RISCVregs[RISCV_a0]);
            } else if (ret_type == 2) {
                ret.push_back(&RISCVregs[RISCV_fa0]);
            }
        }
        return ret;
    }
    std::vector<Register *> GetS_typeReadreg() { return {&rs1, &rs2}; }
    std::vector<Register *> GetB_typeReadreg() { return {&rs1, &rs2}; }
    std::vector<Register *> GetU_typeReadreg() { return {}; }
    std::vector<Register *> GetJ_typeReadreg() { return {}; }
    std::vector<Register *> GetCall_typeReadreg() {
        std::vector<Register *> ret;
        for (int i = 0; i < callireg_num; i++) {
            ret.push_back(&RISCVregs[RISCV_a0 + i]);
        }
        for (int i = 0; i < callfreg_num; i++) {
            ret.push_back(&RISCVregs[RISCV_fa0 + i]);
        }
        return ret;
    }

    std::vector<Register *> GetR_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetR2_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetR4_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetI_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetS_typeWritereg() { return {}; }
    std::vector<Register *> GetB_typeWritereg() { return {}; }
    std::vector<Register *> GetU_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetJ_typeWritereg() { return {&rd}; }
    std::vector<Register *> GetCall_typeWritereg() {
        return {
        &RISCVregs[RISCV_ra],

        &RISCVregs[RISCV_t0],  &RISCVregs[RISCV_t1],  &RISCVregs[RISCV_t2],   &RISCVregs[RISCV_t3],
        &RISCVregs[RISCV_t4],  &RISCVregs[RISCV_t5],  &RISCVregs[RISCV_t6],

        &RISCVregs[RISCV_a0],  &RISCVregs[RISCV_a1],  &RISCVregs[RISCV_a2],   &RISCVregs[RISCV_a3],
        &RISCVregs[RISCV_a4],  &RISCVregs[RISCV_a5],  &RISCVregs[RISCV_a6],   &RISCVregs[RISCV_a7],

        &RISCVregs[RISCV_ft0], &RISCVregs[RISCV_ft1], &RISCVregs[RISCV_ft2],  &RISCVregs[RISCV_ft3],
        &RISCVregs[RISCV_ft4], &RISCVregs[RISCV_ft5], &RISCVregs[RISCV_ft6],  &RISCVregs[RISCV_ft7],
        &RISCVregs[RISCV_ft8], &RISCVregs[RISCV_ft9], &RISCVregs[RISCV_ft10], &RISCVregs[RISCV_ft11],

        &RISCVregs[RISCV_fa0], &RISCVregs[RISCV_fa1], &RISCVregs[RISCV_fa2],  &RISCVregs[RISCV_fa3],
        &RISCVregs[RISCV_fa4], &RISCVregs[RISCV_fa5], &RISCVregs[RISCV_fa6],  &RISCVregs[RISCV_fa7],
        };
    }

    friend class RiscV64InstructionConstructor;

    RiscV64Instruction() : MachineBaseInstruction(MachineBaseInstruction::RiscV), imm(0), use_label(false) {}

public:
    void setOpcode(int op, bool use_label) {
        this->op = op;
        this->use_label = use_label;
    }
    void setRd(Register rd) { this->rd = rd; }
    void setRs1(Register rs1) { this->rs1 = rs1; }
    void setRs2(Register rs2) { this->rs2 = rs2; }
    void setRs3(Register rs3) { this->rs3 = rs3; }
    void setImm(int imm) { this->imm = imm; }
    void setLabel(RiscVLabel label) { this->label = label; }
    void setCalliregNum(int n) { callireg_num = n; }
    void setCallfregNum(int n) { callfreg_num = n; }
    void setRetType(int use) { ret_type = use; }
    Register getRd() { return rd; }
    Register getRs1() { return rs1; }
    Register getRs2() { return rs2; }
    Register getRs3() { return rs3; }
    void setUseLabel(bool use_label) { this->use_label = use_label; }
    bool getUseLabel() { return use_label; }
    int getImm() { return imm; }
    RiscVLabel getLabel() { return label; }
    int getOpcode() { return op; }
    std::vector<Register *> GetReadReg();
    std::vector<Register *> GetWriteReg();
    int GetLatency() { return OpTable[op].latency; }
};

/* ******************************** 指令构造接口 *****************************/
class RiscV64InstructionConstructor {
    static RiscV64InstructionConstructor instance;

    RiscV64InstructionConstructor() {}

public:
    static RiscV64InstructionConstructor *GetConstructor() { return &instance; }
    // 函数命名方法大部分与RISC-V指令格式一致

    // example: addw Rd, Rs1, Rs2 
    RiscV64Instruction *ConstructR(int op, Register Rd, Register Rs1, Register Rs2) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::R_type);
        ret->setRd(Rd);
        ret->setRs1(Rs1);
        ret->setRs2(Rs2);
        return ret;
    }
    // example: fmv.x.w Rd, Rs1
    RiscV64Instruction *ConstructR2(int op, Register Rd, Register Rs1) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::R2_type);
        ret->setRd(Rd);
        ret->setRs1(Rs1);
        return ret;
    }
    // example: fmadd.s Rd, Rs1, Rs2, Rs3
    RiscV64Instruction *ConstructR4(int op, Register Rd, Register Rs1, Register Rs2, Register Rs3) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::R4_type);
        ret->setRd(Rd);
        ret->setRs1(Rs1);
        ret->setRs2(Rs2);
        ret->setRs3(Rs3);
        return ret;
    }
    // example: lw Rd, imm(Rs1) 
    // example: addi Rd, Rs1, imm
    RiscV64Instruction *ConstructIImm(int op, Register Rd, Register Rs1, int imm) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::I_type);
        ret->setRd(Rd);
        ret->setRs1(Rs1);
        ret->setImm(imm);
        return ret;
    }
    // example: lw Rd label(Rs1)   =>  lw Rd %lo(label_name)(Rs1)
    // example: addi Rd, Rs1, label  =>  addi Rd, Rs1, %lo(label_name)
    RiscV64Instruction *ConstructILabel(int op, Register Rd, Register Rs1, RiscVLabel label) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        Assert(OpTable[op].ins_formattype == RvOpInfo::I_type);
        ret->setRd(Rd);
        ret->setRs1(Rs1);
        ret->setLabel(label);
        return ret;
    }
    // example: sw value imm(ptr)
    RiscV64Instruction *ConstructSImm(int op, Register value, Register ptr, int imm) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::S_type);
        ret->setRs1(value);
        ret->setRs2(ptr);
        ret->setImm(imm);
        return ret;
    }
    // example: sw value label(ptr)  =>  sw value %lo(label_name)(ptr)
    RiscV64Instruction *ConstructSLabel(int op, Register value, Register ptr, RiscVLabel label) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        Assert(OpTable[op].ins_formattype == RvOpInfo::S_type);
        ret->setRs1(value);
        ret->setRs2(ptr);
        ret->setLabel(label);
        return ret;
    }
    // example: b(cond) Rs1, Rs2,label  =>  bne Rs1, Rs2, .L3(标签具体如何输出见riscv64_printasm.cc)
    RiscV64Instruction *ConstructBLabel(int op, Register Rs1, Register Rs2, RiscVLabel label) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        Assert(OpTable[op].ins_formattype == RvOpInfo::B_type);
        ret->setRs1(Rs1);
        ret->setRs2(Rs2);
        ret->setLabel(label);
        return ret;
    }
    // example: lui Rd, imm
    RiscV64Instruction *ConstructUImm(int op, Register Rd, int imm) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, false);
        Assert(OpTable[op].ins_formattype == RvOpInfo::U_type);
        ret->setRd(Rd);
        ret->setImm(imm);
        return ret;
    }
    // example: lui Rd, %hi(label_name)
    RiscV64Instruction *ConstructULabel(int op, Register Rd, RiscVLabel label) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        Assert(OpTable[op].ins_formattype == RvOpInfo::U_type);
        ret->setRd(Rd);
        ret->setLabel(label);
        return ret;
    }
    // example: jal rd, label  =>  jal a0, .L4
    RiscV64Instruction *ConstructJLabel(int op, Register rd, RiscVLabel label) {
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        Assert(OpTable[op].ins_formattype == RvOpInfo::J_type);
        ret->setRd(rd);
        ret->setLabel(label);
        return ret;
    }
    // example: call funcname  
    // iregnum 和 fregnum 表示该函数调用会分别用几个物理寄存器和浮点寄存器传参
    // iregnum 和 fregnum 的作用为精确确定call会读取哪些寄存器 (具体见GetCall_typeWritereg()函数)
    // 可以进行更精确的寄存器分配
    // 对于函数调用，我们单独处理这一条指令，而不是用真指令替代，原因是函数调用涉及到部分寄存器的读写
    RiscV64Instruction *ConstructCall(int op, std::string funcname, int iregnum, int fregnum) {
        Assert(OpTable[op].ins_formattype == RvOpInfo::CALL_type);
        RiscV64Instruction *ret = new RiscV64Instruction();
        ret->setOpcode(op, true);
        // ret->setRd(GetPhysicalReg(phy_rd));
        ret->setCalliregNum(iregnum);
        ret->setCallfregNum(fregnum);
        ret->setLabel(RiscVLabel(funcname, false));
        return ret;
    }

};
extern RiscV64InstructionConstructor *rvconstructor;

#include "../common/machine_instruction_structures/machine.h"
class RiscV64Function;
class RiscV64Unit;

class RiscV64Block : public MachineBlock {
public:
    RiscV64Block(int id) : MachineBlock(id) {}
};

class RiscV64BlockFactory : public MachineBlockFactory {
public:
    MachineBlock *CreateBlock(int id) { return new RiscV64Block(id); }
};

class RiscV64Function : public MachineFunction {
public:
    RiscV64Function(std::string name) : MachineFunction(name, new RiscV64BlockFactory()) {}

private:
    // TODO: add your own members here
public:
    // TODO: add your own members here
};
class RiscV64Unit : public MachineUnit {};

class RiscV64RegisterAllocTools : public PhysicalRegistersAllocTools {
protected:
    std::vector<int> getValidRegs(LiveInterval interval);
    std::vector<int> getAliasRegs(int phy_id) { return std::vector<int>({phy_id}); }

public:
    RiscV64RegisterAllocTools() { phy_occupied.resize(64); }
    void clear() {
        phy_occupied.clear();
        Assert(phy_occupied.empty());
        phy_occupied.resize(64);
        mem_occupied.clear();
        Assert(mem_occupied.empty());
    }
};

class RiscV64Spiller : public SpillCodeGen {
private:
    // 生成从栈中读取溢出寄存器的指令
    Register GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                              MachineDataType type);
    // 生成将溢出寄存器写入栈的指令
    Register GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                               MachineDataType type);
};

#endif