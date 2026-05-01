-- Handle collision
function on_collision(other)
    -- Collide with walls

    -- Collide with enemies
    local tag = get_tag(other)
    if tag == "" or tag == "t_player" then
        return
    end
    
    delete_entity(this)
end