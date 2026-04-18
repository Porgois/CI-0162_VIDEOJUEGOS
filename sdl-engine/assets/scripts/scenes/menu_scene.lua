scene = {
    -- Sprite table
    sprites = {
        [0] =
        {asset_id = "cursor", file_path = "./assets/ui/cursors/cursor.png"}
    },

    -- Font table
    fonts = {
        [0] = 
        {font_id = "pixel_sleigh_24", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 24},
        {font_id = "pixel_sleigh_32", file_path = "./assets/fonts/PixelSleigh.ttf", font_size = 32}
    },

    -- Key & action table
    keys = {},

    -- Mouse buttons & actions table
    buttons = {
        [0] = 
        {name = "LMB", button = 1}
    },

    -- Map table
    maps = {
        width = 800,
        height = 600
    },

    -- Entity table
    entities = {
        [0] = 
        { -- cursor
            components = {
                sprite = {assetId = "cursor", width = 4, height = 4, src_rect = {x = 0, y = 0}, flip = false},
                cursor = {}
            }
        },
        { -- Game title
            components = {
                clickable = {},
                text = {text = "Game title", font_id = "pixel_sleigh_32", r = 255, g = 255, b = 255, a = 255},
                transform = {position = {x = 50.0, y = 50.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- New game
            components = {
                clickable = {},
                script = {path = "./assets/scripts/menu_button_01.lua"},
                text = {text = "New game", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255},
                transform = {position = {x = 50.0, y = 110.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Options
            components = {
                clickable = {},
                text = {text = "Options", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255},
                transform = {position = {x = 50.0, y = 170.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        },
        { -- Quit
            components = {
                clickable = {},
                text = {text = "Quit", font_id = "pixel_sleigh_24", r = 255, g = 255, b = 255, a = 255},
                transform = {position = {x = 50.0, y = 230.0}, scale = {x = 1.0, y = 1.0}, rotation = 0.0}
            }
        }
    }
}