#include <iostream>
#include <vector>
#include <sstream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  std::string input;
  while (true)
  {
    std::cout << "$ ";

    std::getline(std::cin, input);
    std::string buf;

    std::stringstream ss(input);
    std::vector<std::string> tokens;

    while (ss >> buf)
	tokens.push_back(buf);	

    if (tokens[0] == "exit") {
        break;
    }
    if (tokens[0] == "echo") {
	int i { 1 };
	buf = "";
	while (i < tokens.size()) {
	  buf += (tokens[i] + " ");
          i++;
        }
        std::cout << buf << std::endl;
    }
    else
        std::cout << input + ": command not found" << std::endl;
  }
 }

