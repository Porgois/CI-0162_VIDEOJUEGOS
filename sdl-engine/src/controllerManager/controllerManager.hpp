#ifndef CONTROLLER_MANAGER_HPP
#define CONTROLLER_MANAGER_HPP

#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <tuple>
#include <iostream>

class ControllerManager {
    private:
        std::map<std::string, int> action_key_name;
        std::map<int, bool> key_down;

        std::map<std::string, int> mouse_button_name;
        std::map<int, bool> mouse_button_down;
        
        int mouse_pos_x;
        int mouse_pos_y;

    public:
        ControllerManager();

        ~ControllerManager();

        void clear();

        // Keyboard
        void addActionKey(const std::string& action_name, int key_code);
        void keyDown(int key_code);
        void keyUp(int key_code);
        bool isActionActive(const std::string& action);

        // Mouse
        void addMouseButton(const std::string& name, int button_code);
        void mouseButtonDown(int button_code);
        void mouseButtonUp(int button_code);
        bool isMouseButtonDown(const std::string& name);

        void setMousePosition(int x, int y);
        std::tuple<int, int> getMousePosition();

};

#endif // CONTROLLER_MANAGER_HPP