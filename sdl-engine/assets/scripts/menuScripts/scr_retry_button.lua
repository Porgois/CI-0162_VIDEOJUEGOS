function on_click()
    local target_scene = GameState and GameState.last_scene_name or ""
    if target_scene and target_scene ~= "" then
        go_to_scene(target_scene, true)
    else
        go_to_scene("sewers_01", true)
    end
end