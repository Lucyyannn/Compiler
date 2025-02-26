%option noyywrap
%x COMMENT

%{
#include <cmath>
#include <vector>
#include <cstring>
#include "../include/symtab.h"
#include "../parser/SysY_parser.tab.h"

extern int line_number; 
extern int col_number; 
extern int cur_col_number; 
extern IdTable id_table;
void yyerror(char* s, ...);
%}

%%
    /* TODO():增加处理注释的代码*/
    /* TODO():增加处理列号的代码(cur_col_number表示当前token开始位置, col_number表示当前token结束位置) */
    /*处理单行注释*/
"//".* {/*什么都不做就行了*/}

    /*处理多行注释*/
"/*" {cur_col_number=col_number; col_number+=yyleng; BEGIN(COMMENT);}
<COMMENT>"*/" {cur_col_number=col_number;  col_number+=yyleng;  BEGIN(INITIAL);}
<COMMENT>[^*\n]* {cur_col_number=col_number; col_number+=yyleng;} /*匹配不包含 * 和换行符 \n 之外的任何字符串*/
<COMMENT>"*" {cur_col_number=col_number; ++col_number;}
<COMMENT>\n {++line_number; col_number = 0;}

"<=" { cur_col_number=col_number; col_number+=yyleng;return LEQ;}         
">=" { cur_col_number=col_number; col_number+=yyleng;return GEQ;}         
"==" { cur_col_number=col_number; col_number+=yyleng;return EQ;}        
"!=" { cur_col_number=col_number; col_number+=yyleng;return NE;}
"&&" { cur_col_number=col_number; col_number+=yyleng;return AND;}         
"||" { cur_col_number=col_number; col_number+=yyleng;return OR;}               
"const" { cur_col_number=col_number; col_number+=yyleng;return CONST;}     
"if" { cur_col_number=col_number; col_number+=yyleng;return IF;}       
"else" { cur_col_number=col_number; col_number+=yyleng;return ELSE;}  
"while" { cur_col_number=col_number; col_number+=yyleng;return WHILE;}
"void" { cur_col_number=col_number; col_number+=yyleng;return NONE_TYPE;}    
"int" { cur_col_number=col_number; col_number+=yyleng;return INT;}    
"float" { cur_col_number=col_number; col_number+=yyleng;return FLOAT;}      
"return" { cur_col_number=col_number; col_number+=yyleng;return RETURN;}    
"break" { cur_col_number=col_number; col_number+=yyleng;return BREAK;}     
"continue" { cur_col_number=col_number; col_number+=yyleng;return CONTINUE;}


\n {++line_number;col_number = 0;}

[ \t\f\r\v] {cur_col_number=col_number; col_number += yyleng;}

[\.\+\-\*\/\=\<\!\%\>] {cur_col_number=col_number; col_number += yyleng;return yytext[0];}

[\{\}\;\(\)\,\[\]] {cur_col_number=col_number; col_number += yyleng;return yytext[0];}

[_a-zA-Z][_a-zA-Z0-9]* {
    cur_col_number = col_number;
    col_number += yyleng;
    yylval.symbol_token = id_table.add_id(std::string(yytext));
    return IDENT;
}
([1-9][0-9]*)|0 {
    cur_col_number = col_number;
    col_number += yyleng;
    // yylval.int_token = stoi(std::string(yytext));
    yylval.int_token = 0;
    for(int i = 0;yytext[i];i++){
        yylval.int_token *= 10;
        yylval.int_token += yytext[i] - '0';
    }
    return INT_CONST;
}
    /*
      在词法分析中，你只需要考虑yylval的三种类型，分别为int_token, float_token, symbol_token
      之所以要定义这三种类型，是因为我们只返回这个token的类型是不够的，我们还需要知道这个token的属性
      例如我们需要知道INT_CONST具体的值是多少
    */
    /*TODO():参考SysY2022定义, 处理更多的词法, 如浮点数，十六进制数等*/

    /*一般十进制浮点数*/
