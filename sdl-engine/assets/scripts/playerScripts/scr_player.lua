local player_velocity = 60
local player_health = 3
local follow = true
local fixed_player_velocity = math.sqrt((player_velocity * player_velocity) / 2)
local revolver

-- Damage animation
local damage_timer = 0
local damage_duration = 0.25

-- State machine
local state = "idle"
local states = {}

states["idle"] = {
    enter = function()
        set_velocity(this, 0, 0)
    end,
    update = function()
        play_animation(this, "idle")
        local vel_x, vel_y = get_movement_input()
        if vel_x ~= 0 or vel_y ~= 0 then
            transition_to("walk")
        end
    end
}

states["walk"] = {
    enter = function() end,
    update = function()
        local vel_x, vel_y = get_movement_input()
        if vel_x == 0 and vel_y == 0 then
            transition_to("idle")
            return
        end
        if vel_x ~= 0 and vel_y ~= 0 then
            vel_x = vel_x * fixed_player_velocity
            vel_y = vel_y * fixed_player_velocity
        else
            vel_x = vel_x * player_velocity
            vel_y = vel_y * player_velocity
        end
        set_velocity(this, vel_x, vel_y)
        play_animation(this, "walk")
    end
}

states["damage"] = {
    enter = function()
        set_velocity(this, 0, 0)
        play_animation(this, "damage")
        shake_camera(0.4, 6.0, 30.0)
        damage_timer = damage_duration
    end,
    update = function()
        damage_timer = damage_timer - get_delta_time()
        if damage_timer <= 0 then
            if player_health <= 0 then
                transition_to("dead")
            else
                transition_to("idle")
            end
        end
    end
}

states["dead"] = {
    enter = function()
        set_velocity(this, 0, 0)
        play_animation(this, "death")
    end,
    update = function()
        die()
    end
}

function die()
    set_focus(false)
    delete_entity(revolver)
end

function transition_to(new_state)
    if states[new_state] then
        state = new_state
        states[new_state].enter()
    end
end

function get_movement_input()
    local vel_x = 0
    local vel_y = 0
    if is_action_active("move_up")    then vel_y = vel_y - 1 end
    if is_action_active("move_down")  then vel_y = vel_y + 1 end
    if is_action_active("move_left")  then vel_x = vel_x - 1 end
    if is_action_active("move_right") then vel_x = vel_x + 1 end
    return vel_x, vel_y
end

function set_focus(focus)
    toggle_sprite_flip(this, focus)
    toggle_camera_follow(this, focus)
    toggle_mouse_follow(revolver, focus)
    toggle_flashlight(this, focus)
end

function take_damage(damage_amount)
    if state == "dead" then return end
    player_health = player_health - damage_amount
    transition_to("damage")
end

function on_collision(other)
    if get_tag(other) == "t_ammo_pickup" then
        if GameState and GameState.add_ammo then
            GameState.add_ammo(3)
        end
        delete_entity(other)
    end
end

function update()
    if GameState and GameState.set_reload_menu and is_button_just_pressed("rmb") then
        GameState.set_reload_menu(not GameState.reload_menu_open)
        return
    end

    if not (GameState and GameState.reload_menu_open) then
        set_focus(true)
        states[state].update()
    else
        set_focus(false)
        set_velocity(this, 0, 0)
        play_animation(this, "idle")
    end
end

function start()
    if has_entity("revolver") then
        print("has revolver!")
        revolver = find_entity("revolver")
    end
    print("Running player script!")
    transition_to("idle")
end