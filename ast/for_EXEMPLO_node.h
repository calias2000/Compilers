#ifndef __FIR_AST_WHILE_NODE_H__
#define __FIR_AST_WHILE_NODE_H__

#include <cdk/ast/expression_node.h>

namespace fir {

  class for_node: public cdk::basic_node {

    cdk::basic_node *_innit;
    cdk::expression_node *_condition;
    cdk::expression_node *_increment;
    cdk::basic_node *_block;

  public:

    inline for_node(int lineno, cdk::basic_node *innit, cdk::expression_node *condition, cdk::expression_node *increment, cdk::basic_node *block) :
        basic_node(lineno), _innit(innit), _condition(condition), _increment(increment), _block(block) {
    }

  public:

    inline cdk::basic_node *innit() {
      return _innit;
    }

    inline cdk::expression_node *condition() {
      return _condition;
    }

    inline cdk::expression_node *increment() {
      return _increment;
    }

    inline cdk::basic_node *block() {
      return _block;
    }


    void accept(basic_ast_visitor *sp, int level) {
      sp->do_for_node(this, level);
    }

  };
}

#endif
