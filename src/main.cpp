/*! \mainpage KauMa
 *
 * \section intro_sec Introduction
 *
 * This project contains my implementations of the various assignments for the
 * module "Kryptoanalyse & Methoden-Audit" (Cryptanalysis & Method Audit) at
 * DHBW Mannheim.
 *
 * \section structure_sec Structure
 *
 * The assignments are all implemented as pure functions in their respective
 * namespaces, without "knowledge" of the surrounding JSON-format. This is done
 * to ensure interoperability and extensibility. The JSON format is parsed by
 * the entrypoint and converted to function calls by some minor glue code, which
 * is responsible for checking the structure of the JSON, calling the function
 * of the requested action, and formatting the return value as JSON.
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <span>
#include <string>
#include <vector>

#ifdef TEST
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#endif

#include "glue.hpp"
#include "main.hpp"

using json = nlohmann::json;

#ifndef TEST
int main(int argc, char *argv[]) {
  // 1. Parse the data
  json input;
  try {
    input = parse(argc, argv);
  } catch (const std::exception &ex) {
    std::cerr << "Parsing Error: " << ex.what() << std::endl;
    // We assume we have the program name in argv[0]
    assert(argc >= 1);
    std::cerr << "Usage: " << argv[0] << " <input.json>" << std::endl;
    return 1;
  }

  // 2. Operate on the data
  json output;
  try {
    output = Glue::execute_action(input);
  } catch (const std::exception &ex) {
    std::cerr << "Execution Error: " << ex.what() << std::endl;
    return 1;
  }

  // 3. Output the result to stdout
  std::cout << std::setw(4) << output << std::endl;
}
#endif

/// @brief parses the commandline arguments according to the labwork-docker
/// specification
/// @param argc Number of arguments
/// @param argv The arguments
/// @return A parsed json value, guranteed to contain the 'action' key
/// @throws std::out_of_range if there are not enough commandline arguments
/// specified
/// @throws nlohmann::json::parse_error if the JSON is syntactically incorrect
/// @throws std::runtime_error if the JSON does not conform to the
/// labwork-docker specification
json parse(int argc, char *argv[]) {
  const std::vector<std::string> args(argv + 1, argv + argc);

  json input;
  if (args.at(0) == "-") {
    std::cin >> input;
  } else {
    std::ifstream input_stream(args.at(0));
    input_stream >> input;
  }

  if (!input["action"].is_string()) {
    throw std::runtime_error(
        "Supplied JSON does not contain the expected 'action' key");
  }
  return input;
}