//
// Created by sunteng on 2023/5/16.
//

#pragma once
#include "/opt/include/koopa.h"
#include <cassert>
#include <cstdio>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


void Visit(const koopa_raw_program_t &);
void Visit(const koopa_raw_slice_t &);
void Visit(const koopa_raw_function_t &);
void Visit(const koopa_raw_basic_block_t &);
void Visit(const koopa_raw_value_t &);
void Visit(const koopa_raw_integer_t &);
void Visit(const koopa_raw_return_t &);



void Visit(const koopa_raw_program_t &program) {
    // 执行一些其他的必要操作
    // ...
    // 访问所有全局变量
    Visit(program.values);
    // 访问所有函数
    std::cout << " .text" << endl;
    Visit(program.funcs);

}



void Visit(const koopa_raw_slice_t &slice) {
    for (size_t i = 0; i < slice.len; ++i) {
        auto ptr = slice.buffer[i];
        // 根据 slice 的 kind 决定将 ptr 视作何种元素
        switch (slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                // 访问函数
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                // 访问基本块
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            case KOOPA_RSIK_VALUE:
                // 访问指令
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            default:
                // 我们暂时不会遇到其他内容, 于是不对其做任何处理
                assert(false);
        }
    }
}

// 访问函数
void Visit(const koopa_raw_function_t &func) {
    // 执行一些其他的必要操作
    // ...
    const auto &func_name = func->name;
    // 访问所有基本块
    cout << " .global " << (func_name+1) << endl;
    cout << (func_name+1)<<":" << endl;

    Visit(func->bbs);
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb) {
    // 执行一些其他的必要操作
    // ...
    // 访问所有指令
    Visit(bb->insts);
}

// 访问指令
void Visit(const koopa_raw_value_t &value) {
    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    switch (kind.tag) {
        case KOOPA_RVT_RETURN:
            // 访问 return 指令
            Visit(kind.data.ret);
            break;
        case KOOPA_RVT_INTEGER:
            // 访问 integer 指令
            Visit(kind.data.integer);
            break;
        default:
            // 其他类型暂时遇不到
            assert(false);
    }
}

void Visit(const koopa_raw_return_t & ret)
{
    const auto &ret_value = ret.value;
    assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
    int32_t int_val = ret_value->kind.data.integer.value;

    std::cout << "li "<<"a0 , "<<int(int_val)<<endl;
    std::cout << "ret" << endl;
}

void Visit(const koopa_raw_integer_t &int_value)
{
    const auto &int_val = int_value.value;
}

