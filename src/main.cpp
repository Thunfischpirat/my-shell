#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> cmds {"exit", "echo", "type"};
  std::stringstream env_p(std::getenv("PATH"));   

  // Uncomment this block to pass the first stage
  std::string input;
  while (true) {
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
	for (int i { 1 }; i <= tokens.size(); i++) {
	   buf += (tokens[i] + " ");
        }
        std::cout << buf << std::endl;
    }
    else if (tokens[0] == "type") {
        bool found { false };
        for (int i{ 0 }; i <= tokens.size(); i++) {
          if (tokens[1] == cmds[i]) {
             std::cout << tokens[1] << " is a shell builtin" << std::endl;
             found = true;
             break;
          }
	}

	if (found)
	   continue;

        while(std::getline(env_p, buf, ':')) {
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
    else 
       std::cout << input + ": command not found" << std::endl;
  }
 }

