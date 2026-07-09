scene = {
    sprites = (function() local t = dofile("./assets/scripts/misc/sprites.lua"); return t end)(),
    fonts = {
        [0] = {font_id = "pixel_sleigh", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 14}
    },
    keys    = (function() local t = dofile("./assets/scripts/misc/keys.lua");    return t end)(),
    buttons = (function() local t = dofile("./assets/scripts/misc/buttons.lua"); return t end)(),
    maps = {
        map_path  = "./assets/maps/level_02.tmx",
        tile_path = "./assets/maps/sewers.tsx",
        tile_name = "terrain_sewers"
    },
    entities = {
        [0] = dofile("./assets/scripts/entities/e_cursor.lua"),
            dofile("./assets/scripts/entities/e_revolver_reload_screen.lua"),
            dofile("./assets/scripts/entities/e_shotgun_reload_screen.lua"),
            dofile("./assets/scripts/entities/e_bullet_holder.lua"),
            dofile("./assets/scripts/entities/e_shell_holder.lua"),
            dofile("./assets/scripts/entities/e_health_indicator.lua")
    }
}