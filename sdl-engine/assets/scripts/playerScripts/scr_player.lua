local player_velocity = 60
local max_health = 3
local current_health = 3
local follow = true
local fixed_player_velocity = math.sqrt((player_velocity * player_velocity) / 2)
local current_weapon = nil
local current_weapon_name = nil
local current_weapon_slot = 1
local current_back_weapon = nil
local current_back_weapon_name = nil
local weapons = {}

-- Weapons
local revolver_weapon = dofile("./assets/scripts/entities/e_revolver.lua")
local shotgun_weapon = dofile("./assets/scripts/entities/e_shotgun.lua")

-- Back weapons
local revolver_weapon_back = dofile("./assets/scripts/entities/e_revolver_back.lua")
local shotgun_weapon_back = dofile("./assets/scripts/entities/e_shotgun_back.lua")

local back_weapon_data = {
    revolver = {
        normal = {offset = {x = 200.0, y = 4.0}, rotation = 90},
        flipped = {offset = {x = -180.0, y = 4.0}, rotation = 90}
    },
    shotgun = {
        normal = {offset = {x = -6.0, y = 0.0}, rotation = 100},
        flipped = {offset = {x = 7.0, y = 0.0}, rotation = -100}
    }
}

local back_weapon_entities = {
    revolver = revolver_weapon_back,
    shotgun = shotgun_weapon_back
}

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

-- Other
follow_camera = true
local flashlight_disabled = false

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
        footstep_timer = footstep_interval -- walk timer resets when walking starts
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
    if current_weapon then
        delete_entity(current_weapon)
        current_weapon = nil
        current_weapon_name = nil
    end
    delete_back_weapon()
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

    if is_action_active("move_up") then
        vel_y = vel_y - 1
    end
    if is_action_active("move_down") then
        vel_y = vel_y + 1
    end
    if is_action_active("move_left") then
        vel_x = vel_x - 1
    end
    if is_action_active("move_right") then
        vel_x = vel_x + 1
    end

    return vel_x, vel_y
end

function set_focus(focus)
    toggle_sprite_flip(this, focus)

    if follow_camera then
        toggle_camera_follow(this, focus)
    end

    if current_weapon then
        toggle_mouse_follow(current_weapon, focus)
    end

    if current_back_weapon then
        toggle_sprite_flip(current_back_weapon, false)
    end

    toggle_flashlight(this, focus)
end

local function get_weapon_file_path(weapon_name)
    return "./assets/scripts/entities/e_" .. weapon_name .. ".lua"
end

local function get_allowed_weapon_list()
    -- Sol2 wraps C++ vectors, so type() may not return "table"
    -- Just check if it exists and can be indexed
    if GameState and GameState.weapons then
        return GameState.weapons
    end
    return {}
end

local function is_weapon_allowed(weapon_name)
    if not weapon_name or weapon_name == "" then
        return false
    end

    local allowed_weapons = get_allowed_weapon_list()
    for i = 1, #allowed_weapons do
        if allowed_weapons[i] == weapon_name then
            return true
        end
    end

    return false
end

local function deep_copy_table(orig)
    if type(orig) ~= "table" then
        return orig
    end
    local copy = {}
    for k, v in pairs(orig) do
        copy[k] = deep_copy_table(v)
    end
    return copy
end

local function delete_back_weapon()
    if current_back_weapon then
        delete_entity(current_back_weapon)
        current_back_weapon = nil
        current_back_weapon_name = nil
    end
end

local function spawn_back_weapon(weapon_name)
    delete_back_weapon()

    if not weapon_name or weapon_name == "" then
        return
    end

    if not is_weapon_allowed(weapon_name) then
        return
    end

    local back_entity = back_weapon_entities[weapon_name]
    if not back_entity then
        return
    end

    local spawn_definition = deep_copy_table(back_entity)
    local player_flipped = is_flipped(this)
    local data_set = back_weapon_data[weapon_name]
    local weapon_data = data_set and (player_flipped and data_set.flipped or data_set.normal)

    if weapon_data and spawn_definition.components then
        if spawn_definition.components.child_of and weapon_data.offset then
            spawn_definition.components.child_of.offset = {
                x = weapon_data.offset.x,
                y = weapon_data.offset.y
            }
        end
        if spawn_definition.components.transform and weapon_data.rotation then
            spawn_definition.components.transform.rotation = weapon_data.rotation
        end

        -- Pre-seed the world position so there's no one-frame pop
        -- at (0,0) before child_of resolves it next frame.
        if spawn_definition.components.transform and weapon_data.offset then
            local player_x, player_y = get_position(this)
            spawn_definition.components.transform.position = {
                x = player_x + weapon_data.offset.x,
                y = player_y + weapon_data.offset.y
            }
        end
    end

    if spawn_definition.components.sprite then
        spawn_definition.components.sprite.flip = player_flipped
    end

    current_back_weapon = spawn_entity(spawn_definition)
    current_back_weapon_name = weapon_name
    current_back_weapon_flip = player_flipped
end

local function refresh_back_weapon_flip()
    if not current_back_weapon or not current_back_weapon_name then
        return
    end
    local is_flipped = is_flipped(this)
    if is_flipped ~= current_back_weapon_flip then
        spawn_back_weapon(current_back_weapon_name)
    end
end

