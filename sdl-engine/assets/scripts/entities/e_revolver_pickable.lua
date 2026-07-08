return {
    name = "revolver_pickable",
    components = {
        animation = {
            clips = {
                glow = {row = 0, num_frames = 7, animation_speed = 8, loop = true}
            }
        },
        script       = {path = "./assets/scripts/miscScripts/scr_pickable.lua"},
        box_collider  = {width = 10, height = 8, offset = {x = 1, y = 2}, is_trigger = true},
        sprite       = {assetId = "revolver_pickable", width = 9, height = 8, src_rect = {x = 0, y = 0}, z_index = 8, pivot = {x = 0, y = 0}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
        tag          = {tag = "t_revolver_pickup"}
    }
}