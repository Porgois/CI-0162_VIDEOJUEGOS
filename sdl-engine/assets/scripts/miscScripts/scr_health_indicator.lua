local current_text = "HEALTH: 3/3"
local current_health = 3
local max_health = 3
local ui_x = 30.0
local ui_y = 555.0

function start()
    if GameState then
        if GameState.player_health ~= nil then
            current_health = GameState.player_health
        end
        if GameState.player_max_health ~= nil then
            max_health = GameState.player_max_health
        end
    end

    current_text = current_health .. "/" .. max_health
    set_text(this, current_text)
    set_position(this, ui_x, ui_y)
end

function update()
    if GameState == nil then
        return
    end

    -- Show only when reload menu is open
    if GameState.reload_menu_open then
        set_position(this, ui_x, ui_y)
    else
        set_position(this, -1000, -1000)
        return
    end

    local health = GameState.player_health or current_health
    local health_max = GameState.player_max_health or max_health
    local new_text = "HEALTH: " .. health .. "/" .. health_max

    if new_text ~= current_text then
        current_text = new_text
        set_text(this, current_text)
    end
end

function update_text(new_text)
    current_text = new_text .. "/" .. max_health
    set_text(this, current_text)
end


