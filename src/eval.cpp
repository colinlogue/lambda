//
// Created by colin on 6/2/20.
//

#include <optional>
#include <sstream>

#include "parse.h"
#include "eval.h"

using std::optional;

namespace lambda
{

    class Reducer
    {
    public:
        /**
         * Reduce a term using normal-order strategy.
         */
        Reducer() = default;
        explicit Reducer(Context context) : context {std::move(context)} {}
        auto operator ()(const Variable& variable) -> Term;
        auto operator ()(const Abstraction& abstr) -> Term;
        auto operator ()(const Application& appl)  -> Term;

        auto reduce(const Term& term, Context local = {}) -> Term;
    private:
        Context context;
    };

    auto Reducer::operator()(const Variable& variable) -> Term
    {
        // attempt to substitute variable
        auto search = context.find(variable.name);
        if (search != context.end())
            return reduce(search->second);

        // otherwise variable can't reduce so return it
        return variable;
    }

    auto Reducer::operator()(const Abstraction& abstr) -> Term
    {
        // abstraction is value, so recursively reduce inner term and return
        return Abstraction {abstr.name, reduce(*abstr.body)};
    }

    auto Reducer::operator()(const Application& appl) -> Term
    {
        // reduce left side first
        Term lhs {reduce(*appl.lhs)};

        // if new lhs is abstraction, substitute rhs into it
        Abstraction* lhs_as_abstr {std::get_if<Abstraction>(&lhs)};
        if (lhs_as_abstr)
        {
            // TODO: abstraction->name.name is ugly
            auto name {lhs_as_abstr->name.name};
            auto body {*(lhs_as_abstr->body)};
            Term subbed {substitute({name, *appl.rhs}, body)};
            return reduce(subbed, context);
        }
        // otherwise reduce rhs and return application
        return Application {lhs, reduce(*appl.rhs)};
    }

    auto Reducer::reduce(const Term& term, Context local) -> Term
    {
        local.merge(context);
        Reducer reducer {local};
        return {std::visit(reducer, term)};
    }

    auto reduce(const Term& term, const Context& context) -> Term
    {
        Reducer reducer {context};
        return reducer.reduce(term);
    }

    auto evaluate(const Term& term, const Context& context) -> EvalResult
    {
        Reducer reducer {context};
        Term reduction {reducer.reduce(term, context)};

        // only abstractions are values
        Abstraction* result_as_abstr {std::get_if<Abstraction>(&reduction)};
        if (result_as_abstr)
            return EvalResult::make_ok(*result_as_abstr);

        // otherwise raise error
        std::stringstream err_msg {};
        err_msg << "Could not reduce term to value";
        return EvalResult::make_err(err_msg.str());
    }
}