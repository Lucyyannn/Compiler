%{
#include <fstream>
#include "SysY_tree.h"
#include "type.h"
Program ast_root;

void yyerror(char *s, ...);
int yylex();
int error_num = 0;
extern int line_number;
extern std::ofstream fout;
extern IdTable id_table;
%}
%union{
    char* error_msg;
    Symbol symbol_token;
    double float_token; // 对于SysY的浮点常量，我们需要先以double类型计算，再在语法树节点创建的时候转为float
    int int_token;
    Program program;  
    CompUnit comp_unit;  std::vector<CompUnit>* comps; 
    Decl decl;
    Def def;  std::vector<Def>* defs;
    FuncDef func_def;
    Expression expression;  std::vector<Expression>* expressions;
    Stmt stmt;
    Block block;
    InitVal initval;  std::vector<InitVal>* initvals;
    FuncFParam formal;   std::vector<FuncFParam>* formals;
    BlockItem block_item;   std::vector<BlockItem>* block_items;
}
//declare the terminals
%token <symbol_token> STR_CONST IDENT
%token <float_token> FLOAT_CONST
%token <int_token> INT_CONST
%token LEQ GEQ EQ NE // <=   >=   ==   != 
%token AND OR // &&    ||
%token CONST IF ELSE WHILE NONE_TYPE INT FLOAT FOR 
%token RETURN BREAK CONTINUE ERROR 

//give the type of nonterminals
%type <program> Program
%type <comp_unit> CompUnit 
%type <comps> Comp_list
%type <decl> Decl VarDecl ConstDecl
%type <def> ConstDef VarDef
%type <defs> ConstDef_list VarDef_list 
%type <func_def> FuncDef 
%type <expression> Exp LOrExp AddExp MulExp RelExp EqExp LAndExp UnaryExp PrimaryExp
%type <expression> ConstExp Lval FuncRParams Cond
%type <expression> IntConst FloatConst 
%type <expression> ArrayDim //新增
%type <expressions> ArrayDim_list Exp_list; //新增ArrayDim_list
%type <stmt> Stmt 
%type <block> Block
%type <block_item> BlockItem
%type <block_items> BlockItem_list
%type <initval> ConstInitVal VarInitVal  
%type <initvals> VarInitVal_list ConstInitVal_list
%type <formal> FuncFParam 
%type <formals> FuncFParams

// THEN和ELSE用于处理if和else的移进-规约冲突：else优先级高，移进
%precedence THEN
%precedence ELSE
%%
//********************* Program *********************************
Program 
:Comp_list
{
    @$ = @1;      //继承位置信息
    ast_root = new __Program($1);
    ast_root->SetLineNumber(line_number);
};
//******** Comp_list ：Decl声明和FuncDef函数定义*******************
Comp_list
:CompUnit {$$ = new std::vector<CompUnit>; ($$)->push_back($1);}
|Comp_list CompUnit {($1)->push_back($2); $$ = $1;};

CompUnit
:Decl {$$ = new CompUnit_Decl($1); $$->SetLineNumber(line_number);}
|FuncDef {$$ = new CompUnit_FuncDef($1); $$->SetLineNumber(line_number);};

//************* Decl: Var和Const *******************************
Decl
:VarDecl{$$ = $1; $$->SetLineNumber(line_number);}
|ConstDecl{$$ = $1; $$->SetLineNumber(line_number);};

//Var
VarDecl
:INT VarDef_list ';'{$$ = new VarDecl(Type::INT,$2); $$->SetLineNumber(line_number);}
| FLOAT VarDef_list ';'{$$ = new VarDecl(Type::FLOAT,$2);$$->SetLineNumber(line_number);};

VarDef_list
: VarDef {$$ = new std::vector<Def>;($$)->push_back($1);}
| VarDef_list ',' VarDef{($1)->push_back($3);$$ = $1;};

//Const
ConstDecl
:CONST INT ConstDef_list ';'{$$ = new ConstDecl(Type::INT,$3); $$->SetLineNumber(line_number);}
|CONST FLOAT ConstDef_list ';'{$$ = new ConstDecl(Type::FLOAT,$3);$$->SetLineNumber(line_number);};
 
ConstDef_list
:ConstDef {$$ = new std::vector<Def>; ($$)->push_back($1);}
| ConstDef_list ',' ConstDef{($1)->push_back($3);$$ = $1;};


//数组维度表达式（支持多维,维度必须为常量）
ArrayDim
: '[' ConstExp ']' {$$=$2;$$->SetLineNumber(line_number);};

