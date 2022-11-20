#ifndef __FIR_AST_INPUT_NODE_H__
#define __FIR_AST_INPUT_NODE_H__

#include <cdk/ast/expression_node.h>

namespace fir {

  class input_node: public cdk::expression_node {

  public:

    input_node(int lineno) :
        cdk::expression_node(lineno) {
    }

  public:
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_input_node(this, level);
    }

  };
}

#endif