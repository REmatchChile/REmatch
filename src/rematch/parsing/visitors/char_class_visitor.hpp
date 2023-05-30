#pragma once

#include "antlr4-runtime.h"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "parsing/bad_regex_exception.hpp"
#include "parsing/variable_catalog.hpp"
#include "parsing/grammar/autogenerated/REmatchParserBaseVisitor.h"

// #define CHAR_CLASS_VISITOR__DEBUG
#ifdef CHAR_CLASS_VISITOR__DEBUG
#define CHAR_CLASS_VISITOR__INFO(arg) std::cout << arg
#else
#define CHAR_CLASS_VISITOR__INFO(arg)
#endif

namespace rematch {
namespace visitors {

struct UnicodeRange {
  UnicodeRange() : lo(0), hi(0) {}
  UnicodeRange(uint32_t l, uint32_t h) : lo(l), hi(h) {}

  bool operator==(const UnicodeRange &rhs) const {
    return lo == rhs.lo && hi == rhs.hi;
  }

  uint32_t lo;
  uint32_t hi;
};

struct UnicodeRangeLess {
  bool operator()(const UnicodeRange &a, const UnicodeRange &b) const {
    return a.hi < b.lo;
  }
};

static constexpr uint32_t UTF8MAX = 0x10FFFF;
using UnicodeRangeSet = std::set<UnicodeRange, UnicodeRangeLess>;

class CharClassVisitor : public REmatchParserBaseVisitor {
 public:
  std::shared_ptr<VariableCatalog> vfact_ptr;
  std::unique_ptr<LogicalVA> lva_ptr;

  CharClassVisitor(std::shared_ptr<VariableCatalog> _vfact_ptr)
      : vfact_ptr(_vfact_ptr) {}

 private:
  UnicodeRangeSet ranges;
  uint32_t current_codepoint;

  bool add_range(uint32_t lo, uint32_t hi) {
    if (hi < lo) return false;

    {
      auto it = ranges.find(UnicodeRange(lo, hi));
      if (it != ranges.end() && it->lo <= lo && hi <= it->hi) return false;
    }

    // Look for an inmediate range to the left of [lo, hi]. If exists
    // then erase and extend [lo, hi] accordingly
    if (lo > 0) {
      auto it = ranges.find(UnicodeRange(lo - 1, lo - 1));
      if (it != ranges.end()) {
        lo = it->lo;
        if (it->hi > hi) hi = it->hi;
        ranges.erase(it);
      }
    }

    // Look for an inmediate range to the right of [lo, hi]. If exists
    // then erase and extend [lo, hi] accordingly
    // Note: The last unicode code point is 0x10FFFF
    if (hi < UTF8MAX) {
      auto it = ranges.find(UnicodeRange(hi + 1, hi + 1));
      if (it != ranges.end()) {
        hi = it->hi;
        // Not necesary to check for lo. Did it in previous step.
        ranges.erase(it);
      }
    }

    // Search for ranges inside [lo, hi] and erase them
    // The remaining ranges that overlap with [lo, hi] must be inside.
    for (;;) {
      auto it = ranges.find(UnicodeRange(lo, hi));
      if (it == ranges.end()) break;
      ranges.erase(it);
    }

    // Add [lo, hi]
    ranges.insert(UnicodeRange(lo, hi));
    return true;
  }

  bool add_single(uint32_t c) { return add_range(c, c); }

  void negate() {
    std::vector<UnicodeRange> new_ranges;
    new_ranges.reserve(ranges.size() + 1);

    auto it = ranges.begin();
    if (it == ranges.end()) {
      new_ranges.push_back(UnicodeRange(0, UTF8MAX));
    } else {
      uint32_t next_lo = 0;
      if (it->lo == 0) {
        next_lo = it->hi + 1;
        ++it;
      }
      for (; it != ranges.end(); ++it) {
        new_ranges.push_back(UnicodeRange(next_lo, it->lo - 1));
        next_lo = it->hi + 1;
      }
      if (next_lo <= UTF8MAX)
        new_ranges.push_back(UnicodeRange(next_lo, UTF8MAX));
    }

    ranges.clear();
    for (auto &range : new_ranges) {
      ranges.insert(range);
    }
  }

