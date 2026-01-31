#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	Vector2 pos;
	Vector2 vel;
	double mass;
} Corpo;

// Gravitational constant
const double G = 1;

// Definindo corpos
const double dt = 0.1;

void atualiza_sistema_gravidade(Corpo* corpos, int n) {
	
    for (int i = 0; i < n; i++) {

        float ax = 0;
        float ay = 0;
        double maxVel = 50.0;

        for (int j = 0; j < n; j++) {

            if (i == j) continue;

            float dx = corpos[j].pos.x - corpos[i].pos.x;
            float dy = corpos[j].pos.y - corpos[i].pos.y;

			float distSq = dx*dx + dy*dy + 0.01f;
            float dist = sqrt(distSq);

            float f = G * corpos[j].mass / distSq;

            ax += f * dx / dist;
            ay += f * dy / dist;
        }

        corpos[i].vel.x += ax * dt;
        corpos[i].vel.y += ay * dt;

        if (corpos[i].vel.x > maxVel) corpos[i].vel.x = maxVel;
		if (corpos[i].vel.x < -maxVel) corpos[i].vel.x = -maxVel;

		if (corpos[i].vel.y > maxVel) corpos[i].vel.y = maxVel;
		if (corpos[i].vel.y < -maxVel) corpos[i].vel.y = -maxVel;
    }

    for (int i = 0; i < n; i++) {
        corpos[i].pos.x += corpos[i].vel.x * dt;
        corpos[i].pos.y += corpos[i].vel.y * dt;
    }
}

void adicionar_corpo_lista(Corpo** arr, int* count, int* capacity, Corpo b) {
	if (*count >= *capacity) { // se o valor de count for maior ou igual ao valor da capacidade...
        *capacity *= 2; // dobra o tamanho da capacidade
        *arr = realloc(*arr, (*capacity) * sizeof(Corpo)); // realoca a memória pro array
    }

    (*arr)[*count] = b; // coloca o próximo corpo no próximo lugar disponível
    printf("Adicionado corpo de massa %f no array.\n", b.mass);
    (*count)++; // seta o valor do próximo lugar disponível
}


int main ()
{
	int WIDTH = 1280;
	int HEIGHT = 800;

	int capacity = 100;
	int count = 0;

	Corpo* corpos = malloc(capacity * sizeof(Corpo)); // realoca memória para capacidade disponível

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
		10000
	};

	Corpo planeta = {
		{WIDTH/2, HEIGHT/2-300},
		{0.01, 0},
		10
	};

	Corpo planeta2 = {
		{WIDTH/2-300, HEIGHT/2},
		{0.01, 0},
		5
	};
	

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

    adicionar_corpo_lista(&corpos, &count, &capacity, sol);
    adicionar_corpo_lista(&corpos, &count, &capacity, planeta);
    adicionar_corpo_lista(&corpos, &count, &capacity, planeta2);
	
	// game loop
	while (!WindowShouldClose()) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		atualiza_sistema_gravidade(corpos, count);

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
		DrawCircleV(corpos[0].pos, 20, WHITE); 

		// DESENHA O PLANETA
		DrawCircleV(corpos[1].pos, 5, RED); 

		// DESENHA O PLANETA2
		DrawCircleV(corpos[2].pos, 7, GREEN); 

		// desenha o trail
		DrawTextureRec(
			trail.texture,
			(Rectangle){0, 0, (float)WIDTH, -(float)HEIGHT},
			(Vector2){0, 0},
			WHITE
		);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	UnloadRenderTexture(trail);
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
