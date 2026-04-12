scene = {
    -- Sprite table
    sprites = {
        [0] =
        {asset_id = "cursor", file_path = "./assets/ui/cursors/cursor.png"},
        {asset_id = "player", file_path = "./assets/sprites/characters/player/player_sprite_sheet.png"},
        {asset_id = "barrel", file_path = "./assets/sprites/environment/barrel_sprite.png"},
        {asset_id = "pillar", file_path = "./assets/sprites/environment/pillar_sprite.png"},
        {asset_id = "test_background", file_path = "./assets/sprites/environment/backgrounds/test_background.png"},
        
    },

    -- Font table
    fonts = {
        [0] = 
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24}
    },

    -- Key & action table
    keys = {
        [0] = 
        {name = "move_up", key = 119},
        {name = "move_left", key = 97},
        {name = "move_down", key = 115},
        {name = "move_right", key = 100}
    },

    -- Mouse buttons & actions table
    buttons = {
        [0] = 
        {name = "LMB", button = 1}
    },

    -- Entity table
    entities = {
        [0] =
        { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, z_index = 2, flip = false},
                cursor = {}
            }
        },
        { -- player
            components = {
                animation = {
                    clips = {
                        idle = {row = 0, num_frames = 4, animation_speed = 9, loop = true},
                        walk = {row = 1, num_frames = 6, animation_speed = 12, loop = true}
                    }
                },
                script  = {path = "./assets/scripts/player.lua"},
                camera_follow = {},
                box_collider = {width = 25, height = 25, offset = {x = 2.5, y = 2.5}},
                rigidbody = {velocity = {x = 0, y = 0}},
                sprite = {assetId = "player", width = 30, height = 30, src_rect = {x = 0, y = 0}, z_index = 1, flip = true},
                transform = {position = {x = 325.0, y = 215.0}, scale = {x = 3.0, y = 3.0}, rotation = 0.0}
            }
        },
        { -- barrel
            components = {
                box_collider = {width = 9, height = 12, offset = {x = 2.0, y = 2.0}},
                sprite = {assetId = "barrel", width = 13, height = 16, src_rect = {x = 0, y = 0}, z_index = 1, flip = false},
                transform = {position = {x = 250.0, y = 150.0}, scale = {x = 3.0, y = 3.0}, rotation = 0.0},
                tag = {tag = "barrel"}
            }
        },
        { -- pillar
            components = {
                box_collider = {width = 9, height = 12, offset = {x = 2.0, y = 2.0}},
                sprite = {assetId = "pillar", width = 42, height = 87, src_rect = {x = 0, y = 0}, z_index = 1, flip = false},
                transform = {position = {x = 350.0, y = 80.0}, scale = {x = 1.35, y = 1.35}, rotation = 0.0},
                tag = {tag = "pillar"}
            }
        }

    }
}
