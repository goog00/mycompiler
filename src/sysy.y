%code requires {
  #include <memory>
  #include <string>
  #include <cstdio>
  #include <iostream>
  #include <vector>

  #include <AST.h>
}

%{

#include <iostream>
#include <memory>
#include <string>
 #include <AST.h>
// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 用来向 parser 传递 lexer 读取到的内容
// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  char char_val;
  int int_val;
  BaseAST *ast_val;
  vector<unique_ptr<BaseAST>> *ast_vector;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN CONST
%token <char_val> OP1_CONST
%token <char_val> OP2_CONST
//%token <char_val> OP3_CONST
%token <str_val> IDENT
%token <int_val> INT_CONST
%token <str_val> Or_CONST
%token <str_val> And_CONST
%token <str_val> Eq_CONST
%token <str_val> Rel_CONST


// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp AddExp MulExp LOrExp LAndExp EqExp RelExp
%type <ast_val> Decl ConstDecl BType ConstDef ConstExp ConstInitVal BlockItem LVal VarDecl InitVal VarDef
%type <ast_vector> BlockArray ConstArray VarArray
%type <int_val> Number
%type <str_val> OrOp
%type <str_val> AndOp
%type <str_val> EqOp
%type <str_val> RelOp
%type <char_val> AddOp
%type <char_val> MulOp



%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担
FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    // $$ = new string("int");
    auto ast = new FuncTypeAST();
//    ast->type = *(new string("int"));
    ast->type = *(new string("i32"));
    $$ = ast;
  }
  ;

Block
  : '{' BlockArray '}' {
//    auto stmt = unique_ptr<string>($2);
//    $$ = new string("{ " + *stmt + " }");

      auto ast = new BlockAST();
      ast->BlockArray = unique_ptr<vector<unique_ptr<BaseAST>>>($2);
      $$ = ast;
  }
  ;

BlockArray
    : BlockArray BlockItem{
       auto v=(vector<unique_ptr<BaseAST>> *)($1);
       v->push_back(std::move(unique_ptr<BaseAST>($2)));
       $$=v;
    }
    |
    {
    $$ = new vector<unique_ptr<BaseAST>>();
    }

BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->tag=0;
    //ast->op=int($1)
    ast->Decl=unique_ptr<BaseAST>($1);
    $$ = ast;
  }

  | Stmt {
    auto ast = new BlockItemAST();
    ast->tag=1;
    ast->Stmt=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;


Stmt
  : LVal '=' Exp ';'{
    auto ast = new StmtAST();
    ast->tag=0;
    ast->LVal=unique_ptr<BaseAST>($1);
    ast->Exp=unique_ptr<BaseAST>($3);
    $$=ast;
  }


  | RETURN Exp ';' {
      auto ast = new StmtAST();
      ast->tag = 1;
      ast->Exp=unique_ptr<BaseAST>($2);
      $$ = ast;
  }
  ;

Exp
  :
  UnaryExp{
      auto ast = new ExpAST();
      ast->tag = 0;
      ast->UnaryExp = unique_ptr<BaseAST>($1);
      $$=ast;
  }
  | LOrExp{
    auto ast =new ExpAST();
    ast->tag=2;
    ast->LOrExp = unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | AddExp{
    auto ast = new ExpAST();
    ast->tag = 1;
    ast->AddExp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LVal{
      auto ast=new PrimaryExpAST();
      ast->tag=2;
      ast->LVal=unique_ptr<BaseAST>($1);
      $$=ast;
    }

;


AddExp
  : MulExp{
    auto ast = new AddExpAST();
    ast->tag = 0;
    ast->MulExp=unique_ptr<BaseAST>($1);
    $$ = ast;
  }

  | AddExp AddOp MulExp{
     auto ast = new AddExpAST();
     ast->tag=1;
         ast->AddExp=unique_ptr<BaseAST>($1);
         ast->AddOp=char($2);
         ast->MulExp=unique_ptr<BaseAST>($3);
         $$=ast;

  };
MulExp
  : UnaryExp{
    auto ast = new MulExpAST();
    ast->tag=0;
    ast->UnaryExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | MulExp MulOp UnaryExp{
    auto ast = new MulExpAST();
    ast->tag=1;
    ast->MulExp=unique_ptr<BaseAST>($1);
    ast->MulOp=char($2);
    ast->UnaryExp=unique_ptr<BaseAST>($3);
    $$=ast;
  };

UnaryExp
  : PrimaryExp{
    auto ast=new UnaryExpAST();
    ast->tag=0;
    ast->PrimaryExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | AddOp UnaryExp
  {
    auto ast=new UnaryExpAST();
    ast->tag=1;
    ast->UnaryOp=char($1);
    ast->UnaryExp=unique_ptr<BaseAST>($2);
    $$=ast;
  }
  | '!' UnaryExp
  {
    auto ast=new UnaryExpAST();
    ast->tag=1;
    ast->UnaryOp='!';
    ast->UnaryExp=unique_ptr<BaseAST>($2);
    $$=ast;
  }

  ;
LOrExp
  : LAndExp{
    auto ast=new LorExpAST();
    ast->tag=0;
    ast->LAndExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | LOrExp OrOp LAndExp{
    auto ast=new LorExpAST();
    ast->tag=1;
    ast->LOrExp=unique_ptr<BaseAST>($1);
    ast->OrOp=*unique_ptr<string>($2);
    ast->LAndExp=unique_ptr<BaseAST>($3);
    $$=ast;
  };

LAndExp
  : EqExp{
    auto ast=new LAndExpAST();
    ast->tag=0;
    ast->EqExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | LAndExp AndOp EqExp
  {
    auto ast=new LAndExpAST();
    ast->tag=1;
    ast->LAndExp=unique_ptr<BaseAST>($1);
    ast->AndOp=*unique_ptr<string>($2);
    ast->EqExp=unique_ptr<BaseAST>($3);
    $$=ast;
  };


EqExp
  : RelExp{
    auto ast=new EqExpAST();
    ast->tag=0;
    ast->RelExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | EqExp EqOp RelExp
  {
    auto ast=new EqExpAST();
    ast->tag=1;
    ast->EqExp=unique_ptr<BaseAST>($1);
    ast->EqOp=*unique_ptr<string>($2);
    ast->RelExp=unique_ptr<BaseAST>($3);
    $$=ast;
  };


RelExp
  : AddExp{
    auto ast=new RelExpAST();
    ast->tag=0;
    ast->AddExp=unique_ptr<BaseAST>($1);
    $$=ast;
  }

  | RelExp RelOp AddExp
  {
    auto ast=new RelExpAST();
    ast->tag=1;
    ast->RelExp=unique_ptr<BaseAST>($1);
    ast->RelOp=*unique_ptr<string>($2);
    ast->AddExp=unique_ptr<BaseAST>($3);
    $$=ast;
  };


RelOp
  : Rel_CONST{
    $$=($1);
  };

EqOp
  : Eq_CONST{
    $$=($1);
  };
AndOp
  : And_CONST{
    $$=($1);
  };
OrOp
  : Or_CONST{
    $$=($1);
  };

AddOp
  : OP1_CONST{
    $$=($1);
  };

MulOp
  : OP2_CONST{
    $$=($1);
  };

PrimaryExp
  : '(' Exp ')' {
    auto ast=new PrimaryExpAST();
    ast->tag=0;
    ast->Exp=unique_ptr<BaseAST>($2);
    $$=ast;
  }

  | Number{
    auto ast=new PrimaryExpAST();
    ast->tag=1;
    ast->number=int($1);
    $$=ast;
  }
  ;


Number
  : INT_CONST {
//    $$ = new string(to_string($1));
      $$ = ($1);
  }
  ;


Decl
  : ConstDecl{
    auto ast = new DeclAST();
    ast->tag=0;
    ast->ConstDecl=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  | VarDecl{
    auto ast = new DeclAST();
    ast->tag=1;
    ast->VarDecl=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;

VarDecl
  : BType VarDef VarArray ';'{
    auto ast = new VarDeclAST();
    ast->BType=unique_ptr<BaseAST>($1);
    ast->VarArray=unique_ptr<vector<unique_ptr<BaseAST>>>($3);
    ast->VarArray->push_back(std::move((unique_ptr<BaseAST>)($2)));
    $$=ast;
  };

VarArray
  : VarArray ',' VarDef{
    auto v= (vector<unique_ptr<BaseAST>> *)($1);
    v->push_back(std::move((unique_ptr<BaseAST>)($3)));
    $$=v;
  }
  | {
    $$=new vector<unique_ptr<BaseAST>>();
  };

VarDef
  : IDENT{
    auto ast=new VarDefAST();
    ast->tag=0;
    ast->IDENT=*unique_ptr<string>($1);
    $$=ast;
  }
  | IDENT '=' InitVal {
    auto ast=new VarDefAST();
    ast->tag=1;
    ast->IDENT=*unique_ptr<string>($1);
    ast->InitVal=unique_ptr<BaseAST>($3);
    $$=ast;
  };


ConstDecl
  : CONST BType ConstDef ConstArray ';'{
    auto ast = new ConstDeclAST();
    ast->BType=unique_ptr<BaseAST>($2);
    ast->ConstArray=unique_ptr<vector<unique_ptr<BaseAST>>>($4);
    ast->ConstArray->push_back(std::move((unique_ptr<BaseAST>)($3)));
    $$=ast;
  };

ConstArray
  : ConstArray ',' ConstDef{
    auto v= (vector<unique_ptr<BaseAST>> *)($1);
    v->push_back(std::move((unique_ptr<BaseAST>)($3)));
    $$=v;
  }
  | {
    $$=new vector<unique_ptr<BaseAST>>();
  };


BType
  : INT {
    auto ast=new BTypeAST();
    $$=ast;
  };

LVal
  : IDENT{
    auto ast=new LValAST();
    ast->IDENT=*unique_ptr<std::string>($1);
    $$=ast;
  };

ConstDef
  : IDENT '=' ConstInitVal {
    auto ast=new ConstDefAST();
    ast->IDENT=*unique_ptr<string>($1);
    ast->ConstInitVal=unique_ptr<BaseAST>($3);
    $$=ast;
  };

ConstInitVal
  : ConstExp{
    auto ast=new ConstInitValAST();
    ast->ConstExp=unique_ptr<BaseAST>($1);
    $$=ast;
  };

InitVal
  : Exp{
    auto ast=new InitValAST();
    ast->Exp=unique_ptr<BaseAST>($1);
    $$=ast;
  };

ConstExp
  : Exp
  {
    auto ast=new ConstExpAST();
    ast->Exp=unique_ptr<BaseAST>($1);
    $$=ast;
  };



%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数


void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
