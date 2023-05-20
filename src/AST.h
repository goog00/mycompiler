//
// Created by sunteng on 2023/5/16.
//
#pragma once
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <map>
#include <cassert>


using namespace std;


static map<string,string> mp={{"||","or"},{"&&","and"},{"==","eq"},{"!=","ne"},{">","gt"},{"<","lt"},{">=","ge"},{"<=","le"}};
static char globalOp='+';
static int t_value = 0;
static int u_value = 0;

static const void extract_ref(int,int,int);
const void extract_ref(int value,int ref,int o_ref)
{
//    string s;
    if(ref == o_ref)
    {
//        s = to_string(value);
        std::cout << value;
    } else {
//        s = to_string(ref-1);
//        s.insert(0,'%',1);
        std::cout<<"%"<<ref -1;
    }

//    return s.c_str();
}






// 所有 AST 的基类
class BaseAST {
public:
//    virtual ~BaseAST() = default;
//    virtual void Dump() const = 0;
      char NowOp='+';

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
//    int number;
//    int op;
   std::unique_ptr<BaseAST> Exp;
   int tag;
   int number;


    void Dump() const override
    {
//        std::cout << "StmtAST { ";
//        std::cout << number;
//        std::cout << " }";
//          std::cout << " ret " <<number<<endl;
          switch(tag)
          {
              case 0:
                  std::cout << " ret " << number<<endl;
                  break;
              case 1:
                  Exp->Dump();
                  if(t_value == 0)
                  {
                      std::cout << "  ret " <<u_value<<endl;
                  }
                  else
                  {
                      std::cout << "  ret %" << t_value-1<<endl;
                  }
                  break;
              default:
                  break;


          }

    }
};

class ExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> UnaryExp;
    std::unique_ptr<BaseAST> LOrExp;
    std::unique_ptr<BaseAST> AddExp;
    void Dump() const override
    {
        switch (tag) {
            case 1:
                AddExp->Dump();
                break;
            case 2:
                LOrExp->Dump();
                break;
            default:
                break;

        }
    }

};


class LorExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> LAndExp;
    std::unique_ptr<BaseAST> LOrExp;
    string OrOp;

    void Dump() const override
    {
        //std::cout << "StmtAST { ";
        switch (tag)
        {
            case 0:
                LAndExp->Dump();
                break;
            case 1:
                int left_o = t_value;
                LOrExp->Dump();
                int left_ref = t_value;
                int left_value = u_value;

                int right_o = t_value;
                LAndExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;


                assert (OrOp == "||");
                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << "ne" << " ";
                std::cout << 0;
                std::cout << ", ";
                extract_ref(left_value, left_ref, left_o);
                cout << endl;
                t_value++;// 判断是否为0 为0为0 不为0为1

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << "ne" << " ";
                std::cout << 0;
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;
                t_value++;// 判断是否为0 为0为0 不为0为1



                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp["||"] << " ";
                std::cout <<"%"<<t_value-2;
                std::cout << ", ";
                std::cout<<"%"<<t_value-1;
                cout << endl;

                ++t_value;
                break;
        }
        // std::cout << " }";
    }
};

class LAndExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> EqExp;
    std::unique_ptr<BaseAST> LAndExp;

    string AndOp;

    void Dump() const override
    {
        //std::cout << "StmtAST { ";
        switch (tag)
        {
            case 0:
                EqExp->Dump();
                break;
            case 1:

                int left_o = t_value;
                LAndExp->Dump();
                int left_ref = t_value;
                int left_value = u_value;

                int right_o = t_value;
                EqExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;

                assert (AndOp == "&&");

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << "ne" << " ";
                std::cout << 0;
                std::cout << ", ";
                extract_ref(left_value, left_ref, left_o);
                cout << endl;
                t_value++;// 判断是否为0 为0为0 不为0为1

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << "ne" << " ";
                std::cout << 0;
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;
                t_value++;// 判断是否为0 为0为0 不为0为1



                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp["&&"] << " ";
                std::cout <<"%"<<t_value-2;
                std::cout << ", ";
                std::cout<<"%"<<t_value-1;
                cout << endl;

                ++t_value;
                break;
        }
        // std::cout << " }";
    }
};

class EqExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> RelExp;
    std::unique_ptr<BaseAST> EqExp;

    string EqOp;

    void Dump() const override
    {
        //std::cout << "StmtAST { ";
        switch (tag)
        {
            case 0:
                RelExp->Dump();
                break;
            case 1:
                int left_o = t_value;
                EqExp->Dump();
                int left_ref = t_value;
                int left_value = u_value;

                int right_o = t_value;
                RelExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;

                assert (EqOp == "=="||EqOp=="!=");

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp[EqOp] << " ";
                extract_ref(left_value, left_ref, left_o);
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;
                ++t_value;
                break;
        }
        // std::cout << " }";
    }
};

class RelExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> AddExp;
    std::unique_ptr<BaseAST> RelExp;

    string RelOp;

    void Dump() const override
    {
        //std::cout << "StmtAST { ";
        switch (tag)
        {
            case 0:
                AddExp->Dump();
                break;
            case 1:
                int left_o = t_value;
                RelExp->Dump();
                int left_ref = t_value;
                int left_value = u_value;

                int right_o = t_value;
                AddExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;

                assert (RelOp == ">="||RelOp=="<="||RelOp == ">"||RelOp=="<");
                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp[RelOp] << " ";
                extract_ref(left_value, left_ref, left_o);
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;
                ++t_value;
                break;
        }
        // std::cout << " }";
    }
};


class AddExpAST : public BaseAST
{
public:
     int tag;
     std::unique_ptr<BaseAST> MulExp;
     std::unique_ptr<BaseAST> AddExp;
     char AddOp;

     void Dump() const override
     {
//         std::cout << " ret " << number <<endl;
         switch (tag)
         {
             case 0:
                 MulExp->Dump();
                 break;
             case 1:
                 int left_o = t_value;
                 AddExp->Dump();
                 int left_ref = t_value;
                 int left_value = u_value;

                 int right_o = t_value;
                 MulExp->Dump();
                 int right_ref = t_value;
                 int right_value = u_value;

                 char *s = nullptr;
                 switch (AddOp)
                 {
                     case '+':
                         s = "add";
                         break;
                     case '-':
                         s = "sub";
                         break;
                 }

                 std::cout << " %" << t_value << ' ' << "= ";
                 std::cout << s << " ";
//                 if(left_ref==left_o)
//                 {
//                     std::cout << left_value;
//                 }
//                 else
//                 {
//                     std::cout << "%" << left_ref -1;
//                 }
                 extract_ref(left_value, left_ref, left_o);
                 std::cout << ", ";
//                 if(right_ref == right_o)
//                 {
//                     std::cout << right_value;
//                 }
//                 else
//                 {
//                     std::cout << "%"<<right_ref -1;
//                 }
                 extract_ref(right_value, right_ref, right_o);

                 cout << endl;
                 ++t_value;
                 break;



         }
     }
};

class MulExpAST : public BaseAST
{
public:
    int tag;
    std::unique_ptr<BaseAST> UnaryExp;
    std::unique_ptr<BaseAST> MulExp;
    char MulOp;
    void Dump() const override
    {
        switch (tag) {
            case 0:
                UnaryExp->Dump();
                break;
            case 1:
                int left_o = t_value;
                MulExp->Dump();
                int left_ref = t_value;
                int left_value = u_value;// left data

                int right_o = t_value;
                UnaryExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;//  right data

                char *s = nullptr;
                switch (MulOp)
                {
                    case '*':
                        s = "mul";
                        break;
                    case '/':
                        s = "div";
                        break;
                    case '%':
                        s = "mod";
                        break;
                    default:
                        break;
                }

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << s << " ";
//                if (left_ref==left_o)
//                {
//                    std::cout << left_value;
//                }
//                else
//                {
//                    std::cout <<"%"<<left_ref - 1;
//                }
                extract_ref(left_value, left_ref, left_o);
                std::cout << ", ";
//                if (right_ref == right_o)
//                {
//                    std::cout << right_value;
//                }
//                else
//                {
//                    std::cout <<"%"<<right_ref - 1;
//                }
                extract_ref(right_value, right_ref, right_o);

                cout << endl;
                ++t_value;
                break;


        }
    }
};
//一元表达式
class UnaryExpAST : public BaseAST
{

public:
    int tag;
    std::unique_ptr<BaseAST> PrimaryExp;
    char UnaryOp;
    std::unique_ptr<BaseAST> UnaryExp;
    void Dump() const override
    {

        switch(tag)
        {
            case 0:
                PrimaryExp->Dump();
                break;
            case 1:
                int o_value = t_value;
                UnaryExp->Dump();
                switch (UnaryOp)
                {
                    case '!':
                        std::cout << " %" << t_value << ' ' << "= ";

                        if (o_value==t_value)
                        {
//                            std::cout << " %" << t_value << ' ' << "= ";
                            std::cout << "eq " << u_value << ", " << 0 << endl;
                            ++t_value;
                        }
                        else
                        {
//                            std::cout << " %" << t_value << ' ' << "= ";
                            std::cout << "eq %" << t_value-1 << ", " << 0 << endl;
                            t_value++;
                        }
                        break;
                    case '-':
                        std::cout << " %" << t_value << ' ' << "= ";

                        if (o_value==t_value)
                        {
//                            std::cout << " %" << t_value << ' ' << "= ";
                            std::cout << "sub " << 0 << ", " << u_value << endl;
                            ++t_value;
                        }
                        else
                        {
//                            std::cout << " %" << t_value << ' ' << "= ";
                            std::cout << "sub " << 0 << ", %" << t_value-1 << endl;
                            t_value++;
                        }
                        break;
                }
                break;
        }
    }
};
class PrimaryExpAST : public BaseAST
{
public:
    int tag;
    int number;
    std::unique_ptr<BaseAST> Exp;

    void Dump() const override
    {
        // std::cout << "StmtAST { ";
        switch (tag)
        {
            case 0:
                Exp->Dump();
                break;
            case 1:
                u_value = number;
                break;
            default:
                break;
        }
        // std::cout << " }";
    }
};


// ...
