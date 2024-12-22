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
	std::cout << tokens[1] << std::endl;
        break;
    }
    else
        std::cout << input + ": command not found\n";
  }
 }

