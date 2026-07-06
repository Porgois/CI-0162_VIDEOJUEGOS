return {
    name = "shotgun_reload_frame",
    components = {
        script    = {path = "./assets/scripts/weaponScripts/scr_shotgun_reload_system.lua"},
        sprite    = {assetId = "shotgun_frame", width = 80, height = 81, src_rect = {x = 0, y = 0}, z_index = 300, pivot = {x = 0, y = 0}, flip = false, is_ui = true, is_unlit = true},
        transform = {position = {x = 10.0, y = 280.0}, scale = {x = 3.0, y = 3.0}, rotation = 0.0}
    }
} 