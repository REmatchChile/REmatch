
// Generated from REmatchParser.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "REmatchParserVisitor.h"


/**
 * This class provides an empty implementation of REmatchParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  REmatchParserBaseVisitor : public REmatchParserVisitor {
public:

  virtual std::any visitRoot(REmatchParser::RootContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlternation(REmatchParser::AlternationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpr(REmatchParser::ExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElement(REmatchParser::ElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGroup(REmatchParser::GroupContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParentheses(REmatchParser::ParenthesesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignation(REmatchParser::AssignationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarname(REmatchParser::VarnameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtom(REmatchParser::AtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCharacterClass(REmatchParser::CharacterClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcAtom(REmatchParser::CcAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcRange(REmatchParser::CcRangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcSingle(REmatchParser::CcSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcLiteral(REmatchParser::CcLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcEscapes(REmatchParser::CcEscapesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcOther(REmatchParser::CcOtherContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcSpecial(REmatchParser::CcSpecialContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSingleSharedAtom(REmatchParser::SingleSharedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(REmatchParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEscapes(REmatchParser::EscapesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecial(REmatchParser::SpecialContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOther(REmatchParser::OtherContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSharedAtom(REmatchParser::SharedAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantifier(REmatchParser::QuantifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantity(REmatchParser::QuantityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantExact(REmatchParser::QuantExactContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantRange(REmatchParser::QuantRangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantMin(REmatchParser::QuantMinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuantMax(REmatchParser::QuantMaxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(REmatchParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnchor(REmatchParser::AnchorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnchorStart(REmatchParser::AnchorStartContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnchorEnd(REmatchParser::AnchorEndContext *ctx) override {
    return visitChildren(ctx);
  }


};