ArrayDim_list
:ArrayDim{$$ = new std::vector<Expression>;$$->push_back($1); }
|ArrayDim_list ArrayDim{$$ = $1; $$->push_back($2);};

//******************** FuncDef 函数定义 ***************************
FuncDef
:INT IDENT '(' FuncFParams ')' Block
{
    $$ = new __FuncDef(Type::INT,$2,$4,$6);
    $$->SetLineNumber(line_number);
}
|INT IDENT '(' ')' Block
{
    $$ = new __FuncDef(Type::INT,$2,new std::vector<FuncFParam>(),$5); 
    $$->SetLineNumber(line_number);
}//返回FLOAT类型的
|FLOAT IDENT '(' FuncFParams ')' Block
{
    $$ = new __FuncDef(Type::FLOAT,$2,$4,$6);
    $$->SetLineNumber(line_number);
}
|FLOAT IDENT '(' ')' Block
{
    $$ = new __FuncDef(Type::FLOAT,$2,new std::vector<FuncFParam>(),$5); 
    $$->SetLineNumber(line_number);
}//返回VOID类型的
|NONE_TYPE IDENT '(' FuncFParams ')' Block
{
    $$ = new __FuncDef(Type::VOID,$2,$4,$6);
    $$->SetLineNumber(line_number);
}
|NONE_TYPE IDENT '(' ')' Block
{
    $$ = new __FuncDef(Type::VOID,$2,new std::vector<FuncFParam>(),$5); 
    $$->SetLineNumber(line_number);
}
;
//*************** 变量定义Def：Var和const ************************
VarDef
:IDENT '=' VarInitVal//包括INT和FLOAT了
{   $$ = new VarDef($1,nullptr,$3);
    $$->SetLineNumber(line_number);}
|IDENT
{   $$ = new VarDef_no_init($1,nullptr); 
    $$->SetLineNumber(line_number);}
//数组
|IDENT ArrayDim_list {
    $$ = new VarDef_no_init($1, $2); 
    $$->SetLineNumber(line_number);
}
|IDENT ArrayDim_list '=' VarInitVal {
    $$ = new VarDef($1, $2, $4); 
    $$->SetLineNumber(line_number);
};   


ConstDef
:IDENT '=' ConstInitVal //包括INT和FLOAT，CONST声明时必须初始化
{   $$ = new ConstDef($1,nullptr,$3); 
    $$->SetLineNumber(line_number);}
|IDENT ArrayDim_list '=' ConstInitVal {
    $$ = new ConstDef($1, $2, $4); 
    $$->SetLineNumber(line_number);
};
//*************** 变量初始化列表：Var和const ************************

ConstInitVal_list
:ConstInitVal{$$ = new std::vector<InitVal>;($$)->push_back($1);}
|ConstInitVal_list ',' ConstInitVal{($1)->push_back($3); $$ = $1;};

ConstInitVal
:ConstExp{ //单值初始化
    $$ = new ConstInitVal_exp($1); 
    $$->SetLineNumber(line_number);}
|'{' ConstInitVal_list '}'{ //初始化列表
    $$ = new ConstInitVal($2);
     $$->SetLineNumber(line_number);}
|'{' '}'{//初始化为空
    $$ = new ConstInitVal(new std::vector<InitVal>()); 
    $$->SetLineNumber(line_number);};

VarInitVal_list
:VarInitVal{$$ = new std::vector<InitVal>;($$)->push_back($1);}
|VarInitVal_list ',' VarInitVal{($1)->push_back($3);$$ = $1;};

VarInitVal
:Exp{ //单值初始化(支持 int b=a ；故不为ConstExp)
    $$ = new VarInitVal_exp($1); 
    $$->SetLineNumber(line_number);}
|'{' VarInitVal_list '}'{ //初始化列表
    $$ = new VarInitVal($2);
     $$->SetLineNumber(line_number);}
|'{' '}'{
    $$ = new VarInitVal(new std::vector<InitVal>()); 
    $$->SetLineNumber(line_number);};

//******************* 函数参数列表 ************************

FuncFParams
:FuncFParam{$$ = new std::vector<FuncFParam>;($$)->push_back($1);}
|FuncFParams ',' FuncFParam{($1)->push_back($3);$$ = $1;};

