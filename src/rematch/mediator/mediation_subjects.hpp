#ifndef MEDIATION_SUBJECTS_HPP
#define MEDIATION_SUBJECTS_HPP

#include "mediator.hpp"

namespace rematch {

struct MediationSubjects {
  SearchDFA search_dfa;
  ExtendedDetVA extended_det_va;
  std::shared_ptr<VariableCatalog> variable_catalog;
};

}

#endif
