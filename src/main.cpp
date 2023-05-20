//
// Created by sunteng on 2023/5/16.
//

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <AST.h>
#include "koopa.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include "raw.h"



using namespace std;


extern FILE  *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);


const string extract_koopa(unique_ptr<BaseAST> &ast)
{
    stringstream ss;
    streambuf* buffer = cout.rdbuf();
    std::cout.rdbuf(ss.rdbuf());
    ast->Dump();
    std::cout << endl;
    string s(ss.str());
    std::cout.rdbuf(buffer);
    return s;
}



int main(int argc, const char *argv[]) {

    cout << "ddddd" << endl;
    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    // 输出解析得到的 AST, 其实就是个字符串
    // dump AST
//    freopen(output,"wr",stdout);
//    ast->Dump();
//    cout << endl;




        FILE *fp = fopen(output,"w");
        if(strcmp(mode,"-koopa")==0)
        {
            const auto &koopa = extract_koopa(ast);
            fputs(koopa.c_str(),fp);
        }

    cout << "mode " << mode << endl;
        if(strcmp(mode,"-riscv")==0)
        {
            //生成ast
            const auto koopa = extract_koopa(ast);

            //解析字符串 str,得到Koopa IR程序
            koopa_program_t program;
            koopa_error_code_t ret_koopa = koopa_parse_from_string(koopa.c_str(),&program);
            assert(ret_koopa == KOOPA_EC_SUCCESS);//确保解析没有错误
            koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
            //讲Koopa IR 程序转换成raw program
            koopa_raw_program_t raw = koopa_build_raw_program(builder,program);
            //释放Koopa IR 程序占用内存
            koopa_delete_program(program);


            stringstream ss;
            streambuf* buffer = cout.rdbuf();
            std::cout.rdbuf(ss.rdbuf());
            Visit(raw);
            std::cout << endl;
            string s(ss.str());
            std::cout.rdbuf(buffer);
            fputs(s.c_str(),fp);
        }


    return 0;
}