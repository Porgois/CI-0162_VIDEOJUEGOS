-- Possible entity spawns
local ammo_pickup_entity = dofile("./assets/scripts/entities/e_ammo_pickup.lua")
local shell_pickup_entity = dofile("./assets/scripts/entities/e_shell_pickup.lua")
local health_pickup_entity = dofile("./assets/scripts/entities/e_health_pickup.lua")

-- Variables
local current_health = 2
local is_dead = false
local animation_timer = 0
local damage_anim_duration = 0.25

-- Drop chance
local drop_chance = 1.0         -- Chance to drop something
local on_destroyed_triggered = false

-- Smart drop system thresholds
local health_threshold = 2
local ammo_threshold = 2

-- State machine
local state = "idle"
local states = {}

local function set_anim(name)
    if state ~= "destroyed" and animation_timer > 0 then
        return
    end
    play_animation(this, name)
end

function transition_to(new_state)
    if states[new_state] then
        state = new_state
        states[new_state].enter()
    end
end

function get_best_drop()
    -- Hierarchy: health > revolver_ammo > shotgun_ammo
    -- Returns which resource type is most needed
    
    local health = GameState.player_health or 100
    local revolver_ammo = GameState.player_ammo or 0
    local shotgun_ammo = GameState.player_shotgun_ammo or 0
    
    if health <= health_threshold then
        return "health"
    elseif revolver_ammo <= ammo_threshold then
        return "revolver_ammo"
    else
        return "shotgun_ammo"
    end
end

function random_drop()
    print("[DESTRUCTABLE SCRIPT] ROLLED FOR DROP!")

    if math.random() > drop_chance then
        print("[DESTRUCTABLE SCRIPT] NO DROP!")
        return
    end

    local drop_type = get_best_drop()
    local picked_entity
    
    if drop_type == "health" then
        print("[DESTRUCTABLE SCRIPT] DROPPED HEALTH!")
        picked_entity = spawn_entity(health_pickup_entity)
    elseif drop_type == "revolver_ammo" then
        print("[DESTRUCTABLE SCRIPT] DROPPED REVOLVER AMMO!")
        picked_entity = spawn_entity(ammo_pickup_entity)
    else  -- shotgun_ammo
        print("[DESTRUCTABLE SCRIPT] DROPPED SHOTGUN AMMO!")
        picked_entity = spawn_entity(shell_pickup_entity)
    end

    local x_pos, y_pos = get_position(this)
    x_pos = x_pos + 10
    y_pos = y_pos + 25
    set_position(picked_entity, x_pos, y_pos)
end

function take_damage(amount)
    if state == "destroyed" then return end
    set_velocity(this, 0, 0)
    current_health = current_health - amount
    
    if current_health <= 0 then
        transition_to("destroyed")
    else
        play_audio("assets/soundEffects/misc/hits/hit_metal.wav", 0, 15)
        animation_timer = damage_anim_duration
        play_animation(this, "damage")
    end
end

states["idle"] = {
    enter = function()
    end,
    update = function()
    end
}

states["destroyed"] = {
    enter = function()
        set_sprite_z_index(this, 8)
        set_velocity(this, 0, 0)
        play_audio("assets/soundEffects/misc/breaks/metal_break.wav", 0, 25)
        play_animation(this, "destroy")
        if not on_destroyed_triggered then
            on_destroyed_triggered = true
            remove_box_collider(this)

            if math.random() <= drop_chance then
                random_drop()
            end
        end
    end,
    update = function()
        -- do nothing
    end
}

function update()
    if state ~= "destroyed" and animation_timer > 0 then
        animation_timer = animation_timer - get_delta_time()
    end
    
    states[state].update()
end

function start()
    transition_to("idle")
end
