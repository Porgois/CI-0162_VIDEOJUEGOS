#ifndef TEXT_RENDER_SYSTEM_HPP
#define TEXT_RENDER_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <algorithm>
#include <vector>

#include "../e.c.s./ecs.hpp"
#include "../assetManager/assetManager.hpp"
#include "../components/textComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/clickableComponent.hpp"
#include "../components/dialogueComponent.hpp"

class TextRenderSystem : public System {
    public:
        TextRenderSystem() {
            requireComponent<TextComponent>();
            requireComponent<TransformComponent>();
        }

        std::vector<std::string> splitLines(const std::string& text) {
            std::vector<std::string> lines;
            size_t start = 0;
            size_t pos = 0;
            while ((pos = text.find('\n', start)) != std::string::npos) {
                lines.push_back(text.substr(start, pos - start));
                start = pos + 1;
            }
            lines.push_back(text.substr(start));
            return lines;
        }

        void update(SDL_Renderer* renderer, const std::unique_ptr<AssetManager>& asset_manager, SDL_Rect& camera, float zoom_level) {
            for (auto entity : getSystemEntities()) {
                auto& text = entity.getComponent<TextComponent>();
                auto& transform = entity.getComponent<TransformComponent>();

                if (text.text.empty()) {
                    text.width = 0;
                    text.height = 0;
                    continue;
                }

                TTF_Font* font = asset_manager->getFont(text.font_id);
                if (!font) {
                    std::cerr << "[TEXT RENDER] Missing font for id: " << text.font_id << std::endl;
                    continue;
                }

                std::vector<std::string> lines = splitLines(text.text);
                struct RenderedLine {
                    SDL_Texture* texture;
                    int width;
                    int height;
                };

                std::vector<RenderedLine> rendered_lines;
                int total_height = 0;
                int max_width = 0;
                int base_x = text.is_ui ? static_cast<int>(transform.position.x) : static_cast<int>((transform.position.x * zoom_level) - camera.x);
                int base_y = text.is_ui ? static_cast<int>(transform.position.y) : static_cast<int>((transform.position.y * zoom_level) - camera.y);

                for (const std::string& line : lines) {
                    int line_width = 0;
                    int line_height = TTF_FontLineSkip(font);
                    SDL_Texture* line_texture = nullptr;

                    if (line.empty()) {
                        // preserve blank line spacing
                        line_height = TTF_FontLineSkip(font);
                    } else if (text.outline_thickness > 0) {
                        TTF_SetFontOutline(font, text.outline_thickness);
                        SDL_Surface* outline_surface = TTF_RenderText_Blended(font, line.c_str(), text.outline_color);
                        TTF_SetFontOutline(font, 0);
                        SDL_Surface* fg_surface = TTF_RenderText_Blended(font, line.c_str(), text.color);
                        if (!outline_surface) {
                            std::cerr << "[TEXT RENDER] Outline render failed: " << TTF_GetError() << std::endl;
                            if (fg_surface) SDL_FreeSurface(fg_surface);
                            continue;
                        }
                        if (!fg_surface) {
                            std::cerr << "[TEXT RENDER] Text render failed: " << TTF_GetError() << std::endl;
                            SDL_FreeSurface(outline_surface);
                            continue;
                        }

                        SDL_Rect fg_rect = { text.outline_thickness, text.outline_thickness, fg_surface->w, fg_surface->h };
                        SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND);
                        SDL_BlitSurface(fg_surface, NULL, outline_surface, &fg_rect);
                        SDL_FreeSurface(fg_surface);

                        line_width = outline_surface->w;
                        line_height = outline_surface->h;
                        line_texture = SDL_CreateTextureFromSurface(renderer, outline_surface);
                        SDL_FreeSurface(outline_surface);
                        if (!line_texture) {
                            std::cerr << "[TEXT RENDER] Failed to create texture from outline surface: " << SDL_GetError() << std::endl;
                            continue;
                        }
                    } else {
                        SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), text.color);
                        if (!surface) {
                            std::cerr << "[TEXT RENDER] Text render failed: " << TTF_GetError() << std::endl;
                            continue;
                        }
                        line_width = surface->w;
                        line_height = surface->h;
                        line_texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface);
                        if (!line_texture) {
                            std::cerr << "[TEXT RENDER] Failed to create texture from surface: " << SDL_GetError() << std::endl;
                            continue;
                        }
                    }

                    max_width = std::max(max_width, line_width);
                    total_height += line_height;
                    rendered_lines.push_back({line_texture, line_width, line_height});
                }

                int content_x = base_x;
                int content_y = base_y;
                const bool is_dialogue = entity.hasComponent<DialogueComponent>();

                if (is_dialogue) {
                    content_x = std::max(0, (800 - max_width) / 2);
                    content_y = std::max(0, (600 - total_height) / 2);
                }

                int y_offset = 0;
                for (const auto& rendered_line : rendered_lines) {
                    if (rendered_line.texture) {
                        int line_x = content_x;
                        if (is_dialogue) {
                            line_x = std::max(0, (800 - rendered_line.width) / 2);
                        }

                        SDL_Rect dstRect = {
                            line_x,
                            content_y + y_offset,
                            static_cast<int>(rendered_line.width * transform.scale.x),
                            static_cast<int>(rendered_line.height * transform.scale.y)
                        };
                        SDL_RenderCopy(renderer, rendered_line.texture, NULL, &dstRect);
                        SDL_DestroyTexture(rendered_line.texture);
                    }
                    y_offset += rendered_line.height;
                }

                text.width = max_width;
                text.height = total_height;
            }
        }
};
#endif // TEXT_RENDER_SYSTEM_HPP