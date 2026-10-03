#include <iostream>
#include <cctype>
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
    DIV_OP,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    string lexeme;
    int position;
};

class Lexer {
private:
    string input;
    int pos = 0;

public:
    Lexer(string text){
        input = text;
    }

    vector<Token> getTokens(){
        vector<Token> tokens;

        while (pos < input.size()){
            if(isspace(input[pos])){
                pos++;
                continue;
            }

            if(isalpha(input[pos])){
                int start = pos;
                string word = "";

                while(pos < input.size() && (isalpha(input[pos]) || isdigit(input[pos]))){
                    word += input[pos];
                    pos++;
                }
            

                if (word == "float"){
                    tokens.push_back({
                        TokenType::KEYWORD_FLOAT,
                        word,
                        start
                    });
                }
                else{
                    bool valid = true;
                    for(char c: word){
                        if (c < 'a' || c > 'z'){
                            valid = false;
                        }
                    }

                    if(valid){
                        tokens.push_back({
                            TokenType::IDENT,
                            word,
                            start
                        });
                    }
                    else{
                        tokens.push_back({
                            TokenType::UNKNOWN,
                            word,
                            start
                        });
                    }
                }

                continue;
            }

            TokenType type;

            switch (input[pos]){
            case '(':
                type = TokenType::LPAREN;
                break;
            case ')':
                type = TokenType::RPAREN;
                break;
            case '{':
                type = TokenType::LBRACE;
                break;
            case '}':
                type = TokenType::RBRACE;
                break;
            case ';':
                type = TokenType::SEMICOLON;
                break;
            case '=':
                type = TokenType::ASSIGN_OP;
                break;
            case '*':
                type = TokenType::MUL_OP;
                break;
            case '/':
                type = TokenType::DIV_OP;
                break;
            default:
                type = TokenType::UNKNOWN;
            }

            tokens.push_back({
                type,
                string(1, input[pos]),
                pos
            });

            pos++;
        }

        tokens.push_back({
            TokenType::END_OF_FILE,
            "EOF",
            pos
        });

        return tokens;
    }

};

class Parser {
};

int main() {
    string program = "float prog (float k){ float 1; float a; float test; test = k / 1 * a; }";
    
    Lexer lexer(program);
    vector<Token> tokens = lexer.getTokens();

    for(Token token : tokens){
        cout << token.lexeme << endl;
    }

    return 0;
}