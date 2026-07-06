local damage_to_deal = 0.25

function set_damage_to_deal(damage)
    damage_to_deal = damage
end

-- Handle collision
function on_collision(other)
    local tag = get_tag(other)

    -- Collide with walls
    if tag == "t_enemy" then -- Collide with enemies or destructable objects
        call_function(other, "take_damage", damage_to_deal)
    end

    if tag == "t_destructable" then
        call_function(other, "take_damage", damage_to_deal)
    end

    if tag == "" or tag == "t_player" then -- Dont collide with player 
        return
    end
    
    delete_entity(this)
end