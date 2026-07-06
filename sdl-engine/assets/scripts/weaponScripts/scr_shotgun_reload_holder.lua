local home_x = 0
local home_y = 0

function start()
    home_x, home_y = get_position(this)
    set_position(this, -10000, -10000)
end

function update()
    if GameState and GameState.shotgun_reload_menu_open then
        set_position(this, home_x, home_y)
    else
        set_position(this, -10000, -10000)
    end
end
