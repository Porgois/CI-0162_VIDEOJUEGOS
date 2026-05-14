function on_collision(other)
    local tag = get_tag(other)

    if tag == "t_player" then
        go_to_scene("sewers_02")
    end

end