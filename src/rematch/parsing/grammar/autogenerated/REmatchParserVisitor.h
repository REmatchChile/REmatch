
// Generated from REmatchParser.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "REmatchParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by REmatchParser.
 */
class  REmatchParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by REmatchParser.
   */
    virtual std::any visitRoot(REmatchParser::RootContext *context) = 0;

    virtual std::any visitAlternation(REmatchParser::AlternationContext *context) = 0;

    virtual std::any visitExpr(REmatchParser::ExprContext *context) = 0;

    virtual std::any visitElement(REmatchParser::ElementContext *context) = 0;

    virtual std::any visitGroup(REmatchParser::GroupContext *context) = 0;

    virtual std::any visitParentheses(REmatchParser::ParenthesesContext *context) = 0;

    virtual std::any visitAssignation(REmatchParser::AssignationContext *context) = 0;

    virtual std::any visitVarname(REmatchParser::VarnameContext *context) = 0;

    virtual std::any visitAtom(REmatchParser::AtomContext *context) = 0;

    virtual std::any visitCharacterClass(REmatchParser::CharacterClassContext *context) = 0;

    virtual std::any visitCcAtom(REmatchParser::CcAtomContext *context) = 0;

    virtual std::any visitCcRange(REmatchParser::CcRangeContext *context) = 0;

    virtual std::any visitCcSingle(REmatchParser::CcSingleContext *context) = 0;

    virtual std::any visitCcLiteral(REmatchParser::CcLiteralContext *context) = 0;

    virtual std::any visitCcEscapes(REmatchParser::CcEscapesContext *context) = 0;

    virtual std::any visitCcOther(REmatchParser::CcOtherContext *context) = 0;

    virtual std::any visitCcSpecial(REmatchParser::CcSpecialContext *context) = 0;

    virtual std::any visitSingleSharedAtom(REmatchParser::SingleSharedAtomContext *context) = 0;

    virtual std::any visitLiteral(REmatchParser::LiteralContext *context) = 0;

    virtual std::any visitEscapes(REmatchParser::EscapesContext *context) = 0;

    virtual std::any visitSpecial(REmatchParser::SpecialContext *context) = 0;

    virtual std::any visitOther(REmatchParser::OtherContext *context) = 0;

    virtual std::any visitSharedAtom(REmatchParser::SharedAtomContext *context) = 0;

    virtual std::any visitQuantifier(REmatchParser::QuantifierContext *context) = 0;

    virtual std::any visitQuantity(REmatchParser::QuantityContext *context) = 0;

    virtual std::any visitQuantExact(REmatchParser::QuantExactContext *context) = 0;

    virtual std::any visitQuantRange(REmatchParser::QuantRangeContext *context) = 0;

    virtual std::any visitQuantMin(REmatchParser::QuantMinContext *context) = 0;

    virtual std::any visitQuantMax(REmatchParser::QuantMaxContext *context) = 0;

    virtual std::any visitNumber(REmatchParser::NumberContext *context) = 0;

    virtual std::any visitAnchor(REmatchParser::AnchorContext *context) = 0;

    virtual std::any visitAnchorStart(REmatchParser::AnchorStartContext *context) = 0;

    virtual std::any visitAnchorEnd(REmatchParser::AnchorEndContext *context) = 0;


};

