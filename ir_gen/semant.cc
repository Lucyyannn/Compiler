#include "semant.h"
#include "../include/SysY_tree.h"
#include "../include/ir.h"
#include "../include/type.h"
#include<iostream>
/*
    语义分析阶段需要对语法树节点上的类型和常量等信息进行标注, 即NodeAttribute类
    同时还需要标注每个变量的作用域信息，即部分语法树节点中的scope变量
    你可以在utils/ast_out.cc的输出函数中找到你需要关注哪些语法树节点中的NodeAttribute类及其他变量
    以及对语义错误的代码输出报错信息
*/

/*
    错误检查的基本要求:
    • 检查 main 函数是否存在 (根据SysY定义，如果不存在main函数应当报错)；
    • 检查未声明变量，及在同一作用域下重复声明的变量；
    • 条件判断和运算表达式：int 和 bool 隐式类型转换（例如int a=5，return a+!a）；
    • 数值运算表达式：运算数类型是否正确 (如返回值为 void 的函数调用结果是否参与了其他表达式的计算)；
    • 检查未声明函数，及函数形参是否与实参类型及数目匹配；
    • 检查是否存在整型变量除以整型常量0的情况 (如对于表达式a/(5-4-1)，编译器应当给出警告或者直接报错)；

    错误检查的进阶要求:
    • 对数组维度进行相应的类型检查 (例如维度是否有浮点数，定义维度时是否为常量等)；
    • 对float进行隐式类型转换以及其他float相关的检查 (例如模运算中是否有浮点类型变量等)；
*/
extern LLVMIR llvmIR;

SemantTable semant_table;
std::vector<std::string> error_msgs{};//存储报错message

bool main_exist=false;

//****************************** 隐式类型转换 ****************************** */

// float -> int 给到int类型的值，不影响原有float数据的类型和精度
int FloatToInt(float floatval ,int line_number){
    if (floatval >= 2147483648.0 || floatval <= -2147483649.0) {
        error_msgs.push_back("float value out of int range , trasform behavior undefined in line " + std::to_string(line_number) + "\n");
    } else {
        return static_cast<int>(floatval);  // 舍弃小数部分
    }
    return 0;
}

// int -> float 可以省去，减少函数调用
float IntToFloat(int intval ) {
    // 保持数值不变
    return   static_cast<float>(intval);  
}

//****************************** 非数组的赋值 ************************************** */
/* Local */
//非数组的赋值：包括assign_stmt和def_init,其实就是修改symbol_table中VarAttribute的initval
//根据Symbol~VarAttribute指定的类型，考虑了int和float类型的隐式转换,
//需要提前将信息载入symbol_table，这里更新varattribute的InitVals{}
void AssignNotArray(Symbol name, NodeAttribute& exp, int line_number){
    //引用attr是要修改的symbol对应的VarAttribute
    VarAttribute& attr = semant_table.symbol_table.get_val(name);
    
    //修改lval的NodeAttribute值
    if(attr.type==Type::INT){
        if(exp.T.type==Type::INT){
            //attr.IntInitVals.resize(1);
            //attr.IntInitVals[0]=exp.V.val.IntVal;
            attr.IntInitVals.push_back(exp.V.val.IntVal);
        }
        else if(exp.T.type==Type::FLOAT){
            attr.IntInitVals.resize(1);
            attr.IntInitVals[0]=FloatToInt(exp.V.val.FloatVal,line_number);
        }else{
            error_msgs.push_back("invalid assignment type in line " + std::to_string(line_number) + "\n"); 
        }
    }else if(attr.type==Type::FLOAT){
        if(exp.T.type==Type::INT){
            attr.FloatInitVals.resize(1);
            attr.FloatInitVals[0]=IntToFloat(exp.V.val.IntVal);
        }else if(exp.T.type==Type::FLOAT){
            attr.FloatInitVals.resize(1);
            attr.FloatInitVals[0]=exp.V.val.FloatVal;
        }else{
            error_msgs.push_back("invalid assignment type in line " + std::to_string(line_number) + "\n");
        }
    }
}

/* Global */
//若发生float<-->int的隐式类型转换，只在全局符号表中登入正确的值，不改变节点本身
void GlobalAssignNotArray(Symbol name, NodeAttribute& exp, int line_number){
    //引用attr是要修改的symbol对应的VarAttribute
    VarAttribute& attr = semant_table.GlobalTable[name];
    
    //修改lval的NodeAttribute值
    if(attr.type==Type::INT){
        if(exp.T.type==Type::INT){
            attr.IntInitVals.resize(1);
            attr.IntInitVals[0]=exp.V.val.IntVal;
        }else if(exp.T.type==Type::FLOAT){
            attr.IntInitVals.resize(1);
            attr.IntInitVals[0]=FloatToInt(exp.V.val.FloatVal,line_number);
        }else{
            error_msgs.push_back("invalid assignment type in line " + std::to_string(line_number) + "\n");
        }
    }else if(attr.type==Type::FLOAT){
        if(exp.T.type==Type::INT){
            attr.FloatInitVals.resize(1);
            attr.FloatInitVals[0]=IntToFloat(exp.V.val.IntVal);
        }else if(exp.T.type==Type::FLOAT){
            attr.FloatInitVals.resize(1);
            attr.FloatInitVals[0]=exp.V.val.FloatVal;
        }else{
            error_msgs.push_back("invalid assignment type in line " + std::to_string(line_number) + "\n");
        }
    }
}

//****************************** 数组的递归初始化 ************************************** */

