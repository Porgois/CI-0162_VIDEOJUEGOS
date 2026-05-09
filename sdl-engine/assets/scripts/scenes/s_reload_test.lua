scene = {
    sprites = {
        [0] =
        {asset_id = "cursor",          file_path = "./assets/ui/cursors/cursor.png"},
        {asset_id = "reload_bullet",   file_path = "./assets/sprites/ui/bullet/bullet_sprite_sheet.png"},
        {asset_id = "revolver_frame",   file_path = "./assets/sprites/ui/bullet/revolver_frame.png"}
    },
    fonts = {
        [0] = {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24}
    },
    keys    = (function() local t = dofile("./assets/scripts/misc/keys.lua");    return t end)(),
    buttons = (function() local t = dofile("./assets/scripts/misc/buttons.lua"); return t end)(),
    maps = {},
    entities = {
        [0] = dofile("./assets/scripts/entities/e_cursor.lua"),
            dofile("./assets/scripts/entities/e_reload_screen.lua"),
            dofile("./assets/scripts/entities/e_bullet_frame.lua")
    }
}