FuncFParam
:INT IDENT{
    $$ = new __FuncFParam(Type::INT,$2,nullptr);
    $$->SetLineNumber(line_number);
}//FLOAT
|FLOAT IDENT{
    $$ = new __FuncFParam(Type::FLOAT,$2,nullptr);
    $$->SetLineNumber(line_number);
}//数组
|INT IDENT '['  ']' {
    std::vector<Expression>* temp = new std::vector<Expression>;
    temp->push_back(nullptr);
    $$ = new __FuncFParam(Type::INT,$2,temp);
    $$->SetLineNumber(line_number);
}
|FLOAT IDENT '['  ']' {
    std::vector<Expression>* temp = new std::vector<Expression>;
    temp->push_back(nullptr);
    $$ = new __FuncFParam(Type::FLOAT,$2,temp);
    $$->SetLineNumber(line_number);
}
|INT IDENT '[' ']' ArrayDim_list{
    $5->insert($5->begin(),nullptr);
    $$ = new __FuncFParam(Type::INT,$2,$5);
    $$->SetLineNumber(line_number);
}
|FLOAT IDENT '[' ']' ArrayDim_list{
    $5->insert($5->begin(),nullptr);
    $$ = new __FuncFParam(Type::FLOAT,$2,$5);
    $$->SetLineNumber(line_number);
}
;

//*************************** 语句块 Block ************************
//语句块
Block
:'{' BlockItem_list '}'{$$ = new __Block($2);$$->SetLineNumber(line_number);}
|'{' '}'{$$ = new __Block(new std::vector<BlockItem>);$$->SetLineNumber(line_number);};

//语句列表
BlockItem_list
:BlockItem{$$ = new std::vector<BlockItem>;($$)->push_back($1);}
|BlockItem_list  BlockItem{($1)->push_back($2);$$ = $1;};

//语句:Decl声明式；Stmt语句
BlockItem
:Decl{$$ = new BlockItem_Decl($1);$$->SetLineNumber(line_number);}
|Stmt{$$ = new BlockItem_Stmt($1);$$->SetLineNumber(line_number);};

//****************************  Stmt  ****************************
Stmt
:Lval '=' Exp ';'{//赋值
    $$ = new assign_stmt($1,$3);
    $$->SetLineNumber(line_number);
}//表达
|Exp ';'{
    $$ = new expr_stmt($1);
    $$->SetLineNumber(line_number);
}//空表达式
|';'{
    $$ = new null_stmt();
    $$->SetLineNumber(line_number);
}//语句块
|Block{
    $$ = new block_stmt($1);
    $$->SetLineNumber(line_number);
}//if 
|IF '(' Cond ')' Stmt %prec THEN{
    $$ = new if_stmt($3,$5);
    $$->SetLineNumber(line_number);
}//if else 
|IF '(' Cond ')' Stmt ELSE Stmt{
    $$ = new ifelse_stmt($3,$5,$7);
    $$->SetLineNumber(line_number);
}//while循环
|WHILE '(' Cond ')' Stmt{
    $$ = new while_stmt($3,$5);
    $$->SetLineNumber(line_number);
}//break
|BREAK ';'{
    $$ = new break_stmt();
    $$->SetLineNumber(line_number);
}//continue
|CONTINUE ';'{
    $$ = new continue_stmt();
    $$->SetLineNumber(line_number);
}//有返回值的return
|RETURN Exp ';'{
    $$ = new return_stmt($2);
    $$->SetLineNumber(line_number);
}//无返回值的return
|RETURN ';'{
    $$ = new return_stmt_void();
    $$->SetLineNumber(line_number);
};

//********************  表达式Exp 和 条件Cond **************************
//******************** Exp
Exp
:AddExp{ //"+""-"类 它包含了更高级别的运算
    $$ = $1; 
    $$->SetLineNumber(line_number);
};

ConstExp
:AddExp{$$ = $1;$$->SetLineNumber(line_number);};

Exp_list
:Exp{$$ = new std::vector<Expression>;($$)->push_back($1);}
|Exp_list ',' Exp{($1)->push_back($3);$$ = $1;};

MulExp
:UnaryExp{$$ = $1;$$->SetLineNumber(line_number);} // 乘除模
|MulExp '*' UnaryExp{$$ = new MulExp_mul($1,$3); $$->SetLineNumber(line_number);}
|MulExp '/' UnaryExp{$$ = new MulExp_div($1,$3); $$->SetLineNumber(line_number);}
|MulExp '%' UnaryExp{$$ = new MulExp_mod($1,$3); $$->SetLineNumber(line_number);}

