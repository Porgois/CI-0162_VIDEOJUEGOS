-- PSYCHIC ---

local ammo_pickup_entity = dofile("./assets/scripts/entities/e_ammo_pickup.lua")
local health_pickup_entity = dofile("./assets/scripts/entities/e_health_pickup.lua")

-- General
local speed = 55
local current_health = 2
local detection_range = 140
local flee_range = 30
local has_been_hit = false
local has_detected_player = false

-- Psychic attack
local psychic_range = 140
local sing_min_duration = 3.0
local sing_max_duration = 5.0
local sing_timer = 0

-- Post-psychic cooldown
local post_psychic_min_wait = 0.15
local post_psychic_max_wait = 0.35
local post_psychic_wait_timer = 0
local post_psychic_target_x, post_psychic_target_y = 0, 0
local post_psychic_move_timeout = 0
local psychic_intro_delay = 0.05
local psychic_intro_timer = 0

-- Patrol
local patrol_range = 60
local patrol_wait_duration = 2.0
local patrol_wait = 0
local patrol_target_x, patrol_target_y = 0, 0
local patrol_move_timeout = 0
local origin_x, origin_y = 0, 0

-- Move timeout
local timeout_buffer = 1.5

-- Animation
local animation_timer = 0
local damage_anim_duration = 0.25
local alert_finished = false

-- Drops
local drop_chance = 1.0
local ammo_chance = 0.2
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
    "assets/soundEffects/footsteps/stone/sndFootShoRock6.wav",
}

-- ============================================================
-- Helpers
-- ============================================================

local function flip_towards(dx)
    set_flip(this, dx < 0)
end

local function calculate_move_timeout(tx, ty)
    local my_x, my_y = get_position(this)
    local dx = tx - my_x
    local dy = ty - my_y
    local dist = math.sqrt(dx * dx + dy * dy)
    return (dist / speed) * timeout_buffer
end

-- State machine (declared early so the helpers below can reference `state`)
local state = "idle"
local states = {}

-- Routes state-driven animation calls through here. While the damage flash
-- is active, state animations (idle/walk/sing/etc.) are suppressed so they
-- don't keep interrupting/resetting "damage" back to frame 0 every tick.
local function set_anim(name)
    if state ~= "dead" and animation_timer > 0 then
        return
    end
    play_animation(this, name)
end

-- Routes state-driven movement through here. While the damage flash is
-- active, velocity is held at zero so the enemy visibly stops on hit,
-- instead of the next frame's state update immediately overwriting the
-- zero-out set inside take_damage().
local function set_move_velocity(vx, vy)
    if state ~= "dead" and animation_timer > 0 then
        set_velocity(this, 0, 0)
        return
    end
    set_velocity(this, vx, vy)
end

function handle_footsteps()
    if state ~= "dead" and animation_timer > 0 then
        return
    end
    footstep_timer = footstep_timer - get_delta_time()
    if footstep_timer <= 0 then
        play_random_audio(footstep_sounds, 0, 3)
        footstep_timer = footstep_interval
    end
end

function distance_to_player()
    local player = find_entity("player")
    local my_x, my_y = get_position(this)
    local px, py = get_position(player)
    local dx, dy = px - my_x, py - my_y
    return math.sqrt(dx * dx + dy * dy)
end

function can_detect_player()
    if not has_entity("player") then return false end
    return has_been_hit or distance_to_player() <= detection_range
end

function pick_patrol_target()
    patrol_target_x = origin_x + (math.random() * 2 - 1) * patrol_range
    patrol_target_y = origin_y + (math.random() * 2 - 1) * patrol_range
end

function toggle_player_flashlight(disabled)
    local player = find_entity("player")
    if player then
        call_function(player, "set_flashlight_disabled", disabled)
    end
    -- flashlight audio
    if disabled then
        play_audio("assets/soundEffects/misc/flashlight/flashlight_off.wav")
    else
        play_audio("assets/soundEffects/misc/flashlight/flashlight_on.wav")
    end
