#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "tokenize.h"

TEST_CASE("Tokenizer basic functionality", "[tokenizer]") {
    SECTION("Simple words") {
        std::string input = "hello world";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "hello");
        CHECK(tokens[1].type == TokenType::Word);
        CHECK(tokens[1].value == "world");
    }
    
    SECTION("Single quotes") {
        std::string input = "echo 'hello world'";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::SingleQuoted);
        CHECK(tokens[1].value == "hello world");
    }
    
    SECTION("Double quotes") {
        std::string input = "echo \"hello world\"";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::DoubleQuoted);
        CHECK(tokens[1].value == "hello world");
    }
}

TEST_CASE("Tokenizer redirections", "[tokenizer]") {
    SECTION("Standard output redirection") {
        std::string input = "echo hello > output.txt";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::Word);
        CHECK(tokens[1].value == "hello");
        CHECK(tokens[2].type == TokenType::Redirect);
        CHECK(tokens[2].value == ">");
        CHECK(tokens[3].type == TokenType::Word);
        CHECK(tokens[3].value == "output.txt");
    }
    
    SECTION("Error redirection") {
        std::string input = "command 2> error.log";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "command");
        CHECK(tokens[1].type == TokenType::RedirectErr);
        CHECK(tokens[1].value == "2>");
        CHECK(tokens[2].type == TokenType::Word);
        CHECK(tokens[2].value == "error.log");
    }
    
    SECTION("Append redirection") {
        std::string input = "echo hello >> output.txt";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::Word);
        CHECK(tokens[1].value == "hello");
        CHECK(tokens[2].type == TokenType::AppendStd);
        CHECK(tokens[2].value == ">>");
        CHECK(tokens[3].type == TokenType::Word);
        CHECK(tokens[3].value == "output.txt");
    }
    
    SECTION("Error append redirection") {
        std::string input = "command 2>> error.log";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "command");
        CHECK(tokens[1].type == TokenType::AppendErr);
        CHECK(tokens[1].value == "2>>");
        CHECK(tokens[2].type == TokenType::Word);
        CHECK(tokens[2].value == "error.log");
    }
}

TEST_CASE("Tokenizer special cases", "[tokenizer]") {
    SECTION("Escape sequences") {
        std::string input = "echo \"hello \\\"world\\\"\"";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::DoubleQuoted);
        CHECK(tokens[1].value == "hello \"world\"");
    }
    
    SECTION("Empty input") {
        std::string input = "";
        auto tokens = tokenize(input);
        
        CHECK(tokens.empty());
    }
    
    SECTION("Multiple spaces") {
        std::string input = "echo    hello     world";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].value == "hello");
        CHECK(tokens[2].value == "world");
    }
    
    SECTION("Mixed quotes and escapes") {
        std::string input = "echo 'single' \"double\" \\\"escaped\\\"";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].type == TokenType::SingleQuoted);
        CHECK(tokens[1].value == "single");
        CHECK(tokens[2].type == TokenType::DoubleQuoted);
        CHECK(tokens[2].value == "double");
        CHECK(tokens[3].type == TokenType::Word);
        CHECK(tokens[3].value == "\"escaped\"");
    }
}

TEST_CASE("Tokenizer edge cases", "[tokenizer]") {
    SECTION("Only spaces") {
        std::string input = "    ";
        auto tokens = tokenize(input);
        CHECK(tokens.empty());
    }
    
    SECTION("Multiple redirections") {
        std::string input = "cmd > out.txt 2> err.txt";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 5);
        CHECK(tokens[0].type == TokenType::Word);
        CHECK(tokens[1].type == TokenType::Redirect);
        CHECK(tokens[2].type == TokenType::Word);
        CHECK(tokens[3].type == TokenType::RedirectErr);
        CHECK(tokens[4].type == TokenType::Word);
    }
    
    SECTION("Escaped spaces") {
        std::string input = "echo hello\\ world";
        auto tokens = tokenize(input);
        
        REQUIRE(tokens.size() == 2);
        CHECK(tokens[0].value == "echo");
        CHECK(tokens[1].value == "hello world");
    }
} 