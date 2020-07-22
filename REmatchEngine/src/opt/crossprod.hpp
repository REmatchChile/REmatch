#ifndef OPT__CROSSPROD_HPP
#define OPT__CROSSPROD_HPP

#include <map>

#include "automata/eva.hpp"
#include "automata/lvastate.hpp"

// Computes the inplace cross product of an ExtendedVA with the two-state
// automaton defined by:
//
//    state | filter   capture
//   --------------------------
//      0   |   1         0
//      1   |   0         1
//
// This is an optimization for the evaluation algorithm. It permits to
// leave out the need to check correct NodeList transpassing between
// transitioning DetStates at reading() step.
void crossProdOpt(ExtendedVA &A) {

    // --- Duplicate states --- //

    std::vector<LVAState*> states0, states1;

    // State pointer to index at statevector
    std::map<LVAState*, size_t> ptr2index;
    size_t initStateIdx;

    for (size_t i = 0; i < A.states.size(); ++i) {
        states0.push_back(new LVAState());
        states1.push_back(new LVAState());

        // Store state's ptr corresponding index
        ptr2index[A.states[i]] = i;

        if(A.states[i] == A.initState())
            initStateIdx = i;

        // Set final states
        if(A.states[i]->isFinal) {
            states0.back()->setFinal(true);
            states1.back()->setFinal(true);
        }
    };

    // --- Connect new states --- //

    // Here p and q represent transition p->q
    LVAState *pOld, *qOld, *p0New, *p1New, *q0New, *q1New;
    size_t qOldIdx; // Needed for pairing new states to old states
    for (size_t i = 0; i < A.states.size(); ++i) {
        pOld = A.states[i];
        p0New = states0[i]; p1New = states1[i];
        for (auto const &filter: pOld->f) {
            qOld = filter->next;
            qOldIdx = ptr2index[qOld];

            q0New = states0[qOldIdx]; q1New = states1[qOldIdx];

            p0New->addFilter(filter->code, q1New);
            p1New->addFilter(filter->code, q0New);
        }
        for (auto const &capture: pOld->c) {
            qOld = capture->next;
            qOldIdx = ptr2index[qOld];

            q0New = states0[qOldIdx]; q1New = states1[qOldIdx];

            p0New->addCapture(capture->code, q0New);
            p1New->addCapture(capture->code, q1New);
        }

    }

    // --- Set new states and prune unreachable states --- //

    // Concat both vectors
    states0.insert(states0.end(), states1.begin(), states1.end());

    A.set_initState(states0[initStateIdx]); // Init state is (q0, 0)
    A.states = std::move(states0);
    A.pruneUselessStates();


}

#endif // OPT__CROSSPROD__HPP