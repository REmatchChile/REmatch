lexer grammar REmatchLexer;

// Special Character Classes
DECIMAL_DIGIT: '\\d';
NOT_DECIMAL_DIGIT: '\\D';
WHITESPACE: '\\s';
NOT_WHITESPACE: '\\S';
ALPHANUMERIC: '\\w';
NOT_ALPHANUMERIC: '\\W';

// Special Characters
TAB: '\\t';
CARRIAGE_RETURN: '\\r';
NEWLINE: '\\n';
VERTICAL_WHITESPACE: '\\v';
FORM_FEED: '\\f';

// Single Tokens
PIPE: '|';
EXCLAMAITON: '!';
L_CURLY: '{';
R_CURLY: '}';
L_PAR: '(';
R_PAR: ')';
COMMA: ',';
QUESTION: '?';
PLUS: '+';
STAR: '*';
HAT: '^';
HYPHEN: '-';
L_BRACK: '[';
R_BRACK: ']';
BACKSLASH: '\\';
ALPHA: [A-Za-z];
DIGIT: [0-9];
DOT: '.';
DOLLAR: '$';

UNRECOGNIZED: .;