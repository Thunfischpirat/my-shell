#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <string>
#include <vector>

enum class TokenType {
    Word,
    SingleQuoted,
    DoubleQuoted,
    Redirect,
    RedirectErr,
    AppendStd,
    AppendErr,
    Pipe
};

enum class State {
    Normal,
    SingleQuote,
    DoubleQuote,
    Escape
};

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& input);

#endif 