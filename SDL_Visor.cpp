#include "SDL_Visor.hpp"

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
    uint8_t Color::R(){return this->red;}
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
            std::cerr << "Error: Can't load texture from image: " << icon_path << " -> " << IMG_GetError() << std::endl;
        }
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
    /*
    Icon::Icon(const std::string &icon_path)
    {
        if (icon_path.size() >= 4 && icon_path.substr(icon_path.size() - 4) == ".txt") {
            std::ifstream file(icon_path);
            if (!file.is_open()) {
                std::cerr << "Error: file not found -> " << icon_path << std::endl;
                return;
            }

            uint8_t stage = 0;
            std::string line;
            uint16_t line_number = 0;
            std::map<char, std::shared_ptr<Color>> symbol_color;

            while (std::getline(file, line)) {
                if (stage == 0) {
                    // Leer la resolución
                    try {
                        this->resolution = static_cast<uint16_t>(std::stoi(line));
                        this->pxl_grid.resize(this->resolution, std::vector<std::shared_ptr<Color>>(this->resolution));
                        stage = 1;
                    } catch (const std::exception& e) {
                        std::cerr << "Invalid resolution in file: " << icon_path << std::endl;
                        return;
                    }
                }
                else if (stage == 1 && !line.empty()) {
                    // Cargar los colores que representa cada carácter
                    std::vector<std::string> color_data = Split_by_spaces(line);
                    if (color_data.size() >= 5) {
                        // RGBA
                        symbol_color[color_data[0][0]] = std::make_shared<Color>(
                            std::stoi(color_data[1]),
                            std::stoi(color_data[2]),
                            std::stoi(color_data[3]),
                            std::stoi(color_data[4])
                        );
                    }
                    else if (color_data.size() == 4) {
                        // RGB
                        symbol_color[color_data[0][0]] = std::make_shared<Color>(
                            std::stoi(color_data[1]),
                            std::stoi(color_data[2]),
                            std::stoi(color_data[3])
                        );
                    }
                    else if (color_data.size() == 1) {
                        // Transparente o color nulo
                        symbol_color[color_data[0][0]] = nullptr;
                    }
                    else {
                        std::cerr << "Invalid color assignment in file: " << icon_path << std::endl;
                        symbol_color[color_data[0][0]] = nullptr;
                    }
                }
                else if (line.empty()) {
                    // Línea vacía, avanzar de etapa
                    stage++;
                }
                else if (stage == 2 && !line.empty()) {
                    // Cargar los colores en la cuadrícula
                    if (line_number < this->resolution) {
                        for (size_t column = 0; column < line.size() && column < this->resolution; ++column) {
                            char symbol = line[column];
                            auto it = symbol_color.find(symbol);
                            if (it != symbol_color.end()) {
                                this->pxl_grid[line_number][column] = it->second;
                            } else {
                                std::cerr << "Undefined symbol '" << symbol << "' in file: " << icon_path << std::endl;
                                this->pxl_grid[line_number][column] = nullptr;
                            }
                        }
                    }
                    line_number++;
                }
            }
            file.close();
        } else {
            // Manejar otros tipos de archivos (por ejemplo, .png)
            std::cerr << "Unsupported file format: " << icon_path << std::endl;
            // Implementar el manejo de otros formatos si es necesario
        }
    }
    */
    //-----------------------------------------:CLASS SCREEN:--------------------------
    //Internal functions
    void Screen::Handle_buttons(SDL_Event *event){
        if (event->type == SDL_MOUSEBUTTONDOWN)
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


    
    Screen::Screen(String title, uint16_t width,uint16_t height,uint32_t renderer_flags){
        event_handler_function = nullptr;
        tmp_surface = nullptr;
        tmp_texture = nullptr;
        button_texture = nullptr;
        close = false;

        //Initializing SDL
        SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR,"0");
        if (SDL_Init(SDL_INIT_VIDEO) != 0){
            std::cerr << "Error: Can't initialize SDL " << std::endl;
            exit(1);
        }
        //Creating window
        window = SDL_CreateWindow(
            title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width,height,
            SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr << "Error: Can't create window->" << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
        //Creating Renderer
        renderer = SDL_CreateRenderer(window,-1,renderer_flags|SDL_RENDERER_TARGETTEXTURE);
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
            std::cerr << "Error: Can't load window icon "<< png_path << " -> " << IMG_GetError() << std::endl;
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
        SDL_SetTextureBlendMode(texture[texture_name],SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer,texture[texture_name]);
    }
    void Screen::Create_button_texture(const uint16_t width, const uint16_t height){
        button_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,height);
        if (button_texture == nullptr){
            std::cerr << "Error: Can't create temporal texture  -> " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetRenderTarget(renderer,button_texture);
    }
    void Screen::End_texture(){
        SDL_SetRenderTarget(renderer,nullptr);
    }
    void Screen::Init_TTF(String font_name, String font_path, uint32_t font_size)
    {
        if (!ttf_initialized)
        {
            if (TTF_Init() != 0)
            {
                std::cerr << "Can't initialize TTF-> " << TTF_GetError() << std::endl;
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
            std::cerr << "Error: Can't load font " << font_name << " from " << font_path << " -> " << TTF_GetError();
            return;
        }
        default_font = font_name;
        
    }
    //Loads png by default
    void Screen::Init_IMG(int flags){
        img_initialized = true;
        if (!IMG_Init(flags)) {
            std::cerr << "Error: Initialize IMG -> " << IMG_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }
    }
    void Screen::Init_IMG(){
        Init_IMG(IMG_INIT_PNG);
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
                if (event.type == SDL_QUIT)
                {
                    close = true;
                }
                event_handler_function(event);
                Handle_buttons(&event);
            }
        }else{
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
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
        SDL_RenderCopy(renderer,texture,nullptr,area);
    }
    void Screen::Draw_saved_texture(String texture_name)
    {
        if (!texture[texture_name]){
            std::cerr << "Error: Texture not loaded " << texture_name << std::endl;
            return;
        }
        SDL_RenderCopy(renderer,texture[texture_name],nullptr,nullptr);
    }
    void Screen::Draw_saved_texture(String texture_name, SDL_Rect *area){
        if (!texture[texture_name]){
            std::cerr << "Error: Texture not loaded " << texture_name << std::endl;
            return;
        }
        
        SDL_RenderCopy(renderer,texture[texture_name],nullptr,area);
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
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        tmp_surface = TTF_RenderText_Solid(
            font[default_font],text.c_str(),
            SDL_Color{color.R(),color.G(),color.B(),color.A()});
        if (!tmp_surface){
            std::cerr << "Error: Can't create surface for: " << text << " -> " << TTF_GetError() <<  std::endl;
            return;
        }
        tmp_texture = SDL_CreateTextureFromSurface(renderer,tmp_surface);
        SDL_FreeSurface(tmp_surface);
        tmp_surface == nullptr;
        if (!tmp_texture){
            std::cerr << "Error: Can't create texture for: " << text << " -> " << SDL_GetError() <<  std::endl;
            return;
        }
        int l_width,l_height,err;
        err = SDL_QueryTexture(tmp_texture,nullptr,nullptr,&l_width,&l_height);
        if (err < 0){
            std::cerr << "Error: Can't get dimensions for: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        
        SDL_Rect text_area = SDL_Rect{x,y,l_width,l_height};
        err = SDL_RenderCopy(renderer,tmp_texture,nullptr,&text_area);
        if (err < 0){
            std::cerr << "Error: Can't copy to renderer: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        SDL_DestroyTexture(tmp_texture);
        tmp_texture = nullptr;
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    void Screen::Show_text_size_fixed(
        const uint16_t x, const uint16_t y, 
        const uint16_t width, const uint16_t height, 
        String text, Color &color) {

            tmp_surface = TTF_RenderText_Solid(
                font[default_font], text.c_str(),
                SDL_Color{ color.R(), color.G(), color.B(), color.A() }
            );

            if (!tmp_surface) {
                std::cerr << "Error: Can't create surface for text: " << text << " -> " << TTF_GetError() << std::endl;
                return;
            }

            tmp_texture = SDL_CreateTextureFromSurface(renderer, tmp_surface);
            SDL_FreeSurface(tmp_surface); 
            tmp_surface = nullptr;

            if (!tmp_texture) {
                std::cerr << "Error: Can't create texture for text: " << text << " -> " << SDL_GetError() << std::endl;
                return;
            }

            int text_width, text_height;
            if (SDL_QueryTexture(tmp_texture, nullptr, nullptr, &text_width, &text_height) < 0) {
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

            SDL_Rect dst = { pos_x, pos_y, final_width, final_height };

            // Renderizar la textura escalada en el área objetivo
            if (SDL_RenderCopy(renderer, tmp_texture, nullptr, &dst) < 0) {
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
        SDL_RenderDrawPoint(renderer,x,y);
    }

    // Draw shapes
    void Screen::Draw_rectangle(
        const uint16_t x, const uint16_t y, 
        const uint16_t width, const uint16_t height, 
        Color& color){
            Set_renderer_color(color);
            SDL_Rect rect = {x,y,width,height};
            SDL_RenderDrawRect(renderer,&rect);
        }
    void Screen::Draw_line_angle(
        const uint16_t x, const uint16_t y, 
        const float_t angle, const uint16_t length, 
        Color& color){
            int x2 = x + length*cos(angle);
            int y2 = y - length*sin(angle);
            Draw_line_pos(x,y,x2,y2,color);
        }
    void Screen::Draw_line_pos(
        const uint16_t x1, const uint16_t y1,
        const uint16_t x2, const uint16_t y2,
        Color &color){
            Set_renderer_color(color);
            SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
        }

        void Screen::Draw_circle(
            const uint16_t x, const uint16_t y, const uint16_t r, 
            const uint16_t resolution, Color &color){

            SDL_Point* points = new SDL_Point[resolution];
            float angle_dif = (2 * M_PI) / resolution;

            for (uint16_t i = 0; i < resolution; ++i) {
                float angle = i * angle_dif;  
                points[i].x = x + r * cos(angle);           
                points[i].y = y + r * sin(angle);  
            }

            Set_renderer_color(color);
            SDL_RenderDrawLines(renderer,points,resolution);
            delete[]points;
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
    // Quit IMG
    if (img_initialized) {
        IMG_Quit();
        ttf_initialized = false;
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
    int tex_width, tex_height;
    if (SDL_QueryTexture(texture, nullptr, nullptr, &tex_width, &tex_height) < 0) {
        std::cerr << "Error: Can't get button texture dimensions -> " << SDL_GetError() << std::endl;
        return;
    }
    SDL_Rect src = {0,0,tex_width,tex_height};
    SDL_Rect trg = {x1,y1,width,heith};
    SDL_RenderCopy(window->renderer,texture,&src,&trg);
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
