local player_velocity = 55
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
        normal = {offset = {x = -5.0, y = 7.0}, rotation = 90},
        flipped = {offset = {x = 5.0, y = 7.0}, rotation = -90}
    },
    shotgun = {
        normal = {offset = {x = -6.0, y = 4.0}, rotation = 100},
        flipped = {offset = {x = 7.0, y = 4.0}, rotation = -100}
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
    if not not_dead then
        return
    end

    if GameState then
        GameState.player_health = current_health
        GameState.current_weapon_name = current_weapon_name or ""
        GameState.current_back_weapon_name = current_back_weapon_name or ""
        GameState.current_weapon_slot = current_weapon_slot
    end

    not_dead = false
    set_focus(false)
    if current_weapon then
        delete_entity(current_weapon)
        current_weapon = nil
    end
    set_current_weapon_state_name("")
    delete_back_weapon()
    go_to_scene("died")
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
        toggle_sprite_flip(current_back_weapon, focus)
    end

    toggle_flashlight(this, focus)
end

local function get_weapon_file_path(weapon_name)
    return "./assets/scripts/entities/e_" .. weapon_name .. ".lua"
end

local function get_allowed_weapon_list()
    if GameState and GameState.weapons then
        return GameState.weapons
    end
    return {}
end

local function get_current_weapon_state_name()
    if GameState and GameState.current_weapon_name and GameState.current_weapon_name ~= "" then
        return GameState.current_weapon_name
    end
    return current_weapon_name or ""
end

local function get_current_back_weapon_state_name()
    if GameState and GameState.current_back_weapon_name and GameState.current_back_weapon_name ~= "" then
        return GameState.current_back_weapon_name
    end
    return current_back_weapon_name or ""
end

function set_current_back_weapon_state_name(weapon_name)
    current_back_weapon_name = weapon_name or ""
    if GameState then
        GameState.current_back_weapon_name = current_back_weapon_name
    end
end

local function get_weapon_name_by_slot(slot)
    local weapon_list = GameState and GameState.weapons or weapons
    if type(slot) ~= "number" or slot < 1 or slot > #weapon_list then
        return nil
    end
    return weapon_list[slot]
end

function set_current_weapon_state_name(weapon_name)
    current_weapon_name = weapon_name or ""
    if GameState then
        GameState.current_weapon_name = current_weapon_name
    end
end

local function serialize_weapon_list(list)
    if type(list) ~= "table" then
        return "{}"
    end

    local parts = {}
    for i = 1, #list do
        parts[#parts + 1] = tostring(list[i])
    end

    return "{" .. table.concat(parts, ",") .. "}"
end

local function normalize_weapon_list(list)
    local normalized = {}
    local seen = {}

    if type(list) ~= "table" then
        return normalized
    end

    for i = 1, #list do
        local weapon_name = list[i]
        if weapon_name and weapon_name ~= "" and not seen[weapon_name] and (weapon_name == "revolver" or weapon_name == "shotgun") then
            normalized[#normalized + 1] = weapon_name
            seen[weapon_name] = true
        end
    end

    return normalized
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

local function sync_weapon_inventory_state()
    local normalized_weapons = normalize_weapon_list(GameState and GameState.weapons or weapons)

    if GameState then
        GameState.weapons = normalized_weapons
    end
    weapons = normalized_weapons

    local preferred_weapon_name = nil
    local current_slot = nil

    if GameState and type(GameState.current_weapon_slot) == "number" and GameState.current_weapon_slot >= 1 and GameState.current_weapon_slot <= #normalized_weapons then
        current_slot = GameState.current_weapon_slot
    elseif #normalized_weapons > 0 then
        current_slot = 1
    end

    if current_slot then
        preferred_weapon_name = get_weapon_name_by_slot(current_slot)
    end

    if GameState and GameState.current_weapon_name and GameState.current_weapon_name ~= "" and is_weapon_allowed(GameState.current_weapon_name) then
        preferred_weapon_name = GameState.current_weapon_name
    elseif preferred_weapon_name == nil and #normalized_weapons > 0 then
        preferred_weapon_name = normalized_weapons[1]
    end

    if GameState then
        if preferred_weapon_name and is_weapon_allowed(preferred_weapon_name) then
            GameState.current_weapon_name = preferred_weapon_name
        else
            GameState.current_weapon_name = ""
        end
    end

    current_weapon_name = preferred_weapon_name or ""

    if GameState and type(GameState.current_weapon_slot) == "number" and GameState.current_weapon_slot >= 1 and GameState.current_weapon_slot <= #normalized_weapons then
        current_weapon_slot = GameState.current_weapon_slot
    elseif #normalized_weapons > 0 then
        current_weapon_slot = 1
    else
        current_weapon_slot = 0
    end

    if GameState then
        GameState.current_weapon_slot = current_weapon_slot
    end
end

local function save_weapon_inventory_state()
    sync_weapon_inventory_state()
    print("[PLAYER] save_weapon_inventory_state: weapons=" .. serialize_weapon_list(GameState and GameState.weapons or weapons) .. ", current=" .. tostring(current_weapon_name) .. ", slot=" .. tostring(current_weapon_slot))
end

local function load_weapon_inventory_state()
    if not GameState then
        return
    end

    local saved_weapons = normalize_weapon_list(GameState.weapons)
    if #saved_weapons > 0 then
        weapons = saved_weapons
        GameState.weapons = saved_weapons
    else
        weapons = {}
        GameState.weapons = {}
    end

    local restored_weapon_name = nil
    local restored_slot = GameState.current_weapon_slot

    if type(restored_slot) ~= "number" or restored_slot < 1 or restored_slot > #saved_weapons then
        restored_slot = (#saved_weapons > 0) and 1 or 0
    end

    if GameState.current_weapon_name and GameState.current_weapon_name ~= "" and is_weapon_allowed(GameState.current_weapon_name) then
        restored_weapon_name = GameState.current_weapon_name
    elseif restored_slot > 0 then
        restored_weapon_name = saved_weapons[restored_slot]
    elseif #saved_weapons > 0 then
        restored_weapon_name = saved_weapons[1]
    end

    if restored_weapon_name and is_weapon_allowed(restored_weapon_name) then
        GameState.current_weapon_name = restored_weapon_name
    else
        GameState.current_weapon_name = ""
        restored_weapon_name = ""
    end

    GameState.current_weapon_slot = restored_slot
    current_weapon_name = restored_weapon_name or ""
    current_weapon_slot = restored_slot
    print("[PLAYER] load_weapon_inventory_state: weapons=" .. serialize_weapon_list(GameState.weapons) .. ", current=" .. tostring(current_weapon_name) .. ", slot=" .. tostring(current_weapon_slot))
end

local function add_weapon_to_inventory(weapon_name)
    if not weapon_name or weapon_name == "" then
        return false
    end

    if is_weapon_allowed(weapon_name) then
        return false
    end

    local existing_weapons = get_allowed_weapon_list()
    local allowed_weapons = {}

    for i = 1, #existing_weapons do
        allowed_weapons[i] = existing_weapons[i]
    end

    allowed_weapons[#allowed_weapons + 1] = weapon_name
    allowed_weapons = normalize_weapon_list(allowed_weapons)
    weapons = allowed_weapons

    if GameState then
        GameState.weapons = allowed_weapons
    end

    return true
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

function delete_back_weapon()
    if current_back_weapon then
        delete_entity(current_back_weapon)
        current_back_weapon = nil
    end
    set_current_back_weapon_state_name("")
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
    set_current_back_weapon_state_name(weapon_name)
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
    sync_weapon_inventory_state()

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
    end

    if not weapon_name or weapon_name == "" then
        set_current_weapon_state_name("")
        return
    end

    local entity_path = get_weapon_file_path(weapon_name)
    local weapon_entity = dofile(entity_path)
    if not weapon_entity then
        return
    end

    current_weapon = spawn_entity(weapon_entity)
    set_current_weapon_state_name(weapon_name)

    if not_dead then
        toggle_mouse_follow(current_weapon, true)
    end
end

local function switch_to_weapon(weapon_name, should_play_sound)
    if not weapon_name or weapon_name == "" or not is_weapon_allowed(weapon_name) then
        return false
    end

    local active_name = get_current_weapon_state_name()
    if should_play_sound then
        if weapon_name == "revolver" then
            play_audio("assets/soundEffects/weapons/swap_revolver.wav", 0, 10)
        elseif weapon_name == "shotgun" then
            play_audio("assets/soundEffects/weapons/swap_shotgun.wav", 0, 10)
        end
    end

    local weapon_list = GameState and GameState.weapons or weapons
    local weapon_index = 1
    for i = 1, #weapon_list do
        if weapon_list[i] == weapon_name then
            weapon_index = i
            break
        end
    end

    current_weapon_slot = weapon_index
    if GameState then
        GameState.current_weapon_slot = current_weapon_slot
    end

    spawn_back_weapon(active_name)
    set_current_weapon(weapon_name)
    save_weapon_inventory_state()
    return true
end

local function switch_weapon()
    if not GameState or not GameState.weapons then
        return
    end

    local weapon_list = GameState.weapons
    local list_size = #weapon_list

    if list_size < 1 then
        return
    end

    local active_name = get_current_weapon_state_name()
    local active_index = 1
    if active_name and active_name ~= "" then
        for i = 1, list_size do
            if weapon_list[i] == active_name then
                active_index = i
                break
            end
        end
    end

    local next_index = active_index + 1
    if next_index > list_size then
        next_index = 1
    end

    local next_weapon_name = weapon_list[next_index]
    if next_weapon_name == active_name then
        return
    end

    switch_to_weapon(next_weapon_name, true)
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

function clone_table(source)
    local copy = {}
    if type(source) == "table" then
        for i = 1, #source do
            copy[i] = source[i]
        end
    end
    return copy
end

function get_scene_state_key()
    if GameState and GameState.current_scene_name and GameState.current_scene_name ~= "" then
        return GameState.current_scene_name
    end

    return "default"
end

function make_empty_state_list(count, default_value)
    local values = {}
    for i = 1, count do
        values[i] = default_value
    end
    return values
end

function snapshot_scene_state()
    if not GameState then
        return
    end

    local scene_key = get_scene_state_key()
    if GameState.scene_state_snapshots == nil then
        GameState.scene_state_snapshots = {}
    end

    if GameState.scene_state_snapshots[scene_key] == nil then
        GameState.scene_state_snapshots[scene_key] = {}
    end

    local snapshot = GameState.scene_state_snapshots[scene_key]
    snapshot.player_health = current_health
    snapshot.player_max_health = max_health
    snapshot.player_revolver_ammo = GameState.player_revolver_ammo or GameState.player_ammo or 0
    snapshot.player_ammo = snapshot.player_revolver_ammo
    snapshot.player_shotgun_ammo = GameState.player_shotgun_ammo or 0
    snapshot.player_max_revolver_ammo = GameState.player_max_revolver_ammo or GameState.player_max_ammo or 12
    snapshot.player_max_shotgun_ammo = GameState.player_max_shotgun_ammo or 6
    if type(GameState.revolver_cylinder) == "table" and #GameState.revolver_cylinder > 0 then
        snapshot.revolver_cylinder = clone_table(GameState.revolver_cylinder)
    else
        snapshot.revolver_cylinder = make_empty_state_list(6, "empty")
    end
    if type(GameState.shotgun_barrel) == "table" and #GameState.shotgun_barrel > 0 then
        snapshot.shotgun_barrel = clone_table(GameState.shotgun_barrel)
    else
        snapshot.shotgun_barrel = make_empty_state_list(2, "empty")
    end
    snapshot.current_weapon_name = current_weapon_name or GameState.current_weapon_name or ""
    snapshot.current_back_weapon_name = current_back_weapon_name or GameState.current_back_weapon_name or ""
    snapshot.current_weapon_slot = current_weapon_slot or GameState.current_weapon_slot or 0
end

function restore_scene_start_state()
    if not GameState then
        return
    end

    local scene_key = get_scene_state_key()
    if GameState.scene_state_snapshots == nil then
        GameState.scene_state_snapshots = {}
    end

    local snapshot = GameState.scene_state_snapshots[scene_key]
    if snapshot == nil then
        -- First time entering this scene: restore persisted health before creating snapshot
        if GameState.player_health ~= nil then
            current_health = tonumber(GameState.player_health) or current_health
        end
        if GameState.player_max_health ~= nil then
            max_health = tonumber(GameState.player_max_health) or max_health
        end
        snapshot_scene_state()
        return
    end

    current_health = tonumber(snapshot.player_health) or current_health
    max_health = tonumber(snapshot.player_max_health) or max_health

    GameState.player_health = current_health
    GameState.player_max_health = max_health
    GameState.player_revolver_ammo = tonumber(snapshot.player_revolver_ammo) or tonumber(snapshot.player_ammo) or GameState.player_revolver_ammo or 0
    GameState.player_ammo = GameState.player_revolver_ammo
    GameState.player_shotgun_ammo = tonumber(snapshot.player_shotgun_ammo) or GameState.player_shotgun_ammo or 0
    GameState.player_max_revolver_ammo = tonumber(snapshot.player_max_revolver_ammo) or GameState.player_max_revolver_ammo or 12
    GameState.player_max_shotgun_ammo = tonumber(snapshot.player_max_shotgun_ammo) or GameState.player_max_shotgun_ammo or 6

    if snapshot.revolver_cylinder ~= nil then
        GameState.revolver_cylinder = clone_table(snapshot.revolver_cylinder)
        if #GameState.revolver_cylinder < 6 then
            local values = make_empty_state_list(6, "empty")
            for i = 1, math.min(#GameState.revolver_cylinder, 6) do
                values[i] = GameState.revolver_cylinder[i]
            end
            GameState.revolver_cylinder = values
        end
    end

    if snapshot.shotgun_barrel ~= nil then
        GameState.shotgun_barrel = clone_table(snapshot.shotgun_barrel)
        if #GameState.shotgun_barrel < 2 then
            local values = make_empty_state_list(2, "empty")
            for i = 1, math.min(#GameState.shotgun_barrel, 2) do
                values[i] = GameState.shotgun_barrel[i]
            end
            GameState.shotgun_barrel = values
        end
    end

    if snapshot.current_weapon_name ~= nil then
        GameState.current_weapon_name = snapshot.current_weapon_name
    end

    if snapshot.current_back_weapon_name ~= nil then
        GameState.current_back_weapon_name = snapshot.current_back_weapon_name
    end

    if snapshot.current_weapon_slot ~= nil then
        GameState.current_weapon_slot = snapshot.current_weapon_slot
    end
end

function save_player_state()
    if GameState then
        GameState.player_health = current_health
        GameState.player_max_health = max_health
    end
end

function load_player_state()
    if not GameState then
        return
    end

    if GameState.player_health ~= nil then
        current_health = tonumber(GameState.player_health) or current_health
    end

    if GameState.player_max_health ~= nil then
        max_health = tonumber(GameState.player_max_health) or max_health
    end

    GameState.player_health = current_health
    GameState.player_max_health = max_health
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
    local tag = get_tag(other)

    if tag == "t_ammo_pickup" or tag == "t_shell_pickup" then -- AMMO
        if GameState then
            local ammo_amount = nil
            if tag == "t_shell_pickup" then
                ammo_amount = get_script_variable(other, "default_shells") or 2
            else
                ammo_amount = get_script_variable(other, "default_revolver_ammo") or 2
            end
            add_weapon_ammo(other, ammo_amount)
        end

        delete_entity(other)
    end

    if tag == "t_health_pickup" then -- HEALTH
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

    if tag == "t_revolver_pickup" or tag == "t_shotgun_pickup" then -- WEAPONS
        local picked_weapon = tag == "t_revolver_pickup" and "revolver" or "shotgun"
        add_weapon_to_inventory(picked_weapon)
        switch_to_weapon(picked_weapon, true)

        if picked_weapon == "revolver" then
            call_function(other, "spawn_text", "Grabbed Revolver!")
        else
            call_function(other, "spawn_text", "Grabbed Shotgun!")
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

local function restart_current_scene()
    local target_scene = ""

    if GameState then
        target_scene = GameState.current_scene_name or GameState.last_scene_name or ""
    end

    if target_scene and target_scene ~= "" then
        go_to_scene(target_scene, true)
    else
        go_to_scene("sewers_01", true)
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

        if is_action_just_pressed("restart_scene") then
            restart_current_scene()
            return
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
    restore_scene_start_state()

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

    load_weapon_inventory_state()
    load_weapon_list()

    -- Determine desired starting weapon
    local desired_weapon = nil
    if GameState and GameState.current_weapon_name and GameState.current_weapon_name ~= "" and is_weapon_allowed(GameState.current_weapon_name) then
        desired_weapon = GameState.current_weapon_name
    elseif GameState and GameState.current_weapon_slot and type(GameState.current_weapon_slot) == "number" and GameState.current_weapon_slot >= 1 then
        desired_weapon = get_weapon_name_by_slot(GameState.current_weapon_slot)
    elseif GameState and GameState.weapons and type(GameState.weapons) == "table" and #GameState.weapons >= 1 then
        desired_weapon = GameState.weapons[1]
    elseif #weapons >= 1 then
        desired_weapon = weapons[1]
    end

    if desired_weapon == nil or desired_weapon == "" then
        current_weapon = nil
        current_weapon_name = nil
        if GameState then
            GameState.current_weapon_name = ""
        end
    elseif is_weapon_allowed(desired_weapon) then
        set_current_weapon(desired_weapon)

        if GameState and GameState.weapons then
            for i = 1, #GameState.weapons do
                if GameState.weapons[i] == current_weapon_name then
                    current_weapon_slot = i
                    GameState.current_weapon_slot = i
                    break
                end
            end
        end
    elseif GameState and GameState.weapons and type(GameState.weapons) == "table" and #GameState.weapons >= 1 then
        set_current_weapon(GameState.weapons[1])
    elseif #weapons >= 1 then
        set_current_weapon(weapons[1])
    end

    if current_weapon and not_dead then
        toggle_mouse_follow(current_weapon, true)
    end

    if current_weapon and current_weapon_name and current_weapon_name ~= "" then
        local desired_back_weapon = get_current_back_weapon_state_name()
        if desired_back_weapon and desired_back_weapon ~= "" and is_weapon_allowed(desired_back_weapon) then
            spawn_back_weapon(desired_back_weapon)
        end
    end

    save_weapon_inventory_state()
    transition_to("idle")
end

if GameState then
    GameState.save_player_inventory_state = save_weapon_inventory_state
    GameState.load_player_inventory_state = load_weapon_inventory_state
    GameState.save_player_state = save_player_state
    GameState.load_player_state = load_player_state
    GameState.restore_scene_start_state = restore_scene_start_state
    GameState.snapshot_scene_state = snapshot_scene_state
end