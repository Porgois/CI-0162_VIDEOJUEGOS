return {
    components = {
        clickable = {width = 45, height = 68, offset_x = 26, offset_y = 10},
        animation = {
            clips = {
                default = {row = 0, num_frames = 1, animation_speed = 1, loop = true},
                grab    = {row = 1, num_frames = 1, animation_speed = 1, loop = true},
                full    = {row = 2, num_frames = 1, animation_speed = 1, loop = true},
                empty   = {row = 3, num_frames = 1, animation_speed = 1, loop = true},
                eject   = {row = 4, num_frames = 1, animation_speed = 1, loop = true}
            }
        },
        script = {path = "./assets/scripts/uiScripts/scr_reload_bullet.lua"},
        sprite    = {assetId = "reload_bullet", width = 48, height = 48, src_rect = {x = 0, y = 0}, z_index = 50, pivot = {x = 0, y = 0}, flip = false, is_ui = true, is_unlit = true},
        transform = {position = {x = 60.0, y = 170.0}, scale = {x = 2.0, y = 2.0}, rotation = 0.0}
    }
}