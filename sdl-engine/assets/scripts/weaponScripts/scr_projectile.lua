local damage_to_deal = 0.0

function set_damage_to_deal(damage)
    damage_to_deal = damage
end

-- Handle collision
function on_collision(other)
    local tag = get_tag(other)
    
    if (tag == "t_enemy" or tag == "t_destructable") then
        call_function(other, "take_damage", damage_to_deal)
    end
    
    if tag == "" or tag == "t_player" or tag == "t_ammo_pickup" or tag == "t_shells_pickup"
        or tag == "t_revolver_pickup" or tag == "t_shotgun_pickup" then
        return
    end
    
    delete_entity(this)
end