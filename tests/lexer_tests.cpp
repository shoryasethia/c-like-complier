#include <iostream>
#include <vector>
#include <string>
#include <cassert>     
#include <functional>  
#include <numeric>     
#include <any>         

#include "../src/Lexer.h"
#include "../src/Token.h"

using namespace std;

// Re-declare the error flag for tests to reset it.
// This allows tests to check if lexical errors were reported.
extern bool had_lexer_error; // From Lexer.cpp

// Helper to run a string through the lexer and return tokens
vector<Token> tokenize_string(const string& source) {
    had_lexer_error = false; // Reset global error flag before each test
    Lexer lexer(source);
    return lexer.tokenize();
}

// Basic assertion function for tokens. Returns true for pass, false for fail.
bool assert_token(const Token& actual, TokenType expected_type,
                  const string& expected_value, int expected_line, int expected_col,
                  const string& test_case_name, const string& step_name) {
    cout << "  Testing " << test_case_name << " - " << step_name << "... ";

    bool type_ok = actual.type == expected_type;
    bool value_ok = actual.value == expected_value;
    bool line_ok = actual.line == expected_line;
    bool col_ok = actual.column == expected_col;

    if (type_ok && value_ok && line_ok && col_ok) {
        cout << "PASS" << endl;
        return true;
    } else {
        cerr << "FAIL: " << test_case_name << " - " << step_name << endl;
        cerr << "  Expected: Type=" << tokenTypeToString(expected_type) << " Value='" << expected_value
                  << "' Line=" << expected_line << " Col=" << expected_col << endl;
        cerr << "  Actual:   " << actual.toString() << endl;
        return false;
    }
}

// Assertion for a number token including its literal value
bool assert_number_token(const Token& actual, int expected_literal,
                         int expected_line, int expected_col,
                         const string& test_case_name, const string& step_name) {
    bool base_ok = assert_token(actual, NUMBER, actual.value, expected_line, expected_col, test_case_name, step_name);
    if (!base_ok) return false; // If the basic token assert fails, no need to check literal

    try {
        int actual_literal = any_cast<int>(actual.literal_value);
        if (actual_literal == expected_literal) {
            cout << "    Literal value OK: " << actual_literal << endl;
            return true;
        } else {
            cerr << "FAIL: " << test_case_name << " - " << step_name << " (Literal mismatch)" << endl;
            cerr << "  Expected literal: " << expected_literal << ", Actual literal: " << actual_literal << endl;
            return false;
        }
    } catch (const bad_any_cast& e) {
        cerr << "FAIL: " << test_case_name << " - " << step_name << " (Bad literal cast)" << endl;
        cerr << "  Error: " << e.what() << endl;
        return false;
    }
}


