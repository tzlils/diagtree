#include <vector>
#include <map>
#include <optional>
#include <string>
#include <memory>
#include <unordered_map>
#include <iterator>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <sstream>

std::string slurp(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

struct Dialogue {
	int id;
	int speaker_id;
	bool closer; // if true, this is the last line in a conversation 
	std::optional<std::string> name; // should only be used for the start of a conversation
	std::string text;
	std::vector<int> next; // if closer is true, this should be empty
	std::vector<int> conditions;
};

class Tree {
	std::vector<Dialogue> dialogues;
	std::vector<bool> conditions;
	std::unordered_map<std::string, int> condition_names;

	public:
	Tree(std::ifstream& file, bool relative_positioning = false) {
		// parse yaml here and populate the members
		// condition_names will be populated using an object at the top of the yaml describing names for each condition
		// then each dialogues condition will be turned from a name into an id to save memory
		std::string data = slurp(file);
		YAML::Node yaml = YAML::Load(data);

		auto condition_data = yaml["Conditions"];
		auto dialogue_data = yaml["Data"];
		conditions.resize(condition_data.size(), 0);
		condition_names.reserve(condition_data.size());
		
		int idx = 0;
		for(auto condition : condition_data) {
			condition_names[condition.as<std::string>()] = idx;
			idx++;
		}

		idx = 0;
		dialogues.reserve(dialogue_data.size());
		for(auto data : dialogue_data) {
			Dialogue dialogue;
			dialogue.id = idx;
			dialogue.speaker_id = data["speaker_id"].as<int>();
			dialogue.closer = data["closer"] ? true : false;
			dialogue.name = data["name"] ? std::optional(data["name"].as<std::string>()) : std::nullopt;
			dialogue.text = data["text"].as<std::string>();
			if(data["next"]) dialogue.next = data["next"].as<std::vector<int>>();
			if(relative_positioning) std::for_each(dialogue.next.begin(), dialogue.next.end(), [&idx](int &next){next += idx;});
			if(data["conditions"]) {
				auto cond_data = data["conditions"].as<std::vector<std::string>>();
				for(int i = 0; i < cond_data.size(); i++) {
					dialogue.conditions.push_back(condition_names.at(cond_data[i]));
				}
			}
			dialogues.insert(dialogues.end(), dialogue);
			idx++;
		}
	}

	Dialogue* find(std::string name) {
		for (auto &dialogue : dialogues) {
			if(!dialogue.name.has_value()) continue;
			if(!dialogue.name.value().compare(name)) return &dialogue;
		};
		return nullptr;
	}

	// if this function returns an empty vector its safe to end the conversation as there are no more applicable dialogues
	std::vector<Dialogue*> get_next(int id) {
		std::vector<Dialogue*> next;
		for (int next_id : dialogues[id].next) {
			for(int cond : dialogues[next_id].conditions) {
				if(!conditions[cond]) goto unfullfilled;
			}
			next.push_back(&dialogues[next_id]);
			unfullfilled:
			continue;
		}
		return next;
	}

	void set_condition(std::string condition_name, bool value) {
		conditions[condition_names.at(condition_name)] = value;
	}
};