AddExp
:MulExp{$$ = $1;$$->SetLineNumber(line_number);} //加减
|AddExp '+' MulExp{$$ = new AddExp_plus($1,$3); $$->SetLineNumber(line_number);}
|AddExp '-' MulExp{$$ = new AddExp_sub($1,$3); $$->SetLineNumber(line_number);};

//******************** Cond
Cond
:LOrExp{//"||"  它包含了更高级别的逻辑
    $$ = $1; 
    $$->SetLineNumber(line_number);
};
RelExp 
:AddExp{$$ = $1;$$->SetLineNumber(line_number);} //> < >= <=
|RelExp '<' AddExp{$$ = new RelExp_lt($1,$3); $$->SetLineNumber(line_number);}
|RelExp '>' AddExp{$$ = new RelExp_gt($1,$3); $$->SetLineNumber(line_number);}
|RelExp LEQ AddExp{$$ = new RelExp_leq($1,$3); $$->SetLineNumber(line_number);}
|RelExp GEQ AddExp{$$ = new RelExp_geq($1,$3); $$->SetLineNumber(line_number);};

EqExp
:RelExp{$$ = $1;$$->SetLineNumber(line_number);} // == != > < >= <=
|EqExp EQ RelExp{$$ = new EqExp_eq($1,$3); $$->SetLineNumber(line_number);}
|EqExp NE RelExp{$$ = new EqExp_neq($1,$3); $$->SetLineNumber(line_number);};

LAndExp
:EqExp{$$ = $1;$$->SetLineNumber(line_number);} //与
|LAndExp AND EqExp{$$ = new LAndExp_and($1,$3); $$->SetLineNumber(line_number);}; 

LOrExp
:LAndExp{$$ = $1;$$->SetLineNumber(line_number);}//或 (与比或的优先级高)
|LOrExp OR LAndExp{$$ = new LOrExp_or($1,$3); $$->SetLineNumber(line_number);};

//********************** PrimaryExp和UnaryExp*************************

//基本表达式
PrimaryExp
:IntConst{ //INT/FLOAT数值、左值表达式、（)的语句
    $$ = $1; 
    $$->SetLineNumber(line_number);}
|FloatConst{
    $$ = $1; 
    $$->SetLineNumber(line_number);}
|Lval{
    $$ = $1; 
    $$->SetLineNumber(line_number);}
|'(' Exp ')'{
    $$ = new PrimaryExp_branch($2);
    $$->SetLineNumber(line_number);
};

IntConst
:INT_CONST{$$ = new IntConst($1);$$->SetLineNumber(line_number);};

FloatConst
:FLOAT_CONST{$$ = new FloatConst($1);$$->SetLineNumber(line_number);};

Lval
:IDENT{//普通变量
    $$ = new Lval($1,nullptr);
    $$->SetLineNumber(line_number);
}
|IDENT ArrayDim_list{ //数组
    $$ = new Lval($1,$2);
    $$->SetLineNumber(line_number);};


//*********************** UnaryExp一元表达式 *****************************
UnaryExp
:PrimaryExp{
    $$ = $1;
}
|IDENT '(' FuncRParams ')'{
    $$ = new Func_call($1,$3);
    $$->SetLineNumber(line_number);
}
|IDENT '(' ')'{
    // 在sylib.h这个文件中,starttime()是一个宏定义
    // #define starttime() _sysy_starttime(__LINE__)
    // 我们在语法分析中将其替换为_sysy_starttime(line_number)
    // stoptime同理
    if($1->get_string() == "starttime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        $$ = new Func_call(id_table.add_id("_sysy_starttime"),temp);
        $$->SetLineNumber(line_number);
    }
    else if($1->get_string() == "stoptime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        $$ = new Func_call(id_table.add_id("_sysy_stoptime"),temp);
        $$->SetLineNumber(line_number);
    }
    else{
        $$ = new Func_call($1,nullptr);
        $$->SetLineNumber(line_number);
    }
}//正负号，负数的识别**
|'+' UnaryExp{ $$ = new UnaryExp_plus($2);$$->SetLineNumber(line_number);}
|'-' UnaryExp{$$ = new UnaryExp_neg($2);$$->SetLineNumber(line_number);}
|'!' UnaryExp{$$ = new UnaryExp_not($2);$$->SetLineNumber(line_number);};

FuncRParams
:Exp_list{$$ = new FuncRParams($1);$$->SetLineNumber(line_number);};

%% 

void yyerror(char* s, ...)
{
    ++error_num;
    fout<<"parser error in line "<<line_number<<"\n";
}