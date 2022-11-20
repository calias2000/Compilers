#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"
#include "fir_parser.tab.h"

//---------------------------------------------------------------------------

void fir::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<data_node size='" << node->size() << "'>" << std::endl;
  closeTag(node, lvl);
}

void fir::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}

void fir::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void fir::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

void fir::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

static std::string qualifier_name(int qualifier) {
    if (qualifier == '*') return "public";
    if (qualifier == '?') return "require";
    if (qualifier == '\0') return "private";
    return "unknown qualifier";
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void fir::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void fir::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void fir::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl);
  reset_new_symbol();
  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_evaluation_node(fir::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_while_node(fir::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------


void fir::xml_writer::do_for_node(fir::for_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("innit", lvl + 2);
  node->innit()->accept(this, lvl + 4);
  closeTag("innit", lvl + 2);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("increment", lvl + 2);
  node->increment()->accept(this, lvl + 4);
  closeTag("increment", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_address_of_node(fir::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("lvalue", lvl + 2);
  node->lvalue()->accept(this, lvl + 4);
  closeTag("lvalue", lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_bloco_node(fir::bloco_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  if (node->declarations()) {
    openTag("declarations", lvl + 2);
    node->declarations()->accept(this, lvl + 4);
    closeTag("declarations", lvl + 2);
  }
  if (node->instructions()) {
    openTag("instructions", lvl + 2);
    node->instructions()->accept(this, lvl + 4);
    closeTag("instructions", lvl + 2);
  }
  closeTag(node, lvl);
}

void fir::xml_writer::do_corpo_node(fir::corpo_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  if (node->prologo()) {
    openTag("prologo", lvl + 2);
    node->prologo()->accept(this, lvl + 4);
    closeTag("prologo", lvl + 2);
  }
  if (node->bloco()){
    _symtab.push();
    openTag("bloco", lvl + 2);
    node->bloco()->accept(this, lvl + 4);
    closeTag("bloco", lvl + 2);
    _symtab.pop();
  }
  if (node->epilogo()) {
    _symtab.push();
    openTag("epilogo", lvl + 2);
    node->epilogo()->accept(this, lvl + 4);
    closeTag("epilogo", lvl + 2);
    _symtab.pop();
  }
  closeTag(node, lvl);
}

void fir::xml_writer::do_function_call_node(fir::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "'>" << std::endl;
  if (node->arguments()) {
    openTag("arguments", lvl + 2);
    node->arguments()->accept(this, lvl + 4);
    closeTag("arguments", lvl + 2);
  }
  closeTag(node, lvl);
}

void fir::xml_writer::do_function_declaration_node(fir::function_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  reset_new_symbol();
  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' type='"
       << cdk::to_string(node->type()) << "' qualifier='" << qualifier_name(node->qualifier()) << "'>" << std::endl;
  openTag("arguments", lvl);
  if (node->arguments()) {
    _symtab.push();
    node->arguments()->accept(this, lvl + 4);
    _symtab.pop();
  }
  closeTag("arguments", lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_function_definition_node(fir::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  reset_new_symbol();

  if (node->corpo())
      _symtab.push();

  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' type='"
       << cdk::to_string(node->type()) << "' qualifier='" << qualifier_name(node->qualifier()) << "'>" << std::endl;

  if (node->return_value()) {
      openTag("return_value", lvl + 2);
      node->return_value()->accept(this, lvl + 4);
      closeTag("return_value", lvl + 2);
  }
  if (node->arguments()) {
      openTag("arguments", lvl + 2);
      node->arguments()->accept(this, lvl + 4);
      closeTag("arguments", lvl + 2);
  }
  if (node->corpo()) {
      openTag("corpo", lvl + 2);
      node->corpo()->accept(this, lvl + 4);
      closeTag("corpo", lvl + 2);
  }

  closeTag(node, lvl);

  if (node->corpo())
      _symtab.pop();
}

void fir::xml_writer::do_identity_node(fir::identity_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void fir::xml_writer::do_if_node(fir::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_if_else_node(fir::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_index_node(fir::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("base", lvl + 2);
  node->base()->accept(this, lvl + 4);
  closeTag("base", lvl + 2);
  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_input_node(fir::input_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_leave_node(fir::leave_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_null_node(fir::null_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_print_node(fir::print_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->arguments()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_restart_node(fir::restart_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_return_node(fir::return_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_sizeof_node(fir::sizeof_node *const node, int lvl) {
  openTag(node, lvl);
  node->expression()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_stack_alloc_node(fir::stack_alloc_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}

void fir::xml_writer::do_variable_declaration_node(fir::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  reset_new_symbol();

  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' type='"
       << cdk::to_string(node->type()) << "' qualifier='" << qualifier_name(node->qualifier()) << "'>" << std::endl;
  
  if (node->initializer()) {
    openTag("initializer", lvl + 2);
    node->initializer()->accept(this, lvl + 4);
    closeTag("initializer", lvl + 2);
  }

  closeTag(node, lvl);
}

void fir::xml_writer::do_prologo_node(fir::prologo_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  openTag(node, lvl);
  
  if (node->declarations()) {
    openTag("declarations", lvl + 2);
    node->declarations()->accept(this, lvl + 4);
    closeTag("declarations", lvl + 2);
  }
  if (node->instructions()) {
    openTag("instructions", lvl + 2);
    node->instructions()->accept(this, lvl + 4);
    closeTag("instructions", lvl + 2);
  }

  closeTag(node, lvl);
}