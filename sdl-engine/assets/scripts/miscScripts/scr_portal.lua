function on_collision(other)
    local tag = get_tag(other)

    if tag == "t_player" then
        local next_scene = get_next_scene_in_list()
        if next_scene ~= nil and next_scene ~= "" then
            go_to_scene(next_scene, true)
        end
    end

end