#pragma once

#include <string>
#include <any> // For holding various types of literal values (C++17)

// ----------------------------------------------------
// TokenType: Defines all possible token types for C--
// ----------------------------------------------------
enum TokenType {
    // Keywords
    KEYWORD_INT, KEYWORD_VOID, KEYWORD_IF, KEYWORD_ELSE,
    KEYWORD_WHILE, KEYWORD_RETURN, KEYWORD_INPUT, KEYWORD_OUTPUT,

    // Operators
    OP_PLUS, OP_MINUS, OP_MULTIPLY, OP_DIVIDE, OP_ASSIGN,
    OP_EQUAL, OP_NOT_EQUAL, OP_LESS, OP_LESS_EQUAL,
    OP_GREATER, OP_GREATER_EQUAL,

    // Delimiters
    DELIM_LPAREN, DELIM_RPAREN, DELIM_LBRACE, DELIM_RBRACE,
    DELIM_LBRACKET, DELIM_RBRACKET, DELIM_SEMICOLON, DELIM_COMMA,

    // Literals & Identifiers
    IDENTIFIER, // Variable names, function names
    NUMBER,     // Integer literals (C-- only has integers)

    // Special token for End-Of-File
    EOF_TOKEN
};

// ----------------------------------------------------
// Token: Represents a single lexical unit
// ----------------------------------------------------
struct Token {
    TokenType type;
    std::string value; // The lexeme (raw string from source)
    std::any literal_value; // The processed literal value (e.g., int for numbers)
    int line;
    int column; // Column where the token starts

    Token(TokenType type, std::string value, std::any literal_value, int line, int column)
        : type(type), value(std::move(value)), literal_value(std::move(literal_value)),
          line(line), column(column) {}

    // Convenience constructor for tokens without a specific literal value
    Token(TokenType type, std::string value, int line, int column)
        : type(type), value(std::move(value)), literal_value(), // default-constructed any (empty)
          line(line), column(column) {}

    // Method to get a string representation of the Token for debugging
    std::string toString() const;
};

// Helper function declaration (implementation in Token.cpp)
std::string tokenTypeToString(TokenType type);