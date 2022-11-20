#ifndef __FIR_TARGETS_XML_TARGET_H__
#define __FIR_TARGETS_XML_TARGET_H__

#include <iostream>
#include <memory>
#include <cdk/targets/basic_target.h>
#include <cdk/symbol_table.h>
#include <cdk/ast/basic_node.h>
#include <cdk/compiler.h>
#include <cdk/emitters/postfix_ix86_emitter.h>
#include "targets/xml_writer.h"
#include "targets/symbol.h"

namespace fir {

  class xml_target: public cdk::basic_target {
    static xml_target _self;

  private:
    xml_target() :
        cdk::basic_target("xml") {
    }

  public:
    bool evaluate(std::shared_ptr<cdk::compiler> compiler) {
      cdk::symbol_table<fir::symbol> symtab;
      xml_writer writer(compiler, symtab);
      compiler->ast()->accept(&writer, 0);
      return true;
    }

  };

} // fir

#endif
