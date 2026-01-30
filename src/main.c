#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "math.h"
#include <stdio.h>

typedef struct {
	Vector2 pos;
	Vector2 vel;
	double mass;
} Corpo;

// Gravitational constant
const double G = 6.674e-11;

void atualiza_posicao_planeta(Corpo* planeta, Corpo* sol, double dt) {
	double dx = sol->pos.x - planeta->pos.x;
	double dy = sol->pos.y - planeta->pos.y;
	double distSq = dx*dx + dy*dy;
	double dist = sqrt(distSq);

	// Fórmula da força entre massas de newton
	double force = (G * planeta->mass * sol->mass) / distSq;

	// Aceleração do planeta
	double ax = G * planeta->mass * dx / distSq;
	double ay = G * planeta->mass * dy / distSq;

	// Atualiza a velocidade
	planeta->vel.x += ax * dt;
	planeta->vel.y += ay * dt;

	// atualiza a posição
	planeta->pos.x += planeta->vel.x * dt;
	planeta->pos.y += planeta->vel.y * dt;
}


int main ()
{
	int WIDTH = 1280;
	int HEIGHT = 800;

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "Simulação");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");


	// Definindo corpos
	Corpo sol = {
		{WIDTH/2, HEIGHT/2},
		{0, 0},
		1989000
	};

	Corpo planeta = {
		{WIDTH/2, HEIGHT/2-300},
		{0.0001, 0},
		5972
	};

	Corpo planeta2 = {
		{WIDTH/2-300, HEIGHT/2},
		{0.0001, 0},
		10000
	};
	// Definindo corpos

	double dt = 4000;

	//trail
	Vector2 pos = {WIDTH/2, HEIGHT/2-300};
    Vector2 prev = pos;

	//trail2
	Vector2 pos2 = {WIDTH/2, HEIGHT/2-300};
    Vector2 prev2 = pos2;

	RenderTexture2D trail = LoadRenderTexture(WIDTH, HEIGHT);

	// Limpa a textura UMA vez
    BeginTextureMode(trail);
        ClearBackground(BLANK);
    EndTextureMode();
	
	// game loop
	while (!WindowShouldClose()) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		sol.pos.x = GetMouseX();
		sol.pos.y = GetMouseY();

		pos.x = planeta.pos.x;
		pos.y = planeta.pos.y;

		pos2.x = planeta2.pos.x;
		pos2.y = planeta2.pos.y;

		// Desenha apenas o novo segmento no trail
        BeginTextureMode(trail);
			// aplica fade no que já existe
    		DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.008f));
            DrawCircleV(pos, 1, GREEN);

			// aplica fade no que já existe
    		DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.008f));
            DrawCircleV(pos2, 1, GREEN);
        EndTextureMode();

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// DESENHA O SOL
		DrawCircleV(sol.pos, 20, WHITE); 

		// DESENHA O PLANETA
		DrawCircleV(planeta.pos, 5, RED); 

		// DESENHA O PLANETA2
		DrawCircleV(planeta2.pos, 7, GREEN); 

		// desenha o trail
		DrawTextureRec(
			trail.texture,
			(Rectangle){0, 0, (float)WIDTH, -(float)HEIGHT},
			(Vector2){0, 0},
			WHITE
		);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

		atualiza_posicao_planeta(&planeta, &sol, dt);
		atualiza_posicao_planeta(&planeta2, &sol, dt);
	}

	UnloadRenderTexture(trail);
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
