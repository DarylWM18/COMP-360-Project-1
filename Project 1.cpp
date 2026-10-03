/*
Names: Kayla Cobb and Daryl Watkins-Mattocks
Date: 10/3/2026
*/


#include <iostream>
#include <fstream>
#include <sstream>
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

string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::KEYWORD_FLOAT: return "KEYWORD";
        case TokenType::IDENT:         return "IDENT";
        case TokenType::LPAREN:        return "LPAREN";
        case TokenType::RPAREN:        return "RPAREN";
        case TokenType::LBRACE:        return "LBRACE";
        case TokenType::RBRACE:        return "RBRACE";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::ASSIGN_OP:     return "ASSIGN_OP";
        case TokenType::MUL_OP:        return "MUL_OP";
        case TokenType::DIV_OP:        return "DIV_OP";
        case TokenType::END_OF_FILE:   return "EOF";
        default:                       return "UNKNOWN";
    }
}

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

        while (pos < (int)input.size()){
            if(isspace(input[pos])){
                pos++;
                continue;
            }

            if(isalpha(input[pos])){
                int start = pos;
                string word = "";

                while(pos < (int)input.size() && (isalpha(input[pos]) || isdigit(input[pos]))){
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

// ============================================================================
// PARSER
//
// Grammar:
//   <program>  -> <keyword> <ident> ( <keyword> <ident> ) { <declares> <assign> }
//   <declares> -> <keyword> <ident> ; [ <declares> ]
//   <assign>   -> <ident> = <expr> ;
//   <expr>     -> <ident> {*|/} <expr> | <ident>
// ============================================================================
class Parser {
private:
    vector<Token> tokens;
    int current = 0;

    Token peek(){
        return tokens[current];
    }

    Token advance(){
        Token t = tokens[current];
        if (current + 1 < (int)tokens.size()){
            current++;
        }
        return t;
    }

    Token expect(TokenType type, string expectedDescription){
        Token t = peek();
        if (t.type == type){
            return advance();
        }

        string message = "Syntax error at position " + to_string(t.position) +
            ": expected " + expectedDescription +
            " but found \"" + t.lexeme + "\" (" + tokenTypeName(t.type) + ")";

        throw message;
    }

    void parseProgram(){
        expect(TokenType::KEYWORD_FLOAT, "keyword \"float\"");
        expect(TokenType::IDENT, "an identifier");
        expect(TokenType::LPAREN, "\"(\"");
        expect(TokenType::KEYWORD_FLOAT, "keyword \"float\"");
        expect(TokenType::IDENT, "an identifier");
        expect(TokenType::RPAREN, "\")\"");
        expect(TokenType::LBRACE, "\"{\"");

        parseDeclares();
        parseAssign();

        expect(TokenType::RBRACE, "\"}\"");
    }

    void parseDeclares(){
        expect(TokenType::KEYWORD_FLOAT, "keyword \"float\"");
        expect(TokenType::IDENT, "an identifier");
        expect(TokenType::SEMICOLON, "\";\"");

        if (peek().type == TokenType::KEYWORD_FLOAT){
            parseDeclares();
        }
    }

    void parseAssign(){
        expect(TokenType::IDENT, "an identifier");
        expect(TokenType::ASSIGN_OP, "\"=\"");
        parseExpr();
        expect(TokenType::SEMICOLON, "\";\"");
    }

    void parseExpr(){
        expect(TokenType::IDENT, "an identifier");

        TokenType t = peek().type;
        if (t == TokenType::MUL_OP || t == TokenType::DIV_OP){
            advance();
            parseExpr();
        }
    }

public:
    Parser(vector<Token> toks){
        tokens = toks;
    }

    bool parseProgramEntry(string &errorOut){
        try {
            parseProgram();
            expect(TokenType::END_OF_FILE, "end of file");
            return true;
        }
        catch (string &message){
            errorOut = message;
            return false;
        }
    }
};

// ============================================================================
// FILE READING + DRIVER
// ============================================================================
string readFile(const string& filename){
    ifstream in(filename);
    if (!in){
        throw runtime_error("Could not open file: " + filename);
    }
    ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void runOnFile(const string& filename){
    cout << "============================================================" << endl;
    cout << "Sample Program file: " << filename << endl;
    cout << "============================================================" << endl;

    string source;
    try {
        source = readFile(filename);
    }
    catch (exception& e){
        cout << e.what() << endl << endl;
        return;
    }

    Lexer lexer(source);
    vector<Token> tokens = lexer.getTokens();

    cout << "\n--- Lexemes and Tokens ---" << endl;
    for (Token token : tokens){
        if (token.type == TokenType::END_OF_FILE) continue;
        cout << "  " << token.position << ":  lexeme = \"" << token.lexeme
             << "\"   token = " << tokenTypeName(token.type) << endl;
    }

    Parser parser(tokens);
    string error;
    bool ok = parser.parseProgramEntry(error);

    cout << "\n--- Parser Result ---" << endl;
    if (ok){
        cout << "The Sample Program is generated by the BNF grammar" << endl << endl;
    } else {
        cout << "The Sample Program cannot be generated by the LearnCompiler BNF Grammar" << endl;
        cout << "First syntax error: " << error << endl << endl;
    }
}

int main(int argc, char* argv[]) {
    vector<string> files;

    if (argc > 1){
        for (int i = 1; i < argc; i++){
            files.push_back(argv[i]);
        }
    }
    else {
        string filename;
        cout << "Enter Sample Program filename (blank to stop): ";
        while (getline(cin, filename) && !filename.empty()){
            files.push_back(filename);
            cout << "Enter Sample Program filename (blank to stop): ";
        }
    }

    if (files.empty()){
        cout << "No input file provided." << endl;
        return 0;
    }

    for (const string& f : files){
        runOnFile(f);
    }

    return 0;
}