-- Projectile
local projectile_entity = dofile("./assets/scripts/entities/e_projectile.lua")
local projectile_speed = 350.0
local can_shoot = true

-- Ammo
local max_ammo = 6
local current_ammo = 6

function start()
    if GameState and GameState.player_ammo ~= nil then
        current_ammo = GameState.player_ammo
    else
        if GameState then GameState.player_ammo = current_ammo end
    end

    if GameState and GameState.player_max_ammo ~= nil then
        max_ammo = GameState.player_max_ammo
    else
        if GameState then GameState.player_max_ammo = max_ammo end
    end
end

-- Cooldown
local shoot_cooldown = 0.3
local cooldown_timer = 0.0

-- Offset
local barrel_offset_x = 6.0

function get_aim_angle()
    local x_pos, y_pos = get_position(this)
    local mouse_x, mouse_y = get_mouse_world_position()
    return math.atan2(mouse_y - y_pos, mouse_x - x_pos)
end

function shoot_projectile()
    if not can_shoot then
        print("[REVOLVER] On cooldown!")
        return
    end

    if GameState and GameState.reload_menu_open then
        return
    end

    if GameState == nil or GameState.spend_casing == nil or not GameState.spend_casing() then
        play_audio("assets/soundEffects/weapons/shoot/dry_fire.wav", 0, 64)
        return
    end

    local projectile = spawn_entity(projectile_entity)
    local x_pos, y_pos = get_pivoted_position(this)
    local angle = get_aim_angle()

    local spawn_x = x_pos + math.cos(angle) * barrel_offset_x
    local spawn_y = y_pos + math.sin(angle) * barrel_offset_x

    set_position(projectile, spawn_x, spawn_y)
    set_rotation(projectile, math.deg(angle))
    set_velocity(projectile, math.cos(angle) * projectile_speed, math.sin(angle) * projectile_speed)
    play_audio("assets/soundEffects/weapons/shoot/shoot.wav", 0, 24)
    shake_camera(0.4, 3.0, 30.0)

    can_shoot = false
    cooldown_timer = shoot_cooldown
    current_ammo = current_ammo - 1
    if GameState then GameState.player_ammo = current_ammo end
end

function update()
    if not can_shoot then
        cooldown_timer = cooldown_timer - get_delta_time()
        if cooldown_timer <= 0.0 then
            can_shoot = true
        end
    end

    if is_button_just_pressed("lmb") then
        shoot_projectile()
    end
end