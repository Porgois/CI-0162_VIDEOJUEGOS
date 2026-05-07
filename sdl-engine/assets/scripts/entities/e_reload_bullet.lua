return {
    components = {
        clickable = {width = 42, height = 50, offset_x = 27, offset_y = 27},
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
        sprite    = {assetId = "reload_bullet", width = 48, height = 48, src_rect = {x = 0, y = 0}, z_index = 3000, pivot = {x = 0, y = 0}, flip = false, is_ui = true},
        transform = {position = {x = 60.0, y = 170.0}, scale = {x = 2.0, y = 2.0}, rotation = 0.0}
    }
}