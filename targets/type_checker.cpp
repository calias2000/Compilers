#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"
#include <cdk/types/primitive_type.h>
#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void fir::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) { }
void fir::type_checker::do_data_node(cdk::data_node *const node, int lvl) { }

void fir::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong type in unary logical expression");
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_ScalarLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary logical expression (left)");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary logical expression (right)");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_BooleanLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}
void fir::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}

void fir::type_checker::do_GeneralLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->type() != node->right()->type()) {
    throw std::string("same type expected on both sides of equality operator");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void fir::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void fir::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else
    throw std::string("Wrong type in argument of unary expression (Integer or double expected).");
}

void fir::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_IDExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong types in binary expression");
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_PIDExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong types in binary expression");
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  do_PIDExpression(node, lvl);
}
void fir::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  do_PIDExpression(node, lvl);
}
void fir::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  do_IDExpression(node, lvl);
}
void fir::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  do_IDExpression(node, lvl);
}
void fir::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  do_ScalarLogicalExpression(node, lvl);
}
void fir::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  do_GeneralLogicalExpression(node, lvl);
}
void fir::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  do_GeneralLogicalExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (!symbol) {
    throw std::string("undeclared variable '" + id + "'");
  } else {
    node->type(symbol->type());
  }
}

void fir::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

//-------------------------------------------------------------------------

