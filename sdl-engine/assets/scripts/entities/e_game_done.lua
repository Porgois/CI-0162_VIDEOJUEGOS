return {
    name = "next_scene_portal",
    components = {
        script = {path = "./assets/scripts/miscScripts/scr_portal_done.lua"},
        box_collider  = {width = 15, height = 12, offset = {x = 6, y = 18}, is_trigger = true},
        transform     = {position = {x = 0.0, y = 0.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0},
        tag           = {tag = "t_portal"}
    }
}
