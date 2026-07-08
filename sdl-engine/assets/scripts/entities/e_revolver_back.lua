return {
    name = "revolver_back",
    components = {
        sprite       = {assetId = "revolver", width = 18, height = 11, src_rect = {x = 0, y = 0}, z_index = 8, pivot = {x = 1, y = 1}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 0.85, y = 0.85}, rotation = 0.0},
        child_of     = {parent = "player", offset = {x = 0.0, y = 0.0}}
    }
}