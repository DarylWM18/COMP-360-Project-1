#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class TokenType {
    KEYWORD_FLOAT,
    IDENT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    ASSIGN_OP,
    MUL_OP,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    string lexeme;
    int position;
};

class Lexer {
};

class Parser {
};

int main() {
}