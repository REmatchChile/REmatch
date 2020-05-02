#include "evaluation.hpp"
#include "automata/detstate.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "mapping.hpp"

Evaluation :: Evaluation(ExtendedVA *A, std::istream &is, std::ostream &os,
                         bool cont, bool lbl)
  : inputStream(is),
    inputString(),
    outputStream(os),
    i(0),
    il(0),
    earlyOutput(cont),
    lineByLine(lbl),
    kNoCaptures(A->vFact->size() == 0)
{
  std::array<rematch::vector<DetState*> *, 3> arr = {
    &currentStates, // IMPORTANT! Don't change the position of this vector!
    &newStates,
    &captureStates
  };

  detA = new DetAutomaton(A->vFact);
  detManager = new DetManager(A, detA, arr);
  memManager = new MemManager();
  enumerator = new Enumerator(os, A->vFact, memManager);
  (lineByLine) ? initLineByLine() : initNoLineByLine();
}

// Init on case lineByLine it's false
inline void Evaluation :: initNoLineByLine() {
  inputStream.seekg(0, inputStream.end);
	inputString.resize(inputStream.tellg());
	inputStream.seekg(0, inputStream.beg);
	inputStream.read(&inputString[0], inputString.size());
  initAutomaton(0);
}

// Init on case lineByLine it's true
inline void Evaluation :: initLineByLine() {
  std::getline(inputStream, inputString);
  initAutomaton(0);
}

// Evaluation algorithm initialization, here the argument i
// represents the document position at which to initialize
// (useful in line by line processing)
inline void Evaluation :: initAutomaton(long i) {
  detA->initState->visited = i;
  detA->initState->currentL->add(memManager->alloc());

  currentStates.clear();
  currentStates.push_back(detA->initState);

  captureStates.clear();
  if(!detA->initState->c.empty())
      captureStates.push_back(detA->initState);

  capture(i);
}


std::map<std::string, std::pair<size_t, size_t>> Evaluation :: next() {
    return this->enumerator->next();
}

inline std::string Evaluation :: pnext() {
  auto mapping = next();
  std::stringstream ss;
  for(auto const& keyValue : mapping) {
    ss << keyValue.first << " = |" << keyValue.second.first << "," << keyValue.second.second << ">\t";
  }
  ss << "\n";
  return ss.str();
}

std::vector<std::string> Evaluation :: getOutputSchema() {
  return detA->vFact->getOutputSchema();
}

bool Evaluation :: match() {
  char a;
  DetState *nextState, *currentState;
  currentState = currentStates[0];

  while( i < inputString.size() ) {
    a = (char) inputString[i];

    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) // Then maybe a determinization is needed
      nextState = detManager->getNextDetState(currentState, a);

    if (nextState->isSuperFinal) {
      return true;
    }

    if (nextState->ss->isNonEmpty)  // Check if not empty set
      currentState = nextState;
    else
      return false;

    i++;
  }

  return currentState->isFinal;
}

void Evaluation :: evaluate() {
  bool(Evaluation::*hasNext)(); // Function pointer declare
  if(kNoCaptures) { //
    detManager->computeFullDetAutomaton();
    // std::cout << detA->pprint() << '\n';
    // std::cout << "Uniform sample: " << detManager->uniformSample(20) << '\n';
    if(match()) enumerator->numOfMappings++;
    return;
  }
  if(earlyOutput) {
    if (lineByLine) {
      hasNext = &Evaluation::hasNext11;
    }
    else {
      hasNext = &Evaluation::hasNext10;
    }
  }
  else {
    if(lineByLine) {
      hasNext = &Evaluation::hasNext01;
    }
    else {
      hasNext = &Evaluation::hasNext00;
    }
  }
  // detManager->computeFullDetAutomaton();


  while((this->*hasNext)()) {
    this->outputStream << this->pnext();
  }


  // std::cout << detA->pprint() << '\n';
  // std::cout << "Uniform sample: " << detManager->uniformSample(20) << '\n';
}

