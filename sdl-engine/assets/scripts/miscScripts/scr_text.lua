local lifetime = 1.0
local float_speed = 2.0

local timer = lifetime

function start()
    timer = lifetime
end

function update()
    timer = timer - get_delta_time()

    if timer <= 0 then
        delete_entity(this)
        return
    end

    local x, y = get_position(this)
    set_position(this, x, y - float_speed * get_delta_time())
end
