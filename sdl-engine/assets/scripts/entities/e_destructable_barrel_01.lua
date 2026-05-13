return {
    name = "m_barrel_01",
    components = {
        animation = {
            clips = {
                idle = {row = 0, num_frames = 1, animation_speed = 9, loop = true},
                damage = {row = 1, num_frames = 2, animation_speed = 12, loop = false},
                destroy = {row = 2, num_frames = 3, animation_speed = 12, loop = false},
            }
        },
        script        = {path = "./assets/scripts/miscScripts/scr_destructable.lua"},
        box_collider  = {width = 8, height = 12, offset = {x = 8, y = 5.5}},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 100.0},
        sprite        = {assetId = "destructable_metal_barrel", width = 24, height = 24, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 0, y = 0}, flip = false},
        tag           = {tag = "t_destructable"},
        transform     = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
    }
}