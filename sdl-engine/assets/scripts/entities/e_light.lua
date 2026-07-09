return {
    name = "light",
    components = {
        sprite    = {assetId = "light", width = 8, height = 19, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 0, y = 0}, flip = false, is_ui = false, is_unlit = false},
        flashlight = { mode = "circle_only", source_radius = 60, cone_width = 0},
        transform     = {position = {x = 360.0, y = 160.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
    }
}