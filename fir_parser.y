%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include "ast/all.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  std::string          *s;	/* symbol name or string literal */
  double                d;

  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  fir::bloco_node      *bloco;
  fir::corpo_node      *corpo;
  fir::prologo_node    *prologo;
};

%token tTYPE_INT tTYPE_STRING tTYPE_FLOAT tNULL tVOID tSIZEOF
%token tIF tTHEN tELSE
%token tWHILE tFOR tDO tLEAVE tRESTART tFINALLY
%token tGE tLE tEQ tNE tAND tOR
%token tWRITE tWRITELN tRETURN

%token <i> tINTEGER
%token <d> tFLOAT
%token <s> tSTRING tID

%type <node> declaration variable functionDec functionDef argdec inst funcVar for_var
%type <sequence> file declarations argdecs opt_vardec insts opt_inst exprs opt_expr funcVars
%type <expression> expr literal
%type <lvalue> leftval
%type <bloco> bloco
%type <prologo> prologo
%type <corpo> corpo
%type <s> string

%type <type> data_type void_type

%nonassoc tIF
%nonassoc tTHEN
%nonassoc tELSE
%nonassoc tWHILE
%nonassoc tDO
%nonassoc tFINALLY

%right '='
%left tOR
%left tAND
%right '~'
%left tNE tEQ
%left '<' tLE tGE '>'
%left '+' '-'
%left '*' '/' '%'
%right tUNARY

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file         :                                                            { compiler->ast($$ = new cdk::sequence_node(LINE)); }
             | declarations                                               { compiler->ast($$ = $1);                           }
             ;

declarations : declaration                                                { $$ = new cdk::sequence_node(LINE, $1);     }
             | declarations declaration                                   { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;



// Declaracao de variaveis e funcoes e definicao de funcoes

declaration  : variable ';'                                               { $$ = $1; }
             | functionDec                                                { $$ = $1; }
             | functionDef                                                { $$ = $1; }
             ;



// '*' - public  '?' - require (nao se podem inicializar) '\0' - private

variable     : data_type '*' tID                                          { $$ = new fir::variable_declaration_node(LINE,$1, '*', *$3, nullptr); delete $3;  }  
             | data_type '*' tID  '=' expr                                { $$ = new fir::variable_declaration_node(LINE,$1, '*', *$3, $5); delete $3;       }
             | data_type     tID                                          { $$ = new fir::variable_declaration_node(LINE,$1, '\0', *$2, nullptr); delete $2; }  
             | data_type     tID  '=' expr                                { $$ = new fir::variable_declaration_node(LINE,$1, '\0', *$2, $4); delete $2;      }
             | data_type '?' tID                                          { $$ = new fir::variable_declaration_node(LINE,$1, '?', *$3, nullptr); delete $3;  }
             ;



// palavra int, string, float e < > e como se representa um pointer

data_type    : tTYPE_STRING                                               { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
             | tTYPE_INT                                                  { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT);    }
             | tTYPE_FLOAT                                                { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
             | '<' data_type '>'                                          { $$ = cdk::reference_type::create(4, $2);               }
             ;

void_type    : tVOID                                                      { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
             ;



// Declaracao de funcoes

functionDec  : data_type     tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '\0', *$2, $4); }
             | data_type '*' tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '*', *$3, $5);  }
             | data_type '?' tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '?', *$3, $5);  }
             | void_type     tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '\0', *$2, $4); }
             | void_type '?' tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '?', *$3, $5);  }
             | void_type '*' tID '(' argdecs ')'                          { $$ = new fir::function_declaration_node(LINE, $1, '*', *$3, $5);  }
             ;



// Definicao de funcoes:
//    - '?' funcoes require apenas podem ser declaradas, nao definidas;
//    - funcoes void nao tem return value

functionDef  : data_type     tID '(' argdecs ')' '-''>' literal corpo     { $$ = new fir::function_definition_node(LINE, $1, '\0', *$2, $4, $8, $9);      }
             | data_type '*' tID '(' argdecs ')' '-''>' literal corpo     { $$ = new fir::function_definition_node(LINE, $1, '*', *$3, $5, $9, $10);      }
             | data_type     tID '(' argdecs ')'                corpo     { $$ = new fir::function_definition_node(LINE, $1, '\0', *$2, $4, nullptr, $6); }
             | data_type '*' tID '(' argdecs ')'                corpo     { $$ = new fir::function_definition_node(LINE, $1, '*', *$3, $5, nullptr, $7);  }
             | void_type     tID '(' argdecs ')'                corpo     { $$ = new fir::function_definition_node(LINE, $1, '\0', *$2, $4, nullptr, $6); }
             | void_type '*' tID '(' argdecs ')'                corpo     { $$ = new fir::function_definition_node(LINE, $1, '*', *$3, $5, nullptr, $7);  }
             ;



