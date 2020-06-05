#include <iostream>

#include "lib/lang_tools/repl/REPL.hpp"
#include "lib/result/Result.hpp"

#include "src/lex.h"
#include "src/parse.h"
#include "src/eval.h"
#include "src/prelude.h"


using lang_tools::REPL;
using namespace lambda;

int main()
{
    // trying out just reducing for the repl than evaluating to an abstraction
    // REPL<Token, Term, Value> repl {lex, parse, evaluate};
    REPL<Token, Term, Term> repl {lex, parse,
                          [](Term term, Context context)
                          {
                                return result::Result<Term, lang_tools::EvalErr>
                                        ::make_ok(reduce(term, context));
                          }
    };
    repl.load_context(prelude);
    repl.run();
}
