#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/frame_size_calculator.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated
#include "fir_parser.tab.h"

//---------------------------------------------------------------------------

void fir::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) { }
void fir::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) { }

void fir::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
}

void fir::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

void fir::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

void fir::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}



//------------------------------------------ TYPES ----------------------------------------


void fir::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if(_inFunctionBody)
    _pf.INT(node->value());
  else
    _pf.SINT(node->value());
}

void fir::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if(_inFunctionBody){
    _pf.DOUBLE(node->value());
  }
  else{
    _pf.SDOUBLE(node->value());
  }
}

void fir::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;
  _pf.RODATA(); // strings are readonly DATA
  _pf.ALIGN(); // make sure the address is aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters
  if (_function) {
    // local variable initializer
    _pf.TEXT();
    _pf.ADDR(mklbl(lbl1));
  } else {
    // global variable initializer
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}



//--------------------------------------------- OPERATIONS ------------------------------------------------------


void fir::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->type()->name() != cdk::TYPE_DOUBLE)
    _pf.ADD();
  else
    _pf.DADD();
}

void fir::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();
}

void fir::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL();
}

void fir::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}

void fir::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void fir::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}



// ------------------------------------------------ EQUALS ---------------------------------------------------------------


void fir::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.LT();
}

void fir::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.LE();
}

void fir::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.GE();
}

void fir::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.GT();
}

void fir::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.NE();
}

void fir::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.EQ();
}



//------------------------------------- VARIABLE AND ASSIGNMENT ----------------------------------------


void fir::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (!symbol->global()) {
    _pf.LOCAL(symbol->offset());
  } else {
    if (symbol->isFunction()){
      _pf.LOCAL(-symbol->type()->size());
    }
    else {
      _pf.ADDR(symbol->name());
    }
  }
}

void fir::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->type()->name() != cdk::TYPE_DOUBLE) {
    _pf.LDINT();
  } else {
    _pf.LDDOUBLE();
  }
}

void fir::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    if (node->rvalue()->type()->name() == cdk::TYPE_INT) _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}



//------------------------------------- AST FOLDER NODES -----------------------------------------------


void fir::postfix_writer::do_address_of_node(fir::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_bloco_node(fir::bloco_node * const node, int lvl) {
  _symtab.push();
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

void fir::postfix_writer::do_corpo_node(fir::corpo_node * const node, int lvl) {
  if (node->prologo()) node->prologo()->accept(this, lvl + 2);
  if (node->bloco()) node->bloco()->accept(this, lvl + 2);
  if (node->epilogo()){
    _pf.LABEL(_returnLabel);
    node->epilogo()->accept(this, lvl + 2);
  }
}

void fir::postfix_writer::do_evaluation_node(fir::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.TRASH(node->argument()->type()->size());
}

void fir::postfix_writer::do_function_call_node(fir::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto symbol = _symtab.find(node->identifier());
  size_t argsSize = 0;

  if (node->arguments()->size() > 0) {
    for (int ax = node->arguments()->size() - 1; ax >= 0; ax--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax));
      arg->accept(this, lvl + 2);
      if (symbol->argument_is_typed(ax, cdk::TYPE_DOUBLE) && arg->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
      }
      argsSize += symbol->argument_size(ax);
    }
  }

  _pf.CALL(node->identifier());

  if (argsSize != 0) {
    _pf.TRASH(argsSize);
  }

  // Load valor de retorno da funcao

  if (symbol->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDFVAL64();
  } else {
    _pf.LDFVAL32();
  }
}

void fir::postfix_writer::do_function_declaration_node(fir::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (_inFunctionBody || _inFunctionArgs) {
    std::cerr << node->lineno() << ": Can only declare a function outside of another function body and arguments.";
    return;
  }

  if (!new_symbol()) return;

  auto function = new_symbol();
  _functions_to_declare.insert(function->name());
  reset_new_symbol();
}