  // Build the automaton for a range of two bytes and stores it in the lva_ptr
  void buildTwoBytesAutomaton(char lo[2], char hi[2]) {
    if (lo[0] == hi[0]) {
      // Same first byte
      auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
      auto A2 = std::make_unique<LogicalVA>(CharClass({lo[1], hi[1]}));
      A1->cat(*A2);
      lva_ptr = std::move(A1);
    } else {
      // Different first byte
      auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
      auto A2 = std::make_unique<LogicalVA>(CharClass({lo[1], '\xBF'}));
      A1->cat(*A2);
      auto B1 = std::make_unique<LogicalVA>(CharClass(hi[0]));
      auto B2 = std::make_unique<LogicalVA>(CharClass({'\x80', hi[1]}));
      B1->cat(*B2);
      A1->alter(*B1);
      if (lo[0] + 1 < hi[0]) {
        // There are more bytes between the first bytes
        auto C1 = std::make_unique<LogicalVA>(CharClass({char(lo[0] + 1), char(hi[0] - 1)}));
        auto C2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
        C1->cat(*C2);
        A1->alter(*C1);
      }
      lva_ptr = std::move(A1);
    }
  }

  // Build the automaton for a range of three bytes and stores it in the lva_ptr
  void buildThreeBytesAutomaton(char lo[3], char hi[3]) {
    if (lo[0] == hi[0]) {
      // Same first byte
      if (lo[1] == hi[1]) {
        // Same second byte
        auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
        auto A2 = std::make_unique<LogicalVA>(CharClass(lo[1]));
        auto A3 = std::make_unique<LogicalVA>(CharClass({lo[2], hi[2]}));
        A1->cat(*A2);
        A1->cat(*A3);
        lva_ptr = std::move(A1);
      } else {
        // Different second byte
        auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
        buildTwoBytesAutomaton(lo + 1, hi + 1);
        auto A2 = std::move(lva_ptr);
        A1->cat(*A2);
        lva_ptr = std::move(A1);
      }
    } else {
      // Different first byte
      auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
      auto A2 = std::make_unique<LogicalVA>(CharClass(lo[1]));
      auto A3 = std::make_unique<LogicalVA>(CharClass({lo[2], '\xBF'}));
      A1->cat(*A2);
      A1->cat(*A3);
      auto B1 = std::make_unique<LogicalVA>(CharClass(hi[0]));
      auto B2 = std::make_unique<LogicalVA>(CharClass(hi[1]));
      auto B3 = std::make_unique<LogicalVA>(CharClass({'\x80', hi[2]}));
      B1->cat(*B2);
      B1->cat(*B3);
      A1->alter(*B1);
      if (uint16_t(lo[0] << 8 | lo[1]) + 1 < uint16_t(hi[0] << 8 | hi[1])) {
        // There are more bytes between the first/second bytes
        char lo_mid[2];
        char hi_mid[2];
        if (lo[1] == '\xBF') {
          // Second byte overflows
          lo_mid[0] = char(lo[0] + 1);
          lo_mid[1] = '\x80';
        } else {
          lo_mid[0] = lo[0];
          lo_mid[1] = char(lo[1] + 1);
        }
        if (hi[1] == '\x80') {
          // Second byte underflows
          hi_mid[0] = char(hi[0] - 1);
          hi_mid[1] = '\xBF';
        } else {
          hi_mid[0] = hi[0];
          hi_mid[1] = char(hi[1] - 1);
        }
        buildTwoBytesAutomaton(lo_mid, hi_mid);
        auto C1 = std::move(lva_ptr);
        auto C2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
        C1->cat(*C2);
        A1->alter(*C1);
      }
      lva_ptr = std::move(A1);
    }
  }

