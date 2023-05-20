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
#include <vector>


using namespace std;





class BaseAST;
struct symbol{
    int tag = 0;
    int value =0;
    symbol(int t,int v){
        tag = t;
        value = v;
    }
};


static map<string,string> mp={{"||","or"},{"&&","and"},{"==","eq"},{"!=","ne"},{">","gt"},{"<","lt"},{">=","ge"},{"<=","le"}};
static map<string,unique_ptr<symbol>> symbolTable = {};
static char globalOp='+';

static int t_value = 0;
static int u_value = 0;

static int now_value = 0;
static string u_ident;
static int return_cnt = 0;




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
//      char NowOp='+';

      virtual ~BaseAST() = default;
      virtual void Dump() const = 0;


};


class BlockItemAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> Stmt;
    std::unique_ptr<BaseAST> Decl;
    int tag;
    void Dump() const override
    {
        switch (tag)
        {
            case 0:
                if(return_cnt > 0)
                    break;
                Decl->Dump();
                break;
            case 1:
                if(return_cnt>0){
                    break;
                }
                Stmt->Dump();
                break;

        }
    }

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

//    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<vector<unique_ptr<BaseAST>>> BlockArray;

    void Dump() const override
    {
//        std::cout << "BlockAST { ";
        std::cout << "%entry: "<<endl;
        for (auto iter = (BlockArray->begin()); iter != (BlockArray->end()); ++iter)
        {
            (*iter)->Dump();
        }

    }
};

class StmtAST : public BaseAST
{
public:
//    int number;
//    int op;
   std::unique_ptr<BaseAST> Exp;
   std::unique_ptr<BaseAST> LVal;
   int tag;
   int number;


    void Dump() const override
    {
//        std::cout << "StmtAST { ";
//        std::cout << number;
//        std::cout << " }";
//          std::cout << " ret " <<number<<endl;

         string s_ident;
         int o_value;
          switch(tag)
          {
              case 0:
//                  std::cout << " ret " << number<<endl;
                  LVal->Dump();
                  s_ident = u_ident;
                  o_value = t_value;
                  Exp->Dump();
                  if(o_value == t_value)
                  {
                      cout << " store " << u_value << "," << " @" << s_ident << endl;
                  }
                  else
                  {
                      cout << " store % " << (t_value-1) << "," << " @%" << s_ident <<endl;
                  }
                  break;
              case 1:

                  o_value = t_value;
                  Exp->Dump();
                  ++return_cnt;
                  if(t_value == o_value)
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
                int left_nvalue = now_value;

                int right_o = t_value;
                LAndExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;
                int right_nvalue = now_value;


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
                int left_nvalue = now_value;

                int right_o = t_value;
                EqExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;
                int right_nvalue = now_value;

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

                now_value = (left_nvalue && right_nvalue);

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
                int left_nvalue = now_value;

                int right_o = t_value;
                RelExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;
                int right_nvalue = now_value;

                assert (EqOp == "=="||EqOp=="!=");

                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp[EqOp] << " ";
                extract_ref(left_value, left_ref, left_o);
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;

                if(EqOp == "=="){
                    now_value = (left_nvalue == right_nvalue);
                } else{
                    now_value = (left_nvalue != right_nvalue);
                }
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
                int left_nvalue = now_value;

                int right_o = t_value;
                AddExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;
                int right_nvalue = now_value;

                assert (RelOp == ">="||RelOp=="<="||RelOp == ">"||RelOp=="<");
                std::cout << " %" << t_value << ' ' << "= ";
                std::cout << mp[RelOp] << " ";
                extract_ref(left_value, left_ref, left_o);
                std::cout << ", ";
                extract_ref(right_value, right_ref, right_o);
                cout << endl;

                if(RelOp == ">=")
                    now_value = (left_nvalue >= right_nvalue);
                else if(RelOp == "<=")
                    now_value = (left_nvalue <= right_nvalue);
                else if(RelOp == "<")
                    now_value= (left_nvalue < right_nvalue);
                else
                    now_value= (left_nvalue > right_nvalue);
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
                 int left_nvalue = now_value;

                 int right_o = t_value;
                 MulExp->Dump();
                 int right_ref = t_value;
                 int right_value = u_value;
                 int right_nvalue = now_value;

                 char *s = nullptr;
                 switch (AddOp)
                 {
                     case '+':
                         now_value = left_nvalue + right_nvalue;
                         s = "add";
                         break;
                     case '-':
                         now_value = left_nvalue - right_nvalue;
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
                int left_nvalue = now_value;

                int right_o = t_value;
                UnaryExp->Dump();
                int right_ref = t_value;
                int right_value = u_value;//  right data
                int right_nvalue = now_value;

                char *s = nullptr;
                switch (MulOp)
                {
                    case '*':
                        now_value = left_nvalue * right_nvalue;
                        s = "mul";
                        break;
                    case '/':
                        now_value = left_nvalue / right_nvalue;
                        s = "div";
                        break;
                    case '%':
                        now_value = left_nvalue % right_nvalue;
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

                        if(o_value !=0)
                            now_value = 0;
                        else
                            now_value = 1;

                        if (o_value == t_value)
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
                        now_value = (-now_value);
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

    std::unique_ptr<BaseAST> LVal;
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
                now_value = number;
                break;
            case 2:
                LVal->Dump();
                int type = symbolTable[u_ident]->tag;
                switch (type) {
                    case 0:
                        u_value=symbolTable[u_ident]->value;
                        now_value = u_value;
                        break;
                    case 1:
                        cout<<" %"<<t_value<<" "<<"= "<<"load "<<"@"<<u_ident<<endl;
                        ++t_value;
                        break;
                }
                break;

        }
        // std::cout << " }";
    }
};


class DeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> ConstDecl;
    std::unique_ptr<BaseAST> VarDecl;
    int tag;
    void Dump() const override
    {
        switch (tag) {
            case 0:
//                std::cout << " ret " << number<<endl;
                ConstDecl->Dump();
                break;
            case 1:
//                Exp->Dump();
//                if(t_value==0)
                VarDecl->Dump();
                break;

        }
    }
};

class ConstDeclAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> BType;
    std::unique_ptr<vector<unique_ptr<BaseAST>>> ConstArray;