for_var      : data_type tID '=' expr                                     { $$ = new fir::variable_declaration_node(LINE,$1, '\0', *$2, $4); delete $2;      }
             | leftval '=' expr                                           { $$ = new cdk::assignment_node(LINE, $1, $3);                                     }
             ;



// Declaracao de variaveis dentro de uma funcao, nao podem ser '*' ou '?'

funcVars     : funcVar ';'                                                { $$ = new cdk::sequence_node(LINE, $1);     }
             | funcVars funcVar ';'                                       { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

funcVar      : data_type tID                                              { $$ = new fir::variable_declaration_node(LINE,$1, '\0', *$2, nullptr); delete $2; }
             | data_type tID '=' expr                                     { $$ = new fir::variable_declaration_node(LINE,$1, '\0', *$2, $4); delete $2;      }
             ;



// Corpo de uma funcao, necessita pelo menos um prologo, bloco ou epilogo

corpo        : '@' prologo bloco '>''>' bloco                             { $$ = new fir::corpo_node(LINE, $2, $3, $6);           }
             |             bloco                                          { $$ = new fir::corpo_node(LINE, nullptr, $1, nullptr); }
             | '@' prologo                                                { $$ = new fir::corpo_node(LINE, $2, nullptr, nullptr); }
             |                   '>''>' bloco                             { $$ = new fir::corpo_node(LINE, nullptr, nullptr, $3); }
             | '@' prologo       '>''>' bloco                             { $$ = new fir::corpo_node(LINE, $2, nullptr, $5);      }
             | '@' prologo bloco                                          { $$ = new fir::corpo_node(LINE, $2, $3, nullptr);      }
             |             bloco '>''>' bloco                             { $$ = new fir::corpo_node(LINE, nullptr, $1, $4);      }
             ;

prologo      : '{' opt_vardec opt_inst '}'                                { $$ = new fir::prologo_node(LINE, $2, $3); }
             ;

bloco        : '{' opt_vardec opt_inst '}'                                { $$ = new fir::bloco_node(LINE, $2, $3); }
             ;

opt_vardec   :                                                            { $$ = new cdk::sequence_node(LINE); }
             | funcVars                                                   { $$ = $1;                           }
             ;



// Argumentos das funcoes separados por ','

argdecs      :                                                            { $$ = new cdk::sequence_node(LINE);         }
             |             argdec                                         { $$ = new cdk::sequence_node(LINE, $1);     }
             | argdecs ',' argdec                                         { $$ = new cdk::sequence_node(LINE, $3, $1); }
             ;

argdec       : data_type tID                                              { $$ = new fir::variable_declaration_node(LINE, $1, '\0', *$2, nullptr); }
             ;



// Instrucoes que podem ocorrer nas diferentes partes do corpo de uma funcao

insts        : inst                                                       { $$ = new cdk::sequence_node(LINE, $1);     }
             | insts inst                                                 { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

inst         : expr ';'                                                   { $$ = new fir::evaluation_node(LINE, $1);         }
             | tWRITE exprs ';'                                           { $$ = new fir::print_node(LINE, $2, false);       }
             | tWRITELN exprs ';'                                         { $$ = new fir::print_node(LINE, $2, true);        }
             | tLEAVE ';'                                                 { $$ = new fir::leave_node(LINE);                  }
             | tRESTART ';'                                               { $$ = new fir::restart_node(LINE);                }
             | tLEAVE tINTEGER ';'                                        { $$ = new fir::leave_node(LINE, $2);              }
             | tRESTART tINTEGER ';'                                      { $$ = new fir::restart_node(LINE, $2);            }
             | tRETURN                                                    { $$ = new fir::return_node(LINE);                 }
             | tWHILE expr tDO inst                                       { $$ = new fir::while_node(LINE, $2, $4, nullptr); }
             | tWHILE expr tDO inst tFINALLY inst                         { $$ = new fir::while_node(LINE, $2, $4, $6);      }
             | tIF expr tTHEN inst                                        { $$ = new fir::if_node(LINE, $2, $4);             }
             | tIF expr tTHEN inst tELSE inst                             { $$ = new fir::if_else_node(LINE, $2, $4, $6);    }
             | tFOR '(' for_var ';' expr ';' expr ')' bloco               { $$ = new fir::for_EXEMPLO_node(LINE, $3, $5, $7, $9); };
             | bloco                                                      { $$ = $1;                                         }
             ;

opt_inst     :                                                            { $$ = new cdk::sequence_node(LINE); }
             | insts                                                      { $$ = $1;                           }
             ;




// Operacoes com expressoes

exprs        : expr                                                       { $$ = new cdk::sequence_node(LINE, $1);     }
             | exprs ',' expr                                             { $$ = new cdk::sequence_node(LINE, $3, $1); }
             ;

expr         : literal                                                    { $$ = $1;                                                    }
             | expr '+' expr                                              { $$ = new cdk::add_node(LINE, $1, $3);                       }
             | expr '-' expr                                              { $$ = new cdk::sub_node(LINE, $1, $3);                       }
             | expr '*' expr                                              { $$ = new cdk::mul_node(LINE, $1, $3);                       }
             | expr '/' expr                                              { $$ = new cdk::div_node(LINE, $1, $3);                       }
             | expr '%' expr                                              { $$ = new cdk::mod_node(LINE, $1, $3);                       }
             | expr  '<' expr                                             { $$ = new cdk::lt_node(LINE, $1, $3);                        }
             | expr tLE  expr                                             { $$ = new cdk::le_node(LINE, $1, $3);                        }
             | expr tEQ  expr                                             { $$ = new cdk::eq_node(LINE, $1, $3);                        }
             | expr tGE  expr                                             { $$ = new cdk::ge_node(LINE, $1, $3);                        }
             | expr  '>' expr                                             { $$ = new cdk::gt_node(LINE, $1, $3);                        }
             | expr tNE  expr                                             { $$ = new cdk::ne_node(LINE, $1, $3);                        }
             | expr tAND  expr                                            { $$ = new cdk::and_node(LINE, $1, $3);                       }
             | expr tOR   expr                                            { $$ = new cdk::or_node (LINE, $1, $3);                       }
             | '-' expr %prec tUNARY                                      { $$ = new cdk::neg_node(LINE, $2);                           }
             | '+' expr %prec tUNARY                                      { $$ = new fir::identity_node(LINE, $2);                      }
             | '~' expr                                                   { $$ = new cdk::not_node(LINE, $2);                           }
             | '@'                                                        { $$ = new fir::input_node(LINE);                             }
             | tID '(' opt_expr ')'                                       { $$ = new fir::function_call_node(LINE, *$1, $3); delete $1; }
             | tSIZEOF '(' expr ')'                                       { $$ = new fir::sizeof_node(LINE, $3);                        }
             | '(' expr ')'                                               { $$ = $2;                                                    }
             | '[' expr ']'                                               { $$ = new fir::stack_alloc_node(LINE, $2);                   }
             | leftval '?'                                                { $$ = new fir::address_of_node(LINE, $1);                    }
             | leftval                                                    { $$ = new cdk::rvalue_node(LINE, $1);                        }
             | leftval '=' expr                                           { $$ = new cdk::assignment_node(LINE, $1, $3);                }
             ;

opt_expr     :                                                            { $$ = new cdk::sequence_node(LINE); }
             | exprs                                                      { $$ = $1;                           }
             ;



// valor do tipo int, float, string, etc (usado no return das funcoes e nas expressoes)

literal      : tINTEGER                                                   { $$ = new cdk::integer_node(LINE, $1); }
             | tFLOAT                                                     { $$ = new cdk::double_node(LINE, $1);  }
             | string                                                     { $$ = new cdk::string_node(LINE, $1);  }
             | tNULL                                                      { $$ = new fir::null_node(LINE);        }
             ;


leftval      : tID                                                        { $$ = new cdk::variable_node(LINE, $1);                                          }
             | tID '(' opt_expr ')' '[' expr ']'                          { $$ = new fir::index_node(LINE, new fir::function_call_node(LINE, *$1, $3), $6); }
             | leftval '[' expr ']'                                       { $$ = new fir::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3);             }
             | '(' expr ')' '[' expr']'                                   { $$ = new fir::index_node(LINE, $2, $5);                                         }
             ;


string       : tSTRING                                                    { $$ = $1;                                               }
             | string tSTRING                                             { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
             ; 

%%