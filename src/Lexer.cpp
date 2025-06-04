#include "Lexer.h"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <cstddef>

using namespace std;

// Global error reporting function
bool had_lexer_error = false;

void reportLexerError(int line, int column, const string& message) {
    cerr << "[Lexer Error] line " << line << ", col " << column << ": " << message << endl;
    had_lexer_error = true;
}

// Static members initialization
const map<string, TokenType> Lexer::keywords = Lexer::createKeywordsMap();

map<string, TokenType> Lexer::createKeywordsMap() {
    map<string, TokenType> map;
    map["int"]    = KEYWORD_INT;
    map["void"]   = KEYWORD_VOID;
    map["if"]     = KEYWORD_IF;
    map["else"]   = KEYWORD_ELSE;
    map["while"]  = KEYWORD_WHILE;
    map["return"] = KEYWORD_RETURN;
    map["input"]  = KEYWORD_INPUT;
    map["output"] = KEYWORD_OUTPUT;
    return map;
}


// Lexer Class Implementation
Lexer::Lexer(const string& source) : source_code(source) {
    column = 0; // Initialize to 0 for 0-based indexing
    current_token_start_column = 1; // Always 1-based for reporting
}

vector<Token> Lexer::tokenize() {
    tokens.clear();
    current_char_idx = 0;
    line = 1;
    column = 0;
    current_token_start_column = 1;

    while (true) {
        skipWhitespaceAndComments(); 

        if (isAtEnd()) {
            break;
        }

        start_lexeme_idx = current_char_idx;
        current_token_start_column = column + 1;
        scanToken();
    }

    if (!tokens.empty() || source_code.length() > 0) { 
        skipWhitespaceAndComments();
    }

    tokens.emplace_back(EOF_TOKEN, "", any(), line, column + 1);
    return tokens;
}

bool Lexer::isAtEnd() const {
    return static_cast<size_t>(current_char_idx) >= source_code.length();
}

char Lexer::advance() {
    return source_code[current_char_idx++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_code[current_char_idx];
}

char Lexer::peekNext() const {
    if (static_cast<size_t>(current_char_idx + 1) >= source_code.length()) return '\0';
    return source_code[current_char_idx + 1];
}

void Lexer::addToken(TokenType type) {
    addToken(type, any());
}

void Lexer::addToken(TokenType type, any literal_value) {
    string lexeme = source_code.substr(start_lexeme_idx, current_char_idx - start_lexeme_idx);
    tokens.emplace_back(type, lexeme, literal_value, line, current_token_start_column);
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        if (isAtEnd()) {
            return;
        }

        char c = source_code[current_char_idx];

        if (c == ' ' || c == '\r' || c == '\t') {
            current_char_idx++;
            column++;
            continue;
        } else if (c == '\n') {
            current_char_idx++;
            line++;
            column = 0;
            continue;
        } else if (c == '/') {
            if (static_cast<size_t>(current_char_idx + 1) < source_code.length() && source_code[current_char_idx + 1] == '/') {
                current_char_idx += 2;
                column += 2;
                while (static_cast<size_t>(current_char_idx) < source_code.length() && source_code[current_char_idx] != '\n') {
                    current_char_idx++;
                    column++;
                }
                continue;
            } else {
                return;
            }
        } else {
            return;
        }
    }
}

void Lexer::readNumber() {
    // Current 'column' is 0-based.
    while (isdigit(peek())) {
        advance();
        column++;
    }
    string num_str = source_code.substr(start_lexeme_idx, current_char_idx - start_lexeme_idx);
    try {
        int num_val = stoi(num_str);
        addToken(NUMBER, num_val); // addToken uses current_token_start_column
    } catch (const out_of_range& e) {
        reportLexerError(line, current_token_start_column, "Number literal '" + num_str + "' is too large.");
        addToken(NUMBER, 0);
    } catch (const invalid_argument& e) {
        reportLexerError(line, current_token_start_column, "Invalid number literal: '" + num_str + "'");
        addToken(NUMBER, 0);
    }
}

void Lexer::readIdentifierOrKeyword() {
    // Current 'column' is 0-based.
    while (isalnum(peek()) || peek() == '_') {
        advance(); 
        column++;  
    }
    string text = source_code.substr(start_lexeme_idx, current_char_idx - start_lexeme_idx);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(IDENTIFIER);
    }
}


bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_code[current_char_idx] != expected) return false;
    current_char_idx++; // Consume
    column++;           
    return true;
}


void Lexer::scanToken() {
    char c = advance(); 
    column++;       

    switch (c) {
        case '(': addToken(DELIM_LPAREN); break;
        case ')': addToken(DELIM_RPAREN); break;
        case '{': addToken(DELIM_LBRACE); break;
        case '}': addToken(DELIM_RBRACE); break;
        case '[': addToken(DELIM_LBRACKET); break;
        case ']': addToken(DELIM_RBRACKET); break;
        case ';': addToken(DELIM_SEMICOLON); break;
        case ',': addToken(DELIM_COMMA); break;
        case '+': addToken(OP_PLUS); break;
        case '-': addToken(OP_MINUS); break;
        case '*': addToken(OP_MULTIPLY); break;
        case '/': addToken(OP_DIVIDE); break;

        case '!':
            if (match('=')) {
                addToken(OP_NOT_EQUAL);
            } else {
                reportLexerError(line, current_token_start_column, "Unexpected character '!' (expected '!=')");
            }
            break;
        case '=': addToken(match('=') ? OP_EQUAL : OP_ASSIGN); break;
        case '<': addToken(match('=') ? OP_LESS_EQUAL : OP_LESS); break;
        case '>': addToken(match('=') ? OP_GREATER_EQUAL : OP_GREATER); break;

        default:
            if (isdigit(c)) {
                readNumber(); // These methods handle their own column increments
            } else if (isalpha(c) || c == '_') {
                readIdentifierOrKeyword(); // These methods handle their own column increments
            } else {
                reportLexerError(line, current_token_start_column, "Unexpected character '" + string(1, c) + "'");
            }
            break;
    }
}