local function load_weapon_list()
    weapons = get_allowed_weapon_list()

    if GameState and GameState.current_weapon_slot and type(GameState.current_weapon_slot) == "number" then
        current_weapon_slot = GameState.current_weapon_slot
    else
        current_weapon_slot = 1
    end

    if #weapons == 0 then
        current_weapon_slot = 0
    elseif current_weapon_slot < 1 or current_weapon_slot > #weapons then
        current_weapon_slot = 1
    end

    if GameState then
        GameState.current_weapon_slot = current_weapon_slot
    end
end

local function set_current_weapon(weapon_name)
    if not is_weapon_allowed(weapon_name) then
        return
    end

    if current_weapon then
        delete_entity(current_weapon)
        current_weapon = nil
        current_weapon_name = nil
    end

    if not weapon_name or weapon_name == "" then
        return
    end

    local entity_path = get_weapon_file_path(weapon_name)
    local weapon_entity = dofile(entity_path)
    if not weapon_entity then
        return
    end

    current_weapon = spawn_entity(weapon_entity)
    current_weapon_name = weapon_name
    if GameState then
        GameState.current_weapon_name = weapon_name
    end

    if not_dead then
        toggle_mouse_follow(current_weapon, true)
    end
end

local function switch_weapon()
    if not GameState or not GameState.weapons then
        return
    end

    local weapon_list = GameState.weapons
    local list_size = #weapon_list

    if list_size < 2 then
        return
    end

    -- Find current weapon index in the list
    local active_index = 1
    if current_weapon_name then
        for i = 1, list_size do
            if weapon_list[i] == current_weapon_name then
                active_index = i
                break
            end
        end
    end

    -- Calculate next weapon index
    local next_index = active_index + 1
    if next_index > list_size then
        next_index = 1
    end

    local next_weapon_name = weapon_list[next_index]

    -- Only switch if it's different and allowed
    if next_weapon_name == current_weapon_name then
        return
    end

    if not is_weapon_allowed(next_weapon_name) then
        return
    end

    -- Update slot
    current_weapon_slot = next_index
    if GameState then
        GameState.current_weapon_slot = current_weapon_slot
    end

    local previous_weapon_name = current_weapon_name
    spawn_back_weapon(previous_weapon_name)
    set_current_weapon(next_weapon_name)
end

-- Function for external scripts (like enemies) to disable/enable flashlight
-- This completely overrides the normal flashlight behavior
function set_flashlight_disabled(disabled)
    flashlight_disabled = disabled
    -- When disabling, turn OFF the flashlight immediately regardless of player focus state
    -- When enabling, turn ON the flashlight immediately
    toggle_flashlight(this, not disabled)
end

function take_damage(damage_amount)
    if state == "dead" then
        return
    end

    play_audio("assets/soundEffects/misc/hits/hit_flesh.wav", 0, 30)
    current_health = current_health - damage_amount

    if GameState then
        GameState.player_health = current_health
    end

    transition_to("damage")
end

function add_weapon_ammo(other, n)
    play_audio("assets/soundEffects/misc/pickups/ammo_pickup.wav", 0, 30)

    if GameState then
        if get_tag(other) == "t_shell_pickup" and GameState.add_shotgun_ammo then -- SHOTGUN AMMO
            GameState.add_shotgun_ammo(n)
            call_function(other, "spawn_text", "+" .. n .. " shells!")
        elseif get_tag(other) == "t_ammo_pickup" and GameState.add_revolver_ammo then -- REVOLVER AMMO
            call_function(other, "spawn_text", "+" .. n .. " bullets!")
            GameState.add_revolver_ammo(n)
        end
    end
end

function on_collision(other)
    if get_tag(other) == "t_ammo_pickup" or get_tag(other) == "t_shell_pickup" then -- AMMO
        if GameState then
            local ammo_amount = get_script_variable(other, "default_revolver_ammo") or 2
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
    if not (GameState and GameState.reload_menu_open) then
        if flashlight_disabled then
            toggle_sprite_flip(this, true)
            if follow_camera then toggle_camera_follow(this, true) end
            if current_weapon then
                toggle_mouse_follow(current_weapon, true)
            end
        else
            set_focus(true)
        end

        if is_action_just_pressed("space") then
            switch_weapon()
        end

        refresh_back_weapon_flip()
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
    elseif GameState then
        GameState.player_health = current_health
    end

    if GameState and GameState.player_max_health ~= nil then
        max_health = GameState.player_max_health
    elseif GameState then
        GameState.player_max_health = max_health
    end

    play_music("assets/soundEffects/environment/2.wav", -1, 4)

    load_weapon_list()

    -- Determine desired starting weapon
    local desired_weapon = nil
    if GameState and GameState.current_weapon_name and GameState.current_weapon_name ~= "" then
        desired_weapon = GameState.current_weapon_name
    elseif GameState and GameState.weapons and type(GameState.weapons) == "table" and #GameState.weapons >= 1 then
        desired_weapon = GameState.weapons[1]
    elseif #weapons >= 1 then
        desired_weapon = weapons[1]
    end

    -- Spawn the desired weapon using consistent path
    if desired_weapon and desired_weapon ~= "" and is_weapon_allowed(desired_weapon) then
        set_current_weapon(desired_weapon)

        -- Synchronize current_weapon_slot with the spawned weapon
        if GameState and GameState.weapons then
            for i = 1, #GameState.weapons do
                if GameState.weapons[i] == current_weapon_name then
                    current_weapon_slot = i
                    GameState.current_weapon_slot = i
                    break
                end
            end
        end
    end

    if current_weapon and not_dead then
        toggle_mouse_follow(current_weapon, true)
    end

    transition_to("idle")
end