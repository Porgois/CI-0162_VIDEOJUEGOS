local damage_to_deal = 1

-- Handle collision
function on_collisions(other)
    print("collision!")
    print(type(other))       -- userdata? table? number?
    print(tostring(other))   -- might reveal the handle/ID
    local tag = get_tag(other)
    print("tag: " .. tostring(tag))
end



function on_collision(other)
    local tag = get_tag(other)

    -- Collide with walls
    if tag == "t_enemy" then -- Collide with enemies
        call_function(other, "take_damage", damage_to_deal)
    end

    if tag == "" or tag == "t_player" then -- Dont collide with player 
        return
    end
    
    delete_entity(this)
end