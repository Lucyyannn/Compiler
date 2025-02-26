#include "../include/symtab.h"

/*
添加符号：如果存在就返回symbol，否则新建并返回symbol
*/
Symbol IdTable::add_id(std::string s) {
    auto it = id_table.find(s);
    if (it == id_table.end()) {
        Symbol new_symbol = new Entry(s);
        id_table[s] = new_symbol;
        return new_symbol;
    } else {
        return id_table[s];
    }
}

void SymbolTable::add_Symbol(Symbol C, VarAttribute val) { symbol_table[current_scope][C] = val; }


Type::ty SymbolTable::lookup_type(Symbol C) {
    for (int i = current_scope; i >= 0; --i) {
        auto it = symbol_table[i].find(C);
        if (it != symbol_table[i].end()) {
            return it->second.type;
        }
    }
    return Type::VOID;
}


int SymbolTable::lookup_scope(Symbol C) {
    for (int i = current_scope; i >= 0; --i) {
        auto it = symbol_table[i].find(C);
        if (it != symbol_table[i].end()) {
            return i;
        }
    }
    return -1;
}


VarAttribute SymbolTable::lookup_val(Symbol C) {
    for (int i = current_scope; i >= 0; --i) {
        auto it = symbol_table[i].find(C);
        if (it != symbol_table[i].end()) {
            return it->second;
        }
    }
    return VarAttribute();
}

VarAttribute& SymbolTable::get_val(const Symbol C){
    for (int i = current_scope; i >= 0; --i) {
        auto it = symbol_table[i].find(C);
        if (it != symbol_table[i].end()) {
            return it->second;
        }
    }
    throw std::runtime_error("Symbol not found");  // 抛出异常
}


void SymbolTable::enter_scope() {
    ++current_scope;
    symbol_table.push_back(std::map<Symbol, VarAttribute>());
}

void SymbolTable::exit_scope() {
    --current_scope;
    symbol_table.pop_back();
}

/*
以下是reg的
*/
int SymbolRegTable::lookup(Symbol C) {
    for (int i = current_scope; i >= 0; --i) {
        auto it = symbol_table[i].find(C);
        if (it != symbol_table[i].end()) {
            return it->second;
        }
    }
    return -1;
}

void SymbolRegTable::add_Symbol(Symbol C, int val) { symbol_table[current_scope][C] = val; }

void SymbolRegTable::enter_scope() {
    ++current_scope;
    symbol_table.push_back(std::map<Symbol, int>());
    ArrayAttribute.push_back(std::map<Symbol,VarAttribute>());
}

void SymbolRegTable::exit_scope() {
    --current_scope;
    symbol_table.pop_back();
    ArrayAttribute.pop_back();
}

//新增：存放函数形参定义的数组，利用VarAttribute主要存放声明时的维度
/* 为什么要区别函数形参定义的数组？
        lval引用时需要访问数组，getelementptr用到声明时的数组维度dimsDeclined、访问时的索引indexs
        getelementptr的前置偏移量，普通数组自添加0；函数形参直接取indexs[0]。

*/
VarAttribute SymbolRegTable::lookup_val(Symbol C) {
    for (int i = current_scope; i >= 0; --i) {
        auto it = ArrayAttribute[i].find(C);
        if (it != ArrayAttribute[i].end()) {
            return it->second;
        }
    }
    return VarAttribute();

}

void SymbolRegTable::add_Array_Val(Symbol C,VarAttribute& val){
    ArrayAttribute[current_scope][C]=val;
}