void fir::postfix_writer::do_function_definition_node(fir::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (_inFunctionBody || _inFunctionArgs) {
    std::cerr << node->lineno() << ": Can only declare a function outside of another function body and arguments.";
    return;
  }

  // remember symbol so that args and body know
  _function = new_symbol();
  _functions_to_declare.erase(_function->name());
  reset_new_symbol();

  if (node->corpo()->epilogo()){
    _returnLabel = mklbl(++_lbl);
  }

  _defaultReturnLabel = mklbl(++_lbl);

  _offset = 8; // prepare for arguments (4: remember to account for return address)

  _symtab.push(); // scope of args


  if (node->arguments()->size() > 0) {
    _inFunctionArgs = true;
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::basic_node *arg = node->arguments()->node(ix);
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (node->qualifier() == '*') _pf.GLOBAL(_function->name(), _pf.FUNC());
  _pf.LABEL(_function->name());


  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab, _function);
  node->accept(&lsc, lvl);
  size_t size_return_value = node->type()->size(); //tou a ir buscar o tamanho se é int ou string etc
  _offset = -size_return_value; // prepare for local variable
  _pf.ENTER(lsc.localsize() + size_return_value); // total stack size reserved for local variables
  _inFunctionBody = true;

  if (node->return_value()){
    node->return_value()->accept(this, lvl);

    if (node->type()->name() == cdk::TYPE_INT || node->type()->name() == cdk::TYPE_STRING || node->type()->name() == cdk::TYPE_POINTER) {
      _pf.LOCAL(_offset);
      _pf.STINT();
    } else if (node->type()->name() == cdk::TYPE_DOUBLE) {
      if (node->return_value()->type()->name() == cdk::TYPE_INT) _pf.I2D();
      _pf.LOCAL(_offset);
      _pf.STDOUBLE();
    } else {
      std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
    }
  }

  os() << "        ;; before body " << std::endl;
  node->corpo()->accept(this, lvl + 4);
  os() << "        ;; after body " << std::endl;

  _inFunctionBody = false;

  if (node->type()->name() != cdk::TYPE_VOID){
    if (node->type()->name() == cdk::TYPE_INT || node->type()->name() == cdk::TYPE_STRING || node->type()->name() == cdk::TYPE_POINTER) {
      _pf.LOCAL(-size_return_value);
      _pf.LDINT();
      _pf.STFVAL32();
    } else if (node->type()->name() == cdk::TYPE_DOUBLE) {
      _pf.LOCAL(-size_return_value);
      _pf.LDDOUBLE();
      _pf.STFVAL64();
    } else {
      std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
    }
  }

  _pf.LABEL(_defaultReturnLabel);
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop();

  if (node->identifier() == "fir") {
    // declare external functions
    for (std::string s : _functions_to_declare)
      _pf.EXTERN(s);
  }
}

void fir::postfix_writer::do_identity_node(fir::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);
}

void fir::postfix_writer::do_if_else_node(fir::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string lbl_else = mklbl(++_lbl);
  std::string lbl_end = mklbl(++_lbl);

  node->condition()->accept(this, lvl);
  _pf.JZ(lbl_else);

  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(lbl_end);

  _pf.LABEL(lbl_else);
  node->elseblock()->accept(this, lvl + 2);

  _pf.LABEL(lbl_end);
}

void fir::postfix_writer::do_if_node(fir::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string lbl_end = mklbl(++_lbl);
  node->condition()->accept(this, lvl);
  _pf.JZ(lbl_end);
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(lbl_end);
}

void fir::postfix_writer::do_index_node(fir::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->base()) {
    node->base()->accept(this, lvl);
  } else {
    if (_function) {
      _pf.LOCV(-_function->type()->size());
    } else {
      std::cerr << "FATAL: " << node->lineno() << ": trying to use return value outside function" << std::endl;
    }
  }
  node->index()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ADD();
}

void fir::postfix_writer::do_input_node(fir::input_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->is_typed(cdk::TYPE_INT))
  {
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE))
  {
    _pf.CALL("readd");
    _pf.LDFVAL64();
  }
}

void fir::postfix_writer::do_leave_node(fir::leave_node * const node, int lvl) {
  if(_leave.size() != 0){
    _pf.JMP(_leave[_leave.size() - node->level()]);
  }  
  else{ 
    throw std::string("leave instruction not in a cycle");
  }
}

void fir::postfix_writer::do_null_node(fir::null_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}

