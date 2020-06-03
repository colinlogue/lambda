//
// Created by colin on 6/3/20.
//

#include <bandit/bandit.h>

#include "eval.h"

using namespace snowhouse;
using namespace bandit;

using namespace lambda;

auto app(Term lhs, Term rhs) -> Application
{
    return Application {lhs, rhs};
}

auto var(std::string name) -> Variable
{
    return Variable {std::move(name)};
}

auto lam(std::string name, Term body) -> Abstraction
{
    return Abstraction {std::move(name), body};
}

go_bandit([]() {
    describe("reduction tests", []() {
        const Term tru {lam("t", lam("f", var("t")))};
        const Term fls {lam("t", lam("f", var("f")))};
        it ("tru doesn't reduce", [&]() {
            AssertThat(reduce(tru), Equals(tru));
        });
    });
});

int main(int argc, char* argv[]) {
    return bandit::run(argc, argv);
}