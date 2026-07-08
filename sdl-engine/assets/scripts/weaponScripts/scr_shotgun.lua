-- Projectile
local projectile_entity = dofile("./assets/scripts/entities/e_projectile.lua")
local projectile_speed = 350.0
local local_projectile_damage = 0.25
local can_shoot = true

-- Spread
local projectile_amount = 8
local spread = 15.0
local spread_jitter = 20.0

-- Pushback (recoil)
local pushback_amount = 10.0
local pushback_recovery_speed = 40.0
local recovering = false
local rest_x = 0.0
local rest_y = 0.0

-- Ammo
local max_ammo = 6

-- Cooldown
local shoot_cooldown = 1.0
local cooldown_timer = 0.0

-- Offset
local barrel_offset_x = 6.0

function start()
    if GameState then
        if GameState.player_shotgun_ammo == nil then
            GameState.player_shotgun_ammo = max_ammo
        end
        if GameState.player_max_shotgun_ammo ~= nil then
            max_ammo = GameState.player_max_shotgun_ammo
        else
            GameState.player_max_shotgun_ammo = max_ammo
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
        print("[SHOTGUN] On cooldown!")
        return
    end

    if GameState and GameState.reload_menu_open then
        return
    end

    local spend = GameState and (GameState.spend_shotgun_casing or GameState.spend_casing)
    if GameState == nil or spend == nil or not spend() then
        play_audio("assets/soundEffects/weapons/shoot/dry_fire.wav", 0, 64)
        return
    end

    play_animation(this, "shoot")
    apply_pushback(get_aim_angle())
    spawn_projectiles()
    play_audio("assets/soundEffects/weapons/shotgun/shotgun_shoot.wav", 0, 24)
    shake_camera(0.4, 8.0, 30.0)

    can_shoot = false
    cooldown_timer = shoot_cooldown
end

function spawn_projectiles()
    local x_pos, y_pos = get_pivoted_position(this)
    local base_angle = get_aim_angle()

    local count = math.max(1, math.floor(projectile_amount))
    local spread_rad = math.rad(spread)
    local jitter_rad = math.rad(spread_jitter)

    for i = 1, count do
        local angle
        if count == 1 then
            angle = base_angle
        else
            -- distribute evenly from -spread/2 to +spread/2
            local t = (i - 1) / (count - 1)          -- 0 .. 1
            local offset = -spread_rad / 2 + t * spread_rad
            angle = base_angle + offset
        end

        -- Add random jitter so pellets aren't perfectly uniform
        if jitter_rad > 0.0 then
            angle = angle + (math.random() * 2.0 - 1.0) * jitter_rad
        end

        -- Spawn position always uses base_angle (muzzle point)
        -- Only the velocity direction varies per pellet.
        spawn_single_projectile(x_pos, y_pos, base_angle, angle)
    end
end

function spawn_single_projectile(x_pos, y_pos, spawn_angle, velocity_angle)
    local projectile = spawn_entity(projectile_entity)
    call_function(projectile, "set_damage_to_deal", local_projectile_damage)

    local spawn_x = x_pos + math.cos(spawn_angle) * barrel_offset_x
    local spawn_y = y_pos + math.sin(spawn_angle) * barrel_offset_x

    set_position(projectile, spawn_x, spawn_y)
    set_rotation(projectile, math.deg(velocity_angle))
    set_velocity(projectile, math.cos(velocity_angle) * projectile_speed, math.sin(velocity_angle) * projectile_speed)
end

function update()
    if GameState and GameState.set_shotgun_reload_menu and is_button_just_pressed("rmb") then
        local opening = not GameState.shotgun_reload_menu_open
        GameState.set_shotgun_reload_menu(opening)

        if opening then
            play_audio("assets/soundEffects/misc/flashlight/flashlight_on.wav")
            play_audio("assets/soundEffects/weapons/shotgun/shotgun_close.wav", 0, 30)
        else
            play_audio("assets/soundEffects/misc/flashlight/flashlight_off.wav")
            play_audio("assets/soundEffects/weapons/shotgun/shotgun_open.wav", 0, 30)
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

-- SAVING / LOADING
function save_player_state()
    if GameState then
        if GameState.player_shotgun_ammo == nil then
            GameState.player_shotgun_ammo = max_ammo
        end
        GameState.player_shotgun_ammo = GameState.player_shotgun_ammo
        print("[SHOTGUN] save_player_state: GameState.player_shotgun_ammo=" .. tostring(GameState.player_shotgun_ammo))
    end
end

function load_player_state()
    if GameState then
        if GameState.player_shotgun_ammo == nil then
            GameState.player_shotgun_ammo = max_ammo
            print("[SHOTGUN] load_player_state: initialized player_shotgun_ammo=" .. tostring(GameState.player_shotgun_ammo))
        else
            print("[SHOTGUN] load_player_state: restored player_shotgun_ammo=" .. tostring(GameState.player_shotgun_ammo))
        end
    end
end

if GameState ~= nil then
    local previous_save_player_state = GameState.save_player_state
    GameState.save_player_state = function()
        if previous_save_player_state then
            previous_save_player_state()
        end
        save_player_state()
    end

    local previous_load_player_state = GameState.load_player_state
    GameState.load_player_state = function()
        if previous_load_player_state then
            previous_load_player_state()
        end
        load_player_state()
    end

    GameState.save_shotgun_player_state = GameState.save_player_state
    GameState.load_shotgun_player_state = GameState.load_player_state
end