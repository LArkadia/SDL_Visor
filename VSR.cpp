// SDL_Visor.cpp
#include "./VSR.hpp"

namespace vsr
{
//-----------------------------------------:CLASS COLOR:--------------------------
    Color::Color(
        uint8_t red,
        uint8_t green,
        uint8_t blue,
        uint8_t alpha){
            this->red= red;
            this->green= green;
            this->blue = blue;
            this->alpha= alpha;
    }

    Color::Color(
        uint8_t red,
        uint8_t green,
        uint8_t blue){
            this->red= red;
            this->green= green;
            this->blue = blue;
            this->alpha= 255;
    }
    void Color::Reset_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha){
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
    }
    void Color::Reset_color(uint8_t red, uint8_t green, uint8_t blue){
        Reset_color(red,green,blue,255);
    }
    uint8_t Color::R() { return this->red; }
    uint8_t Color::G(){return this->green;}
    uint8_t Color::B(){return this->blue;}
    uint8_t Color::A(){return this->alpha;}
    Color::~Color(){}
    //-----------------------------------------:CLASS ICON:--------------------------
    std::vector<std::string> Split_by_spaces(const std::string& str) {
        std::istringstream iss(str);
        std::vector<std::string> results;
        std::string s;
        while (iss >> s) {
            results.push_back(s);
        }
        return results;
    }
    Icon::Icon(String icon_path,SDL_Renderer* renderer){
        
        texture = IMG_LoadTexture(renderer,icon_path.c_str());
        if (!texture)
        {
            std::cerr << "Error: Can't load texture from image: " << icon_path << " -> " << SDL_GetError() << std::endl;
        }
        SDL_SetTextureScaleMode(texture,SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
    }
    
    SDL_Texture *Icon::Get_texture()
    {
        return texture;
    }
    Icon::~Icon(){
        if (!texture)
        {
            SDL_DestroyTexture(texture);
        }
        
    }
    //-----------------------------------------:CLASS SCREEN:--------------------------
    //Internal functions
    void Screen::Handle_buttons(SDL_Event *event){
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            for (auto &&button : buttons){
                if (button->In_area(event->button.x,event->button.y))
                {
                    button->Run_function();
                }
                
            }
        }
        
        
    }
    
    
    
    
    // Create and set


    bool Screen::ttf_initialized = false;
    bool Screen::img_initialized = false;


    
    Screen::Screen(String title, uint16_t width,uint16_t height,SDL_PropertiesID renderer_properties){
        event_handler_function = nullptr;
        tmp_surface = nullptr;
        tmp_texture = nullptr;
        button_texture = nullptr;
        close = false;

        //Initializing SDL
        SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR,"0");
        if (!SDL_Init(SDL_INIT_VIDEO)){
            std::cerr << "Error: Can't initialize SDL " << std::endl;
            exit(1);
        }
        //Creating window
        window = SDL_CreateWindow(
            title.c_str(),
            width,height,
            SDL_WINDOW_RESIZABLE);
        if (!window)
        {
            std::cerr << "Error: Can't create window->" << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
        //Creating Renderer
        if (renderer_properties)
        {
            if (!SDL_SetPointerProperty(renderer_properties,SDL_PROP_RENDERER_CREATE_WINDOW_POINTER,window))
            {
                std::cerr << "Error: Can't set renderer properties -> " << SDL_GetError() << std::endl;
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(1);
            }
            renderer = SDL_CreateRendererWithProperties(renderer_properties);
        }else{
            renderer = SDL_CreateRenderer(window,nullptr);
        }
        if (!renderer)
        {
            std::cerr << "Error: Can't create renderer -> " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }
        


    }
    void Screen::Set_icon(String png_path){
        if(!img_initialized){return;}
        tmp_surface = IMG_Load(png_path.c_str());
        if (!tmp_surface)
        {
            std::cerr << "Error: Can't load window icon "<< png_path << " -> " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetWindowIcon(window,tmp_surface);
    }
    void Screen::Set_renderer_color(Color &color){
        SDL_SetRenderDrawColor(renderer,color.R(),color.G(),color.B(),color.A());
    }
    void Screen::Set_events_handler(void (*event_handler)(SDL_Event &))
    {
        event_handler_function = event_handler;
    }
    void Screen::Create_texture(String texture_name, const uint16_t width, const uint16_t height){
        texture[texture_name] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,height);
        if (texture[texture_name] == nullptr){
            std::cerr << "Error: Can't create texture " << texture_name << " -> " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetTextureScaleMode(texture[texture_name],SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(texture[texture_name],SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer,texture[texture_name]);
    }
    void Screen::Create_button_texture(const uint16_t width, const uint16_t height){
        button_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,height);
        if (button_texture == nullptr){
            std::cerr << "Error: Can't create temporal texture  -> " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetTextureScaleMode(button_texture,SDL_SCALEMODE_NEAREST);
        SDL_SetRenderTarget(renderer,button_texture);
    }
    void Screen::End_texture(){
        SDL_SetRenderTarget(renderer,nullptr);
    }
    void Screen::Init_TTF(String font_name, String font_path, uint32_t font_size)
    {
        if (!ttf_initialized)
        {
            if (!TTF_Init())
            {
                std::cerr << "Can't initialize TTF-> " << SDL_GetError() << std::endl;
                return;
            }
            ttf_initialized = true;
        }
        Load_font(font_name,font_path,font_size);
    }
    void Screen::Load_font(String font_name,String font_path,uint32_t font_size){
        font[font_name] = TTF_OpenFont(font_path.c_str(),font_size);
        if (!font[font_name])
        {
            std::cerr << "Error: Can't load font " << font_name << " from " << font_path << " -> " << SDL_GetError();
            return;
        }
        default_font = font_name;
        
    }
    //Loads png by default
    void Screen::Init_IMG(int flags){
        (void)flags;
        img_initialized = true;
    }
    void Screen::Init_IMG(){
        Init_IMG(0);
    }
    void Screen::Set_default_font(String font_name)
    {
        if (!font[font_name])
        {
            std::cerr << "Font not loaded " << font_name << std::endl;
            return;
        }
        default_font = font_name;
    }
    bool Screen::IMG_initialized(){
        return img_initialized;
    }
    // Task handle
    void Screen::Present_renderer(){
        SDL_RenderPresent(renderer);
    }
    bool Screen::Handle_events(){
        SDL_Event event;
        if (event_handler_function != nullptr)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    close = true;
                }
                event_handler_function(event);
                Handle_buttons(&event);
            }
        }else{
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    close = true;
                }
                Handle_buttons(&event);
            }
        }
        return !close;
    }
    void Screen::Draw_texture(SDL_Texture *texture, SDL_Rect* area){
        if (!texture)
        {
            std::cerr << "Error: texture is a nullptr" << std::endl;
            return;
        }
        SDL_FRect render_area;
        SDL_FRect* render_area_pointer = nullptr;
        if (area)
        {
            render_area = SDL_FRect{static_cast<float>(area->x),static_cast<float>(area->y),static_cast<float>(area->w),static_cast<float>(area->h)};
            render_area_pointer = &render_area;
        }
        SDL_RenderTexture(renderer,texture,nullptr,render_area_pointer);
    }
    void Screen::Draw_saved_texture(String texture_name)
    {
        if (!texture[texture_name]){
            std::cerr << "Error: Texture not loaded " << texture_name << std::endl;
            return;
        }
        SDL_RenderTexture(renderer,texture[texture_name],nullptr,nullptr);
    }
    void Screen::Draw_saved_texture(String texture_name, SDL_Rect *area){
        if (!texture[texture_name]){
            std::cerr << "Error: Texture not loaded " << texture_name << std::endl;
            return;
        }
        SDL_FRect render_area;
        SDL_FRect* render_area_pointer = nullptr;
        if (area)
        {
            render_area = SDL_FRect{static_cast<float>(area->x),static_cast<float>(area->y),static_cast<float>(area->w),static_cast<float>(area->h)};
            render_area_pointer = &render_area;
        }
        SDL_RenderTexture(renderer,texture[texture_name],nullptr,render_area_pointer);
    }
    void Screen::Clean_screen()
    {
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);
    }
    void Screen::Clean_screen(Color &color){
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer,color.R(),color.G(),color.B(),color.A());
        SDL_RenderClear(renderer);
    }
    void Screen::Show_text(
    const uint16_t x, const uint16_t y, 
    String text, Color& color){
        tmp_surface = TTF_RenderText_Solid(
            font[default_font],text.c_str(),0,
            SDL_Color{color.R(),color.G(),color.B(),color.A()});
        if (!tmp_surface){
            std::cerr << "Error: Can't create surface for: " << text << " -> " << SDL_GetError() <<  std::endl;
            return;
        }
        tmp_texture = SDL_CreateTextureFromSurface(renderer,tmp_surface);
        SDL_DestroySurface(tmp_surface);
        tmp_surface = nullptr;
        if (!tmp_texture){
            std::cerr << "Error: Can't create texture for: " << text << " -> " << SDL_GetError() <<  std::endl;
            return;
        }
        SDL_SetTextureScaleMode(tmp_texture,SDL_SCALEMODE_LINEAR);
        float l_width,l_height;
        bool err;
        err = SDL_GetTextureSize(tmp_texture,&l_width,&l_height);
        if (!err){
            std::cerr << "Error: Can't get dimensions for: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        
        SDL_FRect text_area = SDL_FRect{static_cast<float>(x),static_cast<float>(y),l_width,l_height};
        err = SDL_RenderTexture(renderer,tmp_texture,nullptr,&text_area);
        if (!err){
            std::cerr << "Error: Can't copy to renderer: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        SDL_DestroyTexture(tmp_texture);
        tmp_texture = nullptr;
    }

    void Screen::Show_text_size_fixed(
        const uint16_t x, const uint16_t y, 
        const uint16_t width, const uint16_t height, 
        String text, Color &color) {

            tmp_surface = TTF_RenderText_Solid(
                font[default_font], text.c_str(),0,
                SDL_Color{ color.R(), color.G(), color.B(), color.A() }
            );

            if (!tmp_surface) {
                std::cerr << "Error: Can't create surface for text: " << text << " -> " << SDL_GetError() << std::endl;
                return;
            }

            tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
            SDL_DestroySurface(tmp_surface); 
            tmp_surface = nullptr;

            if (!tmp_texture) {
                std::cerr << "Error: Can't create texture for text: " << text << " -> " << SDL_GetError() << std::endl;
                return;
            }
            SDL_SetTextureScaleMode(tmp_texture,SDL_SCALEMODE_NEAREST);

            float text_width, text_height;
            if (!SDL_GetTextureSize(tmp_texture, &text_width, &text_height)) {
                std::cerr << "Error: Can't query texture dimensions for text: " << text << " -> " << SDL_GetError() << std::endl;
                SDL_DestroyTexture(tmp_texture);
                tmp_texture = nullptr;
                return;
            }

            float scale_w = static_cast<float>(width) / text_width;
            float scale_h = static_cast<float>(height) / text_height;
            float scale = std::min(scale_w, scale_h); 

            int final_width = static_cast<int>(text_width * scale);
            int final_height = static_cast<int>(text_height * scale);

            int pos_x = x + (width - final_width) / 2;
            int pos_y = y + (height - final_height) / 2;

            SDL_FRect dst = { static_cast<float>(pos_x), static_cast<float>(pos_y), static_cast<float>(final_width), static_cast<float>(final_height) };

            // Renderizar la textura escalada en el área objetivo
            if (!SDL_RenderTexture(renderer, tmp_texture, nullptr, &dst)) {
                std::cerr << "Error: Can't render text to target: " << text << " -> " << SDL_GetError() << std::endl;
            }

            SDL_DestroyTexture(tmp_texture);
            tmp_texture = nullptr;
    }

    void Screen::Display_buttons(){
        for (auto &&button : buttons)
        {
            button->Display(this);
        }
        
    }

    void Screen::Draw_point(const uint16_t x, const uint16_t y, Color &color){
        Set_renderer_color(color);
        SDL_RenderPoint(renderer,x,y);
    }

    // Draw shapes
    void Screen::Draw_rectangle(
        const uint16_t x, const uint16_t y, 
        const uint16_t width, const uint16_t height, 
        Color& color){
            Set_renderer_color(color);
            SDL_FRect rect = {static_cast<float>(x),static_cast<float>(y),static_cast<float>(width),static_cast<float>(height)};
            SDL_RenderRect(renderer,&rect);
        }
    void Screen::Draw_filled_rectangle(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height, Color &color){
            Set_renderer_color(color);
            SDL_FRect rect = {static_cast<float>(x),static_cast<float>(y),static_cast<float>(width),static_cast<float>(height)};
            SDL_RenderFillRect(renderer,&rect);
        }
    void Screen::Draw_line_angle(
            const uint16_t x, const uint16_t y,
            const float_t angle, const uint16_t length,
            Color &color)
        {
            int x2 = x + length*cos(angle);
            int y2 = y - length*sin(angle);
            Draw_line_pos(x,y,x2,y2,color);
        }
    void Screen::Draw_line_pos(
        const uint16_t x1, const uint16_t y1,
        const uint16_t x2, const uint16_t y2,
        Color &color){
            Set_renderer_color(color);
            SDL_RenderLine(renderer,x1,y1,x2,y2);
        }

        void Screen::Draw_circle(
            const uint16_t x, const uint16_t y, const uint16_t r, 
            const uint16_t resolution, Color &color){

            SDL_FPoint* points = new SDL_FPoint[resolution];
            float angle_dif = (2 * SDL_PI_F) / resolution;

            for (uint16_t i = 0; i < resolution; ++i) {
                float angle = i * angle_dif;  
                points[i].x = x + r * cos(angle);           
                points[i].y = y + r * sin(angle);  
            }

            Set_renderer_color(color);
            SDL_RenderLines(renderer,points,resolution);
            delete[]points;
        }

        void Screen::Draw_filled_circle(
            const uint16_t x, const uint16_t y, const uint16_t r,
            Color &color) 
        {
            for (int16_t dy = -r; dy <= r; ++dy) {
                int16_t y_line = y + dy;
                // Calcula la mitad del ancho visible para esta línea usando la ecuación del círculo
                int16_t dx = static_cast<int16_t>(sqrt(r * r - dy * dy));
        
                // Punto inicial en x: centro - dx
                // Ancho total: 2 * dx + 1 para incluir el pixel central
                Draw_filled_rectangle(x - dx, y_line, 2 * dx + 1, 1, color);
            }
        }

        // Create buttons
        void Screen::Create_simple_button(
            const uint16_t x, const uint16_t y, 
            const uint16_t width, const uint16_t height, 
            String display_text, Color &background_color, Color &foreground_color, 
            void (*event_function)(void)){
                button_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,height);
                if (!button_texture)
                {
                    std::cerr << "Error: Can't create button texture for " << display_text << " -> " << SDL_GetError() << std::endl;
                    return;
                }
                SDL_SetTextureScaleMode(button_texture,SDL_SCALEMODE_NEAREST);
                SDL_SetRenderTarget(renderer,button_texture);
                Set_renderer_color(background_color);
                SDL_RenderClear(renderer);
                Show_text_size_fixed(0,0,width,height,display_text,foreground_color);
                SDL_SetRenderTarget(renderer,nullptr);
                Button* button = new Button(x,y,width,height,button_texture,event_function);
                button_texture = nullptr;
                buttons.push_back(button);


        }
/*
        void Screen::Create_button_from_texture(
            String texture_name, 
            const uint16_t x, const uint16_t y, 
            void (*event_function)(void)){


        }

        void Screen::Create_button_from_tmp_texture(
            const uint16_t x, const uint16_t y, 
            void (*event_function)(void)){


        }
*/
        // Get
        void Screen::Get_window_sizes(int *width, int *height)
        {
            SDL_GetWindowSize(window, width, height);
    }

    SDL_Renderer *Screen::Get_renderer()
    {
        return renderer;
    }

//Destroy
    Screen::~Screen() {
    // freeint textures

    for (auto& pair : texture) {
        if (pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
            pair.second = nullptr;
        }
    }
    texture.clear();

    // Freeing fonts
    for (auto& pair : font) {
        if (pair.second != nullptr) {
            TTF_CloseFont(pair.second);
            pair.second = nullptr;
        }
    }
    font.clear();
    // Destroy tmp_texture
    if (tmp_texture != nullptr) {
        SDL_DestroyTexture(tmp_texture);
        tmp_texture = nullptr;
    }

    // Destroy Renderer
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Destroy Window
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    // Quit TTF
    if (ttf_initialized) {
        TTF_Quit();
        ttf_initialized = false;
    }
    // SDL_image doesn't require explicit shutdown
    if (img_initialized) {
        img_initialized = false;
    }
    // Quit SDL
    SDL_Quit();
}

Screen::Button::Button(
    const uint16_t x, const uint16_t y, 
    const uint16_t width, const uint16_t height, 
    SDL_Texture *button_texture, void (*function_to_run)(void)){
        this->width = width;
        this->heith = height;
        this->x1 = x;
        this->y1 = y;
        this->x2 = x + width;
        this->y2 = y + height;
        texture = button_texture;
        event_function = function_to_run;
}

void Screen::Button::Display(Screen *window)
{
    float tex_width, tex_height;
    if (!SDL_GetTextureSize(texture, &tex_width, &tex_height)) {
        std::cerr << "Error: Can't get button texture dimensions -> " << SDL_GetError() << std::endl;
        return;
    }
    SDL_FRect src = {0,0,tex_width,tex_height};
    SDL_FRect trg = {static_cast<float>(x1),static_cast<float>(y1),static_cast<float>(width),static_cast<float>(heith)};
    SDL_RenderTexture(window->renderer,texture,&src,&trg);
}

void Screen::Button::Run_function(){
    if (event_function != nullptr)
    {
        event_function();
    }
}

bool Screen::Button::In_area(uint16_t x, uint16_t y){
    return ( x1 < x && y1 < y && x2 > x && y2 > y);
}

} // namespace vsr