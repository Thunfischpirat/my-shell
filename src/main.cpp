#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    const std::vector<std::string> cmds {"exit", "echo", "type", "pwd", "cd"};

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

        std::stringstream ss(input);
        std::vector<std::string> tokens;
        
        bool single_quotation{ false };
        bool double_quotation{ false };
        bool literal_next{ false };
        bool special_literal{ false };
        bool redirect{ false };
        bool redirect_err{ false };
        bool append_std{ false };
        bool append_err{ false };
        char prev_char;
        for (const char &c : input) {
            if (c == '\'' && !single_quotation && !double_quotation) {
                single_quotation = true;
            }
            else if (c == '"' && !single_quotation && !double_quotation && !literal_next) {
                double_quotation = true;
            }
            else if (!single_quotation && !double_quotation && c == '>') {
                if (prev_char == ' ')
                    redirect = true;
                else if (prev_char == '1') {
                    buf = "";
                    redirect = true;
                }
                else if (prev_char == '2') {
                    buf = "";
                    redirect_err = true;
                }
                else if (prev_char == '>') {
                    if (redirect)
                        append_std = true;
                    else if (redirect_err)
                        append_err = true;
                }
            }
            else if (c == '\'' && single_quotation && !double_quotation && !special_literal) {
                single_quotation = false;
                buf += "";
            }	   
            else if (c == '"' && !single_quotation && double_quotation && !special_literal) {
                double_quotation = false;
                buf += "";
            }   
            else if (special_literal) {
                if (c == '$' || c == '`' || c == '\\' || c == '"') {
                    buf += c;
                    special_literal = false;
                }
                else {
                    buf += '\\';
                    buf += c;
                    special_literal = false;
                }
            }
            else if (c == '\\' && !single_quotation) {
                if (!double_quotation) {
                    buf += "";                
                    literal_next = true;
                }
                else {
                    buf += "";
                    special_literal = true;
                }                  
            }
            else if (single_quotation || double_quotation || literal_next) 
                buf += c;
            else if (c != ' ')  
                buf += c;
            else if (buf != "") {
                tokens.push_back(buf);
                buf = "";
            } 
            prev_char = c;
        }
        if (!(buf == ""))
            tokens.push_back(buf);

        if (tokens.empty()) {
            continue;
        }

        if (tokens[0] == "exit") {
            return 0;
        }
        else if (tokens[0] == "echo") {
            buf = "";
            for (size_t i { 1 }; i < tokens.size() - 1; i++) {
               buf += (tokens[i] + " ");
            }
            if (redirect) {
                std::ofstream output_file;
                std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                output_file.open(tokens[2], mode); 
                output_file << buf << std::endl;
                output_file.close();               
            }
            else if (redirect_err) {
                std::ofstream output_file;
                std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                output_file.open(tokens[2], mode);
                std::cout << buf << std::endl;
                output_file << "";
                output_file.close();               
            }
            else {
                buf += tokens.back();
                std::cout << buf << std::endl;
            }
        }
        else if (tokens[0] == "type") {
            for (const auto& cmd : cmds) {
                if (tokens[1] == cmd) {
                    if (redirect) {
                        std::ofstream output_file;
                        std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                        output_file.open(tokens[2], mode);
                        output_file << tokens[1] << " is a shell builtin" << std::endl;
                        output_file.close();
                    } else if (redirect_err) {
                        std::ofstream output_file;
                        std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                        output_file.open(tokens[2], mode);
                        output_file << ""; 
                        output_file.close();
                    } else {
                        std::cout << tokens[1] << " is a shell builtin" << std::endl;
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
                    if (p.path().filename() == tokens[1]) {
                        if (redirect) {
                            std::ofstream output_file;
                            std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                            output_file.open(tokens[2], mode);
                            output_file << tokens[1] << " is " << p.path().string() << std::endl;
                            output_file.close();
                        } else if (redirect_err) {
                            std::ofstream output_file;
                            std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                            output_file.open(tokens[2], mode);
                            output_file << "";
                            output_file.close();
                        } else {
                            std::cout << tokens[1] << " is " << p.path().string() << std::endl;
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
                    output_file.open(tokens[2], mode);
                    output_file << tokens[1] << ": not found";
                    output_file.close();
                } else    
                    std::cout << tokens[1] << ": not found" << std::endl;
        }   
        else if (tokens[0] == "pwd") {
            const std::filesystem::path cwd = std::filesystem::current_path();
            if (redirect) {
                std::ofstream output_file;
                std::ios::openmode mode = append_std ? std::ios::app : std::ios::out; 
                output_file.open(tokens[2], mode);
                output_file << cwd.string() << std::endl;
                output_file.close();
            } else if (redirect_err) {
                std::ofstream output_file;
                std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                output_file.open(tokens[2], mode);
                output_file << "" << std::endl;
                output_file.close();
            } else {
                std::cout << cwd.string() << std::endl;
            }    
        }
        else if (tokens[0] == "cd") {
            if (tokens[1] == "~") {
                const char* home = std::getenv("HOME");
                if (!home) {
                    std::cerr << "HOME environment variable not set\n";
                    continue;
                }
                std::filesystem::current_path(home);
            }
            else if (!std::filesystem::exists(tokens[1])) {
                if (redirect_err) {
                    std::ofstream output_file;
                    std::ios::openmode mode = append_err ? std::ios::app : std::ios::out; 
                    output_file.open(tokens[2], mode);
                    output_file << "cd: " << tokens[1] << ": No such file or directory" << std::endl;
                    output_file.close();
                }
                else
                    std::cout << "cd: " << tokens[1] << ": No such file or directory" << std::endl;
            }
            else 
                std::filesystem::current_path(tokens[1]);
        }
        else {
            while(std::getline(env_p, buf, ':')) {
                if (found)
                    break;
                if (!std::filesystem::exists(buf)) 
                    continue;
                for (const auto& p : std::filesystem::directory_iterator(buf)) {
                    if (p.path().filename() == tokens[0]) {
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
                    output_file.open(tokens[2], mode);
                    output_file << input + ": command not found" << std::endl;
                    output_file.close();
                } 
                else {
                    std::cout << input + ": command not found" << std::endl;
                }    
 
        }
    }
}