//数组声明时，根据初始化列表收集初始化值
/*
int a[2][2][2][2]={    {  {{1, 2},{ 3, 4}},  {{ 5, 6},{ 7, 8}}   },
                       {  {{9,10},{11,12}},  {{13,14},{15,16}}   } 
                  }

int a[2][2][2][2]={    {  {{    },{3,  }},   { 5, 6 ,{7, 8}}     },
                       {  { 9,10 ,11,12 },     13,14,15,16       } 
                  }
（1）needed_num代表当前层次需要的初始值个数；
    仅在递归过程中纵向传递，不可横向共用。

（2）depth代表当前层次（由{}层数决定）：
    若当前is_exp!=0，说明又进入一层{}，否则是单值；
    第一层depth=0,needed_num[0]=total_num；依此类推;
    is_exp!=0  -->  depth++  -->  needed_num[depth] = needed_num[depth-1]/attr.dims[depth-1]

（3）dfs

 (4) IR新增逻辑：遇到显示初始化为0的值，记录其位置；

*/
void GatherArrayrecursion(VarAttribute & attr, InitVal init,std::vector<int> needed_num,int depth,int& pos){
    //curr_num : 这一层已经收集到的初始值个数
    int curr_num=0;
    //initval包含了该层{}内的所有内容
    std::vector<InitVal> initval{};
    if(attr.ConstTag){
        initval=*(((ConstInitVal*)init)->initval);
    }else{
        initval=*(((VarInitVal*)init)->initval);
    }

    //int型
    if(attr.type==Type::INT){
        for(int i=0;i<initval.size();i++){
            //若不再是列表而是单值，直接加入
            if(initval[i]->is_exp()){
                attr.IntInitVals.push_back(initval[i]->attribute.V.val.IntVal);
                curr_num++; 
                attr.RealInitvalPos.push_back(++pos);//全局位置
                // /* 若initval不是const，说明是表达式类型（如a+b, arr[2][3])*/
                // if(!initval[i]->attribute.V.ConstTag){
                //     if(attr.ConstTag){
                //         auto ConstInitExp=*(ConstInitVal_exp*)initval[i];
                //         ConstInitExp.isLval=true;
                        
                //     }else{
                //         auto VarInitExp=*(VarInitVal_exp*)initval[i];
                //         VarInitExp.isLval=true;
                //     }
                // }
            }//若是列表，递归解析
            else{
                GatherArrayrecursion(attr,initval[i],needed_num,depth+1,pos);
                curr_num+=needed_num[depth+1];
            }
        }
        //补齐该层元素个数
        for(int i=curr_num;i<needed_num[depth];i++){
            attr.IntInitVals.push_back(0);
            ++pos;
            attr.RealInitvalPos.push_back(0);//全局位置设为0
        }
        return;
    //float型
    }else if(attr.type==Type::FLOAT){
        for(int i=0;i<initval.size();i++){
            //initval[i]->TypeCheck();
            //若不再是列表而是单值，直接加入
            if(initval[i]->is_exp()){
                attr.FloatInitVals.push_back(initval[i]->attribute.V.val.FloatVal);
                curr_num++;
                attr.RealInitvalPos.push_back(++pos);//全局位置
                // /* 若initval不是const，说明是表达式类型（如a+b, arr[2][3])*/
                // if(!initval[i]->attribute.V.ConstTag){
                //     if(attr.ConstTag){
                //         auto ConstInitExp=*(ConstInitVal_exp*)initval[i];
                //         ConstInitExp.isLval=true;
                        
                //     }else{
                //         auto VarInitExp=*(VarInitVal_exp*)initval[i];
                //         VarInitExp.isLval=true;
                //     }
                // }
            }//若是列表，递归解析
            else{
                GatherArrayrecursion(attr,initval[i],needed_num,depth+1,pos);
            }
        }
        //补齐该层元素个数
        for(int i=curr_num;i<needed_num[depth];i++){
            attr.FloatInitVals.push_back(0.0);
            ++pos;
            attr.RealInitvalPos.push_back(0);//全局位置设为0
        }
        return;
    }
}

void GatherArrayInitVals(VarAttribute & attr, InitVal init){
/* 辅助信息准备:total_num , needed_num[]*/
    //计算total_num：数组中元素总数
    int total_num=attr.dims[0];//前提：dims不能为空，且正整数
    for(int i=1;i<attr.dims.size();i++){
        total_num*=attr.dims[i];
    }
    //计算needed_num[i]:各层需要的初始值个数
    std::vector<int> needed_num{};
    needed_num.resize(attr.dims.size());
    needed_num[0]=total_num;
    for(int depth=1;depth<needed_num.size();depth++){
        needed_num[depth]=needed_num[depth-1]/attr.dims[depth-1];
    }

    //初始化depth，表示当前层次 ；pos表示当前值的位置序号
    int depth=0;
    int pos=0; //整个数组的,[1,size()]
    attr.RealInitvalPos.clear();
    /* 解析初始化列表 */
    GatherArrayrecursion(attr,init,needed_num,depth,pos);
    return;
}


//****************************** 数组的访问 ************************************** */

//Lval中，根据索引取数组中的值
//可能是整个数组、部分数组、单个值
// 数组定义a[5][3][4],dims:{5,3,4}       求a[3][1][2],indexs:{3,1,2}
void AssignArrayValue(NodeAttribute& attribute, VarAttribute attr,std::vector<int> indexs){
    int index=0;
    if(attribute.T.type==Type::INT){
        int total_num=attr.IntInitVals.size();//该维度的val总数
        for(int i=0;i<indexs.size();i++){
            //indexs[i]是第i+1维度的索引值
            //attr.dims[i]是第i+1维度的度数
            index+=indexs[i]*(total_num/attr.dims[i]);
            total_num/=attr.dims[i];
        }
        attribute.V.val.IntVal=attr.IntInitVals[index];

    }else if(attribute.T.type==Type::FLOAT){
        int total_num=attr.FloatInitVals.size();
        for(int i=0;i<indexs.size();i++){
            //indexs[i]是第i+1维度的索引值
            //attr.dims[i]是第i+1维度的度数
            index+=indexs[i]*(total_num/attr.dims[i]);
            total_num/=attr.dims[i];
        }
        attribute.V.val.IntVal=attr.FloatInitVals[index];
    }
}

