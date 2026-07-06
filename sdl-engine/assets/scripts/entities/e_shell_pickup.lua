return {
    name = "shell_pickup",
    components = {
       animation = {
            clips = {
                glow = {row = 0, num_frames = 8, animation_speed = 8, loop = true}
            }
        },
        script = {path = "./assets/scripts/miscScripts/scr_pickable.lua"},
        box_collider  = {width = 10, height = 8, offset = {x = 1, y = 2}, is_trigger = true},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 200.0},
        sprite        = {assetId = "shell_pickup", width = 11, height = 15, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 0, y = 0}, flip = false},
        transform     = {position = {x = 380.0, y = 230.0}, scale = {x = 0.8, y = 0.8}, rotation = 0.0},
        tag           = {tag = "t_shell_pickup"}
    }
}