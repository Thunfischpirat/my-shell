#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include "src/tokenize.h"


int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const std::vector<std::string> cmds {"exit", "echo", "type", "pwd", "cd"};
    const std::vector<std::string> quotes {"\'", "\""};
    const std::vector<std::string> redirects {">", "1>", "2>", ">>", "2>>"};

    std::string input;
    while (true) {
        const char* path = std::getenv("PATH");
        if (!path) {
            std::cerr << "PATH environment variable not set\n";
            continue;
        }
        std::stringstream env_p(path);   
        bool found { false };

        std::cout << "$ ";

        std::getline(std::cin, input);
        std::string buf{ "" };

        auto tokens = tokenize(input);
        
        std::vector<std::string> command_tokens;
        bool redirect = false;
        bool redirect_err = false;
        bool append_std = false;
        bool append_err = false;
        
        for (const auto& token : tokens) {
            switch (token.type) {
                case TokenType::Word:
                case TokenType::SingleQuoted:
                case TokenType::DoubleQuoted:
                    command_tokens.push_back(token.value);
                    break;
                case TokenType::Redirect:
                    redirect = true;
                    break;
                case TokenType::RedirectErr:
                    redirect_err = true;
                    break;
                case TokenType::AppendStd:
                    redirect = true;
                    append_std = true;
                    break;
                case TokenType::AppendErr:
                    redirect_err = true;
                    append_err = true;
                    break;
            }
        }

        if (command_tokens.empty()) {
            continue;
        }

        if (command_tokens[0] == "exit") {
            return 0;
        }

        else if (command_tokens[0] == "echo") {
            buf = "";
            for (size_t i { 1 }; i < command_tokens.size() - 1; i++) {
               buf += (command_tokens[i] + " ");
            }
            if (redirect) {
                std::ofstream output_file;
                std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                output_file.open(command_tokens[2], mode); 
                output_file << buf << std::endl;
                output_file.close();               
            }
            else if (redirect_err) {
                std::ofstream output_file;
                std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                output_file.open(command_tokens[2], mode);
                std::cout << buf << std::endl;
                output_file << "";
                output_file.close();               
            }
            else {
                buf += command_tokens.back();
                std::cout << buf << std::endl;
            }
        }
        else if (command_tokens[0] == "type") {
            for (const auto& cmd : cmds) {
                if (command_tokens[1] == cmd) {
                    if (redirect) {
                        std::ofstream output_file;
                        std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                        output_file.open(command_tokens[2], mode);
                        output_file << command_tokens[1] << " is a shell builtin" << std::endl;
                        output_file.close();
                    } else if (redirect_err) {
                        std::ofstream output_file;
                        std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                        output_file.open(command_tokens[2], mode);
                        output_file << ""; 
                        output_file.close();
                    } else {
                        std::cout << command_tokens[1] << " is a shell builtin" << std::endl;
                    }
                    found = true;
                    break;
                }
            }
            while(std::getline(env_p, buf, ':')) {
                if (found)
                    break;
                if (!std::filesystem::exists(buf))
                    continue;    
                for (const auto& p : std::filesystem::directory_iterator(buf)) {
                    if (p.path().filename() == command_tokens[1]) {
                        if (redirect) {
                            std::ofstream output_file;
                            std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                            output_file.open(command_tokens[2], mode);
                            output_file << command_tokens[1] << " is " << p.path().string() << std::endl;
                            output_file.close();
                        } else if (redirect_err) {
                            std::ofstream output_file;
                            std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                            output_file.open(command_tokens[2], mode);
                            output_file << "";
                            output_file.close();
                        } else {
                            std::cout << command_tokens[1] << " is " << p.path().string() << std::endl;
                        }
                        found = true;
                        break;
                    } 
                } 
            } 
            if (!found)
                if (redirect_err) {
                    std::ofstream output_file;
                    std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                    output_file.open(command_tokens[2], mode);
                    output_file << command_tokens[1] << ": not found";
                    output_file.close();
                } else    
                    std::cout << command_tokens[1] << ": not found" << std::endl;
        }   
        else if (command_tokens[0] == "pwd") {
            const std::filesystem::path cwd = std::filesystem::current_path();
            if (redirect) {
                std::ofstream output_file;
                std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                output_file.open(command_tokens[2], mode);
                output_file << cwd.string() << std::endl;
                output_file.close();
            } else if (redirect_err) {
                std::ofstream output_file;
                std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                output_file.open(command_tokens[2], mode);
                output_file << "" << std::endl;
                output_file.close();
            } else {
                std::cout << cwd.string() << std::endl;
            }    
        }
        else if (command_tokens[0] == "cd") {
            if (command_tokens[1] == "~") {
                const char* home = std::getenv("HOME");
                if (!home) {
                    std::cerr << "HOME environment variable not set\n";
                    continue;
                }
                std::filesystem::current_path(home);
            }
            else if (!std::filesystem::exists(command_tokens[1])) {
                if (redirect_err) {
                    std::ofstream output_file;
                    std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                    output_file.open(command_tokens[2], mode);
                    output_file << "cd: " << command_tokens[1] << ": No such file or directory" << std::endl;
                    output_file.close();
                }
                else
                    std::cout << "cd: " << command_tokens[1] << ": No such file or directory" << std::endl;
            }
            else 
                std::filesystem::current_path(command_tokens[1]);
        }
        else {
            while(std::getline(env_p, buf, ':')) {
                if (found)
                    break;
                if (!std::filesystem::exists(buf)) 
                    continue;
                for (const auto& p : std::filesystem::directory_iterator(buf)) {
                    if (p.path().filename() == command_tokens[0]) {
                        std::system(input.c_str());
                        found = true;
                        break;
                    }
                }
            }
            if (!found) 
                if (redirect_err) {   
                    std::ofstream output_file;
                    std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                    output_file.open(command_tokens[2], mode);
                    output_file << input + ": command not found" << std::endl;
                    output_file.close();
                } 
                else {
                    std::cout << input + ": command not found" << std::endl;
                }    
 
        }
    }
}

