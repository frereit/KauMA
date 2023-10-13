#pragma once
#include <nlohmann/json.hpp>

#ifndef TEST
int main(int argc, char *argv[]);
#endif
nlohmann::json parse(int argc, char *argv[]);