//*****************************   program  ********************************* */
void __Program::TypeCheck() {

    //进入作用域
    semant_table.symbol_table.enter_scope();//scope=00

    //对所有编译单元逐一进行类型检查
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
    //检查main函数是否存在
    if(!main_exist){
        error_msgs.push_back("Function main() doesn't exist .\n");
    }
    semant_table.symbol_table.exit_scope();
}

//*****************************   Exp  ********************************* */
void Exp::TypeCheck() {
    addexp->TypeCheck();

    attribute = addexp->attribute;
}

void ConstExp::TypeCheck() {
    addexp->TypeCheck();
    attribute = addexp->attribute;
    if (!attribute.V.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(line_number) + "\n");
    }
}

//******************* 算数运算表达式(考虑了bool-->int)

/*
int +\- int => int 
int *\% int => int
int1 / int2   => int (要取整)  判断int2==0

float1 +-*\ float2 => float  判断float2==0
float 不支持 %

int +-*\ float => float    
float +-*\ int => float  
int float 不支持%
*/
void AddExp_plus::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();
    //先检查数据类型，不能为void
    if(addexp->attribute.T.type==Type::VOID||mulexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("computing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置运算结果数据类型
    if((addexp->attribute.T.type==Type::INT||addexp->attribute.T.type==Type::BOOL) && (mulexp->attribute.T.type==Type::INT||mulexp->attribute.T.type==Type::BOOL)){
        attribute.T.type=Type::INT;
    }else{
        attribute.T.type=Type::FLOAT;
    }
    //对于Const常量，在编译期求值
    attribute.V.ConstTag= addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;
    if(attribute.V.ConstTag){
        switch (addexp->attribute.T.type){
            //addexp 是 int型
            case Type::INT:
                switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=addexp->attribute.V.val.IntVal+mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.IntVal+mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=addexp->attribute.V.val.IntVal+mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            //addexp 是 float 型
            case Type::FLOAT:
                switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal+mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal+mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal+mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            //addexp 是 bool 型
            case Type::BOOL:
                 switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=addexp->attribute.V.val.BoolVal+mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.BoolVal+mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=addexp->attribute.V.val.BoolVal+mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void AddExp_sub::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();

    //先检查数据类型，不能为void
    if(addexp->attribute.T.type==Type::VOID||mulexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("computing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置运算结果数据类型
    if((addexp->attribute.T.type==Type::INT||addexp->attribute.T.type==Type::BOOL) && (mulexp->attribute.T.type==Type::INT||mulexp->attribute.T.type==Type::BOOL)){
        attribute.T.type=Type::INT;
    }else{
        attribute.T.type=Type::FLOAT;
    }
    //对于Const常量，在编译期求值
    attribute.V.ConstTag= addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;
     if(attribute.V.ConstTag){
        switch (addexp->attribute.T.type){
            //addexp 是 int型
            case Type::INT:
                switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=addexp->attribute.V.val.IntVal-mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.IntVal-mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=addexp->attribute.V.val.IntVal-mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            //addexp 是 float 型
            case Type::FLOAT:
                switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal-mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal-mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.FloatVal-mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            //addexp 是 bool 型
            case Type::BOOL:
                 switch (mulexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=addexp->attribute.V.val.BoolVal-mulexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=addexp->attribute.V.val.BoolVal-mulexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=addexp->attribute.V.val.BoolVal-mulexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void MulExp_mul::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();
 
    //先检查数据类型，不能为void
    if(mulexp->attribute.T.type==Type::VOID||unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("computing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置运算结果属性
    attribute.V.ConstTag= mulexp->attribute.V.ConstTag & unary_exp->attribute.V.ConstTag;
    if((mulexp->attribute.T.type==Type::INT||mulexp->attribute.T.type==Type::BOOL) && (unary_exp->attribute.T.type==Type::INT||unary_exp->attribute.T.type==Type::INT)){
        attribute.T.type=Type::INT;
    }else{
        attribute.T.type=Type::FLOAT;
    }
    //对于Const常量，在编译期求值
    if(attribute.V.ConstTag){
        switch (mulexp->attribute.T.type){
            //mulexp 是 int型
            case Type::INT:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.IntVal*unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.IntVal*unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.IntVal*unary_exp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //mulexp 是 float 型
            case Type::FLOAT:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal*unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal*unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal*unary_exp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            case Type::BOOL:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.BoolVal*unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.BoolVal*unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.BoolVal*unary_exp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void MulExp_div::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    //先检查数据类型，不能为void
    if(mulexp->attribute.T.type==Type::VOID||unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("computing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //判断被除数是否是0
    if(unary_exp->attribute.V.ConstTag){
        if(unary_exp->attribute.T.type==Type::INT){
            if(unary_exp->attribute.V.val.IntVal==0){
                error_msgs.push_back("Divisor is 0 in line " + std::to_string(line_number) + "\n");
                return;
            }
        }else if(unary_exp->attribute.T.type==Type::FLOAT){
            if(unary_exp->attribute.V.val.FloatVal==0.0){
                error_msgs.push_back("Divisor is 0 in line " + std::to_string(line_number) + "\n");
                return;
            }
        }else if(unary_exp->attribute.T.type==Type::BOOL){
            if(!unary_exp->attribute.V.val.FloatVal){
                error_msgs.push_back("Divisor is 0 in line " + std::to_string(line_number) + "\n");
                return;
            }
        }
    }
    //设置运算结果属性
    attribute.V.ConstTag= mulexp->attribute.V.ConstTag & unary_exp->attribute.V.ConstTag;
    if((mulexp->attribute.T.type==Type::INT||mulexp->attribute.T.type==Type::BOOL) && (unary_exp->attribute.T.type==Type::INT||unary_exp->attribute.T.type==Type::INT)){
        attribute.T.type=Type::INT;
    }else{
        attribute.T.type=Type::FLOAT;
    }
    //对于Const常量，在编译期求值
    if(attribute.V.ConstTag){
        switch (mulexp->attribute.T.type){
            //mulexp 是 int型
            case Type::INT:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.IntVal/unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.IntVal/unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.IntVal;//bool非0，一定是1
                    default:
                        break;
                }
                break;
            //mulexp 是 float 型
            case Type::FLOAT:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal/unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal/unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.FloatVal;//bool非0，一定是1
                        break;
                    default:
                        break;
                }
                break;
            case Type::BOOL:
                switch (unary_exp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.BoolVal/unary_exp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.FloatVal=mulexp->attribute.V.val.BoolVal/unary_exp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.IntVal=mulexp->attribute.V.val.BoolVal;//bool非0，一定是1
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void MulExp_mod::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    //检查数据类型，不能为void
    if(mulexp->attribute.T.type==Type::VOID||unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("computing unit is invalid in line " + std::to_string(line_number) + "\n");
        return;
    }
    //检查数据类型，float不能参与mod运算
    if(mulexp->attribute.T.type==Type::FLOAT||unary_exp->attribute.T.type==Type::FLOAT){
        error_msgs.push_back("float cannot participate in modulus operations in line " + std::to_string(line_number) + "\n");
        return;
    }
    //设置运算结果数据类型
    attribute.T.type=Type::INT;
    //如果是常量，在编译期求值
    attribute.V.ConstTag= mulexp->attribute.V.ConstTag & unary_exp->attribute.V.ConstTag;
    if(attribute.V.ConstTag){
        if(unary_exp->attribute.T.type==Type::INT){
            if(unary_exp->attribute.V.val.IntVal==0){
                error_msgs.push_back("Modulus is 0 in line " + std::to_string(line_number) + "\n");
                return;
            }
            if(mulexp->attribute.T.type==Type::INT){
                attribute.V.val.IntVal=mulexp->attribute.V.val.IntVal % unary_exp->attribute.V.val.IntVal;
            }else if(mulexp->attribute.T.type==Type::BOOL){
                attribute.V.val.IntVal=mulexp->attribute.V.val.BoolVal % unary_exp->attribute.V.val.IntVal;
            }
        }else if(unary_exp->attribute.T.type==Type::BOOL){
            if(!unary_exp->attribute.V.val.BoolVal){
                error_msgs.push_back("Modulus is 0 in line " + std::to_string(line_number) + "\n");
                return;
            }
            attribute.V.val.IntVal=0;//模数非0，只能是1.  x%1=0
        }
        
    }
}

//******************* 关系运算表达式
//  <=
void RelExp_leq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    //检查数据类型，不能为void
    if(relexp->attribute.T.type==Type::VOID||addexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (relexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal <= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal <= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal <= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal <= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal <= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal <= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 bool 型
            case Type::BOOL:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal <= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal <= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal <= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
//  <
void RelExp_lt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    //检查数据类型，不能为void
    if(relexp->attribute.T.type==Type::VOID||addexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (relexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal < addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal < addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal < addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal < addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal < addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal < addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 bool 型
            case Type::BOOL:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal < addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal < addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal < addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
// >=
void RelExp_geq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    //检查数据类型，不能为void
    if(relexp->attribute.T.type==Type::VOID||addexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (relexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal >= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal >= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal >= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal >= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal >= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal >= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 bool 型
            case Type::BOOL:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal >= addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal >= addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal >= addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
// >
void RelExp_gt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    //检查数据类型，不能为void
    if(relexp->attribute.T.type==Type::VOID||addexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (relexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal > addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal > addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.IntVal > addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal > addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal > addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.FloatVal > addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 bool 型
            case Type::BOOL:
                switch (addexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal > addexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal > addexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=relexp->attribute.V.val.BoolVal > addexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
// ==
void EqExp_eq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    //检查数据类型，不能为void
    if(eqexp->attribute.T.type==Type::VOID||relexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=eqexp->attribute.V.ConstTag & relexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (eqexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal == relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal == relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal == relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal == relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal == relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal == relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::BOOL:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal == relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal == relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal == relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
// !=
void EqExp_neq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    //检查数据类型，不能为void
    if(eqexp->attribute.T.type==Type::VOID||relexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("comparing unit is invalid in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=eqexp->attribute.V.ConstTag & relexp->attribute.V.ConstTag;
    //若为Const，在编译期求值
    if(attribute.V.ConstTag){
        switch (eqexp->attribute.T.type){
            //relexp 是 int型
            case Type::INT:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal != relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal != relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.IntVal != relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::FLOAT:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal != relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal != relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.FloatVal != relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
                break;
            //relexp 是 float 型
            case Type::BOOL:
                switch (relexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal != relexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal != relexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=eqexp->attribute.V.val.BoolVal != relexp->attribute.V.val.BoolVal;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
//********************** 逻辑运算
// &&
void LAndExp_and::TypeCheck() {
    landexp->TypeCheck();
    eqexp->TypeCheck();
    //检查数据类型，不能为空
    if(landexp->attribute.T.type==Type::VOID||eqexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("condition unit must be boolen type in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=landexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;
    if(attribute.V.ConstTag){
        switch(landexp->attribute.T.type){
            case Type::INT:
                switch(eqexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.IntVal && eqexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.IntVal && eqexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.IntVal && eqexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            case Type::FLOAT:
                switch(eqexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.FloatVal && eqexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.FloatVal && eqexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.FloatVal && eqexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            case Type::BOOL:
                switch(eqexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.BoolVal && eqexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.BoolVal && eqexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=landexp->attribute.V.val.BoolVal && eqexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}
// ||
void LOrExp_or::TypeCheck() {
    lorexp->TypeCheck();
    landexp->TypeCheck();
    //检查数据类型，不能为空
    if(landexp->attribute.T.type==Type::VOID||lorexp->attribute.T.type==Type::VOID){
        error_msgs.push_back("condition unit must be boolen type in line " + std::to_string(line_number) + "\n");
    }
    //设置结果返回类型
    attribute.T.type=Type::BOOL;
    //标注ConstTag
    attribute.V.ConstTag=lorexp->attribute.V.ConstTag & landexp->attribute.V.ConstTag;
    if(attribute.V.ConstTag){
        switch(lorexp->attribute.T.type){
            case Type::INT:
                switch(landexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.IntVal || landexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.IntVal || landexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.IntVal || landexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            case Type::FLOAT:
                switch(landexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.FloatVal || landexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.FloatVal || landexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.FloatVal || landexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
                break;
            case Type::BOOL:
                switch(landexp->attribute.T.type){
                    case Type::INT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.BoolVal || landexp->attribute.V.val.IntVal;
                        break;
                    case Type::FLOAT:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.BoolVal || landexp->attribute.V.val.FloatVal;
                        break;
                    case Type::BOOL:
                        attribute.V.val.BoolVal=lorexp->attribute.V.val.BoolVal || landexp->attribute.V.val.BoolVal;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

//*****************************   Lval  ********************************* */

void Lval::TypeCheck() { 
    scope=semant_table.symbol_table.get_current_scope();
    is_left=false;//此处参考框架代码SysY：使用is_left区分赋值语句中的左值和普通的表达式引用
    //检查数组维度是否是int,constexp（在编译期求值）
    //indexs收集数组索引
    std::vector<int>indexs{};
    if(dims!=nullptr){
        for(auto dim:*dims){
            dim->TypeCheck();
            // if(dim->attribute.T.type!=Type::INT){
            //     error_msgs.push_back("Array dims muast be int value in line " + std::to_string(line_number) + "\n");
            // }
            // if(!dim->attribute.V.ConstTag){
            //     error_msgs.push_back("Array dims must be const expression in line " + std::to_string(line_number) + "\n");
            // }
            indexs.push_back(dim->attribute.V.val.IntVal);
        }
    }
    //判断是否未定义
    //若为Const，需要在编译期求值。则从全局/局部的symbol_table中取出该lval的attribute，传递信息
    VarAttribute attr;
    if(semant_table.symbol_table.lookup_scope(name)!=-1){//局部
        attr=semant_table.symbol_table.lookup_val(name);
    }else if(semant_table.GlobalTable.find(name)!=semant_table.GlobalTable.end()){//全局
        attr=semant_table.GlobalTable[name];
    }else{
         error_msgs.push_back("undefined lval in line " + std::to_string(line_number) + "\n");
         return;
    }

    // 标记信息：值
    attribute.V.ConstTag=attr.ConstTag; 
    dimsDeclined=*(&attr.dims); //标记声明时的数组维度（数值为空向量）
    if(attr.ConstTag){ 
        //lval不是数组 m,n
        if(attr.dims.size()==0){
            if(attr.type==Type::INT){
                attribute.V.val.IntVal=attr.IntInitVals[0];
            }else if(attr.type==Type::FLOAT){
                attribute.V.val.FloatVal=attr.FloatInitVals[0];
            }
        }//lval是数组 a,a[1],a[1][0],a[3][2][1]
        else{//lval是数组值
            attribute.V.ConstTag=false;//////////////数组的ConstTag是false,使得引用时load
            if(indexs.size()==attr.dims.size()){
                AssignArrayValue(attribute, attr,indexs);
            }
        }
    }
    //标记信息：数组
    attribute.T.type=attr.type;
    if(indexs.size()<attr.dims.size()){
        attribute.T.type=Type::PTR;
    }
}


//*****************************   UnaryExp  ********************************* */
void UnaryExp_plus::TypeCheck() { 
    unary_exp->TypeCheck();

    //运算值非空
    if(unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("invalid expression in line " + std::to_string(line_number) + "\n");
    }
    //标记属性
    attribute.V.ConstTag=unary_exp->attribute.V.ConstTag;
    if(unary_exp->attribute.T.type==Type::INT){
        attribute.T.type=Type::INT;
        attribute.V.val.IntVal=unary_exp->attribute.V.val.IntVal;
    }else if(unary_exp->attribute.T.type==Type::FLOAT){
        attribute.T.type=Type::FLOAT;
        attribute.V.val.FloatVal=unary_exp->attribute.V.val.FloatVal;
    }else if(unary_exp->attribute.T.type==Type::BOOL){//Bool -> Int 隐式转换
        attribute.T.type=Type::INT;
        attribute.V.val.IntVal=unary_exp->attribute.V.val.BoolVal;
    }
}

void UnaryExp_neg::TypeCheck() { 
    unary_exp->TypeCheck();

    //运算值非空
    if(unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("invalid expression in line " + std::to_string(line_number) + "\n");
    }
    //标记属性
    attribute.V.ConstTag=unary_exp->attribute.V.ConstTag;
    if(unary_exp->attribute.T.type==Type::INT){
        attribute.T.type=Type::INT;
        attribute.V.val.IntVal= -unary_exp->attribute.V.val.IntVal;
    }else if(unary_exp->attribute.T.type==Type::FLOAT){
        attribute.T.type=Type::FLOAT;
        attribute.V.val.FloatVal= -unary_exp->attribute.V.val.FloatVal;
    }else if(unary_exp->attribute.T.type==Type::BOOL){//Bool -> Int 隐式转换
        attribute.T.type=Type::INT;
        attribute.V.val.IntVal=-unary_exp->attribute.V.val.BoolVal;
    }
}

void UnaryExp_not::TypeCheck() { 
    unary_exp->TypeCheck();

    //运算值非空
    if(unary_exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("invalid expression in line " + std::to_string(line_number) + "\n");
    }
    //标记属性
    attribute.V.ConstTag=unary_exp->attribute.V.ConstTag;
    attribute.T.type=Type::BOOL;
    //隐式类型转换
    switch(unary_exp->attribute.T.type){
        case Type::BOOL:
            attribute.V.val.BoolVal=!unary_exp->attribute.V.val.BoolVal;
        case Type::INT:
            attribute.V.val.BoolVal=!unary_exp->attribute.V.val.IntVal;
        case Type::FLOAT:
            attribute.V.val.BoolVal=!unary_exp->attribute.V.val.FloatVal;
        default:
            break;
    }
}
void IntConst::TypeCheck() {
    attribute.T.type = Type::INT;
    attribute.V.ConstTag = true;
    attribute.V.val.IntVal = val;
}
void FloatConst::TypeCheck() {
    attribute.T.type = Type::FLOAT;
    attribute.V.ConstTag = true;
    attribute.V.val.FloatVal = val;
}
void StringConst::TypeCheck() {  }
//*****************************   PrimaryExp  ********************************* */
void PrimaryExp_branch::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}
//*****************************   Stmt  ********************************* */
//assign_stmt：一定非const，不需要在编译期求值
void assign_stmt::TypeCheck() { 
    lval->TypeCheck(); //已经证实了lval在符号表中
    exp->TypeCheck();
    ((Lval*)lval)->is_left=true;//此处参考框架代码SysY：使用is_left区分赋值语句中的左值和普通的表达式引用
    if(exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("expression to be assigned is invalid in line " + std::to_string(line_number) + "\n");
    }
}
void expr_stmt::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}
void block_stmt::TypeCheck() { 
    b->TypeCheck(); 
    attribute = b->attribute;
}
void ifelse_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("ifelse cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
    elsestmt->TypeCheck();
}

void if_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
}

void while_stmt::TypeCheck() { 
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("while cond type is invalid " + std::to_string(line_number) + "\n");
    }
    body->TypeCheck();
 }

void continue_stmt::TypeCheck() {}

void break_stmt::TypeCheck() {}

void return_stmt::TypeCheck() { 
    return_exp->TypeCheck(); 
    if (return_exp->attribute.T.type == Type::VOID) {
        error_msgs.push_back("not-void return type is invalid in line " + std::to_string(line_number) + "\n");
    }
}

void return_stmt_void::TypeCheck() {}

//*****************************   InitVal  ********************************* */
//数组的初始化经由ConstInitVal逐一调用ConstInitVal_exp
void ConstInitVal::TypeCheck() { 
    //初始化值不能没有
    if(initval==nullptr){
        error_msgs.push_back("Initval cannot be null in line " + std::to_string(line_number) + "\n");
    }
    //对初始化值挨个检验
    for(auto InitVal : *initval){
        InitVal->TypeCheck();
    }
 }

//非数组的初始化直接调用ConstInitVal_exp
void ConstInitVal_exp::TypeCheck() { 
    exp->TypeCheck();
    //检查类型非void
    if(exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("Initval is invalid in line " + std::to_string(line_number) + "\n");
    }
    //检查类型非const
    if(!exp->attribute.V.ConstTag){
        error_msgs.push_back("Const initval is not const value in line " + std::to_string(line_number) + "\n");
    }
    //标记属性
    attribute=exp->attribute;
 }

void VarInitVal::TypeCheck() {
    //初始化值不能没有
    if(initval==nullptr){
        error_msgs.push_back("Initval cannot be null in line " + std::to_string(line_number) + "\n");
    }
    //对初始化值挨个检验
    for(auto InitVal : *initval){
        InitVal->TypeCheck();
    }
}

void VarInitVal_exp::TypeCheck() {
    exp->TypeCheck();
    //检查类型非void
    if(exp->attribute.T.type==Type::VOID){
        error_msgs.push_back("Initval is invalid in line " + std::to_string(line_number) + "\n");
    }
    //标记属性
    attribute=exp->attribute;
    if(!attribute.V.ConstTag){
        isLval=true;
    }
    
}

//**************************   VarDef  & ConstDef ****************************** */
void VarDef_no_init::TypeCheck() {}

void VarDef::TypeCheck() {}

void ConstDef::TypeCheck() { attribute.V.ConstTag=1;}

//**************************   VarDecl  & ConstDecl ****************************** */
void VarDecl::TypeCheck() { 

    for(auto def: *var_def_list){
        //设置scope
        def->scope=semant_table.symbol_table.get_current_scope();
        //检查变量是否重复声明
        if(semant_table.symbol_table.lookup_scope(def->get_name())==semant_table.symbol_table.get_current_scope()){
            error_msgs.push_back("multi-defination of "+ def->get_name()->get_string()+" in line " + std::to_string(line_number) + "\n");
        }

        /* 收集属性信息，登记symbol_table */
        //数组：维度+初始值
        if(def->DimsExist()){
            VarAttribute attr;
            attr.type=type_decl;  
            //attr.type=Type::PTR;
            attr.ConstTag=0;
            for(auto dim:def->get_dims()){//维度
                dim->TypeCheck();
                attr.dims.push_back(dim->attribute.V.val.IntVal);
            }
            if(def->has_initval){
                if(def->get_init()==nullptr){
                    error_msgs.push_back("Initval is invalid in line " + std::to_string(line_number) + "\n");
                    continue;
                }
                def->get_init()->TypeCheck();
                GatherArrayInitVals(attr,def->get_init()); //初始值
            }
            //登记入表symbol_table
            semant_table.symbol_table.add_Symbol(def->get_name(),attr);
            def->attribute.VarAttr=attr;//标记在语法树节点上，便于IR访问信息

        }//非数组：初始值
        else{
            VarAttribute attr;
            attr.type=type_decl;
            attr.ConstTag=0;
            semant_table.symbol_table.add_Symbol(def->get_name(),attr);
            def->attribute.VarAttr=attr;//标记在语法树节点上，便于IR访问信息

            if(def->has_initval){
                def->get_init()->TypeCheck();
                AssignNotArray(def->get_name(),def->get_init()->attribute,line_number);
            }
        }
    }
    
}

void ConstDecl::TypeCheck() { 

    for(auto def: *var_def_list){
        //设置scope
        def->scope=semant_table.symbol_table.get_current_scope();
        //检查变量是否重复声明
        if(semant_table.symbol_table.lookup_scope(def->get_name())==semant_table.symbol_table.get_current_scope()){
            error_msgs.push_back("Multi definations of "+ def->get_name()->get_string()+" in line " + std::to_string(line_number) + "\n");
        }

        def->TypeCheck();

        /* 收集属性信息，登记symbol_table */
        //数组：维度+初始值
        if(def->DimsExist()){
            VarAttribute attr;
            attr.type=type_decl;
            //attr.type=Type::PTR;  //int/float
            attr.ConstTag=1;
            for(auto dim:def->get_dims()){//维度
                dim->TypeCheck();
                attr.dims.push_back(dim->attribute.V.val.IntVal);
            }
            def->get_init()->TypeCheck();
            GatherArrayInitVals(attr,def->get_init()); //初始值
            //登记入表symbol_table
            semant_table.symbol_table.add_Symbol(def->get_name(),attr);
            def->attribute.VarAttr=attr;//标记在语法树节点上，便于IR访问信息

        }//非数组：初始值
        else{
            VarAttribute attr;
            attr.type=type_decl;
            attr.ConstTag=1;
            semant_table.symbol_table.add_Symbol(def->get_name(),attr);
            def->attribute.VarAttr=attr;//标记在语法树节点上，便于IR访问信息

            //Const 一定有初始化值
            def->get_init()->TypeCheck();
            if(!def->get_init()->attribute.V.ConstTag){
                error_msgs.push_back("Const defination not be initialized by const expression in line " + std::to_string(line_number) + "\n");
            }
            AssignNotArray(def->get_name(),def->get_init()->attribute,line_number);
        }
    }
}
//**************************   BlockItem  ****************************** */
void BlockItem_Decl::TypeCheck() { decl->TypeCheck(); }

void BlockItem_Stmt::TypeCheck() { stmt->TypeCheck(); }

void __Block::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    //遍历这个block作用域下的每一条语句item,逐一进行类型检查
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.exit_scope();
}
//*****************************   Func call  ********************************* */
void FuncRParams::TypeCheck() { }

void Func_call::TypeCheck() { 
 //检查函数是否未定义
    auto it=semant_table.FunctionTable.find(name);
    if(it==semant_table.FunctionTable.end()){
        error_msgs.push_back("An undefined function being called in line " + std::to_string(line_number) + "\n");
        return;
    }
    auto FuncDef=it->second;//函数定义
    attribute.T.type=FuncDef->return_type;

    //检查实参与形参的对应(个数相等，类型匹配,需要考虑隐式类型转换)
    std::vector<Expression> RParams{};
    if(((FuncRParams*)funcr_params)!=nullptr){
        RParams=*(((FuncRParams*)funcr_params)->params);//实参列表
    }

    //参数个数匹配检查
    if(FuncDef->formals->size()!=RParams.size()){
        error_msgs.push_back("The numbers of Formal and of Real Params dismatch in function call "+name->get_string()
        +" in line " + std::to_string(line_number) + "\n");
    }
    //参数类型匹配检查
    int length=FuncDef->formals->size()<=RParams.size()?FuncDef->formals->size():RParams.size();
        if(length>0){
            for(int i=0;i<length;i++){
                FuncFParam fParam=FuncDef->formals->at(i);//第i个形参
                Expression rParam=RParams[i];//第i个实参
                rParam->TypeCheck();
                
                //实参检查:存在、类型检查
                if(rParam->attribute.T.type==Type::VOID){
                    error_msgs.push_back("The real param is invalid in function call "+ name->get_string()
                    +" in line " + std::to_string(line_number) + "\n");
                    continue;
                }
                else /* 对于SysY运行时库中的getarray(),getfarray(),只需要检查实参是PTR*/ //因为它没有经过funcDef的类型检查，attribute.T.type未标记为PTR
                     /* 对于putarray(),putfarray(),只需要检查第二个实参是PTR*/
                    if(name->get_string()=="getarray"||name->get_string()=="getfarray"){
                        if(rParam->attribute.T.type!=Type::PTR){
                            error_msgs.push_back("The type of formal and of real params dismatch in function call "+ name->get_string()
                            +" in line " + std::to_string(line_number) + "\n");
                        }
        
                    }
                    else if(name->get_string()=="putarray"||name->get_string()=="putfarray"){
                        if(i==1 && rParam->attribute.T.type!=Type::PTR){
                            error_msgs.push_back("The type of formal and of real params dismatch in function call "+ name->get_string()
                            +" in line " + std::to_string(line_number) + "\n");
                        }
                        
                    }
                    else{
                        /* 对于其它函数，检查实参类型匹配*/
                        if((rParam->attribute.T.type==Type::PTR&&fParam->attribute.T.type!=Type::PTR)||(rParam->attribute.T.type!=Type::PTR&&fParam->attribute.T.type==Type::PTR)){
                            error_msgs.push_back("The type of formal and of real params dismatch in function call "+ name->get_string()
                            +" in line " + std::to_string(line_number) + "\n");
                        }
                    }
            }
        }
}
//**************************   Func def ****************************** */
//函数参数：检查标识符定义作用域、数组维度要求
//将函数形参信息加入symbol_table，并记录
//设置scope=1: int main（）是唯一scope=0的函数，无参；；函数定义不能嵌套，故函数scope=1
//数组的PTR与int/float如何区分： FParam->attribute.VarAttr.type-->int/float; FParam->attribute.T.type-->ptr
void __FuncFParam::TypeCheck() {
    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    scope = 1;

    // 如果dims为nullptr, 表示该变量不含数组下标, 如果你在语法分析中采用了其他方式处理，这里也需要更改
    if (dims != nullptr) {    
        auto dim_vector = *dims;//存储各个数组维度值的向量

        // the fisrt dim of FuncFParam is empty
        // eg. int f(int A[][30][40])
        val.dims.push_back(-1);    // 这里用-1表示empty，你也可以使用其他方式
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();
            // if (d->attribute.V.ConstTag == false) {//数组维度必须是常量表达式
            //     error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
            //                          "\n");
            // }
            // if (d->attribute.T.type == Type::FLOAT) {//数组维度必须是int型 
            //     error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            // }
            val.dims.push_back(d->attribute.V.val.IntVal);
        }
        attribute.T.type = Type::PTR; //数组视为指针类型
    } else {
        attribute.T.type = type_decl;
    }
    //对符号作用域进行检查
    if (name != nullptr) {
        if (semant_table.symbol_table.lookup_scope(name) != -1) {//在这个作用域内能找到
            error_msgs.push_back("multiple difinitions of formals in function of" + name->get_string() + " in line " +
                                 std::to_string(line_number) + "\n");
        }
        semant_table.symbol_table.add_Symbol(name, val);
        attribute.VarAttr=val;//标记在语法树节点上，便于IR访问信息
    }
}

//函数定义：
void __FuncDef::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    semant_table.FunctionTable[name] = this;

    //判断main函数存在【返回类型为int，无参】
    if(name->get_string()=="main"){//name 是一个Symbol，是指针类型，指向的类有成员name,接口get_string()
        if(return_type==Type::INT){
            main_exist=1;
        }
    }
    
    //对函数参数逐一进行类型检查
    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();
    }

    // block TypeCheck：对函数定义块内语句逐一进行类型检查
    if (block != nullptr) {
        auto item_vector = *(block->item_list);
        for (auto item : item_vector) {
            item->TypeCheck();
        }
    }

    semant_table.symbol_table.exit_scope();
}
//**************************   CompUnit   ****************************** */
void CompUnit_Decl::TypeCheck() { 
    //检查全局变量：是否未声明、是否重复定义

    //获取该声明的类型、定义列表
    Type::ty type=decl->get_type();
    std::vector<Def> def_list=decl->get_def();
    bool consttag=decl->const_tag;

    //遍历：对该声明中的每一个定义
    for(auto def:def_list){
        //设置scope=0
        def->scope=0;
        //检查该symbol是否重复定义
        if(semant_table.GlobalTable.find(def->get_name())!=semant_table.GlobalTable.end()){
            error_msgs.push_back("multiple global definitions of " + def->get_name()->get_string() 
         + " in line " +  std::to_string(line_number) + "\n");
        }

        def->TypeCheck();

        /* 初始化 */
        //非数组
        if(!def->DimsExist()){
            //标记属性
            VarAttribute varattribute;
            varattribute.type=type;
            varattribute.ConstTag=consttag;
            varattribute.InitValByType(type);

            //登记到全局symbol_table中
            semant_table.GlobalTable[def->get_name()]=varattribute;
            
            //收集初始化值
            if(def->has_initval){
                //initval不能为空
                if(def->get_init()==nullptr){
                    error_msgs.push_back("Initval is invalid in line " +  std::to_string(line_number) + "\n");
                }
                def->get_init()->TypeCheck();
                GlobalAssignNotArray(def->get_name(),def->get_init()->attribute,line_number);
            }
        }//数组
        else{
            //标记属性
            VarAttribute varattribute;
            varattribute.type=type;
            varattribute.ConstTag=consttag;
            //收集维度
            for(auto dim:def->get_dims()){
                dim->TypeCheck();
                // if(dim->attribute.T.type!=Type::INT){
                //     error_msgs.push_back("Array dim must be int value in line " +  std::to_string(line_number) + "\n");
                // }
                // if(!dim->attribute.V.ConstTag){
                //     error_msgs.push_back("Array dim must be const expression in line " +  std::to_string(line_number) + "\n");
                // }
                varattribute.dims.push_back(dim->attribute.V.val.IntVal);
            }
            //收集初始值
            if(def->has_initval){
                def->get_init()->TypeCheck();
                GatherArrayInitVals(varattribute,def->get_init());
            }
            //登记到全局symbol_table中
            semant_table.GlobalTable[def->get_name()]=varattribute;
        }

    }
}

void CompUnit_FuncDef::TypeCheck() { func_def->TypeCheck(); }

