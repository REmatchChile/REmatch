#include "visitor.hpp"
#include "ast.hpp"
#include "automata/lva.hpp"
#include "automata/lvastate.hpp"
#include "factories/factories.hpp"
#include "charclass.hpp"

#include <iostream>
#include <bitset>

// Charclass visitor
CharClass &charclass_visitor :: operator()(automata::special_set const &ss) const
{
    CharClass *chrcls = new CharClass(ss, true);
    return *chrcls;
}


CharClass &charclass_visitor :: operator()(automata::charset const &cs) const
{
    bool negated;
    std::set<CharClass::range> ranges;
    std::set<char> singles;

    negated = bool(cs.negated);
    for (automata::charset::element const &element : cs.elements)
    {
        // If element is a char
        if (element.which() == 0)
        {
            singles.insert(get<char>(element));
        }
        // If element is a range (boost::tuple<char, char>)
        else
        {
            automata::range b_tuple = get<automata::range>(element);
            std::tuple<char, char> s_tuple = std::make_tuple(b_tuple.get<0>(), b_tuple.get<1>());
            ranges.insert(s_tuple);
        }
    }

    CharClass *chrcls = new CharClass(negated, ranges, singles);
    return *chrcls;
}

CharClass &charclass_visitor :: operator()(char const &sc) const
{
    std::string s = std::string(1, sc);
    CharClass *chrcls = new CharClass(s, false);
    return *chrcls;
}

// File to automata visitor
file_to_automata :: file_to_automata()
{
    automata = new LogicalVA();
}

LVAState *file_to_automata :: get_state(std::string state_name)
{
    if (!states_map.count(state_name))
    {
        LVAState* state = new LVAState();
        states_map[state_name] = state;
        automata->states.push_back(state);
    }
    return states_map.at(state_name);
}

std::bitset<32> file_to_automata :: get_variable_code(automata::variable const &variable)
{
    if (!automata->varFactory()->isMember(variable.name))
    {
        automata->varFactory()->addVar(variable.name);
    }
    if (variable.is_opening)
    {
        return automata->varFactory()->getOpenCode(variable.name);
    }
    return automata->varFactory()->getCloseCode(variable.name);
}

int file_to_automata :: get_filter_code(automata::charclass const &automata_charclass) {
    CharClass charclass = boost::apply_visitor(charclass_visitor(), automata_charclass);
    if (!automata->filterFactory()->isMember(charclass))
    {
        automata->filterFactory()->addFilter(charclass);
    }
    return automata->filterFactory()->getCode(charclass);
}

// Case 1: char transition
void file_to_automata :: operator()(automata::char_transition const &ct)
{
    LVAState *from_state = get_state(ct.from_state);
    LVAState *to_state = get_state(ct.to_state);
    int filter_code = get_filter_code(ct.text);
    from_state->addFilter(filter_code, to_state);
}

// Case 2: variable transition
void file_to_automata :: operator()(automata::variable_transition const &vt)
{
    LVAState *from_state = get_state(vt.from_state);
    LVAState *to_state = get_state(vt.to_state);

    std::bitset<32> bitset_code;
    for (automata::variable const &variable : vt.variables)
    {
        bitset_code |= get_variable_code(variable);
    }
    from_state->addCapture(bitset_code, to_state);
}

// Case 3: epsilon transition
void file_to_automata :: operator()(automata::epsilon_transition const &et)
{
    LVAState *from_state = get_state(et.from_state);
    LVAState *to_state = get_state(et.to_state);
    from_state->addEpsilon(to_state);
}

// Case 4: initial state
void file_to_automata :: operator()(automata::initial_state const &is)
{
    LVAState *state = get_state(is.state);
    automata->initState()->addEpsilon(state);
}

// Case 5: final state
void file_to_automata :: operator()(automata::final_state const &fs)
{
    LVAState *state = get_state(fs.state);
    state->setFinal(true);
    automata->finalStates.push_back(state);
}