end

function pursue_player()
    local player = find_entity("player")
    local my_x, my_y = get_position(this)
    local px, py = get_position(player)
    local dx, dy = px - my_x, py - my_y
    local dist = math.sqrt(dx * dx + dy * dy)

    if dist == 0 then 
        return 
    end

    handle_footsteps()
    set_move_velocity((dx / dist) * speed, (dy / dist) * speed)
    flip_towards(dx)
end

function flee_from_player()
    local player = find_entity("player")
    if not player then 
        return 
    end

    local my_x, my_y = get_position(this)
    local px, py = get_position(player)
    local dx, dy = my_x - px, my_y - py
    local dist = math.sqrt(dx * dx + dy * dy)

    if dist == 0 then 
        return
    end

    handle_footsteps()
    set_move_velocity((dx / dist) * speed, (dy / dist) * speed)
    flip_towards(-dx)
end

function random_drop()
    if math.random() > drop_chance then return end
    local picked_entity
    if math.random() <= ammo_chance then
        picked_entity = spawn_entity(ammo_pickup_entity)
    else
        picked_entity = spawn_entity(health_pickup_entity)
    end
    local x, y = get_position(this)
    set_position(picked_entity, x + 10, y + 25)
end

-- ============================================================
-- States
-- ============================================================

function transition_to(new_state)
    if states[new_state] then
        state = new_state
        states[new_state].enter()
    end
end

states["idle"] = {
    enter = function()
        set_velocity(this, 0, 0)
    end,
    update = function()
        set_anim("idle")
        if can_detect_player() then
            transition_to("pursue")
        else
            transition_to("patrol")
        end
    end,
}

states["patrol"] = {
    enter = function()
        has_detected_player = false
        set_velocity(this, 0, 0)
        patrol_wait = patrol_wait_duration
        pick_patrol_target()
        patrol_move_timeout = calculate_move_timeout(patrol_target_x, patrol_target_y)
    end,
    update = function()
        if can_detect_player() then
            transition_to("pursue")
            return
        end

        if patrol_wait > 0 then
            patrol_wait = patrol_wait - get_delta_time()
            set_anim("idle")
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
            patrol_move_timeout = calculate_move_timeout(patrol_target_x, patrol_target_y)
            return
        end

        patrol_move_timeout = patrol_move_timeout - get_delta_time()
        if patrol_move_timeout <= 0 then
            pick_patrol_target()
            patrol_move_timeout = calculate_move_timeout(patrol_target_x, patrol_target_y)
            return
        end

        handle_footsteps()
        set_move_velocity((dx / dist) * speed, (dy / dist) * speed)
        flip_towards(dx)
        set_anim("walk")
    end,
}

states["pursue"] = {
    enter = function() end,
    update = function()
        if not has_entity("player") then
            transition_to("patrol")
            return
        end

        local dist = distance_to_player()

        if dist <= psychic_range then
            transition_to("psychic")
        elseif dist > flee_range then
            transition_to("patrol")
        else
            flee_from_player()
            set_anim("walk")
        end
    end
}

