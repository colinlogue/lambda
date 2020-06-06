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
        Reducer() = delete;
        explicit Reducer(Context context) : context {std::move(context)} {}
        auto operator ()(const Variable& variable) -> Term;
        auto operator ()(const Abstraction& abstr) -> Term;
        auto operator ()(const Application& appl)  -> Term;

        auto reduce_term(const Term& term, Context local = {}) -> Term;

    private:
        Context context;
    };

    auto Reducer::operator()(const Variable& variable) -> Term
    {
        // attempt to substitute variable
        auto search = context.find(variable.name);
        if (search != context.end())
            return reduce_term(search->second);

        // otherwise variable can't reduce so return it
        return variable;
    }

    auto Reducer::operator()(const Abstraction& abstr) -> Term
    {
        // abstraction is value, so recursively reduce inner term and return
        return Abstraction {abstr.name, reduce_term(*abstr.body)};
    }

    auto Reducer::operator()(const Application& appl) -> Term
    {
        // reduce left side first
        Term lhs {reduce_term(*appl.lhs)};

        // if new lhs is abstraction, substitute rhs into it
        Abstraction* lhs_as_abstr {std::get_if<Abstraction>(&lhs)};
        if (lhs_as_abstr)
        {
            // TODO: abstraction->name.name is ugly
            auto name {lhs_as_abstr->name.name};
            auto body {*(lhs_as_abstr->body)};
            Term subbed {substitute({name, *appl.rhs}, body)};
            return reduce_term(subbed, context);
        }
        // otherwise reduce rhs and return application
        return Application {lhs, reduce_term(*appl.rhs)};
    }

    auto Reducer::reduce_term(const Term& term, Context local) -> Term
    {
        Context global {context};
        local.merge(global);
        Reducer reducer {local};
        return {std::visit(reducer, term)};
    }

    /**
     * Attempts to contract a term by seeing if any of the terms in the context
     * are equivalent and subbing in variable names for any that match.
     */
    auto contract_term(const Term& term, const Context& context) -> Term
    {
        for (auto [name, val] : context)
        {
            if (term == val) return Variable {name};
        }
        const Abstraction* term_as_abstr {std::get_if<Abstraction>(&term)};
        if (term_as_abstr != nullptr)
        {
            const Abstraction& abstr {*term_as_abstr};
            return Abstraction {abstr.name, contract_term(*abstr.body, context)};
        }
        auto term_as_appl {std::get_if<Application>(&term)};
        if (term_as_appl != nullptr)
        {
            const Application& appl {*term_as_appl};
            return Application {contract_term(*appl.lhs, context), contract_term(*appl.rhs, context)};
        }
        return term;
    }

    auto reduce(const Term& term, const Context& context) -> Term
    {
        Reducer reducer {context};
        Term reduction {reducer.reduce_term(term)};
        return reducer.reduce_term(term);
    }

    auto evaluate(const Term& term, const Context& context) -> EvalResult
    {
        Reducer reducer {context};
        Term reduction {reducer.reduce_term(term, context)};

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