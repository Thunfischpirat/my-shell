#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> cmds {"exit", "echo", "type", "pwd", "cd"};

  std::string input;
  while (true) {

    std::stringstream env_p(std::getenv("PATH"));   
    bool found { false };

    std::cout << "$ ";

    std::getline(std::cin, input);
    std::string buf;

    std::stringstream ss(input);
    std::vector<std::string> tokens;

    while (ss >> buf)
	tokens.push_back(buf);	

    if (tokens[0] == "exit") {
        return 0;
    }
    else if (tokens[0] == "echo") {
	buf = "";
	for (int i { 1 }; i < tokens.size(); i++) {
	   buf += (tokens[i] + " ");
        }
        std::cout << buf << std::endl;
    }
    else if (tokens[0] == "type") {
        for (int i{ 0 }; i < cmds.size(); i++) {
          if (tokens[1] == cmds[i]) {
             std::cout << tokens[1] << " is a shell builtin" << std::endl;
             found = true;
             break;
          }
	}
        while(std::getline(env_p, buf, ':')) {
	   if (found)
	      break;
           if (!std::filesystem::exists(buf))
	      continue;	
	   for (auto &p : std::filesystem::directory_iterator(buf)) {
	      if (p.path().filename() == tokens[1]) {
	          std::cout << tokens[1] << " is " << p.path().string() << std::endl;
	          found = true;
	          break;
	      } 
	   } 
        } 
        if (!found)	
           std::cout << tokens[1] << ": not found" << std::endl;
    }   
    else if (tokens[0] == "pwd") {
	std::filesystem::path cwd = std::filesystem::current_path();
	std::cout << cwd.string() << std::endl;
    }
    else if (tokens[0] == "cd") {
	if (tokens[1] == "~") {
	   std::filesystem::current_path(std::getenv("HOME"));
	}
        else if (!std::filesystem::exists(tokens[1]))
           std::cout << "cd: " << tokens[1] << ": No such file or directory" << std::endl;
	else 
	   std::filesystem::current_path(tokens[1]);
    }
    else {
       while(std::getline(env_p, buf, ':')) {
	 if (found)
            break;
	 if (!std::filesystem::exists(buf)) 
	   continue;
	 for (auto &p : std::filesystem::directory_iterator(buf)) {
	   if (p.path().filename() == tokens[0]) {
		std::system(input.c_str());
		found = true;
		break;
	   }
         }
       }
       if (!found)	
          std::cout << input + ": command not found" << std::endl;
     }
   }
 }

