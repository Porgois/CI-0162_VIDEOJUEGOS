#include <iostream>
#include <sol/sol.hpp>
#include <string>
#include <cmath>

int power(int num_1, int num_2) {
    return std::pow(num_1, num_2);
}

void LuaTest() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    // load c++ function
    lua["cPower"] = power;

    lua.script_file("./scripts/script01.lua");

    std::cout << "[C++] Aged obtained from state: " << static_cast<int>(lua["var_age"]) << std::endl;

    sol::table config = lua["config"];

    int width = static_cast<int>(config["resolution"]["width"]);
    std::cout << "[C++] Lua table window width: " << width << std::endl;

    sol::function factorial = lua["factorial"];
    int factorial_result = factorial(5);
    std::cout << "[C++] Factorial result: " << factorial_result << std::endl;
}

int main(int argc, char* argv[]) {
    LuaTest();

    return 0;
}