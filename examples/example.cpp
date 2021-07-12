#include <diagtree/tree.hpp>
#include <fstream>
#include <string>
#include <cstdio>
#include <iostream>


int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "No file provided\n";
		return 0;
	}
	std::ifstream f(argv[1], std::ios::in);
	auto tree = Tree(f, true);
	int playerID = 0;
	std::vector<std::string> speakerNames{"Me", "Person 1", "Person 2"};

	auto head = tree.find("start");
	while(true) {
		if(!head) break;
		std::cout << "[" << speakerNames[head->speaker_id] << "] " << head->text << "\n";
		if(head->closer) break;
		auto next = tree.get_next(head->id);
		if(next.empty()) break;
		/*
		* Bit of a dillema, i don't know what to do incase of mixed `next` entries
		* Maybe throw an error over it?
		*/
		if(next[0]->speaker_id != playerID) {
			head = next[0];
		} else {
			int idx = 1;
			std::cout << "\nOptions:\n";
			for(auto &user_line : next) {
				std::cout << "\t[" << std::to_string(idx) << "] " << user_line->text << "\n";
				idx++;
			}
			int num = 0;
			std::cin >> num;
			head = next[num-1];
		}
	}
	std::cout << "Conversation over!\n";
}