  // Build the automaton for a range of four bytes and stores it in the lva_ptr
  void buildFourBytesAutomaton(char lo[4], char hi[4]) {
    if (lo[0] == hi[0]) {
      // Same first byte
      if (lo[1] == hi[1]) {
        // Same second byte
        if (lo[2] == hi[2]) {
          // Same third byte
          auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
          auto A2 = std::make_unique<LogicalVA>(CharClass(lo[1]));
          auto A3 = std::make_unique<LogicalVA>(CharClass(lo[2]));
          auto A4 = std::make_unique<LogicalVA>(CharClass({lo[3], hi[3]}));
          A1->cat(*A2);
          A1->cat(*A3);
          A1->cat(*A4);
          lva_ptr = std::move(A1);
        } else {
          // Different third byte
          auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
          auto A2 = std::make_unique<LogicalVA>(CharClass(lo[1]));
          buildTwoBytesAutomaton(lo + 2, hi + 2);
          auto A3 = std::move(lva_ptr);
          A1->cat(*A2);
          A1->cat(*A3);
          lva_ptr = std::move(A1);
        }
      } else {
        // Different second byte
        auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
        buildThreeBytesAutomaton(lo + 1, hi + 1);
        auto A2 = std::move(lva_ptr);
        A1->cat(*A2);
        lva_ptr = std::move(A1);
      }
    } else {
      // Different first byte
      auto A1 = std::make_unique<LogicalVA>(CharClass(lo[0]));
      auto A2 = std::make_unique<LogicalVA>(CharClass(lo[1]));
      auto A3 = std::make_unique<LogicalVA>(CharClass(lo[2]));
      auto A4 = std::make_unique<LogicalVA>(CharClass({lo[3], '\xBF'}));
      A1->cat(*A2);
      A1->cat(*A3);
      A1->cat(*A4);
      auto B1 = std::make_unique<LogicalVA>(CharClass(hi[0]));
      auto B2 = std::make_unique<LogicalVA>(CharClass(hi[1]));
      auto B3 = std::make_unique<LogicalVA>(CharClass(hi[2]));
      auto B4 = std::make_unique<LogicalVA>(CharClass({'\x80', hi[3]}));
      B1->cat(*B2);
      B1->cat(*B3);
      B1->cat(*B4);
      A1->alter(*B1);
      if (uint64_t(lo[0] << 16 | lo[1] << 8 | lo[2]) + 1 < uint64_t(hi[0] << 16 | hi[1] << 8 | hi[2])) {
        // There are more bytes between the first/second/third bytes
        char lo_mid[3];
        char hi_mid[3];
        if (lo[2] == '\xBF') {
          // Third byte overflows
          if (lo[1] == '\xBF') {
            // Second byte overflows
            lo_mid[0] = char(lo[0] + 1);
            lo_mid[1] = '\x80';
            lo_mid[2] = '\x80';
          } else {
            lo_mid[0] = lo[0];
            lo_mid[1] = char(lo[1] + 1);
            lo_mid[2] = '\x80';
          }
        } else {
          lo_mid[0] = lo[0];
          lo_mid[1] = lo[1];
          lo_mid[2] = char(lo[2] + 1);
        }
        if (hi[2] == '\x80') {
          // Third byte underflows
          if (hi[1] == '\x80') {
            // Second byte underflows
            hi_mid[0] = char(hi[0] - 1);
            hi_mid[1] = '\xBF';
            hi_mid[2] = '\xBF';
          } else {
            hi_mid[0] = hi[0];
            hi_mid[1] = char(hi[1] - 1);
            hi_mid[2] = '\xBF';
          }
        } else {
          hi_mid[0] = hi[0];
          hi_mid[1] = hi[1];
          hi_mid[2] = char(hi[2] - 1);
        }
        buildThreeBytesAutomaton(lo_mid, hi_mid);
        auto C1 = std::move(lva_ptr);
        auto C2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
        C1->cat(*C2);
        A1->alter(*C1);
      }
      lva_ptr = std::move(A1);
    }
  }

