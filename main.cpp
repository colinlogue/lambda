#include <iostream>

#include "lib/lang_tools/repl/REPL.hpp"
#include "lib/result/Result.hpp"

#include "src/lex.h"
#include "src/parse.h"
#include "src/eval.h"


using lang_tools::REPL;
using namespace lambda;

int main()
{

    REPL<Token, Term, Value> repl {lex, parse, evaluate};
}