// The 00 in hasNext00 represents no early output and no
// line by line processing.
bool Evaluation :: hasNext00() {
  if(!enumerator->hasNext() && i < this->inputString.size()) {
    char a;
    outputList.reset();
    while(i < this->inputString.size()) {
      a = (char) inputString[i];
      reading(a, i);
      // addToHistogram(captureStates.size());
      this->currentStates.swap(this->newStates);
      capture(i+1);
      i++;
    }
    for(auto &state: currentStates) {
      if(state->isFinal)
        this->outputList.append(state->currentL);
    }
    this->enumerator->addNodeList(outputList);
  }
  return enumerator->hasNext();
}

// The 10 in hasNext10 represents early output and no
// line by line processing.
inline bool Evaluation :: hasNext10() {
  if(!enumerator->hasNext() && i < this->inputString.size()) {
    char a;
    outputList.reset();
    while(i < this->inputString.size()) {
      a = (char) inputString[i];
      readingEO(a, i);
      this->currentStates.swap(this->newStates);
      captureEO(i+1);
      i++;
    }
    if(i == this->inputString.size()) {
      for(auto &state: currentStates) {
        if(state->isFinal)
          this->outputList.append(state->currentL);
      }
    }
    if(!this->outputList.empty())
      this->enumerator->addNodeList(outputList);
  }
  return enumerator->hasNext();
}

// The 01 in hasNext01 represents no early output and
// line by line processing.
inline bool Evaluation :: hasNext01() {
  if(!enumerator->hasNext() && (i - il) < this->inputString.size()) {
    char a;
    outputList.reset();
    while((i - il) < this->inputString.size()) {
      a = (char) inputString[(i-il)];
      reading(a, i);
      this->currentStates.swap(this->newStates);
      capture(i+1);
      i++;
    }
    if((i - il) == this->inputString.size()) {
      for(auto &state: currentStates) {
        if(state->isFinal)
          this->outputList.append(state->currentL);
      }
    }
    if(!this->outputList.empty())
      this->enumerator->addNodeList(outputList);
  }
  if (!enumerator->hasNext()) {
    if(std::getline(this->inputStream, this->inputString)) {
      i++;
      il = i;
      initAutomaton(il);
      return hasNext01();
    }
  }
  return enumerator->hasNext();
}

// The 11 in hasNext11 represents early output and
// line by line processing.
inline bool Evaluation :: hasNext11() {
  if(!enumerator->hasNext() && (i - il) < this->inputString.size()) {
    char a;
    outputList.reset();
    while((i - il) < this->inputString.size()) {
      a = (char) inputString[(i-il)];
      readingEO(a, i);
      this->currentStates.swap(this->newStates);
      captureEO(i+1);
      i++;
    }
    if((i - il) == this->inputString.size()) {
      for(auto &state: currentStates) {
        if(state->isFinal)
          this->outputList.append(state->currentL);
      }
    }
    if(!this->outputList.empty())
      this->enumerator->addNodeList(outputList);
  }
  if (!enumerator->hasNext()) {
    if(std::getline(this->inputStream, this->inputString)) {
      i++;
      il = i;
      initAutomaton(il);
      return hasNext11();
    }
  }
  return enumerator->hasNext();
}


