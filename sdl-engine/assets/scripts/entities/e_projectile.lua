return {
    name = "projectile",
    components = {
        flashlight    = { mode = "circle_only", source_radius = 15, origin_offset_x = 4, origin_offset_y = 2},
        box_collider  = {width = 16, height = 8, offset = {x = 0.0, y = 0.0}},
        sprite        = {assetId = "projectile", width = 12, height = 5, src_rect = {x = 0, y = 0}, z_index = 9, pivot = {x = 0, y = 0}, flip = false, is_unlit = true},
        transform     = {position = {x = 360.0, y = 160.0}, scale = {x = 0.8, y = 0.8}, rotation = 0.0},
        rigidbody     = {is_dynamic = false, is_solid = false, mass = 1.0},
        script        = {path = "./assets/scripts/weaponScripts/scr_projectile.lua"}
    }
}