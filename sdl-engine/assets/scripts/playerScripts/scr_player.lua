local player_velocity = 60
local max_health = 3
local current_health = 3
local follow = true
local fixed_player_velocity = math.sqrt((player_velocity * player_velocity) / 2)
local revolver

-- Damage animation
local damage_timer = 0
local damage_duration = 0.25

local not_dead = true

-- Footsteps
local footstep_timer = 0
local footstep_interval = 0.27
local footstep_sounds = {
    "assets/soundEffects/footsteps/stone/sndFootShoRock1.wav",
    "assets/soundEffects/footsteps/stone/sndFootShoRock2.wav",
    "assets/soundEffects/footsteps/stone/sndFootShoRock3.wav",
    "assets/soundEffects/footsteps/stone/sndFootShoRock4.wav",
    "assets/soundEffects/footsteps/stone/sndFootShoRock5.wav",
    "assets/soundEffects/footsteps/stone/sndFootShoRock6.wav"
}

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
    enter = function() 
        footstep_timer = footstep_interval -- reset timer when starting to walk
    end,
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
        handle_footsteps()
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
            if current_health <= 0 then
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
    not_dead = false
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
    play_audio("assets/soundEffects/misc/hits/hit_flesh.wav", 0, 30)
    current_health = current_health - damage_amount
    if GameState then GameState.player_health = current_health end
    transition_to("damage")
end

function add_weapon_ammo(other, n)
    play_audio("assets/soundEffects/misc/pickups/ammo_pickup.wav", 0, 30)
    call_function(other, "spawn_text", "+" .. n .." ammo!")
    GameState.add_ammo(n)

end

function on_collision(other)
  
    if get_tag(other) == "t_ammo_pickup" then -- AMMO
       
        if GameState and GameState.add_ammo then
            local ammo_amount = get_script_variable(other, "default_ammo") or 2 -- In case it fails
            add_weapon_ammo(other, ammo_amount)
        end

        delete_entity(other)
    end

    if get_tag(other) == "t_health_pickup" then -- HEALTH
       
        if current_health < max_health then
            current_health = current_health + 1
            if GameState then GameState.player_health = current_health end
            play_audio("assets/soundEffects/misc/pickups/health_pickup.wav", 0, 20)
            call_function(other, "spawn_text", "+1 health!")
        else
            play_audio("assets/soundEffects/misc/pickups/health_pickup.wav", 0, 15)
            call_function(other, "spawn_text", "health full!")
        end
        delete_entity(other)
    end

end

function handle_footsteps()
    footstep_timer = footstep_timer - get_delta_time()
    if footstep_timer <= 0 then
        play_random_audio(footstep_sounds, 0, 3)
        footstep_timer = footstep_interval
    end
end


function update()
    if GameState and GameState.set_reload_menu and not_dead and is_button_just_pressed("rmb") then
        GameState.set_reload_menu(not GameState.reload_menu_open)

        -- Play revolver open/close and flashlight audio
        if not GameState.reload_menu_open then
            play_audio("assets/soundEffects/misc/flashlight/flashlight_off.wav")
            play_audio("assets/soundEffects/weapons/reload/cyllinder_close.wav", 0, 10)
        else
            play_audio("assets/soundEffects/misc/flashlight/flashlight_on.wav")
            play_audio("assets/soundEffects/weapons/reload/cyllinder_open.wav", 0, 10)
        end

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
    if GameState and GameState.player_health ~= nil then
        current_health = GameState.player_health
    else
        if GameState then GameState.player_health = current_health end
    end

    if GameState and GameState.player_max_health ~= nil then
        max_health = GameState.player_max_health
    else
        if GameState then GameState.player_max_health = max_health end
    end

    play_music("assets/soundEffects/environment/2.wav", -1, 4)

    if has_entity("revolver") then
        print("has revolver!")
        revolver = find_entity("revolver")
    end
    print("Running player script!")
    transition_to("idle")
end