return {
    name = "player",
    components = {
        animation = {
            clips = {
                idle = {row = 0, num_frames = 4, animation_speed = 9, loop = true},
                walk = {row = 1, num_frames = 6, animation_speed = 12, loop = true},
                damage = {row = 2, num_frames = 2, animation_speed = 12, loop = false},
                death  = {row = 3, num_frames = 3, animation_speed = 12, loop = false}
            }
        },
        script        = {path = "./assets/scripts/playerScripts/scr_player.lua"},
        camera_follow = {},
        flashlight = { mode = "full", source_radius = 20, cone_width = 530},
        box_collider  = {width = 12, height = 16, offset = {x = 9.5, y = 7.5}},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 1.0},
        sprite        = {assetId = "player", width = 30, height = 30, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 0, y = 0}, flip = true},
        tag           = {tag = "t_player"},
        transform     = {position = {x = 360.0, y = 160.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
    }
}