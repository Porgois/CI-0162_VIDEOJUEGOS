scene = {
   sprites = {
        [0] =
        {asset_id = "cursor",          file_path = "./assets/ui/cursors/cursor.png"},
        {asset_id = "player",          file_path = "./assets/sprites/characters/player/player_sprite_sheet.png"},
        {asset_id = "destructable_metal_barrel",  file_path = "./assets/sprites/environment/metal_barrel_sprite_sheet.png"},
        {asset_id = "enemy_duck",          file_path = "./assets/sprites/characters/enemies/enemy_duck_sprite_sheet.png"},
        {asset_id = "revolver",        file_path = "./assets/sprites/weapons/revolver.png"},
        {asset_id = "shotgun",        file_path = "./assets/sprites/weapons/shotgun.png"},
        {asset_id = "ammo_pickup",     file_path = "./assets/sprites/weapons/ammo_pickup_sprite_sheet.png"},
        {asset_id = "health_pickup",     file_path = "./assets/sprites/weapons/health_pickup_sprite_sheet.png"},
        {asset_id = "terrain_sewers",  file_path = "./assets/sprites/tiles/sewers.png"},
        {asset_id = "projectile",      file_path = "./assets/sprites/effects/projectiles/projectile_bullet.png"},
        {asset_id = "reload_bullet",   file_path = "./assets/sprites/ui/bullet/bullet_sprite_sheet.png"},
        {asset_id = "revolver_frame",  file_path = "./assets/sprites/ui/bullet/revolver_frame.png"},
        {asset_id = "bullet_holder",   file_path = "./assets/sprites/ui/bullet/bullet_holder.png"},
        {asset_id = "reload_shell",   file_path = "./assets/sprites/ui/shell/shotgun_shell_ui_spritesheet.png"},
        {asset_id = "shotgun_frame",  file_path = "./assets/sprites/ui/shell/shotgun_frame.png"},
        {asset_id = "shell_holder",   file_path = "./assets/sprites/ui/shell/shell_holder.png"},
        {asset_id = "flashlight_cone", file_path = "./assets/sprites/masks/cone.png"},
        {asset_id = "flashlight_source", file_path = "./assets/sprites/masks/circle.png"},
        {asset_id = "gate", file_path = "./assets/sprites/misc/gate_sprite_sheet.png"},
        {asset_id = "valve", file_path = "./assets/sprites/misc/valve_sprite_sheet.png"}
    },
    fonts = {
        [0] = {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24}
    },
    keys    = (function() local t = dofile("./assets/scripts/misc/keys.lua");    return t end)(),
    buttons = (function() local t = dofile("./assets/scripts/misc/buttons.lua"); return t end)(),
    maps = {},
    entities = {
        [0] = dofile("./assets/scripts/entities/e_cursor.lua"),
            dofile("./assets/scripts/entities/e_shotgun_reload_screen.lua"),
            dofile("./assets/scripts/entities/e_shell_holder.lua"
        )
    }
}