states["psychic"] = {
    enter = function()
        set_velocity(this, 0, 0)
        psychic_intro_timer = psychic_intro_delay

        local player = find_entity("player")
        local my_x, my_y = get_position(this)
        local px, py = get_position(player)
        local dx, dy = px - my_x, py - my_y

        flip_towards(dx)

        if not has_detected_player then
            play_animation(this, "alert")
            play_audio("assets/soundEffects/enemies/other/alert.wav", 0)
            alert_finished = false
        else
            alert_finished = true  -- skip the wait if already seen player
        end

        has_detected_player = true
        sing_timer = sing_min_duration + math.random() * (sing_max_duration - sing_min_duration)
    end,
    update = function()
        if not has_entity("player") then
            transition_to("patrol")
            return
        end

        if not alert_finished then
            if not is_animation_finished(this) then
                return
            end
            alert_finished = true
        end

        if psychic_intro_timer > 0 then
            psychic_intro_timer = psychic_intro_timer - get_delta_time()
            set_anim("idle")
            return
        end

        -- play sing sound and enable flashlight effect
        if psychic_intro_timer ~= -1 then
            play_audio("assets/soundEffects/enemies/other/sing.wav", 0)
            toggle_player_flashlight(true)
            psychic_intro_timer = -1
        end

        if sing_timer > 0 then
            sing_timer = sing_timer - get_delta_time()
            set_anim("sing")
            return
        end

        toggle_player_flashlight(false)
        post_psychic_wait_timer = post_psychic_min_wait + math.random() * (post_psychic_max_wait - post_psychic_min_wait)
        post_psychic_target_x = origin_x + (math.random() * 2 - 1) * patrol_range
        post_psychic_target_y = origin_y + (math.random() * 2 - 1) * patrol_range
        transition_to("post_psychic")
    end
}

states["post_psychic"] = {
    enter = function()
        post_psychic_move_timeout = calculate_move_timeout(post_psychic_target_x, post_psychic_target_y)
        local my_x, my_y = get_position(this)
        local dx = post_psychic_target_x - my_x
        local dy = post_psychic_target_y - my_y
        local dist = math.sqrt(dx * dx + dy * dy)
        if dist > 0 then
            set_velocity(this, (dx / dist) * speed, (dy / dist) * speed)
            flip_towards(dx)
        else
            set_velocity(this, 0, 0)
        end
    end,
    update = function()
        if not has_entity("player") then
            transition_to("patrol")
            return
        end

        local my_x, my_y = get_position(this)
        local dx = post_psychic_target_x - my_x
        local dy = post_psychic_target_y - my_y
        local dist = math.sqrt(dx * dx + dy * dy)

        if dist > 4 then
            post_psychic_move_timeout = post_psychic_move_timeout - get_delta_time()
            if post_psychic_move_timeout <= 0 then
                post_psychic_target_x = origin_x + (math.random() * 2 - 1) * patrol_range
                post_psychic_target_y = origin_y + (math.random() * 2 - 1) * patrol_range
                post_psychic_move_timeout = calculate_move_timeout(post_psychic_target_x, post_psychic_target_y)
                return
            end
            handle_footsteps()
            set_move_velocity((dx / dist) * speed, (dy / dist) * speed)
            flip_towards(dx)
            set_anim("walk")
            return
        end

        set_velocity(this, 0, 0)

        if post_psychic_wait_timer > 0 then
            post_psychic_wait_timer = post_psychic_wait_timer - get_delta_time()
            set_anim("idle")
            return
        end

        transition_to("pursue")
    end,
}

states["dead"] = {
    enter = function()
        set_sprite_z_index(this, 8)
        set_velocity(this, 0, 0)
        play_animation(this, "death")
        play_audio("assets/soundEffects/enemies/damaged/psychic_death.wav", 0)

        if not on_death_triggered then
            on_death_triggered = true
            remove_box_collider(this)
            toggle_player_flashlight(false)

            if math.random() <= drop_chance then
                random_drop()
            end
        end
    end,
    update = function() end,
}

-- ============================================================
-- Core callbacks
-- ============================================================

function take_damage(amount)
    if state == "dead" then return end

    set_velocity(this, 0, 0)
    current_health = current_health - amount
    play_audio("assets/soundEffects/misc/hits/hit_flesh.wav", 0, 30)
    has_been_hit = true

    if current_health <= 0 then
        transition_to("dead")
    else
        play_audio("assets/soundEffects/enemies/damaged/psychic_pain.wav", 0, 110)
        animation_timer = damage_anim_duration
        play_animation(this, "damage")
    end
end

function update()
    if state ~= "dead" and animation_timer > 0 then
        animation_timer = animation_timer - get_delta_time()
    end

    states[state].update()
end

function start()
    origin_x, origin_y = get_position(this)
    pick_patrol_target()
    transition_to("patrol")
end