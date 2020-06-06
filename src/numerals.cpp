//
// Created by colin on 6/6/20.
//

#include <sstream>

#include "numerals.h"
#include "helpers.h"


namespace lambda
{
    const static Term zero {lam("s", lam("z", var("z")))};
    const static Term succ {lam("n", lam("s", lam("z", app(var("s"), app(app(var("n"), var("s")), var("z"))))))};

    auto parse_numeral(const std::string& str) -> ParseResult
    {
        try
        {
            int val {std::stoi(str)};
            if (val < 0)
            {
                std::stringstream err_msg {};
                err_msg << "Invalid numeral: " << val << " is less than zero.";
                return ParseResult::make_err(err_msg.str());
            }
            return ParseResult::make_ok(to_numeral(static_cast<uint>(val)));
        }
        catch (...)
        {
            return ParseResult::make_err("Unable to parse " + str + " as numeral");
        }
    }

    auto contract_numeral(const Term& term) -> Term
    {
        std::optional<int> maybe_num {from_numeral(term)};
        if (maybe_num.has_value())
            return Variable {std::to_string(maybe_num.value())};
        return term;
    }

    auto from_numeral(const Term& term) -> std::optional<int>
    {
        // goal is to check if matches \s.\z.s s s z for some number of s

        // first check if outermost term is abstraction
        const Abstraction* s_abstr {std::get_if<Abstraction>(&term)};
        if (s_abstr)
        {
            std::string s_name {s_abstr->name.name};

            // then check if next outermost term is abstraction
            const Abstraction* z_abstr {std::get_if<Abstraction>(&(*s_abstr->body))};
            if (z_abstr)
            {
                std::string z_name {z_abstr->name.name};

                // then check if body is just the z variable (case of zero)
                const Variable* body_as_var {std::get_if<Variable>(&(*z_abstr->body))};
                if (body_as_var)
                {
                    if (body_as_var->name == z_name)
                        return 0;
                    else
                        return {};
                }

                // or if it is an application of the s-bound variable some number of times to z
                const Application* body_as_app {std::get_if<Application>(&(*z_abstr->body))};
                int count {0};
                while (body_as_app)
                {
                    ++count;

                    // if lhs is not s variable, return nothing
                    const Variable* lhs_as_var {std::get_if<Variable>(&(*body_as_app->lhs))};
                    if (!lhs_as_var || lhs_as_var->name != s_name)
                        return {};

                    // check if rhs is just z variable
                    const Variable* rhs_as_var {std::get_if<Variable>(&(*body_as_app->rhs))};
                    if (rhs_as_var)
                    {
                        if (rhs_as_var->name == z_name)
                            return count;
                    }

                    // continue loop is rhs is application
                    body_as_app = std::get_if<Application>(&(*body_as_app->rhs));
                }
            }
        }
        return {};
    }

    auto to_numeral(uint val) -> Term
    {
        Term body {var("z")};
        while (val > 0)
        {
            body = app(var("s"), body);
            --val;
        }
        return lam("s", lam("z", body));
    }
}