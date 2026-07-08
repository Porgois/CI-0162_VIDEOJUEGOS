return {
    name = "shotgun_pickup",
    components = {
        animation = {
            clips = {
                glow = {row = 0, num_frames = 9, animation_speed = 8, loop = true}
            }
        },
        script       = {path = "./assets/scripts/miscScripts/scr_pickable.lua"},
        box_collider  = {width = 20, height = 8, offset = {x = 2, y = 0}, is_trigger = true},
        sprite       = {assetId = "shotgun_pickable", width = 24, height = 10, src_rect = {x = 0, y = 0}, z_index = 8, pivot = {x = 0, y = 0}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
        tag          = {tag = "t_shotgun_pickup"}

    }
}