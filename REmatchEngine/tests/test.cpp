#define BOOST_TEST_MODULE input_and_doc_to_spanners
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "automata/lva.hpp"
#include "parser_automata/parser.hpp"
#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "regex/regex.hpp"
#include "match.hpp"

#include <sstream>
#include <istream>
#include <iostream>
#include <algorithm>

#include <dirent.h>
#include <stdio.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>

namespace data = boost::unit_test::data;
using spanner = std::pair<unsigned int,unsigned int>;

/*
    HELPER FUNCTIONS
*/

std::vector<std::string> get_test_folders(std::string dir_path) {
    // Returns all the folders that start with "test" inside the given folder (POSIX only?)
    std::vector<std::string> subfolders;
    DIR *dir = opendir(dir_path.c_str());
    dirent *entry = readdir(dir);
    while (entry != NULL){
        if (entry->d_type == DT_DIR) {
            std::string subfolder = entry->d_name;
            if (subfolder.rfind("test", 0) == 0) {
                subfolders.push_back(subfolder);
            }
        }
        entry = readdir(dir);
    }
    closedir(dir);

    std::sort(subfolders.begin(), subfolders.end());

    return subfolders;
}

bool file_exists(const std::string file_path) {
    // Returns whether the given file exists or not
    ifstream f(file_path.c_str());
    return f.good();
}

