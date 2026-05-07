player_velocity = 60
can_move = true
local follow = true

fixed_player_velocity = math.sqrt( (player_velocity * player_velocity) / 2)

function on_awake()
    print("On awake running!")
end

function on_collision(other)
    -- Collide with enemies

    -- Collide with pickups
end

function update()
    if GameState and GameState.set_reload_menu and is_button_just_pressed("rmb") then
        GameState.set_reload_menu(not GameState.reload_menu_open)
        return
    end

    if not can_move or (GameState and GameState.reload_menu_open) then
        play_animation(this, "idle")
        toggle_camera_follow(this, false)
        toggle_camera_follow(this, false)
        set_velocity(this, 0, 0)
        return
    end

    toggle_camera_follow(this, true)
    toggle_camera_follow(this, true)
    set_velocity(this, 0, 0)
    vel_x = 0
    vel_y = 0

    if is_action_active("move_up") then
        vel_y = vel_y + -1
    end
    if is_action_active("move_down") then
       vel_y = vel_y + 1
    end
    if is_action_active("move_left") then
        vel_x = vel_x + -1
    end
    if is_action_active("move_right") then
        vel_x = vel_x + 1
    end

    if vel_x ~= 0 and vel_y ~= 0 then
        vel_x = vel_x * fixed_player_velocity
        vel_y = vel_y * fixed_player_velocity
    else
        vel_x = vel_x * player_velocity
        vel_y = vel_y * player_velocity
    end

    set_velocity(this, vel_x, vel_y)

    handle_animation(vel_x, vel_y)

end

function handle_animation(speed_x, speed_y)
    if speed_x ~= 0 or speed_y ~= 0 then
        play_animation(this, "walk")
    else
        play_animation(this, "idle")
    end

end