
// Generated from REmatchLexer.g4 by ANTLR 4.11.1


#include "REmatchLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct REmatchLexerStaticData final {
  REmatchLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  REmatchLexerStaticData(const REmatchLexerStaticData&) = delete;
  REmatchLexerStaticData(REmatchLexerStaticData&&) = delete;
  REmatchLexerStaticData& operator=(const REmatchLexerStaticData&) = delete;
  REmatchLexerStaticData& operator=(REmatchLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag rematchlexerLexerOnceFlag;
REmatchLexerStaticData *rematchlexerLexerStaticData = nullptr;

void rematchlexerLexerInitialize() {
  assert(rematchlexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<REmatchLexerStaticData>(
    std::vector<std::string>{
      "DECIMAL_DIGIT", "NOT_DECIMAL_DIGIT", "WHITESPACE", "NOT_WHITESPACE", 
      "ALPHANUMERIC", "NOT_ALPHANUMERIC", "TAB", "CARRIAGE_RETURN", "NEWLINE", 
      "VERTICAL_WHITESPACE", "FORM_FEED", "PIPE", "EXCLAMAITON", "L_CURLY", 
      "R_CURLY", "L_PAR", "R_PAR", "COMMA", "QUESTION", "PLUS", "STAR", 
      "HAT", "HYPHEN", "L_BRACK", "R_BRACK", "BACKSLASH", "ALPHA", "DIGIT", 
      "DOT", "UNRECOGNIZED"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'\\d'", "'\\D'", "'\\s'", "'\\S'", "'\\w'", "'\\W'", "'\\t'", 
      "'\\r'", "'\\n'", "'\\v'", "'\\f'", "'|'", "'!'", "'{'", "'}'", "'('", 
      "')'", "','", "'\\u003F'", "'+'", "'*'", "'^'", "'-'", "'['", "']'", 
      "'\\'", "", "", "'.'"
    },
    std::vector<std::string>{
      "", "DECIMAL_DIGIT", "NOT_DECIMAL_DIGIT", "WHITESPACE", "NOT_WHITESPACE", 
      "ALPHANUMERIC", "NOT_ALPHANUMERIC", "TAB", "CARRIAGE_RETURN", "NEWLINE", 
      "VERTICAL_WHITESPACE", "FORM_FEED", "PIPE", "EXCLAMAITON", "L_CURLY", 
      "R_CURLY", "L_PAR", "R_PAR", "COMMA", "QUESTION", "PLUS", "STAR", 
      "HAT", "HYPHEN", "L_BRACK", "R_BRACK", "BACKSLASH", "ALPHA", "DIGIT", 
      "DOT", "UNRECOGNIZED"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,30,132,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,1,0,1,0,1,0,1,1,1,1,1,1,1,2,1,2,1,2,1,3,1,3,1,3,1,4,1,
  	4,1,4,1,5,1,5,1,5,1,6,1,6,1,6,1,7,1,7,1,7,1,8,1,8,1,8,1,9,1,9,1,9,1,10,
  	1,10,1,10,1,11,1,11,1,12,1,12,1,13,1,13,1,14,1,14,1,15,1,15,1,16,1,16,
  	1,17,1,17,1,18,1,18,1,19,1,19,1,20,1,20,1,21,1,21,1,22,1,22,1,23,1,23,
  	1,24,1,24,1,25,1,25,1,26,1,26,1,27,1,27,1,28,1,28,1,29,1,29,0,0,30,1,
  	1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,
  	15,31,16,33,17,35,18,37,19,39,20,41,21,43,22,45,23,47,24,49,25,51,26,
  	53,27,55,28,57,29,59,30,1,0,2,2,0,65,90,97,122,1,0,48,57,131,0,1,1,0,
  	0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,
  	1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,
  	0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,
  	0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,
  	1,0,0,0,0,47,1,0,0,0,0,49,1,0,0,0,0,51,1,0,0,0,0,53,1,0,0,0,0,55,1,0,
  	0,0,0,57,1,0,0,0,0,59,1,0,0,0,1,61,1,0,0,0,3,64,1,0,0,0,5,67,1,0,0,0,
  	7,70,1,0,0,0,9,73,1,0,0,0,11,76,1,0,0,0,13,79,1,0,0,0,15,82,1,0,0,0,17,
  	85,1,0,0,0,19,88,1,0,0,0,21,91,1,0,0,0,23,94,1,0,0,0,25,96,1,0,0,0,27,
  	98,1,0,0,0,29,100,1,0,0,0,31,102,1,0,0,0,33,104,1,0,0,0,35,106,1,0,0,
  	0,37,108,1,0,0,0,39,110,1,0,0,0,41,112,1,0,0,0,43,114,1,0,0,0,45,116,
  	1,0,0,0,47,118,1,0,0,0,49,120,1,0,0,0,51,122,1,0,0,0,53,124,1,0,0,0,55,
  	126,1,0,0,0,57,128,1,0,0,0,59,130,1,0,0,0,61,62,5,92,0,0,62,63,5,100,
  	0,0,63,2,1,0,0,0,64,65,5,92,0,0,65,66,5,68,0,0,66,4,1,0,0,0,67,68,5,92,
  	0,0,68,69,5,115,0,0,69,6,1,0,0,0,70,71,5,92,0,0,71,72,5,83,0,0,72,8,1,
  	0,0,0,73,74,5,92,0,0,74,75,5,119,0,0,75,10,1,0,0,0,76,77,5,92,0,0,77,
  	78,5,87,0,0,78,12,1,0,0,0,79,80,5,92,0,0,80,81,5,116,0,0,81,14,1,0,0,
  	0,82,83,5,92,0,0,83,84,5,114,0,0,84,16,1,0,0,0,85,86,5,92,0,0,86,87,5,
  	110,0,0,87,18,1,0,0,0,88,89,5,92,0,0,89,90,5,118,0,0,90,20,1,0,0,0,91,
  	92,5,92,0,0,92,93,5,102,0,0,93,22,1,0,0,0,94,95,5,124,0,0,95,24,1,0,0,
  	0,96,97,5,33,0,0,97,26,1,0,0,0,98,99,5,123,0,0,99,28,1,0,0,0,100,101,
  	5,125,0,0,101,30,1,0,0,0,102,103,5,40,0,0,103,32,1,0,0,0,104,105,5,41,
  	0,0,105,34,1,0,0,0,106,107,5,44,0,0,107,36,1,0,0,0,108,109,5,63,0,0,109,
  	38,1,0,0,0,110,111,5,43,0,0,111,40,1,0,0,0,112,113,5,42,0,0,113,42,1,
  	0,0,0,114,115,5,94,0,0,115,44,1,0,0,0,116,117,5,45,0,0,117,46,1,0,0,0,
  	118,119,5,91,0,0,119,48,1,0,0,0,120,121,5,93,0,0,121,50,1,0,0,0,122,123,
  	5,92,0,0,123,52,1,0,0,0,124,125,7,0,0,0,125,54,1,0,0,0,126,127,7,1,0,
  	0,127,56,1,0,0,0,128,129,5,46,0,0,129,58,1,0,0,0,130,131,9,0,0,0,131,
  	60,1,0,0,0,1,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  rematchlexerLexerStaticData = staticData.release();
}

}

REmatchLexer::REmatchLexer(CharStream *input) : Lexer(input) {
  REmatchLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *rematchlexerLexerStaticData->atn, rematchlexerLexerStaticData->decisionToDFA, rematchlexerLexerStaticData->sharedContextCache);
}

REmatchLexer::~REmatchLexer() {
  delete _interpreter;
}

std::string REmatchLexer::getGrammarFileName() const {
  return "REmatchLexer.g4";
}

const std::vector<std::string>& REmatchLexer::getRuleNames() const {
  return rematchlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& REmatchLexer::getChannelNames() const {
  return rematchlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& REmatchLexer::getModeNames() const {
  return rematchlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& REmatchLexer::getVocabulary() const {
  return rematchlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView REmatchLexer::getSerializedATN() const {
  return rematchlexerLexerStaticData->serializedATN;
}

const atn::ATN& REmatchLexer::getATN() const {
  return *rematchlexerLexerStaticData->atn;
}




void REmatchLexer::initialize() {
  ::antlr4::internal::call_once(rematchlexerLexerOnceFlag, rematchlexerLexerInitialize);
}
