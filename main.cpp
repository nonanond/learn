#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <regex>
#include <sstream>
#include <signal.h>
#include <vector>
#include <utility>
#include <random>

using std::cout, std::wcout, std::cin, std::wcin, std::endl, std::cerr, std::string, std::wstring, std::string_view, std::vector, std::pair, std::getline, std::ifstream, std::ofstream, std::fstream, std::regex, std::wregex, std::smatch, std::wsmatch, std::sregex_iterator, std::wsregex_iterator, std::istreambuf_iterator;
using namespace std::filesystem;
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals; 

constexpr auto USE_REGEX = false;

constexpr auto saves_file_name = "saves.toml";
auto saves = fstream {};

void sigint_handler(int sig) {
	/* using a char[] so that sizeof will work */
	// const char msg[] = "Ahhh! SIGINT!\n";
	// write(0, msg, sizeof(msg));
	if (saves.is_open ())
		saves.close ();
	exit (0);
}



auto main () -> int {
	{
		struct sigaction sa = {};
		sa.sa_handler = sigint_handler;
		sa.sa_flags = 0;

		if (sigaction (SIGINT, &sa, NULL) == -1) {
			cerr << "internal error >> sigaction" << endl;
			exit(1);
		}
	}

	saves.open (saves_file_name);
	if (not saves.is_open ()) {
		cerr << "internal error >> no save file named \"" << saves_file_name << "\"" << endl;
		exit (0);
	}

	auto const content = string {istreambuf_iterator<char> {saves}, istreambuf_iterator<char> {}};
	saves.close ();

	// Demand "[questions]"
	// auto pattern = regex {R"(\[questions\])"};
	// auto matches = smatch {};
	
	// {
	// 	auto iter = sregex_iterator {content.begin (), content.end (), pattern};
	// 	if (iter == sregex_iterator {}) {
	// 		cerr << "internal error >> no toml table with name \"questions\" found" << endl;
	// 		exit (1);
	// 	}
	// }

	auto input = string {};
	cout << "Lets Learn!" << endl;
	cout << "intro >> enter \"fastmode\" to create new questions" << endl;
	cout << "intro >> enter \"learn\" to play" << endl;
	
	MODE:
	cout << "mode >> ";
	getline (cin, input);
	if (input == "add") {
		while (true) {
			saves.open(saves_file_name, std::ios_base::app);
			QUESTION:
			cout << "question >> ";
			getline (cin, input);
			if (input.length () == 0) {
				goto QUESTION;
			}
			// cin >> input;
			saves << "\"" << input << "\"";
			ANSWER:
			cout << "answer >> ";
			getline (cin, input);
			if (input.length () == 0) {
				goto ANSWER;
			}
			// cin >> input;
			saves << " = \"" << input << "\"\n\n";
			saves.close();
		}
	} else if (input == "learn") {
		auto knowledge = vector <pair <string, string>> {};
		auto pattern = regex {R"(\"{1,3}([åäöÅÄÖ,.\(\)\[\]/\-|]*\w*[åäöÅÄÖ,.\(\)\[\]/-|]*\w*\s*\?*)+\"{1,3})"};
		for (auto iter = sregex_iterator {content.begin (), content.end (), pattern}; iter != sregex_iterator {}; ++iter) {
			knowledge.push_back ({iter -> str (), (++iter) -> str ()});
			knowledge.back().first.erase(0, 1);
			knowledge.back().first.pop_back();
			knowledge.back().second.erase(0, 1);
			knowledge.back().second.pop_back();
		}

		static thread_local auto engine = std::default_random_engine {std::random_device {} ()};
		
		for (auto i = 0; i < knowledge.size (); ++i) {
			auto dist = std::uniform_int_distribution<> {0, (int) knowledge.size ()};
			auto rand = dist (engine);
			auto iter = knowledge.begin () + rand;
			// auto iter = knowledge.begin () + i;
			auto& question = iter -> first;
			auto& answer = iter -> second;
			cout << question << endl;
			auto input = string {};
			getline (cin, input); // wait for user to press enter before printing answer
			cout << answer << endl << endl;
			getline (cin, input); // wait for user to press enter before printing next question
			--i;
			// let the question be if user enter '-'
			if (input == "-") {
			
			} else if (input.length () == 0) {
				knowledge.erase (iter);
			}
		}
	} else {
		cout << "error >> mode does not exist" << endl;
		goto MODE;
	}
	
	return 0;
}