void fir::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {

    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else {
      throw std::string("wrong assignment to integer");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {

    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      node->type(node->rvalue()->type());
    } else if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    } else {
      throw std::string("wrong assignment to pointer");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {

    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else {
      throw std::string("wrong assignment to float");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {

    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    } else {
      throw std::string("wrong assignment to string");
    }

  } else {
    throw std::string("wrong types in assignment");
  }
}


void fir::type_checker::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}



//--------------------------------- AST FOLDER NODES ------------------------------------------------------


void fir::type_checker::do_address_of_node(fir::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void fir::type_checker::do_bloco_node(fir::bloco_node *const node, int lvl) { }

void fir::type_checker::do_corpo_node(fir::corpo_node *const node, int lvl) { }

void fir::type_checker::do_function_call_node(fir::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->identifier();
  auto symbol = _symtab.find(id);

  if (!symbol->isFunction()){
    throw std::string("symbol '" + id + "' is not a function.");
  }
  if (symbol == nullptr){
    throw std::string("symbol '" + id + "' is undeclared.");
  }

  node->type(symbol->type());

  if (node->arguments()->size() != symbol->number_of_arguments()) {
    throw std::string(
        "number of arguments in call (" + std::to_string(node->arguments()->size()) + ") must match declaration ("
            + std::to_string(symbol->number_of_arguments()) + ").");
  } else {
    node->arguments()->accept(this, lvl + 4);
    for (size_t ax = 0; ax < node->arguments()->size(); ax++) {
      if (node->argument(ax)->type() == symbol->argument_type(ax)) continue;
      if (symbol->argument_is_typed(ax, cdk::TYPE_DOUBLE) && node->argument(ax)->is_typed(cdk::TYPE_INT)) continue;
      throw std::string("type mismatch for argument " + std::to_string(ax + 1) + " of '" + id + "'.");
    }
  }
}

void fir::type_checker::do_function_declaration_node(fir::function_declaration_node *const node, int lvl) {

  std::string id;

  if (node->identifier() == "fir")
    id = "_main";
  else if (node->identifier() == "_main")
    id = "._main";
  else
    id = node->identifier();

  auto function = fir::make_symbol(node->qualifier(), node->type(), id, false, true, true);

  std::vector < std::shared_ptr < cdk::basic_type >> argtypes;
  for (size_t ax = 0; ax < node->arguments()->size(); ax++)
    argtypes.push_back(node->argument(ax)->type());
  function->set_argument_types(argtypes);

  std::shared_ptr<fir::symbol> previous = _symtab.find(function->name());

  if (previous) {
    _symtab.replace(function->name(), function);
    _parent->set_new_symbol(function);
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

void fir::type_checker::do_function_definition_node(fir::function_definition_node *const node, int lvl) {

  if(node->return_value()){
    if (node->type() != nullptr && node->is_typed(cdk::TYPE_VOID)) throw std::string("initializer specified for void function.");
    node->return_value()->accept(this, lvl);
  }

  std::string id;

  if (node->identifier() == "fir")
    id = "_main";
  else if (node->identifier() == "_main")
    id = "._main";
  else
    id = node->identifier();

  auto function = fir::make_symbol(node->qualifier(), node->type(), id, false, true);

  std::vector < std::shared_ptr < cdk::basic_type >> argtypes;
  size_t var = 0;
  while (var < node->arguments()->size()){
    argtypes.push_back(node->argument(var)->type());
    var++;
  }
  function->set_argument_types(argtypes);

  std::shared_ptr<fir::symbol> previous = _symtab.find(function->name());
  if (previous) {
    if (previous->forward()) {
      _symtab.replace(function->name(), function);
      _parent->set_new_symbol(function);
    } else {
      throw std::string("conflicting definition for '" + function->name() + "'");
    }
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

void fir::type_checker::do_identity_node(fir::identity_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void fir::type_checker::do_if_node(fir::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT)) throw std::string("expected integer condition");
}

void fir::type_checker::do_if_else_node(fir::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT)) throw std::string("expected integer condition");
}

void fir::type_checker::do_index_node(fir::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->base()->accept(this, lvl + 2);
  if (!node->base()->is_typed(cdk::TYPE_POINTER))
    throw std::string("Index left-value must be a pointer.");

  node->index()->accept(this, lvl + 2);

  if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("Integer expression expected in left-value index.");

  node->type(cdk::reference_type::cast(node->base()->type())->referenced());
}

void fir::type_checker::do_input_node(fir::input_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

void fir::type_checker::do_leave_node(fir::leave_node *const node, int lvl) { }

void fir::type_checker::do_null_node(fir::null_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

void fir::type_checker::do_print_node(fir::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
}

void fir::type_checker::do_restart_node(fir::restart_node *const node, int lvl) { }

void fir::type_checker::do_return_node(fir::return_node *const node, int lvl) { }

void fir::type_checker::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_stack_alloc_node(fir::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in allocation expression");
  }
  auto mytype = cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  node->type(mytype);
}

void fir::type_checker::do_variable_declaration_node(fir::variable_declaration_node *const node, int lvl) {
  if (node->initializer() != nullptr) {
    node->initializer()->accept(this, lvl + 2);

    if (node->is_typed(cdk::TYPE_INT)) {
      if (!node->initializer()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type for initializer (integer expected).");
    } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
        throw std::string("wrong type for initializer (integer or double expected).");
      }
    } else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
        throw std::string("wrong type for initializer (string expected).");
      }
    } else if (node->is_typed(cdk::TYPE_POINTER)) {
      if (!node->initializer()->is_typed(cdk::TYPE_POINTER)) {
        auto in = (cdk::literal_node<int>*)node->initializer();
        if (in == nullptr || in->value() != 0) throw std::string("wrong type for initiiiializer (pointer expected).");
      }
    } else {
      throw std::string("unknown type for initializer.");
    }
  }

  const std::string &id = node->identifier();
  auto symbol = fir::make_symbol(node->qualifier(), node->type(), id, (bool)node->initializer(), false);
  if (_symtab.insert(id, symbol)) {
    _parent->set_new_symbol(symbol);
  } else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

void fir::type_checker::do_prologo_node(fir::prologo_node *const node, int lvl) { }

void fir::type_checker::do_while_node(fir::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void fir::type_checker::do_for_node(fir::for_node *const node, int lvl) {
  node->innit()->accept(this, lvl + 4);
  node->condition()->accept(this, lvl + 4);
  node->increment()->accept(this, lvl + 4);
}