
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "frame.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "../engine.h"

void corpo_pop_up (Corpo corpo, Rectangle retangulo, const char *text) {
    GuiPanel(retangulo, text);

    // quando a massa de cada objeto acabar mudando, fazer um if pra economizar ram pra não mudar cada vez que precisar mostrar o valor
    const char *t_mass = TextFormat("Massa: %.2f\n", corpo.mass); 
    DrawText(t_mass, (int)corpo.pos.x + 50, (int)corpo.pos.y + 100, 12, BLACK);
}
