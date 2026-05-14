return {
    name = "valve",
    components = {
       animation = {
            clips = {
                idle = {row = 0, num_frames = 1, animation_speed = 1, loop = false},
                rotate = {row = 1, num_frames = 3, animation_speed = 8, loop = true}
            }
        },
        script = {path = "./assets/scripts/miscScripts/scr_valve.lua"},
        box_collider  = {width = 15, height = 12, offset = {x = 6, y = 18}, is_trigger = true},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 200.0},
        sprite        = {assetId = "valve", width = 17, height = 17, src_rect = {x = 0, y = 0}, z_index = 8, pivot = {x = 0, y = 0}, flip = false},
        transform     = {position = {x = 0.0, y = 0.0}, scale = {x = 0.8, y = 0.8}, rotation = 0.0},
        tag           = {tag = "t_valve"}
    }
}