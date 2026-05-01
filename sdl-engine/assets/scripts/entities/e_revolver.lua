return {
    name = "revolver",
    components = {
        script       = {path = "./assets/scripts/weaponScripts/s_revolver.lua"},
        sprite       = {assetId = "revolver", width = 9, height = 8, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 1, y = 1}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 0.85, y = 0.85}, rotation = 0.0},
        child_of     = {parent = "player", offset = {x = 0.0, y = 0.0}},
        mouse_follow = {orbit_radius = 9.0}
    }
}