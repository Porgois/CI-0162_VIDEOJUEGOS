-- Global Variables
player_velocity = 150
fixed_player_velocity = math.sqrt( (player_velocity * player_velocity) / 2)

function update()
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

function handle_controls()

end

function handle_animation(speed_x, speed_y)
    if speed_x ~= 0 or speed_y ~= 0 then
        play_animation(this, "walk")
    else
        play_animation(this, "idle")
    end

end