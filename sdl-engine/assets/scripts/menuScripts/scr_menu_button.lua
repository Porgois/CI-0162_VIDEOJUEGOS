function clear_persistent_progress()
    if not GameState then
        return
    end

    local config = GameConfig
    GameState.player_health = config and config.player_health or 3
    GameState.player_max_health = config and config.player_max_health or 3
    GameState.player_revolver_ammo = config and config.player_revolver_ammo or 4
    GameState.player_max_revolver_ammo = config and config.player_max_revolver_ammo or 12
    GameState.player_shotgun_ammo = config and config.player_shotgun_ammo or 2
    GameState.player_max_shotgun_ammo = config and config.player_max_shotgun_ammo or 6
    GameState.player_ammo = GameState.player_revolver_ammo
    GameState.weapons = {}
    GameState.current_weapon_name = ""
    GameState.current_back_weapon_name = ""
    GameState.current_weapon_slot = 0
    GameState.current_scene_name = ""
    GameState.last_scene_name = ""
    GameState.scene_state_snapshots = {}
    GameState.zones = {}
    GameState.dropped_bullet = nil
    GameState.drop_state = "idle"
    GameState.zone_config_queue = {}
    GameState.slotted_bullets = {}
    GameState.revolver_cylinder = {}
    for slot = 1, 6 do
        GameState.revolver_cylinder[slot] = "empty"
    end
    GameState.shotgun_barrel = {}
    for slot = 1, 2 do
        GameState.shotgun_barrel[slot] = "empty"
    end
    GameState.reload_menu_open = false
    GameState.revolver_reload_menu_open = false
    GameState.shotgun_reload_menu_open = false
    GameState.reloading = false
end

function on_click()
    local tag = get_tag(this)

    -- Start
    if tag == "t_start_button" then
        clear_persistent_progress()
        go_to_scene("story_intro") -- story_intro
    end

    -- Controls
    if tag == "t_controls_button" then
        go_to_scene("controls")
    end

    -- Back
    if tag == "t_back_button" then
        clear_persistent_progress()
        go_to_scene("menu")
    end

    -- Exit
    if tag == "t_exit_button" then
        quit_game()
    end
end