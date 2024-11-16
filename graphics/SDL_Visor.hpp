#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cinttypes>
#include <iostream>

#include <fstream>
#include <sstream>

#define Vector std::vector
#define String std::string
#define Shared_ptr std::shared_ptr

namespace vsr
{
//-----------------------------------------:CLASS COLOR:--------------------------
    class Color
    {
    private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;

    public:
        // Create and set 
        Color(  
            uint8_t red,
            uint8_t green,
            uint8_t blue,
            uint8_t alpha);
        Color(  
            uint8_t red,
            uint8_t green,
            uint8_t blue);
        //Get
        uint8_t R();
        uint8_t G();
        uint8_t B();
        uint8_t A();
        //Destroy
        ~Color();
    };

//-----------------------------------------:CLASS ICON:--------------------------
    Vector<String>Split_by_spaces(const std::string& str);
    class Icon
    {
    private:
        uint16_t resolution;
        Vector<Vector<Shared_ptr<Color>>> pxl_grid;
        
    public:
        Icon(const String &icon_path);
        Vector<Vector<Shared_ptr<Color>>> Get_pxl_grid();
        ~Icon();
    };


//-----------------------------------------:CLASS SCREEN:--------------------------
    
    class Screen
    {
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* tmp_surface;
        SDL_Texture* tmp_texture;
        bool close;
        bool ttf_initialized;
        String default_font;
        std::map<String,TTF_Font*> font;
        std::map<String,SDL_Texture*> texture;
        void (*event_handler_function)(SDL_Event& event);


    public:
    //Create and set
        Screen(String title, uint16_t width,uint16_t heigth,uint32_t renderer_flags);
        void Set_icon(String png_path);
        void Set_events_handler(void (*event_handler)(SDL_Event&));
        void Create_texture(String texture_name, const uint16_t width, const uint16_t heigth);
        void Init_TTF (String font_name,String font_path,uint32_t font_size);
        void Load_font(String font_name,String font_path,uint32_t font_size);
        void Set_default_font(String font_name);


    //Task handle
        void Present_renderer();
        bool Handle_events();
        void Draw_texture(String texture_name);
        void Clean_screen();
        void Clean_screen(Color &color);
        void Show_text(
            const uint16_t x, const uint16_t y, 
            String Text, Color& color);



    //Draw shapes
        void Draw_rectangle(
            const uint16_t x, const uint16_t y, 
            const uint16_t width, const uint16_t heigth, 
            Color& color);

        void Draw_line_angle(
            const uint16_t x, const uint16_t y, 
            const float_t angle, const uint16_t length, 
            Color& color);

        void Draw_line_pos(
            const uint16_t x1, const uint16_t y1,
            const uint16_t x2, const uint16_t y2,
            Color &color);
        
    //Get
        void Get_window_sizes(int* width, int* heigth);


    //Destroy
        ~Screen();
    };

} // namespace SSV
