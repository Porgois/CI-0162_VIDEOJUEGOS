-- Default values
default_revolver_ammo = 4
default_shells = 2
default_health = 1

-- "Magnetism"
local effective_pickup_range = 25
local speed = 20

-- Text
local text_entity = dofile("./assets/scripts/entities/e_text.lua")
local text_spawn_offset = 3

function get_default_ammo()
    return default_ammo
end

function start()
    play_animation(this, "glow")
end

function spawn_text(text_contents)
    if not has_entity("player") then 
        return 
    end

    local player = find_entity("player")
    local text = spawn_entity(text_entity)
    set_text(text, text_contents)

    local player_x, player_y = get_position(player)
    set_position(text, player_x, player_y + text_spawn_offset)
end

function distance_to_player()
    local player = find_entity("player")
    
    local my_x, my_y = get_position(this)
    local player_x, player_y = get_position(player)
    local dx = player_x - my_x
    local dy = player_y - my_y
    return math.sqrt(dx * dx + dy * dy)
end

function go_to_player()
    if not has_entity("player") then return end
    local dist = distance_to_player()
    
    if dist <= effective_pickup_range then
        local player = find_entity("player")

        local my_x, my_y = get_position(this)
        local player_x, player_y = get_position(player)
        local dx = (player_x + 8) - my_x
        local dy = (player_y + 8) - my_y
        local d = math.sqrt(dx * dx + dy * dy)

        if d < 4 then
            set_velocity(this, 0, 0)
            return
        end

        local approach_speed = math.min(speed, (d / effective_pickup_range) * speed + 15)
        set_velocity(this, (dx / d) * approach_speed, (dy / d) * approach_speed)
    else
        set_velocity(this, 0, 0)
    end
end

function update()
    go_to_player()
end