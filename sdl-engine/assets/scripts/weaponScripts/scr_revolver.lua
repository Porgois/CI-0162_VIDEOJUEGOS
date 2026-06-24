-- Projectile
local projectile_entity = dofile("./assets/scripts/entities/e_projectile.lua")
local projectile_speed = 350.0
local can_shoot = true

-- Ammo
local max_ammo = 6

-- Cooldown
local shoot_cooldown = 1.0
local cooldown_timer = 0.0

-- Offset
local barrel_offset_x = 6.0

function start()
    if GameState then
        if GameState.player_ammo == nil then
            GameState.player_ammo = max_ammo
        end

        if GameState.player_max_ammo ~= nil then
            max_ammo = GameState.player_max_ammo
        else
            GameState.player_max_ammo = max_ammo
        end
    end
end

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

function save_player_state()
    if GameState then
        if GameState.player_ammo == nil then
            GameState.player_ammo = max_ammo
        end
        print("[REVOLVER] save_player_state: GameState.player_ammo=" .. tostring(GameState.player_ammo))
    end
end

function load_player_state()
    if GameState then
        if GameState.player_ammo == nil then
            GameState.player_ammo = max_ammo
            print("[REVOLVER] load_player_state: initialized player_ammo=" .. tostring(GameState.player_ammo))
        else
            print("[REVOLVER] load_player_state: restored player_ammo=" .. tostring(GameState.player_ammo))
        end
    end
end

if GameState ~= nil then
    GameState.save_player_state = save_player_state
    GameState.load_player_state = load_player_state
end