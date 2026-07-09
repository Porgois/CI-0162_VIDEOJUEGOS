return {
    name = "shotgun",
    components = {
        animation = {
            clips = {
                shoot = {row = 0, num_frames = 3, animation_speed = 24, loop = false}
            }
        },
        script       = {path = "./assets/scripts/weaponScripts/scr_shotgun.lua"},
        sprite       = {assetId = "shotgun", width = 35, height = 16, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 1, y = 1}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 0.75, y = 0.75}, rotation = 0.0},
        child_of     = {parent = "player", offset = {x = -4.0, y = 1.0}},
        mouse_follow = {orbit_radius = 13.0}
    }
}