    void Dump() const override
    {
        BType->Dump();
        auto ptr = ConstArray->rbegin();
        (*ptr)->Dump();
        for(auto iter = (ConstArray->begin()); (iter != ConstArray->end()-1);++iter)
        {
            (*iter)->Dump();
        }

    }
};


class VarDeclAST : public BaseAST
{

public:
    std::unique_ptr<BaseAST> BType;
    std::unique_ptr<vector<unique_ptr<BaseAST>>> VarArray;
    void Dump() const override
    {
        BType->Dump();
        auto ptr = VarArray->rbegin();
        (*ptr)->Dump();

        for(auto iter = VarArray->begin();iter != (VarArray->end()-1);++iter)
        {
            (*iter)->Dump();
        }
    }
};

class VarDefAST : public BaseAST
{
public:
    std::string IDENT;
    std::unique_ptr<BaseAST> InitVal;

    int tag;
    void Dump() const override
    {
        int o_value;
        switch (tag)
        {
            case 0:
                cout << " @" << IDENT << " "<< "= "<< "alloc  "<< "i32" << endl;
                symbolTable.emplace(IDENT, make_unique<symbol>(1,-1));
                break;
            case 1:
                o_value = t_value;
                InitVal->Dump();
                if(o_value==t_value)
                {
                   cout << " @" << IDENT << " " << "= " << "alloc " << "i32" << endl;
                   int con_value = now_value;
                   cout << " store " << con_value << "," << " @" << IDENT << endl;

                }
                else
                {

                    cout << " @" << IDENT << " "<< "= "<< "alloc "<< "i32" << endl;
                    cout << " store %" << t_value-1 << ","<< " @" << IDENT << endl;

                }
                symbolTable.emplace(IDENT, make_unique<symbol>(1, -1));
                break;
            default:
                break;

        }
    }
};

//class ConstDeclAST : public BaseAST
//{
//public :
//    std::string IDENT;
//    std::unique_ptr<BaseAST> ConstInitVal;
//
//    void Dump() const override
//    {
//        ConstInitVal->Dump();
//        int con_value = now_value;
//        symbolTable.emplace(IDENT,make_unique<symbol>(0, con_value));
//    }
//};

class ConstDefAST : public BaseAST
{
public:
    std::string IDENT;
    std::unique_ptr<BaseAST> ConstInitVal;
    void Dump() const override
    {
        ConstInitVal->Dump();
        int con_value = now_value;
        symbolTable.emplace(IDENT,make_unique<symbol>(0,con_value));
    }
};


class BTypeAST : public BaseAST
{
public:
    int tag =0;
    void Dump() const override
    {
        return;
    }
};

class LValAST : public BaseAST
{
public:
    std::string IDENT;
    void Dump() const override
    {
        u_ident = IDENT;
    }
};

class ConstInitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> ConstExp;

    void Dump() const override
    {
        ConstExp->Dump();
    }
};

class InitValAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> Exp;

    void Dump() const override
    {
        Exp->Dump();
    }
};

class ConstExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> Exp;

    void Dump() const override
    {
//        stringstream ss;
//        stringbuf* buffer = cout.rdbuf();
//        std::cout.rdbuf(ss.rdbuf);
//        int o_value = t_value;
        Exp->Dump();
//        t_value = o_value;
//        string s(ss.str());
//        std::cout.rdbuf(buffer);
    }
};
// ...