  // Build the automaton for a character class and stores it in the lva_ptr
  void buildCharacterClassAutomaton() {
    auto it = ranges.begin();
    std::stack<UnicodeRange> stack;
    stack.push(*it);
    // 1 byte automaton
    {
      CharClass ccb;
      while (!stack.empty()) {
        UnicodeRange r = stack.top();
        if (r.lo > 0x7F) break;
        stack.pop();
        if (r.hi <= 0x7F) {
          // Handle range
          ccb.add_range(r.lo, r.hi);
          if (stack.empty() && ++it != ranges.end()) stack.push(*it);
        } else {
          // Split range
          stack.emplace(0x80, r.hi);
          stack.emplace(r.lo, 0x7F);
        }
      }
      if (!ccb.empty()) {
        lva_ptr = std::make_unique<LogicalVA>(ccb);
      }
    }
    // 2 bytes automaton
    while (!stack.empty()) {
      UnicodeRange r = stack.top();
      if (r.lo > 0x7FF) break;
      stack.pop();
      if (r.hi <= 0x7FF) {
        // Handle range
        char lo[2] = {char((r.lo >> 6) | 0xC0), char((r.lo & 0x3F) | 0x80)};
        char hi[2] = {char((r.hi >> 6) | 0xC0), char((r.hi & 0x3F) | 0x80)};
        if (lva_ptr != nullptr) {
          auto A = std::move(lva_ptr);
          buildTwoBytesAutomaton(lo, hi);
          lva_ptr->alter(*A);
        } else {
          buildTwoBytesAutomaton(lo, hi);
        }
        if (stack.empty() && ++it != ranges.end()) stack.push(*it);
      } else {
        // Split range
        stack.emplace(0x800, r.hi);
        stack.emplace(r.lo, 0x7FF);
      }
    }
    // 3 bytes automaton
    while (!stack.empty()) {
      UnicodeRange r = stack.top();
      if (r.lo > 0xFFFF) break;
      stack.pop();
      if (r.hi <= 0xFFFF) {
        // Handle range
        char lo[3] = {char((r.lo >> 12) | 0xE0), char(((r.lo >> 6) & 0x3F) | 0x80), char((r.lo & 0x3F) | 0x80)};
        char hi[3] = {char((r.hi >> 12) | 0xE0), char(((r.hi >> 6) & 0x3F) | 0x80), char((r.hi & 0x3F) | 0x80)};
        if (lva_ptr != nullptr) {
          auto A = std::move(lva_ptr);
          buildThreeBytesAutomaton(lo, hi);
          lva_ptr->alter(*A);
        } else {
          buildThreeBytesAutomaton(lo, hi);
        }
        if (stack.empty() && ++it != ranges.end()) stack.push(*it);
      } else {
        // Split range
        stack.emplace(0x10000, r.hi);
        stack.emplace(r.lo, 0xFFFF);
      }
    }
    // 4 bytes automaton
    while (!stack.empty()) {
      UnicodeRange r = stack.top();
      stack.pop();
      // Handle range
      char lo[4] = {char((r.lo >> 18) | 0xF0), char(((r.lo >> 12) & 0x3F) | 0x80), char(((r.lo >> 6) & 0x3F) | 0x80), char((r.lo & 0x3F) | 0x80)};
      char hi[4] = {char((r.hi >> 18) | 0xF0), char(((r.hi >> 12) & 0x3F) | 0x80), char(((r.hi >> 6) & 0x3F) | 0x80), char((r.hi & 0x3F) | 0x80)};
      if (lva_ptr != nullptr) {
        auto A = std::move(lva_ptr);
        buildFourBytesAutomaton(lo, hi);
        lva_ptr->alter(*A);
      } else {
        buildFourBytesAutomaton(lo, hi);
      }
      if (stack.empty() && ++it != ranges.end()) stack.push(*it);
    }
  }

 private:
  std::any visitAlternation(REmatchParser::AlternationContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitAlternation" << std::endl);
    // Build the automaton for the first expression
    visit(ctx->expr(0));
    auto A = std::move(lva_ptr);
    // Alternate the remaining expressions
    for (size_t i = 1; i < ctx->expr().size(); ++i) {
      visit(ctx->expr(i));
      A->alter(*lva_ptr);
    }
    lva_ptr = std::move(A);

    return 0;
  }

  std::any visitExpr(REmatchParser::ExprContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitExpr" << std::endl);
    // Build the automaton for the first element
    visit(ctx->element(0));
    auto A = std::move(lva_ptr);
    // Concatenate the remaining elements
    for (size_t i = 1; i < ctx->element().size(); ++i) {
      visit(ctx->element(i));
      A->cat(*lva_ptr);
    }
    lva_ptr = std::move(A);

    return 0;
  }

  std::any visitElement(REmatchParser::ElementContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitElement" << std::endl);
    // Build the automaton for the group
    visit(ctx->group());
    // Apply the quantifier
    auto quantifier = ctx->quantifier();
    if (quantifier) {
      visit(quantifier);
    }

    return 0;
  }

