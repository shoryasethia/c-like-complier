#include "Lexer.h"
#include "Token.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

// Forward declaration for the error reporting function
extern bool had_lexer_error; // Declare the error flag from Lexer.cpp

void runFile(const string& path);
void runPrompt();
void run(const string& source);

int main(int argc, char* argv[]) {
    if (argc > 2) {
        cerr << "Usage: " << argv[0] << " [script_file]" << endl;
        return 64; 
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}

void runFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << path << "'" << endl;
        exit(66); 
    }

    stringstream buffer;
    buffer << file.rdbuf(); 
    string source = buffer.str();

    run(source);

    if (had_lexer_error) { 
        exit(65); 
    }
}

void runPrompt() {
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) { /
            break;
        }
        run(line);
        had_lexer_error = false; 
    }
}

void run(const string& source) {
    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    // Output tokens
    for (const auto& token : tokens) {
        cout << token.toString() << endl;
    }
}