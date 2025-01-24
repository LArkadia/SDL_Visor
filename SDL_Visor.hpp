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
    uint8_t red,green,blue,alpha;

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
        //Set
        void Reset_color(uint8_t red,
            uint8_t green,
            uint8_t blue,
            uint8_t alpha);
        void Reset_color(uint8_t red,
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
        SDL_Texture* texture;
        SDL_Renderer* renderer;
        
    public:
        Icon(String icon_path,SDL_Renderer* renderer);
        
        SDL_Texture* Get_texture();
        ~Icon();
    };


//-----------------------------------------:CLASS SCREEN:--------------------------
    
    class Screen
    {
    private:
    //-----------------------------------------:SUB CLASS BUTTON:--------------------------
    
    class Button
    {
    private:
        uint16_t x1,y1,x2,y2;
        uint16_t width,heith;
        void (*event_function)(void);
        SDL_Texture* texture;
    public:
        Button(const uint16_t x, const uint16_t y,
                const uint16_t width, const uint16_t height,
                SDL_Texture* button_texture,
                void (*function_to_run)(void));
        void Display(Screen *window);
        void Run_function();
        bool In_area(uint16_t x, uint16_t y);
        ~Button();
    };

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* tmp_surface;
        SDL_Texture *tmp_texture,*button_texture;
        bool close;
        static bool ttf_initialized;
        static bool img_initialized;
        String default_font;
        std::map<String,TTF_Font*> font;
        std::map<String,SDL_Texture*> texture;
        void (*event_handler_function)(SDL_Event& event);
        Vector<Button*> buttons;

        //Internal functions
        void Handle_buttons(SDL_Event* event);
        

    public:
    //Create and set
        Screen(String title, uint16_t width,uint16_t height,uint32_t renderer_flags);
        void Set_icon(String png_path);
        void Set_renderer_color(Color &color);
        void Set_events_handler(void (*event_handler)(SDL_Event&));
        void Create_texture(String texture_name, const uint16_t width, const uint16_t height);
        void Create_button_texture(const uint16_t width, const uint16_t height);
        void End_texture();
        void Init_TTF (String font_name,String font_path,uint32_t font_size);
        void Load_font(String font_name,String font_path,uint32_t font_size);
        void Init_IMG(int flags);
        void Init_IMG();
        void Set_default_font(String font_name);
        static bool IMG_initialized();


    //Task handle
        void Present_renderer();
        bool Handle_events();
        void Draw_texture(SDL_Texture* texture,SDL_Rect* area);
        void Draw_saved_texture(String texture_name);
        void Draw_saved_texture(String texture_name,SDL_Rect* area);
        void Clean_screen();
        void Clean_screen(Color &color);
        void Show_text(
            const uint16_t x, const uint16_t y, 
            String Text, Color& color);
        void Show_text_size_fixed(
            const uint16_t x, const uint16_t y, 
            const uint16_t width, const uint16_t height,
            String Text, Color& color);
        void Display_buttons();

        //Draw shapes
        void Draw_point(const uint16_t x,const uint16_t y,Color& color);
        void Draw_rectangle(
            const uint16_t x,    const uint16_t y, 
            const uint16_t width,const uint16_t height, 
            Color& color);
        void Draw_filled_rectangle(
            const uint16_t x,    const uint16_t y, 
            const uint16_t width,const uint16_t height, 
            Color& color);

        void Draw_line_angle(
            const uint16_t x,    const uint16_t y, 
            const float_t angle, const uint16_t length, 
            Color& color);

        void Draw_line_pos(
            const uint16_t x1,   const uint16_t y1,
            const uint16_t x2,   const uint16_t y2,
            Color &color);
        
        void Draw_circle(
            const uint16_t x,   const uint16_t y,    const uint16_t r,
            const uint16_t resolution,  Color &color
        );
        
    //Create buttons
        void Create_simple_button(
            const uint16_t x,       const uint16_t y,
            const uint16_t width,   const uint16_t height,
            String display_text,    Color &background_color, Color &foreground_color,
            void (*event_function)(void)
        );
        /*
        void Create_button_from_texture(
            String texture_name,
            const uint16_t x, const uint16_t y,
            void (*event_function)(void)
        );
        void Create_button_from_tmp_texture(
            const uint16_t x, const uint16_t y,
            void (*event_function)(void)
        );
        */
    //Get
        void Get_window_sizes(int* width, int* height);
        SDL_Renderer* Get_renderer() ;

    //Destroy

        ~Screen();
    };

} // namespace SSV
