#ifndef CONTROLLER_MANAGER_HPP
#define CONTROLLER_MANAGER_HPP

#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <iostream>

class ControllerManager {
    private:
        std::map<std::string, int> action_key_name;
        std::map<int, bool> key_down;

    public:
        ControllerManager();

        ~ControllerManager();

        void clear();

        // Keyboard
        void addActionKey(const std::string& action_name, int key_code);
        void keyDown(int key_code);
        void keyUp(int key_code);
        bool isActionActive(const std::string& action);
};

#endif // CONTROLLER_MANAGER_HPP