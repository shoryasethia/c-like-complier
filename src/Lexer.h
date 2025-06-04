#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include "Token.h"

// Define a simple error reporting function similar to Lox's `Lox::error`
void reportLexerError(int line, int column, const std::string& message);

class Lexer {
public:
    Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    const std::string source_code;
    std::vector<Token> tokens;

    // Position tracking:
    // current_char_idx: Current index in source_code (points to the character *to be consumed next*)
    int current_char_idx = 0;

    // start_lexeme_idx: Index of the first character of the current lexeme being scanned.
    // This defines the substring for the token's 'value'.
    int start_lexeme_idx = 0;

    // line: Current line number (1-based)
    int line = 1;

    // column: Current column number (0-based, points to the column of current_char_idx)
    // This is the INTERNAL column tracker, 0-based to align with string indexing.
    int column = 0;  

    // current_token_start_column: The 1-based column where the *current token* began.
    // This is captured once at the start of scanToken() for the token being built.
    int current_token_start_column = 1; 

    static const std::map<std::string, TokenType> keywords;
    static std::map<std::string, TokenType> createKeywordsMap();

    bool isAtEnd() const;
    // advance() now only moves current_char_idx; column update is external or by skip/scan
    char advance(); 
    char peek() const;
    char peekNext() const;

    // These addToken overloads will implicitly use 'current_token_start_column'
    void addToken(TokenType type);
    void addToken(TokenType type, std::any literal_value);

    void scanToken();
    void skipWhitespaceAndComments(); // Handles spaces, tabs, newlines, and single-line // comments
    void readNumber();
    void readIdentifierOrKeyword();
    bool match(char expected); // Conditional advance for two-character operators
};