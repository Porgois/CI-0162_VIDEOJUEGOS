-- Projectile
local projectile_entity = dofile("./assets/scripts/entities/e_projectile.lua")
local projectile_speed = 350.0
local can_shoot = true

-- Ammo
local max_ammo = 6

-- Cooldown
local shoot_cooldown = 0.25
local cooldown_timer = 0.0

-- Offset
local barrel_offset_x = 6.0

-- Pushback (recoil)
local pushback_amount = 4.0
local pushback_recovery_speed = 40.0
local recovering = false
local rest_x = 0.0
local rest_y = 0.0

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

function apply_pushback(angle)
    if not recovering then
        rest_x, rest_y = get_child_of_offset(this)
    end

    local push_x = -math.cos(angle) * pushback_amount
    local push_y = -math.sin(angle) * pushback_amount

    local current_offset_x, current_offset_y = get_child_of_offset(this)
    set_child_of_offset(this, current_offset_x + push_x, current_offset_y + push_y)
    recovering = true
end

function update_pushback_recovery()
    if not recovering then
        return
    end

    local current_offset_x, current_offset_y = get_child_of_offset(this)
    local dx = rest_x - current_offset_x
    local dy = rest_y - current_offset_y
    local dist = math.sqrt(dx * dx + dy * dy)

    if dist < 1.0 then
        set_child_of_offset(this, rest_x, rest_y)
        recovering = false
        return
    end

    local step = pushback_recovery_speed * get_delta_time()
    if step >= dist then
        set_child_of_offset(this, rest_x, rest_y)
        recovering = false
    else
        local nx, ny = dx / dist, dy / dist
        set_child_of_offset(this, current_offset_x + nx * step, current_offset_y + ny * step)
    end
end

function shoot_projectile()
    if not can_shoot then
        print("[REVOLVER] On cooldown!")
        return
    end

    if GameState and GameState.reload_menu_open then
        return
    end

    local spend = GameState and (GameState.spend_revolver_casing or GameState.spend_casing)
    if GameState == nil or spend == nil or not spend() then
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

    apply_pushback(get_aim_angle())
    play_audio("assets/soundEffects/weapons/shoot/shoot.wav", 0, 24)
    shake_camera(0.4, 3.0, 30.0)

    can_shoot = false
    cooldown_timer = shoot_cooldown
end

function update()
    if GameState and GameState.set_revolver_reload_menu and is_button_just_pressed("rmb") then
        local opening = not GameState.revolver_reload_menu_open
        GameState.set_revolver_reload_menu(opening)

        if opening then
            play_audio("assets/soundEffects/misc/flashlight/flashlight_on.wav")
            play_audio("assets/soundEffects/weapons/reload/cyllinder_open.wav", 0, 10)
        else
            play_audio("assets/soundEffects/misc/flashlight/flashlight_off.wav")
            play_audio("assets/soundEffects/weapons/reload/cyllinder_close.wav", 0, 10)
        end

        return
    end

    if not can_shoot then
        cooldown_timer = cooldown_timer - get_delta_time()
        if cooldown_timer <= 0.0 then
            can_shoot = true
        end
    end

    update_pushback_recovery()

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
    GameState.save_revolver_player_state = save_player_state
    GameState.load_revolver_player_state = load_player_state
end