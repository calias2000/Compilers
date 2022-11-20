#ifndef __FIR_AST_CORPO_NODE_H__
#define __FIR_AST_CORPO_NODE_H__

#include "ast/prologo_node.h"
#include "ast/bloco_node.h"

namespace fir {

  class corpo_node: public cdk::basic_node {

    fir::prologo_node *_prologo;
    fir::bloco_node *_bloco;
    fir::bloco_node *_epilogo;

  public:
    
    corpo_node(int lineno, fir::prologo_node *prologo, fir::bloco_node *bloco, fir::bloco_node *epilogo) :
        cdk::basic_node(lineno), _prologo(prologo), _bloco(bloco), _epilogo(epilogo) {
    }

  public:
    
    fir::prologo_node* prologo(){
      return _prologo;
    }

    fir::bloco_node* bloco(){
      return _bloco;
    }

    fir::bloco_node* epilogo(){
      return _epilogo;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_corpo_node(this, level);
    }

  };
}

#endif
