-- Possible entity spawns
local ammo_pickup_entity = dofile("./assets/scripts/entities/e_ammo_pickup.lua")
local health_pickup_entity = dofile("./assets/scripts/entities/e_health_pickup.lua")

-- General values
local speed = 35
local current_health = 3
local is_dead = false
local attack_damage = 1
local attack_range = 20
local effective_damage_range = 30
local detection_range = 80
local patrol_range = 30
local attack_delay = 0.5
local attack_delay_timer = 0
local attack_hit = false

-- Animation
local animation_timer = 0
local damage_anim_duration = 0.25
local attack_duration = 0.5
local attack_timer = 0

-- Patrol
local origin_x, origin_y = 0, 0
local patrol_target_x, patrol_target_y = 0, 0
local patrol_wait = 0
local patrol_wait_duration = 2.0

-- Drop chance
local drop_chance = 1.0         -- 75% chance to drop anything
local ammo_chance = 0.2        -- 20% chance for ammo, 80% for health
local on_death_triggered = false

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

local function flip_towards(dx)
    set_flip(this, dx < 0)
end

-- State machine
local state = "idle"
local states = {}

states["idle"] = {
    enter = function()
        set_velocity(this, 0, 0)
    end,
    update = function()
        play_animation(this, "idle")
        if can_detect_player() then
            transition_to("pursue")
            return
        end
        transition_to("patrol")
    end
}

states["patrol"] = {
    enter = function()
        patrol_wait = patrol_wait_duration
        pick_patrol_target()
    end,
    update = function()
        if can_detect_player() then
            transition_to("pursue")
            return
        end

        if patrol_wait > 0 then
            patrol_wait = patrol_wait - get_delta_time()
            play_animation(this, "idle")
            return
        end

        local my_x, my_y = get_position(this)
        local dx = patrol_target_x - my_x
        local dy = patrol_target_y - my_y
        local dist = math.sqrt(dx * dx + dy * dy)

        if dist < 4 then
            set_velocity(this, 0, 0)
            patrol_wait = patrol_wait_duration
            pick_patrol_target()
            return
        end

        handle_footsteps()
        set_velocity(this, (dx / dist) * speed, (dy / dist) * speed)
        flip_towards(dx)
        play_animation(this, "walk")
    end
}

states["pursue"] = {
    enter = function() end,
    update = function()
        if not has_entity("player") then
            transition_to("patrol")
            return
        end
        local dist = distance_to_player()
        if dist > detection_range then
            transition_to("patrol")
            return
        end
        if dist <= attack_range then
            transition_to("attack")
            return
        end
        pursue_player()
        play_animation(this, "walk")
    end
}

states["attack"] = {
    enter = function()
        set_velocity(this, 0, 0)
        play_audio("assets/soundEffects/enemies/attacks/bite_02.wav")
        play_animation(this, "attack")
        attack_timer = attack_duration
        attack_delay_timer = attack_delay
        attack_hit = false
    end,
    update = function()
        attack_timer = attack_timer - get_delta_time()

        if not attack_hit then
            attack_delay_timer = attack_delay_timer - get_delta_time()
            if attack_delay_timer <= 0 then
                attack()
                attack_hit = true
            end
        end

        if attack_timer <= 0 then
            if not has_entity("player") then
                transition_to("patrol")
                return
            end
            local dist = distance_to_player()
            if dist <= attack_range then
                transition_to("attack")
            else
                transition_to("pursue")
            end
        end
    end
}

states["dead"] = {
    enter = function()
        set_velocity(this, 0, 0)
        play_animation(this, "death")
        play_audio("assets/soundEffects/enemies/damaged/death.wav", 0)
        if not on_death_triggered then
            on_death_triggered = true
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

function handle_footsteps()
    footstep_timer = footstep_timer - get_delta_time()
    if footstep_timer <= 0 then
        play_random_audio(footstep_sounds, 0, 3)
        footstep_timer = footstep_interval
    end
end

function transition_to(new_state)
    if states[new_state] then
        state = new_state
        states[new_state].enter()
    end
end

function can_detect_player()
    if not has_entity("player") then return false end
    return distance_to_player() <= detection_range
end

function pick_patrol_target()
    patrol_target_x = origin_x + (math.random() * 2 - 1) * patrol_range
    patrol_target_y = origin_y + (math.random() * 2 - 1) * patrol_range
end

function attack()
    print("[LUA] ENEMY ATTEMPTS ATTACK!")
    if distance_to_player() <= effective_damage_range then
        local player = find_entity("player")
        call_function(player, "take_damage", attack_damage)
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

function distance_to_player()
    local player = find_entity("player")
    local my_x, my_y = get_position(this)
    local player_x, player_y = get_position(player)
    local dx = player_x - my_x
    local dy = player_y - my_y
    return math.sqrt(dx * dx + dy * dy)
end

function pursue_player()
    local player = find_entity("player")

    local my_x, my_y = get_position(this)
    local player_x, player_y = get_position(player)
    local dx = player_x - my_x
    local dy = player_y - my_y
    local dist = math.sqrt(dx * dx + dy * dy)

    if dist == 0 then 
        return 
    end
    
    handle_footsteps()
    set_velocity(this, (dx / dist) * speed, (dy / dist) * speed)
    flip_towards(dx)
end

function take_damage(amount)
    if state == "dead" then return end
    set_velocity(this, 0, 0)
    
    current_health = current_health - amount
    play_audio("assets/soundEffects/misc/hits/hit_flesh.wav", 0, 30)

    if current_health <= 0 then
        transition_to("dead")
    else
        play_audio("assets/soundEffects/enemies/damaged/hit.wav", 0, 110)
        animation_timer = damage_anim_duration
    end
end

function update()
    if animation_timer > 0 then
        animation_timer = animation_timer - get_delta_time()
        play_animation(this, "damage")
        return
    end
    states[state].update()
end

function start()
    origin_x, origin_y = get_position(this)
    pick_patrol_target()
    transition_to("patrol")
end