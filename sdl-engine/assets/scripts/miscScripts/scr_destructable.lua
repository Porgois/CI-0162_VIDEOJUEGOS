-- Possible entity spawns
local ammo_pickup_entity = dofile("./assets/scripts/entities/e_ammo_pickup.lua")
local health_pickup_entity = dofile("./assets/scripts/entities/e_health_pickup.lua")

-- Variables
local current_health = 2
local is_dead = false
local animation_timer = 0
local damage_anim_duration = 0.25

-- Drop chance
local drop_chance = 1.0         -- 75% chance to drop anything
local ammo_chance = 0.2        -- 20% chance for ammo, 80% for health
local on_destroyed_triggered = false

-- State machine
local state = "idle"
local states = {}

function transition_to(new_state)
    if states[new_state] then
        state = new_state
        states[new_state].enter()
    end
end

function random_drop()
    print("[DESTRUCTABLE SCRIPT] ROLLED FOR DROP!")

    if math.random() > drop_chance then
        print("[DESTRUCTABLE SCRIPT] NO DROP!")
        return
    end

    local picked_entity
    if math.random() <= ammo_chance then
        print("[DESTRUCTABLE SCRIPT] DROPPED AMMO!")
        picked_entity = spawn_entity(ammo_pickup_entity)
    else
        print("[DESTRUCTABLE SCRIPT] DROPPED HEALTH!")
        picked_entity = spawn_entity(health_pickup_entity)
    end

    local x_pos, y_pos = get_position(this)
    x_pos = x_pos + 10
    y_pos = y_pos + 25
    set_position(picked_entity, x_pos, y_pos)
end

function take_damage(amount)
    if state == "destroyed" then return end
    current_health = current_health - amount
    
    if current_health <= 0 then
        transition_to("destroyed")
    else
        play_audio("assets/soundEffects/misc/hits/hit_metal.wav", 0, 15)
        animation_timer = damage_anim_duration
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
    if animation_timer > 0 then
        animation_timer = animation_timer - get_delta_time()
        play_animation(this, "damage")
        return
    end
    states[state].update()
end

function start()
    transition_to("idle")
end
