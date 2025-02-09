#include "tokenize.h"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string current_token;
    
    State state = State::Normal;
    State previous_state = State::Normal;
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        char next = (i + 1 < input.length()) ? input[i + 1] : '\0';
        
        switch (state) {
            case State::Normal:
                if (c == '\'') {
                    if (!current_token.empty()) {
                        tokens.push_back({TokenType::Word, current_token});
                        current_token.clear();
                    }
                    previous_state = State::Normal;
                    state = State::SingleQuote;
                }
                else if (c == '"') {
                    if (!current_token.empty()) {
                        tokens.push_back({TokenType::Word, current_token});
                        current_token.clear();
                    }
                    previous_state = State::Normal;
                    state = State::DoubleQuote;
                }
                else if (c == '\\') {
                    previous_state = State::Normal;
                    state = State::Escape;
                }
                else if (c == '>') {
                    if (!current_token.empty()) {
                        tokens.push_back({TokenType::Word, current_token});
                        current_token.clear();
                    }
                    if (next == '>') {
                        tokens.push_back({TokenType::AppendStd, ">>"});
                        i++; 
                    } else {
                        tokens.push_back({TokenType::Redirect, ">"});
                    }
                }
                else if (c == '2' && next == '>') {
                    if (!current_token.empty()) {
                        tokens.push_back({TokenType::Word, current_token});
                        current_token.clear();
                    }
                    if (i + 2 < input.length() && input[i + 2] == '>') {
                        tokens.push_back({TokenType::AppendErr, "2>>"});
                        i += 2; 
                    } else {
                        tokens.push_back({TokenType::RedirectErr, "2>"});
                        i++; 
                    }
                }
                else if (c == ' ') {
                    if (!current_token.empty()) {
                        tokens.push_back({TokenType::Word, current_token});
                        current_token.clear();
                    }
                }
                else {
                    current_token += c;
                }
                break;
                
            case State::SingleQuote:
                if (c == '\'') {
                    tokens.push_back({TokenType::SingleQuoted, current_token});
                    current_token.clear();
                    previous_state = State::SingleQuote;
                    state = State::Normal;
                }
                else {
                    current_token += c;
                }
                break;
                
            case State::DoubleQuote:
                if (c == '\\' && (next == '"' || next == '\\' || next == '$' || next == '`')) {
                    previous_state = State::DoubleQuote;
                    state = State::Escape;
                }
                else if (c == '"') {
                    tokens.push_back({TokenType::DoubleQuoted, current_token});
                    current_token.clear();
                    previous_state = State::DoubleQuote;
                    state = State::Normal;
                }
                else {
                    current_token += c;
                }
                break;
                
            case State::Escape:
                current_token += c;
                state = previous_state;
                previous_state = State::Escape;
                break;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back({TokenType::Word, current_token});
    }
    
    return tokens;
} 