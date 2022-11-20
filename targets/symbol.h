#ifndef __FIR_TARGETS_SYMBOL_H__
#define __FIR_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace fir {

  class symbol {

    std::string _name;
    int _qualifier;
    std::shared_ptr<cdk::basic_type> _type;
    std::vector<std::shared_ptr<cdk::basic_type>> _argument_types;
    bool _initialized;
    int _offset = 0;
    bool _function;
    bool _forward = false;

  public:
    symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized,
           bool function, bool forward = false) :
        _name(name), _qualifier(qualifier), _type(type), _initialized(initialized), _function(function), _forward(forward) {
    }

    ~symbol() {
    }


    // Retorna o id (nome) dado ao symbol (funcao/variavel)

    const std::string& name() const {
      return _name;
    }

    const std::string& identifier() const {
      return name();
    }


    // Retorna o qualifier do symbol, ou seja o que permite perceber se e public, require ou private

    int qualifier() const {
      return _qualifier;
    }


    // Retorna o tipo do symbol, ou seja, int, string, etc..

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }


    // Recebe um tipo (int, string, etc..) e verifica se e o mesmo que esta associado ao symbol

    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }


    // Serve para saber se a funcao/variavel ja foi inicializada ou nao

    bool initialized() const {
      return _initialized;
    }


    int offset() const {
      return _offset;
    }

    void set_offset(int offset) {
      _offset = offset;
    }


    // Serve para verificar se o symbol em questao e uma funcao

    bool isFunction() const {
      return _function;
    }


    // Serve para se verificar se uma variavel e global (offset == 0) ou nao

    bool global() const {
      return _offset == 0;
    }

    bool forward() const {
      return _forward;
    }


    // Mete num vetor os tipos dos argumentos da funcao <int, string>

    void set_argument_types(const std::vector<std::shared_ptr<cdk::basic_type>> &types) {
      _argument_types = types;
    }

    bool argument_is_typed(size_t ax, cdk::typename_type name) const {
      return _argument_types[ax]->name() == name;
    }


    // Retorna o tipo do argumento naquela posicao do vetor
    // E utilizado para verificar que o tipo do argumento enviado para uma funcao
    //      e o mesmo que esta e suposto receber

    std::shared_ptr<cdk::basic_type> argument_type(size_t ax) const {
      return _argument_types[ax];
    }


    // Retorna o tamanho do argumento em questao, ou seja o tamanho de int/string/etc..

    size_t argument_size(size_t ax) const {
      return _argument_types[ax]->size();
    }


    // Numero de argumentos da funcao
    // E utilizado por exemplo para verificar que o numero de argumentos na chamada de uma funcao
    //      e igual ao numero de argumentos com que foi declarada

    size_t number_of_arguments() const {
      return _argument_types.size();
    }

  };

  inline auto make_symbol(int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name,
                          bool initialized, bool function, bool forward = false) {
    return std::make_shared<symbol>(qualifier, type, name, initialized, function, forward);
  }

} // fir

#endif