return {
    name = "gate",
    components = {
       animation = {
            clips = {
                open = {row = 3, num_frames = 1, animation_speed = 1, loop = false},
                open_one = {row = 2, num_frames = 1, animation_speed = 1, loop = false},
                open_two = {row = 1, num_frames = 1, animation_speed = 1, loop = false},
                closed = {row = 0, num_frames = 1, animation_speed = 1, loop = false}
            }
        },
        script = {path = "./assets/scripts/miscScripts/scr_gate.lua"},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 200.0},
        sprite        = {assetId = "gate", width = 27, height = 36, src_rect = {x = 0, y = 0}, z_index = 8, pivot = {x = 0, y = 0}, flip = false},
        transform     = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
        tag           = {tag = "t_gate"}
    }
}