void fir::postfix_writer::do_print_node(fir::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));

    std::shared_ptr<cdk::basic_type> etype = child->type();
    child->accept(this, lvl);
    if (etype->name() == cdk::TYPE_INT) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4);
    } else if (etype->name() == cdk::TYPE_DOUBLE) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8);
    } else if (etype->name() == cdk::TYPE_STRING) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4);
    } else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

void fir::postfix_writer::do_restart_node(fir::restart_node * const node, int lvl) {
  if(_restart.size() != 0){
    _pf.JMP(_restart[_restart.size() - node->level()]);
  }  
  else{ 
    throw std::string("leave instruction not in a cycle");
  }
}

void fir::postfix_writer::do_return_node(fir::return_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_returnLabel != "") _pf.JMP(_returnLabel);
  _pf.JMP(_defaultReturnLabel);
}

void fir::postfix_writer::do_sizeof_node(fir::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}

void fir::postfix_writer::do_stack_alloc_node(fir::stack_alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ALLOC(); // allocate
  _pf.SP(); // put base pointer in stack
}

void fir::postfix_writer::do_variable_declaration_node(fir::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto id = node->identifier();

  std::cout << "INITIAL OFFSET: " << _offset << std::endl;

  int offset = 0, typesize = node->type()->size();

  std::cout << "ARG: " << id << ", " << typesize << std::endl;
  if (_inFunctionBody) {
    std::cout << "IN BODY" << std::endl;
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    std::cout << "IN ARGS" << std::endl;
    offset = _offset;
    _offset += typesize;
  } else {
    std::cout << "GLOBAL!" << std::endl;
    offset = 0;
  }
  std::cout << "OFFSET: " << id << ", " << offset << std::endl;

  auto symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    if (node->initializer()) {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initializer()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else {
        std::cerr << "cannot initialize" << std::endl;
      }
    }
  } else {
    if (!_function) {
      if (node->initializer() == nullptr) {
        _pf.BSS();
        _pf.ALIGN();
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      } else {

        if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
          if (node->constant()) {
            _pf.RODATA();
          } else {
            _pf.DATA();
          }
          _pf.ALIGN();
          _pf.LABEL(id);

          if (node->is_typed(cdk::TYPE_INT)) {
            node->initializer()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_POINTER)) {
            node->initializer()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
            if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
              node->initializer()->accept(this, lvl);
            } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
              cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
              cdk::double_node ddi(dclini->lineno(), dclini->value());
              ddi.accept(this, lvl);
            } else {
              std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
            }
          }
        } else if (node->is_typed(cdk::TYPE_STRING)) {
          if (node->constant()) {
            int litlbl;
            _pf.RODATA();
            _pf.ALIGN();
            _pf.LABEL(mklbl(litlbl = ++_lbl));
            _pf.SSTRING(dynamic_cast<cdk::string_node*>(node->initializer())->value());
            _pf.ALIGN();
            _pf.LABEL(id);
            _pf.SADDR(mklbl(litlbl));
          } else {
            _pf.DATA();
            _pf.ALIGN();
            _pf.LABEL(id);
            node->initializer()->accept(this, lvl);
          }
        } else {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
        }
      }
    }
  }
}

void fir::postfix_writer::do_prologo_node(fir::prologo_node * const node, int lvl) {
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
}

void fir::postfix_writer::do_while_node(fir::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string cond = mklbl(++_lbl);
  _restart.push_back(cond);

  std::string finally, end;

  if (node->finally()){
    finally = mklbl(++_lbl);
    _leave.push_back(finally);
  }
  else {
    end = mklbl(++_lbl);
    _leave.push_back(end);
  }  

  _pf.LABEL(cond);
  node->condition()->accept(this, lvl);
  if (node->finally()) _pf.JZ(finally);
  else _pf.JZ(end);

  node->block()->accept(this, lvl + 2);
  _pf.JMP(cond);

  if (node->finally()){
    _pf.LABEL(finally);
    node->finally()->accept(this, lvl + 2);
  } 
  else{
    _pf.LABEL(end);
  }

  _restart.pop_back();
  _leave.pop_back();
}
