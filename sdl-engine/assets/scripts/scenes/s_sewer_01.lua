scene = {
    sprites = {
        [0] =
        {asset_id = "cursor",          file_path = "./assets/ui/cursors/cursor.png"},
        {asset_id = "player",          file_path = "./assets/sprites/characters/player/player_sprite_sheet.png"},
        {asset_id = "revolver",        file_path = "./assets/sprites/weapons/revolver.png"},
        {asset_id = "terrain_sewers",  file_path = "./assets/sprites/tiles/sewers.png"}
    },
    fonts = {
        [0] = {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24}
    },
    keys    = (function() local t = dofile("./assets/scripts/misc/keys.lua");    return t end)(),
    buttons = (function() local t = dofile("./assets/scripts/misc/buttons.lua"); return t end)(),
    maps = {
        map_path  = "./assets/maps/sewers_01.tmx",
        tile_path = "./assets/maps/sewers.tsx",
        tile_name = "terrain_sewers"
    },
    entities = {
        [0] = dofile("./assets/scripts/entities/e_cursor.lua"),
            dofile("./assets/scripts/entities/e_player.lua"),
            dofile("./assets/scripts/entities/e_revolver.lua")
    }
}