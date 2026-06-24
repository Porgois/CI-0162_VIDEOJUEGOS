return {
    name = "enemy_psychic",
    components = {
        animation = {
            clips = {
                idle = {row = 0, num_frames = 4, animation_speed = 9, loop = true},
                walk = {row = 1, num_frames = 6, animation_speed = 14, loop = true},
                damage = {row = 2, num_frames = 2, animation_speed = 12, loop = true},
                death  = {row = 3, num_frames = 3, animation_speed = 12, loop = false},
                alert  = {row = 4, num_frames = 6, animation_speed = 9, loop = false},
                sing = {row = 5, num_frames = 5, animation_speed = 8, loop = true}
            }
        },
        script        = {path = "./assets/scripts/enemyScripts/scr_enemy_psychic.lua"},
        box_collider  = {width = 12, height = 16, offset = {x = 9.5, y = 22.5}},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 100.0},
        sprite        = {assetId = "enemy_psychic", width = 30, height = 48, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 0, y = 0}, flip = false, is_lit_only = true},
        tag           = {tag = "t_enemy"},
        transform     = {position = {x = 660.0, y = 210.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
    }
}