#include "graphics/SDL_Visor.cpp"

int main(int argc, char const *argv[])
{
    vsr::Screen Ventana("Prueba",600,600,SDL_RENDERER_ACCELERATED);
    while (Ventana.Handle_events())
    {
        
    }
    
    return 0;
}