  std::any visitGroup(REmatchParser::GroupContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitGroup" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitQuantifier(REmatchParser::QuantifierContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitQuantifier" << std::endl);
    if (ctx->QUESTION()) {
      lva_ptr->optional();
    } else if (ctx->PLUS()) {
      lva_ptr->strict_kleene();
    } else if (ctx->STAR()) {
      lva_ptr->kleene();
    }
    // Quantifier range
    else {
      auto qty = ctx->quantity();
      int lo = 0, hi = -1;
      if (qty->quantExact()) {
        lo = std::stoi(qty->getText());
        hi = lo;
      } else if (qty->quantRange()) {
        lo = std::stoi(qty->quantRange()->number(0)->getText());
        hi = std::stoi(qty->quantRange()->number(1)->getText());
      } else if (qty->quantMin()) {
        lo = std::stoi(qty->getText());
      } else if (qty->quantMax()) {
        hi = std::stoi(qty->getText());
      }
      lva_ptr->repeat(lo, hi);
    }

    return 0;
  }

  std::any visitParentheses(REmatchParser::ParenthesesContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitParentheses" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitAssignation(REmatchParser::AssignationContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitAssignation" << std::endl);
    // Build the automaton for the alternation
    visit(ctx->alternation());
    // Assign the codes from the variable
    std::string var = ctx->varname()->getText();
    std::bitset<64> op_code = vfact_ptr->open_code(var);
    std::bitset<64> cl_code = vfact_ptr->close_code(var);
    lva_ptr->assign(op_code, cl_code);

    return 0;
  }

  std::any visitAtom(REmatchParser::AtomContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitAtom" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitLiteral(REmatchParser::LiteralContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitLiteral" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitEscapes(REmatchParser::EscapesContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitEscapes" << std::endl);
    // Ignore the starting backslash
    lva_ptr =
        std::make_unique<LogicalVA>(CharClass(ctx->getText()[1]));

    return 0;
  }

  std::any visitSpecial(REmatchParser::SpecialContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitSpecial" << std::endl);
    // Dot (Any Operator) needs to be handled for UTF-8 literals
    if (ctx->DOT()) {
      // 1 byte automaton
      auto A1 = std::make_unique<LogicalVA>(CharClass({'\x00', '\x7F'}));
      // 2 bytes automaton
      auto B1 = std::make_unique<LogicalVA>(CharClass({'\xC2', '\xDF'}));
      auto B2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      B1->cat(*B2);
      // 3 bytes automaton
      auto C1 = std::make_unique<LogicalVA>(CharClass({'\xE0', '\xEF'}));
      auto C2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      auto C3 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      C1->cat(*C2);
      C1->cat(*C3);
      // 4 bytes automaton
      auto D1 = std::make_unique<LogicalVA>(CharClass({'\xF0', '\xF7'}));
      auto D2 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      auto D3 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      auto D4 = std::make_unique<LogicalVA>(CharClass({'\x80', '\xBF'}));
      D1->cat(*D2);
      D1->cat(*D3);
      D1->cat(*D4);
      // Alternate all the automata
      A1->alter(*B1);
      A1->alter(*C1);
      A1->alter(*D1);

      lva_ptr = std::move(A1);

      return 0;
    }
    // Regular special literals
    else if (ctx->TAB()) {
      lva_ptr = std::make_unique<LogicalVA>(CharClass('\t'));
    } else if (ctx->CARRIAGE_RETURN()) {
      lva_ptr = std::make_unique<LogicalVA>(CharClass('\r'));
    } else if (ctx->NEWLINE()) {
      lva_ptr = std::make_unique<LogicalVA>(CharClass('\n'));
    } else if (ctx->VERTICAL_WHITESPACE()) {
      lva_ptr = std::make_unique<LogicalVA>(CharClass('\v'));
    } else if (ctx->FORM_FEED()) {
      lva_ptr = std::make_unique<LogicalVA>(CharClass('\f'));
    } else {
      throw BadRegexException("Unhandled Special Literal: " + ctx->getText());
    }

    return 0;
  }

  std::any visitOther(REmatchParser::OtherContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitOther" << std::endl);
    std::string text = ctx->getText();
    // Build the automaton for the first character
    lva_ptr = std::make_unique<LogicalVA>(CharClass(text[0]));
    // Concatenate the remaining characters
    for (size_t i = 1; i < text.size(); ++i) {
      auto A = std::make_unique<LogicalVA>(CharClass(text[i]));
      lva_ptr->cat(*A);
    }

    return 0;
  }

  std::any visitSingleSharedAtom(REmatchParser::SingleSharedAtomContext* ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitSingleSharedAtom" << std::endl);
    // Note: Single shared atom is like a "hard-coded" character class
    // Build the single shared atom set of ranges
    visit(ctx->sharedAtom());
    // Build the automaton for the single shared atom
    buildCharacterClassAutomaton();

    return 0;
  }

  std::any visitCharacterClass(REmatchParser::CharacterClassContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCharacterClass" << std::endl);
    // Build the character class set of ranges
    for (auto &atom : ctx->ccAtom()) {
      visit(atom);
    }
    // Check if the character class must be negated
    if (ctx->HAT()) {
      negate();
    }
    // Build the automaton for the character class
    buildCharacterClassAutomaton();

    return 0;
  }

  std::any visitCcAtom(REmatchParser::CcAtomContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcAtom" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitCcRange(REmatchParser::CcRangeContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcRange" << std::endl);
    // Build the codepoint range and add it to the set
    visit(ctx->ccLiteral(0));
    uint32_t lo = current_codepoint;
    visit(ctx->ccLiteral(1));
    if (lo > current_codepoint) {
      throw BadRegexException(
          "Character Class range start code is greater than end code: " +
          ctx->getText());
    }
    add_range(lo, current_codepoint);

    return 0;
  }