inline void Evaluation :: reading(char a, long ip) {
  this->captureStates.clear();
  this->newStates.clear();
  DetState *nextState, *currentState;
  // uint nfa_current_states = 0;
  NodeList *prevList;

  for (size_t idx = 0; idx < currentStates.size(); idx++) {
    currentState = currentStates[idx];
    // nfa_current_states += currentState->ss->subset.size();

    #ifndef NOPT_CROSSPROD
    prevList = currentState->currentL;
    #else
    if(currentState->visited == ip+1) {
      prevList = currentState->oldL;
    }
    else {
      prevList = currentState->currentL;
    }
    #endif


    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) { // Then maybe a determinization is needed
      nextState = detManager->getNextDetState(currentState, a, idx);
    }

    if (nextState->ss->isNonEmpty) {  // Check if not empty set
      if (nextState->visited <= ip) { // If not already visited
        nextState->visited = ip+1; // Mark as visited

        #ifdef NOPT_CROSSPROD
        // Store nextState's currentL in oldL
        nextState->oldL->head = nextState->currentL->head;
        nextState->oldL->tail = nextState->currentL->tail;
        #endif

        // Pass the list to nextState
        nextState->currentL->head = prevList->head;
        nextState->currentL->tail = prevList->tail;


        this->newStates.push_back(nextState);

        if (!nextState->c.empty())
          captureStates.push_back(nextState);

      }
      else { // If already visited
        // Append (concat) nextState's currentL with previous list
        nextState->currentL->append(prevList);
      }
    }
    else { // If empty set is reached then consider adding to garbage collection
      currentState->currentL->resetRefs();
      memManager->addPossibleGarbage(prevList->head);
    }
  }
  // addToHistogram(nfa_current_states);
}

inline void Evaluation :: readingEO(char a, long ip) {
  this->captureStates.clear();
  this->newStates.clear();
  DetState* nextState;

  for (auto &currentState: currentStates) {

    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) { // Then maybe a determinization is needed
      nextState = detManager->getNextDetState(currentState, a);
    }

    if(nextState->isSuperFinal) {  // Early Output check
      outputList.append(currentState->currentL);
    }
    else {
      if (nextState->ss->isNonEmpty) {  // Check if not empty set
        if (nextState->visited <= ip) { // If not already visited
          nextState->visited = ip+1; // Mark as visited

          // Pass the list to nextState
          nextState->currentL->head = currentState->currentL->head;
          nextState->currentL->tail = currentState->currentL->tail;

          this->newStates.push_back(nextState);

          if (!nextState->c.empty())
            this->captureStates.push_back(nextState);

        }
        else { // If already visited
          // Append (concat) nextState's currentL with previous list
          nextState->currentL->append(currentState->currentL);
        }
      }
      else { // If empty set is reached then consider adding to garbage collection
        currentState->currentL->resetRefs();
        memManager->addPossibleGarbage(currentState->currentL->head);
      }
    }
  }
}

inline void Evaluation :: capture(long ip) {
  DetState* nextState;
  Node* newNode;

  for (auto &currentState: captureStates) {
    currentState->copiedList->head = currentState->currentL->head;
    currentState->copiedList->tail = currentState->currentL->tail;
  }

  for (auto &currentState: captureStates) {
    for (auto &capture: currentState->c) {

      nextState = capture->next;

      newNode = memManager->alloc(capture->S, ip, currentState->copiedList->head,
                                                currentState->copiedList->tail);

        if (nextState->visited < ip) {
          nextState->currentL->resetAndAdd(newNode);
          nextState->visited = ip;

          this->currentStates.push_back(nextState);
        }
        else {
          nextState->currentL->add(newNode);
        }
    }
  }
} // capture()

inline void Evaluation :: captureEO(long ip) {
  DetState* nextState;
  Node* newNode;

  for (auto &currentState: captureStates) {
    currentState->copiedList->head = currentState->currentL->head;
    currentState->copiedList->tail = currentState->currentL->tail;
  }

  for (auto &currentState: captureStates) {
    for (auto &capture: currentState->c) {

      nextState = capture->next;

      newNode = memManager->alloc(capture->S, ip, currentState->copiedList->head,
                                                currentState->copiedList->tail);
      if(nextState->isSuperFinal) {
        this->outputList.add(newNode);
      }
      else {
        if (nextState->visited < ip) {
          nextState->currentL->resetAndAdd(newNode);
          nextState->visited = ip;

          this->currentStates.push_back(nextState);
        }
        else {
          nextState->currentL->add(newNode);
        }
      }
    }
  }
} // captureEO()