std::string read_file(std::string file_path) {
    // Returns the file content as a string
    std::ifstream t(file_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

std::string strip(std::string in) {
    // Returns a copy of the given string without all space characters
    std::string final;
    for (size_t i = 0; i < in.length(); i++) {
        if(!isspace(in[i])) final += in[i];
    }
    return final;
}

std::string repr(std::string str) {
    // Returns a copy of the given string with escaped blank characters
    boost::replace_all(str, "\n", "\\n");
    boost::replace_all(str, "\t", "\\t");
    boost::replace_all(str, "\r", "\\r");
    return str;
}

std::set<std::string> parse_set(std::string str) {
    // Parses a string of the form "(<var>[<int>,<int>])*" to a set of
    // "<var>[<int>,<int>]" strings. This function is very dumb, it doesn't
    // check the validity of the input.
    // TODO: Regex parsing must disallow use of the '[' and ']' chars on vars
    std::set<std::string> outp;
    int pos;
    while(str.length()) {
        pos = str.find('>');
        outp.insert(str.substr(0, pos+1));
        str = str.substr(pos+1);
    }
    return outp;
}

std::set<std::string> flatten_set(std::set<std::set<std::string>> cont) {
    // Flattens the set of sets of "<var>[<int>,<int>]" strings to a set of
    // "(<var>[<int>,<int>])*" strings. Kind of the reverse of
    std::set<std::string> outp;
    for(auto setelem: cont) {
        std::stringstream ss;
        for(auto strelem : setelem) {
            ss << strelem;
        }
        outp.insert(ss.str());
    }
    return outp;
}

std::string file2str(std::string filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, in.end);
		contents.resize(in.tellg());
		in.seekg(0, in.beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw std::runtime_error("Error loading file");
}
/*
    MAIN TEST FUNCTION
*/

void check_spanners(rematch::RegEx &rgx, std::string document_file, std::string spanners_file) {
    std::string doc(file2str(document_file));
    std::ifstream spanner_fstream(spanners_file);
	std::stringstream output_sstream;

    std::set<std::set<std::string>> expected_results;
    for (std::string line; getline(spanner_fstream, line);) {
        expected_results.insert(parse_set(strip(line)));
    }

    std::set<std::set<std::string>> real_results;
    std::unique_ptr<rematch::Match> match_ptr;
    while(match_ptr = rgx.findIter(doc)) {
        std::string output = match_ptr->print();
        real_results.insert(parse_set(strip(output)));
    }

    std::set<std::set<std::string>> additional_values;
    std::set<std::set<std::string>> missing_values;
    std::set_difference(real_results.begin(), real_results.end(),
                        expected_results.begin(), expected_results.end(),
                        std::inserter(additional_values, additional_values.end()));
    std::set_difference(expected_results.begin(), expected_results.end(),
                        real_results.begin(), real_results.end(),
                        std::inserter(missing_values, missing_values.end()));

    std::set<std::string> additional_values_flat = flatten_set(additional_values);
    std::set<std::string> missing_values_flat = flatten_set(missing_values);

    std::string additional_values_stream = boost::algorithm::join(additional_values_flat, " & ");
    std::string missing_values_stream = boost::algorithm::join(missing_values_flat, " & ");

    BOOST_TEST_INFO("Additional values yielded by the evaluator: " << additional_values_stream);
    BOOST_TEST_INFO("Missing values yielded by the evaluator: " << missing_values_stream);
    BOOST_TEST_REQUIRE(real_results == expected_results, "Expected spanners do not match with given spanners");
}

/*
  TEST CASE 1:
    - Name: test_input_automata_file
    - Description: load the automata from automata.txt, process document.txt and check if
        given spanners are the same than those in spanners.txt
    - Tests folder: /tests/test_input_automata_file/
    - Test specific folder: every folder that starts with "test" inside de tests folder
    - Test specific folder files: automata.txt, document.txt, spanners.txt
*/

// const std::string test_input_automata_file_folder = "tests/test_input_automata_file";
// const std::vector<std::string> test_input_automata = get_test_folders(test_input_automata_file_folder);

// BOOST_DATA_TEST_CASE(
//     test_input_automata_file,
//     data::make(test_input_automata),
//     test_folder)
// {
//     std::string current_test_folder = test_input_automata_file_folder + "/" + test_folder + "/";
//     BOOST_TEST_MESSAGE("RUNNING TEST " + current_test_folder + "\n");

//     std::string automata_file = current_test_folder + "automata.txt";
//     std::string document_file = current_test_folder + "document.txt";
//     std::string spanners_file = current_test_folder + "spanners.txt";
//     BOOST_TEST_REQUIRE(file_exists(automata_file), "automata.txt missing");
//     BOOST_TEST_REQUIRE(file_exists(document_file), "document.txt missing");
//     BOOST_TEST_REQUIRE(file_exists(spanners_file), "spanners.txt missing");

//     LogicalVA A = parse_automata_file(automata_file);
//     check_spanners(A, document_file, spanners_file);

//     BOOST_TEST_MESSAGE("=== SUCCESS!\n");
// }


/*
  TEST CASE 2:
    - Name: test_input_regex_file
    - Description: load the automata from regex.txt, process document.txt and check if
        given spanners are the same than those in spanners.txt
    - Tests folder: /tests/test_input_regex_file/
    - Test specific folder: every folder that starts with "test" inside de tests folder
    - Test specific folder files: regex.txt, document.txt, spanners.txt
*/

const std::string test_input_regex_file_folder = "tests/test_input_regex_file";
const std::vector<std::string> test_input_regex = get_test_folders(test_input_regex_file_folder);

BOOST_DATA_TEST_CASE(
    test_input_regex_file,
    data::make(test_input_regex),
    test_folder)
{
    std::string current_test_folder = test_input_regex_file_folder + "/" + test_folder + "/";
    BOOST_TEST_MESSAGE("RUNNING TEST " + current_test_folder + "\n");

    std::string regex_file = current_test_folder + "regex.txt";
    std::string document_file = current_test_folder + "document.txt";
    std::string spanners_file = current_test_folder + "spanners.txt";
    BOOST_TEST_REQUIRE(file_exists(regex_file), "regex.txt missing.");
    BOOST_TEST_REQUIRE(file_exists(document_file), "document.txt missing.");
    BOOST_TEST_REQUIRE(file_exists(spanners_file), "spanners.txt missing.");

	std::string regex = read_file(regex_file);
    rematch::RegEx rgx(regex);
    check_spanners(rgx, document_file, spanners_file);

    BOOST_TEST_MESSAGE("=== SUCCESS!\n");
}