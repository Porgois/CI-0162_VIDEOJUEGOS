return {
    name = "ammo_pickup",
    components = {
        box_collider  = {width = 10, height = 8, offset = {x = 1, y = 2}},
        rigidbody     = {is_dynamic = false, is_solid = true, mass = 1.0},
        sprite        = {assetId = "ammo_pickup", width = 12, height = 11, src_rect = {x = 0, y = 0}, z_index = 10, pivot = {x = 0, y = 0}, flip = false},
        transform     = {position = {x = 380.0, y = 230.0}, scale = {x = 0.7, y = 0.7}, rotation = 0.0},
        tag           = {tag = "t_ammo_pickup"}
    }
}