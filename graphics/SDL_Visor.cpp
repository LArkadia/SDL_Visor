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
    Vector<Vector<Shared_ptr<Color>>> Icon::Get_pxl_grid()
    {
        return pxl_grid;
    }
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
//-----------------------------------------:CLASS SCREEN:--------------------------
//Create and set
    Screen::Screen(String title, uint16_t width,uint16_t heigth,uint32_t renderer_flags){
        event_handler_function = nullptr;
        tmp_surface = nullptr;
        tmp_texture = nullptr;
        ttf_initialized = false;
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
            width,heigth,
            SDL_WINDOW_SHOWN);
        if (!window)
        {
            std::cerr << "Error: Can't create window->" << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
        //Creating Renderer
        renderer = SDL_CreateRenderer(window,-1,renderer_flags);
        if (!renderer)
        {
            std::cerr << "Error: Can't create renderer -> " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }
        


    }
    void Screen::Set_icon(String png_path){
        tmp_surface = IMG_Load(png_path.c_str());
        if (!tmp_surface)
        {
            std::cerr << "Error: Can't load window icon "<< png_path << " -> " << IMG_GetError() << std::endl;
            return;
        }
        SDL_SetWindowIcon(window,tmp_surface);
    }
    void Screen::Set_events_handler(void (*event_handler)(SDL_Event&)){
        event_handler_function = event_handler;
    }
    void Screen::Create_texture(String texture_name, const uint16_t width, const uint16_t heigth){
        texture[texture_name] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width,heigth);
        if (texture[texture_name] == nullptr){
            std::cerr << "Error: Can't create texture " << texture_name << " -> " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetRenderTarget(renderer,texture[texture_name]);
    }
    void Screen::Init_TTF (String font_name,String font_path,uint32_t font_size){
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
    void Screen::Set_default_font(String font_name){
        if (!font[font_name])
        {
            std::cerr << "Font not loaded " << font_name << std::endl;
            return;
        }
        default_font = font_name;
    }
//Task handle
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
            }
        }else{
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    close = true;
                }
            }
        }
        return !close;
    }
    void Screen::Draw_texture(String texture_name){
        if (!texture[texture_name])
        {
            std::cerr << "Error: Texture not loaded " << texture_name << std::endl;
            return;
        }
        SDL_RenderCopy(renderer,texture[texture_name],nullptr,nullptr);
    }
    void Screen::Clean_screen(){
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
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
            font[default_font],text.c_str(),
            SDL_Color{color.R(),color.G(),color.B(),color.A()});
        if (!tmp_surface){
            std::cerr << "Error: Can't create surface for: " << text << " -> " << TTF_GetError() <<  std::endl;
            return;
        }
        tmp_texture = SDL_CreateTextureFromSurface(renderer,tmp_surface);
        //SDL_FreeSurface(tmp_surface);
        tmp_surface = nullptr;
        if (!tmp_texture){
            std::cerr << "Error: Can't create texture for: " << text << " -> " << SDL_GetError() <<  std::endl;
            return;
        }
        int l_width,l_heigth,err;
        err = SDL_QueryTexture(tmp_texture,nullptr,nullptr,&l_width,&l_heigth);
        if (err < 0){
            std::cerr << "Error: Can't get dimensions for: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        
        SDL_Rect text_area = SDL_Rect{x,y,l_width,l_heigth};
        err = SDL_RenderCopy(renderer,tmp_texture,nullptr,&text_area);
        if (err < 0){
            std::cerr << "Error: Can't copy to renderer: " << text << " -> " << SDL_GetError() <<  std::endl;
            SDL_DestroyTexture(tmp_texture);
            return;
        }
        SDL_DestroyTexture(tmp_texture);
        tmp_texture = nullptr;
    }

//Draw shapes
    void Screen::Draw_rectangle(
        const uint16_t x, const uint16_t y, 
        const uint16_t width, const uint16_t heigth, 
        Color& color){}
    void Screen::Draw_line_angle(
        const uint16_t x, const uint16_t y, 
        const float_t angle, const uint16_t length, 
        Color& color){}
    void Screen::Draw_line_pos(
        const uint16_t x1, const uint16_t y1,
        const uint16_t x2, const uint16_t y2,
        Color &color){}
    
//Get
    void Screen::Get_window_sizes(int* width, int* heigth){}

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

    // Free tmp_surface
    if (tmp_surface != nullptr) {
        //SDL_FreeSurface(tmp_surface);
        tmp_surface = nullptr;
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

    // Quit SDL
    SDL_Quit();
}



} // namespace vsr


