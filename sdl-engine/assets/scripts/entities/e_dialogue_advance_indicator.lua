return {
    components = {
        animation = {
            clips = {
                jump = {row = 0, num_frames = 3, animation_speed = 8, loop = true}
            }
        },
        script        = {path = "./assets/scripts/menuScripts/scr_dialogue_advance.lua"},
        sprite = {assetId = "dialogue_advance", width = 18, height = 18, src_rect = {x = 0, y = 0}, z_index = 2, pivot = {x = 0, y = 0}, flip = false, is_ui = true, is_unlit = true},
        transform = {position = {x = 350.0, y = 250.0}, scale = {x = 2.0, y = 2.0}, rotation = 0.0}
    }
}