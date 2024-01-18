parser grammar REmatchParser;

options {
  tokenVocab = REmatchLexer;
}

root: alternation EOF;

alternation: expr ('|' expr)*;

expr: element+;

element: group quantifier?;

group: parentheses | assignation | atom;

parentheses: '(' alternation ')';

assignation: '!' varname '{' alternation '}';
varname: ALPHA (ALPHA | DIGIT)*;

atom: characterClass | singleSharedAtom | literal | anchor;

characterClass: '[' '^'? ccAtom+ ']';
ccAtom: ccRange | sharedAtom | ccSingle;
ccRange: ccLiteral '-' ccLiteral;
ccSingle: ccLiteral;
ccLiteral: ccEscapes | ccSpecial | ccOther;
ccEscapes: '\\' ('^' | '-' | ']' | '\\');
ccOther: ~('^' | '-' | ']' | '\\');

ccSpecial:
  TAB
  | CARRIAGE_RETURN
  | NEWLINE
  | VERTICAL_WHITESPACE
  | FORM_FEED;

singleSharedAtom: sharedAtom;

literal: escapes | special | other;
escapes:
  '\\' (
    '['
    | ']'
    | '('
    | ')'
    | '{'
    | '}'
    | '*'
    | '+'
    | '?'
    | '|'
    | '.'
    | '\\'
    | '^'
    | '$'
  );
special:
  DOT
  | TAB
  | CARRIAGE_RETURN
  | NEWLINE
  | VERTICAL_WHITESPACE
  | FORM_FEED;
other:
  ~(
    '['
    | ']'
    | '('
    | ')'
    | '{'
    | '}'
    | '*'
    | '+'
    | '?'
    | '|'
    | '\\'
    | '^'
    | '$'
  );

sharedAtom:
  DECIMAL_DIGIT
  | NOT_DECIMAL_DIGIT
  | WHITESPACE
  | NOT_WHITESPACE
  | ALPHANUMERIC
  | NOT_ALPHANUMERIC;

quantifier: '?' | '+' | '*' | '{' quantity '}';
quantity: quantExact | quantRange | quantMin | quantMax;
quantExact: number;
quantRange: number ',' number;
quantMin: number ',';
quantMax: ',' number;

number: DIGIT+;

anchor: anchorStart | anchorEnd;

anchorStart: '^';
anchorEnd: '$';