  std::any visitSharedAtom(REmatchParser::SharedAtomContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitSharedAtom" << std::endl);
    // Add the specific range to the set
    if (ctx->DECIMAL_DIGIT()) {
      // 0-9
      add_range(48, 57);
    } else if (ctx->NOT_DECIMAL_DIGIT()) {
      // NOT(0-9)
      add_range(0, 47);
      add_range(58, UTF8MAX);
    } else if (ctx->WHITESPACE()) {
      // \t\n\v\f\r and space
      add_range(9, 13);
      add_single(32);
    } else if (ctx->NOT_WHITESPACE()) {
      // NOT(\t\n\v\f\r and space)
      add_range(0, 8);
      add_range(14, 31);
      add_range(33, UTF8MAX);
    } else if (ctx->ALPHANUMERIC()) {
      // 0-9A-Za-z_
      add_range(48, 57);
      add_range(65, 90);
      add_single(95);
      add_range(97, 122);
    } else if (ctx->NOT_ALPHANUMERIC()) {
      // NOT(0-9A-Za-z_)
      add_range(0, 47);
      add_range(58, 64);
      add_range(91, 94);
      add_single(96);
      add_range(123, UTF8MAX);
    } else {
      throw BadRegexException("Unhandled Shared Atom: " + ctx->getText());
    }

    return 0;
  }

  std::any visitCcSingle(REmatchParser::CcSingleContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcSingle" << std::endl);
    // Get the codepoint and add it to the set
    visit(ctx->ccLiteral());
    add_single(current_codepoint);

    return 0;
  }

  std::any visitCcLiteral(REmatchParser::CcLiteralContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcLiteral" << std::endl);
    visitChildren(ctx);

    return 0;
  }

  std::any visitCcEscapes(REmatchParser::CcEscapesContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcEscapes" << std::endl);
    // Get the codepoint ignoring the starting backslash
    current_codepoint = ctx->getText()[1];

    return 0;
  }

  std::any visitCcOther(REmatchParser::CcOtherContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitCcOther" << std::endl);
    std::string str = ctx->getText();
    // Get the codepoint of the utf-8 character
    switch (str.size()) {
      case 1:
        current_codepoint = str[0];
        break;
      case 2:
        current_codepoint = ((str[0] & 0x1F) << 6) + (str[1] & 0x3F);
        break;
      case 3:
        current_codepoint = ((str[0] & 0x0F) << 12) + ((str[1] & 0x3F) << 6) + (str[2] & 0x3F);
        break;
      case 4:
        current_codepoint = ((str[0] & 0x07) << 18) + ((str[1] & 0x3F) << 12) +
                            ((str[2] & 0x3F) << 6) + (str[3] & 0x3F);
        break;
      default:
        throw BadRegexException("Invalid UTF-8 character: " + str);
    }

    return 0;
  }


  std::any visitAnchor(REmatchParser::AnchorContext *ctx) override {
    CHAR_CLASS_VISITOR__INFO("visitAnchor" << std::endl);
    if (!lva_ptr) {
      lva_ptr = std::make_unique<LogicalVA>();
    }

    if (ctx->anchorStart()) {
      lva_ptr->add_anchor(true);
    } else if (ctx->anchorEnd()) {
      lva_ptr->add_anchor(false);
    }

    return 0;
  }
};
}  // namespace visitors
}  // namespace rematch
