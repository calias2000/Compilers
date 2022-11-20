#ifndef __FIR_AST_FUNCTION_DEFINITION_NODE_H__
#define __FIR_AST_FUNCTION_DEFINITION_NODE_H__

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/corpo_node.h"

namespace fir {

  class function_definition_node: public cdk::typed_node {

    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    fir::corpo_node *_corpo;
    cdk::expression_node *_return_value;

  public:
    
    function_definition_node(int lineno, std::shared_ptr<cdk::basic_type> funType, int qualifier, const std::string &identifier,
                             cdk::sequence_node *arguments, cdk::expression_node *return_value, fir::corpo_node *corpo) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _corpo(corpo), _return_value(return_value) {
      type(funType);
    }

  public:

    int qualifier() {
      return _qualifier;
    }

    const std::string& identifier() const {
      return _identifier;
    }

    cdk::sequence_node* arguments() {
      return _arguments;
    }

    cdk::typed_node* argument(size_t ax) {
      return dynamic_cast<cdk::typed_node*>(_arguments->node(ax));
    }

    fir::corpo_node* corpo() {
      return _corpo;
    }

    cdk::expression_node* return_value(){
      return _return_value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };
}

#endif
