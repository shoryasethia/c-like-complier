#include "Token.h"
#include <iostream> // For cerr, cout
#include <string>   // Already included by Token.h, but explicit for clarity
#include <any>      // Already included by Token.h

using namespace std; // Added as per request

// Helper to convert TokenType enum to a readable string
string tokenTypeToString(TokenType type) {
    switch (type) {
        case KEYWORD_INT: return "KEYWORD(int)";
        case KEYWORD_VOID: return "KEYWORD(void)";
        case KEYWORD_IF: return "KEYWORD(if)";
        case KEYWORD_ELSE: return "KEYWORD(else)";
        case KEYWORD_WHILE: return "KEYWORD(while)";
        case KEYWORD_RETURN: return "KEYWORD(return)";
        case KEYWORD_INPUT: return "KEYWORD(input)";
        case KEYWORD_OUTPUT: return "KEYWORD(output)";

        case OP_PLUS: return "OP(+)";
        case OP_MINUS: return "OP(-)";
        case OP_MULTIPLY: return "OP(*)";
        case OP_DIVIDE: return "OP(/)";
        case OP_ASSIGN: return "OP(=)";
        case OP_EQUAL: return "OP(==)";
        case OP_NOT_EQUAL: return "OP(!=)";
        case OP_LESS: return "OP(<)";
        case OP_LESS_EQUAL: return "OP(<=)";
        case OP_GREATER: return "OP(>)";
        case OP_GREATER_EQUAL: return "OP(>=)";

        case DELIM_LPAREN: return "DELIM(()";
        case DELIM_RPAREN: return "DELIM())";
        case DELIM_LBRACE: return "DELIM({)";
        case DELIM_RBRACE: return "DELIM(})";
        case DELIM_LBRACKET: return "DELIM([)";
        case DELIM_RBRACKET: return "DELIM(])";
        case DELIM_SEMICOLON: return "DELIM(;)";
        case DELIM_COMMA: return "DELIM(,)";

        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case EOF_TOKEN: return "EOF";
        default: return "UNKNOWN_TYPE";
    }
}

// Implementation of Token::toString()
string Token::toString() const {
    string type_str = tokenTypeToString(type);
    string literal_repr = ""; // Representation of literal value

    // Handle literal_value based on its actual type
    if (type == NUMBER) {
        try {
            literal_repr = " (literal: " + to_string(any_cast<int>(literal_value)) + ")";
        } catch (const bad_any_cast& e) {
            literal_repr = " (literal: CAST_ERROR)"; // Fallback for bad cast
            cerr << "Warning: Bad any_cast for NUMBER token literal. " << e.what() << endl;
        }
    }
    // Other token types (keywords, operators, delimiters, identifiers, EOF)
    // don't have a distinct literal value that needs to be printed here;
    // their 'value' (lexeme) is usually sufficient.

    return "Token: " + type_str + "(" + value + ")" + literal_repr +
           " at line " + to_string(line) + ", col " + to_string(column);
}