// Function to run all lexer tests
void run_lexer_tests() {
    cout << "--- Running Lexer Tests ---" << endl;
    bool all_tests_passed = true;

    auto run_test_block = [&](const string& name, const function<bool()>& test_func) {
        cout << "\nTest Block: " << name << endl;
        bool block_passed = test_func(); // Execute the test function
        if (block_passed) {
            cout << "SUCCESS: All tests in '" << name << "' block passed." << endl;
        } else {
            cout << "FAILURE: Some tests in '" << name << "' block failed." << endl;
            all_tests_passed = false; // Mark overall test suite as failed
        }
        return block_passed; // Return block status
    };

    // Test 1: Keywords
    run_test_block("Keywords", [&]() {
        string source = "int void if else while return input output"; // Length 42 chars
        vector<Token> tokens = tokenize_string(source);
        if (had_lexer_error) { cerr << "Fail: Lexical errors detected." << endl; return false; }
        if (tokens.size() != 9) {
            cerr << "Fail: Incorrect token count. Expected 9, got " << tokens.size() << endl;
            return false;
        }
        
        bool ok = true; // Flag for current test block
        ok &= assert_token(tokens[0], KEYWORD_INT, "int", 1, 1, "Keywords", "int");
        ok &= assert_token(tokens[1], KEYWORD_VOID, "void", 1, 5, "Keywords", "void");
        ok &= assert_token(tokens[2], KEYWORD_IF, "if", 1, 10, "Keywords", "if");
        ok &= assert_token(tokens[3], KEYWORD_ELSE, "else", 1, 13, "Keywords", "else");
        ok &= assert_token(tokens[4], KEYWORD_WHILE, "while", 1, 18, "Keywords", "while");
        ok &= assert_token(tokens[5], KEYWORD_RETURN, "return", 1, 24, "Keywords", "return");
        ok &= assert_token(tokens[6], KEYWORD_INPUT, "input", 1, 31, "Keywords", "input");
        ok &= assert_token(tokens[7], KEYWORD_OUTPUT, "output", 1, 37, "Keywords", "output");
        ok &= assert_token(tokens[8], EOF_TOKEN, "", 1, 43, "Keywords", "EOF");
        return ok;
    });


    // Test 2: Operators
    run_test_block("Operators", [&]() {
        string source = "+ - * / = == != < <= > >="; // Length 26 chars
        vector<Token> tokens = tokenize_string(source);
        if (had_lexer_error) { cerr << "Fail: Lexical errors detected." << endl; return false; }
        if (tokens.size() != 12) {
            cerr << "Fail: Incorrect token count. Expected 12, got " << tokens.size() << endl;
            return false;
        }
        
        bool ok = true;
        ok &= assert_token(tokens[0], OP_PLUS, "+", 1, 1, "Operators", "+");
        ok &= assert_token(tokens[1], OP_MINUS, "-", 1, 3, "Operators", "-");
        ok &= assert_token(tokens[2], OP_MULTIPLY, "*", 1, 5, "Operators", "*");
        ok &= assert_token(tokens[3], OP_DIVIDE, "/", 1, 7, "Operators", "/");
        ok &= assert_token(tokens[4], OP_ASSIGN, "=", 1, 9, "Operators", "=");
        ok &= assert_token(tokens[5], OP_EQUAL, "==", 1, 11, "Operators", "==");
        ok &= assert_token(tokens[6], OP_NOT_EQUAL, "!=", 1, 14, "Operators", "!=");
        ok &= assert_token(tokens[7], OP_LESS, "<", 1, 17, "Operators", "<");
        ok &= assert_token(tokens[8], OP_LESS_EQUAL, "<=", 1, 19, "Operators", "<=");
        ok &= assert_token(tokens[9], OP_GREATER, ">", 1, 22, "Operators", ">");
        ok &= assert_token(tokens[10], OP_GREATER_EQUAL, ">=", 1, 24, "Operators", ">=");
        ok &= assert_token(tokens[11], EOF_TOKEN, "", 1, 26, "Operators", "EOF");
        return ok;
    });

    // Test 3: Delimiters
    run_test_block("Delimiters", [&]() {
        string source = "{ } ( ) [ ] ; ,"; // Length 15 chars
        vector<Token> tokens = tokenize_string(source);
        if (had_lexer_error) { cerr << "Fail: Lexical errors detected." << endl; return false; }
        if (tokens.size() != 9) {
            cerr << "Fail: Incorrect token count. Expected 9, got " << tokens.size() << endl;
            return false;
        }
        
        bool ok = true;
        ok &= assert_token(tokens[0], DELIM_LBRACE, "{", 1, 1, "Delimiters", "{");
        ok &= assert_token(tokens[1], DELIM_RBRACE, "}", 1, 3, "Delimiters", "}");
        ok &= assert_token(tokens[2], DELIM_LPAREN, "(", 1, 5, "Delimiters", "(");
        ok &= assert_token(tokens[3], DELIM_RPAREN, ")", 1, 7, "Delimiters", ")");
        ok &= assert_token(tokens[4], DELIM_LBRACKET, "[", 1, 9, "Delimiters", "[");
        ok &= assert_token(tokens[5], DELIM_RBRACKET, "]", 1, 11, "Delimiters", "]");
        ok &= assert_token(tokens[6], DELIM_SEMICOLON, ";", 1, 13, "Delimiters", ";");
        ok &= assert_token(tokens[7], DELIM_COMMA, ",", 1, 15, "Delimiters", ",");
        ok &= assert_token(tokens[8], EOF_TOKEN, "", 1, 16, "Delimiters", "EOF"); 
        return ok;
    });

    // Test 4: Literals (Numbers) and Identifiers
    run_test_block("Literals & Identifiers", [&]() {
        string source = "0 123 myVar func123 _name_ func_with_underscore"; // Length 48 chars
        vector<Token> tokens = tokenize_string(source);
        if (had_lexer_error) { cerr << "Fail: Lexical errors detected." << endl; return false; }
        if (tokens.size() != 7) {
            cerr << "Fail: Incorrect token count. Expected 7, got " << tokens.size() << endl;
            return false;
        }
        
        bool ok = true;
        ok &= assert_number_token(tokens[0], 0, 1, 1, "Literals & Identifiers", "0 literal");
        ok &= assert_number_token(tokens[1], 123, 1, 3, "Literals & Identifiers", "123 literal");
        ok &= assert_token(tokens[2], IDENTIFIER, "myVar", 1, 7, "Literals & Identifiers", "myVar identifier");
        ok &= assert_token(tokens[3], IDENTIFIER, "func123", 1, 13, "Literals & Identifiers", "func123 identifier");
        ok &= assert_token(tokens[4], IDENTIFIER, "_name_", 1, 21, "Literals & Identifiers", "_name_ identifier");
        ok &= assert_token(tokens[5], IDENTIFIER, "func_with_underscore", 1, 28, "Literals & Identifiers", "func_with_underscore identifier");
        ok &= assert_token(tokens[6], EOF_TOKEN, "", 1, 48, "Literals & Identifiers", "EOF");
        return ok;
    });


    // Test 5: Mixed code with comments and whitespace, multi-line
    run_test_block("Mixed Code, Comments, Whitespace", [&]() {
        string source = R"(
        // This is a C-- comment
        int main (void) { // Function declaration
            int x; // Variable declaration
            x = 10; // Assignment
            return x; // Return statement
        } // End of main
        )"; 

        vector<Token> tokens = tokenize_string(source);
        if (had_lexer_error) { cerr << "Fail: Lexical errors detected." << endl; return false; }
        
        if (tokens.size() != 18) {
            cerr << "Fail: Incorrect token count. Expected 17, got " << tokens.size() << endl;
            cerr << "  Actual tokens:" << endl;
            for (const auto& t : tokens) { cerr << "    " << t.toString() << endl; }
            return false;
        }

        bool ok = true;
        ok &= assert_token(tokens[0], KEYWORD_INT, "int", 3, 9, "Mixed Code", "int");
        ok &= assert_token(tokens[1], IDENTIFIER, "main", 3, 13, "Mixed Code", "main");
        ok &= assert_token(tokens[2], DELIM_LPAREN, "(", 3, 18, "Mixed Code", "(");
        ok &= assert_token(tokens[3], KEYWORD_VOID, "void", 3, 19, "Mixed Code", "void");
        ok &= assert_token(tokens[4], DELIM_RPAREN, ")", 3, 23, "Mixed Code", ")");
        ok &= assert_token(tokens[5], DELIM_LBRACE, "{", 3, 25, "Mixed Code", "{");
        ok &= assert_token(tokens[6], KEYWORD_INT, "int", 4, 13, "Mixed Code", "int");
        ok &= assert_token(tokens[7], IDENTIFIER, "x", 4, 17, "Mixed Code", "x");
        ok &= assert_token(tokens[8], DELIM_SEMICOLON, ";", 4, 18, "Mixed Code", ";");
        ok &= assert_token(tokens[9], IDENTIFIER, "x", 5, 13, "Mixed Code", "x");
        ok &= assert_token(tokens[10], OP_ASSIGN, "=", 5, 15, "Mixed Code", "=");
        ok &= assert_number_token(tokens[11], 10, 5, 17, "Mixed Code", "10 literal");
        ok &= assert_token(tokens[12], DELIM_SEMICOLON, ";", 5, 19, "Mixed Code", ";");
        ok &= assert_token(tokens[13], KEYWORD_RETURN, "return", 6, 13, "Mixed Code", "return");
        ok &= assert_token(tokens[14], IDENTIFIER, "x", 6, 20, "Mixed Code", "x");
        ok &= assert_token(tokens[15], DELIM_SEMICOLON, ";", 6, 21, "Mixed Code", ";");
        ok &= assert_token(tokens[16], DELIM_RBRACE, "}", 7, 9, "Mixed Code", "}");
        // ok &= assert_token(tokens.back(), EOF_TOKEN, "", 9, 1, "Mixed Code", "EOF"); 
        ok &= assert_token(tokens.back(), EOF_TOKEN, "", 8, 9, "Mixed Code", "EOF (current actual)");
        return ok;
    });

    // Test 6: Error handling - unexpected character
    run_test_block("Error Handling - Unexpected Char", [&]() {
        string source = "int @foo; bar#zoo"; // Length 19 chars
        vector<Token> tokens = tokenize_string(source);
        if (!had_lexer_error) {
            cerr << "Fail: Expected lexer error but none reported." << endl;
            return false;
        }
        if (tokens.size() != 6) { 
             cerr << "Fail: Incorrect token count for error test. Expected 6 tokens, got " << tokens.size() << endl;
             for (const auto& t : tokens) { cerr << "    " << t.toString() << endl; }
             return false;
        }

        bool ok = true;
        ok &= assert_token(tokens[0], KEYWORD_INT, "int", 1, 1, "Error Handling", "int");
        ok &= assert_token(tokens[1], IDENTIFIER, "foo", 1, 6, "Error Handling", "foo"); 
        ok &= assert_token(tokens[2], DELIM_SEMICOLON, ";", 1, 9, "Error Handling", ";"); 
        ok &= assert_token(tokens[3], IDENTIFIER, "bar", 1, 11, "Error Handling", "bar"); 
        ok &= assert_token(tokens[4], IDENTIFIER, "zoo", 1, 15, "Error Handling", "zoo"); 
        ok &= assert_token(tokens[5], EOF_TOKEN, "", 1, 18, "Error Handling", "EOF");
        return ok;
    });

    // Test 7: Error handling - standalone '!'
    run_test_block("Error Handling - standalone '!'", [&]() {
        string source = "int ! var;"; // Length 10 chars
        vector<Token> tokens = tokenize_string(source);
        if (!had_lexer_error) {
            cerr << "Fail: Expected lexer error for standalone '!' but none reported." << endl;
            return false;
        }
        if (tokens.size() != 4) {
             cerr << "Fail: Incorrect token count for ! error test. Expected 4 tokens, got " << tokens.size() << endl;
             for (const auto& t : tokens) { cerr << "    " << t.toString() << endl; }
             return false;
        }

        bool ok = true;
        ok &= assert_token(tokens[0], KEYWORD_INT, "int", 1, 1, "Error Handling", "int");
        ok &= assert_token(tokens[1], IDENTIFIER, "var", 1, 7, "Error Handling", "var"); 
        ok &= assert_token(tokens[2], DELIM_SEMICOLON, ";", 1, 10, "Error Handling", ";"); 
        ok &= assert_token(tokens[3], EOF_TOKEN, "", 1, 11, "Error Handling", "EOF"); 
        return ok;
    });


    // Final summary
    if (all_tests_passed) {
        cout << "\n=== ALL LEXER TESTS PASSED ===\n" << endl;
    } else {
        cerr << "\n!!! SOME LEXER TESTS FAILED !!!\n" << endl;
        exit(1); 
    }
}


int main() {
    run_lexer_tests();
    return 0;
}