((([0-9]+\.[0-9]*)|(\.[0-9]+))([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+) {
    cur_col_number = col_number;
    col_number += yyleng;

    double result = 0.0;
    double fraction = 0.1;
    int is_fraction = 0;
    int exponent = 0;
    int exp_sign = 1;

    for (int i = 0; yytext[i]; i++) {
        if (yytext[i] >= '0' && yytext[i] <= '9') {
            if (!is_fraction) {
                result = result * 10 + (yytext[i] - '0'); /*计算整数部分*/
            } else {
                result += (yytext[i] - '0') * fraction;  /*计算浮点数部分*/
                fraction *= 0.1;
            }
        } else if (yytext[i] == '.') {
            is_fraction = 1;
        } else if (yytext[i] == 'e' || yytext[i] == 'E') {
            /*进入指数计算部分*/
            i++;
            if (yytext[i] == '-') {
                exp_sign = -1;
                i++;
            } else if (yytext[i] == '+') {
                i++;
            }
            while (yytext[i] >= '0' && yytext[i] <= '9') {
                exponent = exponent * 10 + (yytext[i] - '0');
                i++;
            }
            break;
        }
    }

    result *= pow(10, exponent * exp_sign);
    
    yylval.float_token = result;
    return FLOAT_CONST;
}


    /*十六进制浮点数*/
0[xX](((([0-9a-fA-F]+\.[0-9a-fA-F]*)|(\.[0-9a-fA-F]+))([pP][+-]?[0-9]+)?)|([0-9a-fA-F]+[pP][+-]?[0-9]+)) {
    cur_col_number = col_number;
    col_number += strlen(yytext);

    double result = 0.0;
    double fraction = 1.0 / 16.0;
    int is_fraction = 0;
    int exponent = 0;
    int exp_sign = 1;
    int start = 2;  // Skip '0x' or '0X'

    for (int i = start; yytext[i]; i++) {
        if ((yytext[i] >= '0' && yytext[i] <= '9') || (yytext[i] >= 'a' && yytext[i] <= 'f') || (yytext[i] >= 'A' && yytext[i] <= 'F')) {
            int value;
            if (yytext[i] >= '0' && yytext[i] <= '9') {
                value = yytext[i] - '0';
            } else if (yytext[i] >= 'a' && yytext[i] <= 'f') {
                value = yytext[i] - 'a' + 10;
            } else {
                value = yytext[i] - 'A' + 10;
            }

            if (!is_fraction) {
                result = result * 16 + value;
            } else {
                result += value * fraction;
                fraction /= 16.0;
            }
        } else if (yytext[i] == '.') {
            is_fraction = 1;
        } else if (yytext[i] == 'p' || yytext[i] == 'P') {
            i++;
            if (yytext[i] == '-') {
                exp_sign = -1;
                i++;
            } else if (yytext[i] == '+') {
                i++;
            }
            while (yytext[i] >= '0' && yytext[i] <= '9') {
                exponent = exponent * 10 + (yytext[i] - '0');
                i++;
            }
            break;
        }
    }

    result *= pow(2.0, exponent * exp_sign);
    
    yylval.float_token = result;
    return FLOAT_CONST;
}



    /*十六进制整数*/
[0][xX][0-9a-fA-F]+ {
    cur_col_number = col_number; //计算列号
    col_number += yyleng;
    yylval.int_token = 0;        //计算值大小
    for (int i=2;yytext[i];i++){ 
        yylval.int_token *= 16;
        if (yytext[i]>='0'&&yytext[i]<='9'){
            yylval.int_token +=yytext[i]-'0';
        }else if(yytext[i]>='a'&&yytext[i]<='f'){
            yylval.int_token +=yytext[i]-'a'+10;
        }else{
            yylval.int_token +=yytext[i]-'A'+10;
        }
    }
    return INT_CONST;
}

    /*八进制整数*/
[0][0-7]+ {
    cur_col_number = col_number; //计算列号
    col_number += yyleng;
    yylval.int_token = 0;        //计算值大小
    for (int i=1;yytext[i];i++){
        yylval.int_token *= 8;
        yylval.int_token +=yytext[i]-'0';
    }
    return INT_CONST;
}



    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%