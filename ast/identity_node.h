#ifndef __FIR_IDENTITY_NODE_H__
#define __FIR_IDENTITY_NODE_H__

#include <cdk/ast/unary_operation_node.h>
#include <cdk/ast/expression_node.h>

namespace fir {

  class identity_node: public cdk::unary_operation_node {

    public:

      inline identity_node(int lineno, cdk::expression_node *arg) :
        cdk::unary_operation_node(lineno, arg) {}

    public:
    	
      void accept(basic_ast_visitor *sp, int level) {
        sp->do_identity_node(this, level);
      }

  };
}

#endif