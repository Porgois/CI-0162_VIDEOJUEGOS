return {
    name = "shotgun_back",
    components = {
        sprite       = {assetId = "shotgun", width = 24, height = 10, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 1, y = 1}, flip = false},
        transform    = {position = {x = 0.0, y = 0.0}, scale = {x = 0.75, y = 0.75}, rotation = 0.0},
        child_of     = {parent = "player", offset = {x = -4.0, y = 3.0}}
    }
}