#include "./SDL_Visor.cpp"


void button1_fun(){
    std::cout << "Boton 1 ejecutado correctamente" << std::endl;
}

int main(int argc, char const *argv[])
{
    vsr::Screen Ventana("Prueba",220,110,SDL_RENDERER_ACCELERATED);
    Ventana.Init_TTF("NotoSans","fonts/NotoSans/NotoSans-Medium.ttf",20);
    vsr::Color Blanco(255,255,255,255);
    vsr::Color Negro(0,0,0,0);
    vsr::Color Verde_oscuro(0,100,0,0);
    Ventana.Create_simple_button(10,40,100,20,"Soy un boton",Verde_oscuro,Blanco,&button1_fun);

    while (Ventana.Handle_events()){
        Ventana.Clean_screen();
        Ventana.Show_text(10,10,"Hola mundo",Blanco);
        Ventana.Draw_line_pos(10,40,200,60,Blanco);
        Ventana.Display_buttons();
        Ventana.Present_renderer();
        SDL_Delay(200);
    }
    
    return 0;
}


