//
// Created by sunteng on 2023/5/16.
//
#pragma once
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>


using namespace std;



// 所有 AST 的基类
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;
    void Dump() const override {
       // std::cout << "CompUnitAST { ";
        func_def->Dump();
       // std::cout << " }";
    }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void Dump() const override {
       // std::cout << "FuncDefAST { ";
       std::cout << "fun ";
       std::cout << "@" << ident << "(): ";
        func_type->Dump();
//        std::cout << ", " << ident << ", ";
       std::cout <<"{ " << endl;
        block->Dump();
//        std::cout << " }";
        std::cout << "}";
    }
};

class FuncTypeAST : public BaseAST
{
public:
    std::string type;
    void Dump() const override
    {
//        std::cout << "FuncTypeAST { ";
//        std::cout << type;
//        std::cout << " }";
       std::cout << (type+" ");
    }
};

class BlockAST : public BaseAST
{
public:

    std::unique_ptr<BaseAST> stmt;

    void Dump() const override
    {
//        std::cout << "BlockAST { ";
        std::cout << "%entry: "<<endl;
        stmt->Dump();
//        std::cout << " }";
    }
};

class StmtAST : public BaseAST
{
public:
    int number;
    int op;

    void Dump() const override
    {
//        std::cout << "StmtAST { ";
//        std::cout << number;
//        std::cout << " }";
          std::cout << " ret " <<number<<endl;
    }
};


// ...
