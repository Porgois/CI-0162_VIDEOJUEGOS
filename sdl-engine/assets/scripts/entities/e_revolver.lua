return {
    name = "revolver",
    components = {
        animation = {
            clips = {
                shoot = {row = 0, num_frames = 3, animation_speed = 20, loop = false}
            }
        },
        script       = {path = "./assets/scripts/weaponScripts/scr_revolver.lua"},
        sprite       = {assetId = "revolver", width = 18, height = 11, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 1, y = 1}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 0.85, y = 0.85}, rotation = 0.0},
        child_of     = {parent = "player", offset = {x = 0.0, y = 0.0}},
        mouse_follow = {orbit_radius = 13.0}
    }
}