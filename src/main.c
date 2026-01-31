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
const double G = 0.9;

// Definindo corpos
const double dt = 0.3;

const double SOFTENING = 1;

void atualiza_sistema_gravidade(Corpo* corpos, int n) {

	static Vector2 acc[1024];

	// zera aceleração
	for (int i = 0; i < n; i++) {
		acc[i] = (Vector2){0,0};
	}
	
	// calcula aceleração
    for (int i = 0; i < n; i++) {
        double maxVel = 50.0;
        for (int j = 0; j < n; j++) {

            if (i == j) continue;

            float dx = corpos[j].pos.x - corpos[i].pos.x;
            float dy = corpos[j].pos.y - corpos[i].pos.y;

			float distSq = dx*dx + dy*dy + SOFTENING*SOFTENING;

			double invDist = 1.0 / sqrt(distSq);
			double invDist3 = invDist * invDist * invDist;

            acc[i].x += G * corpos[j].mass * dx * invDist3;
            acc[i].y += G * corpos[j].mass * dy * invDist3;
        }

		// meia etapa velocidade
        corpos[i].vel.x += acc[i].x * dt * 0.5;
        corpos[i].vel.y += acc[i].y * dt * 0.5;

        if (corpos[i].vel.x > maxVel) corpos[i].vel.x = maxVel;
		if (corpos[i].vel.x < -maxVel) corpos[i].vel.x = -maxVel;

		if (corpos[i].vel.y > maxVel) corpos[i].vel.y = maxVel;
		if (corpos[i].vel.y < -maxVel) corpos[i].vel.y = -maxVel;
    }

	// posição
    for (int i = 0; i < n; i++) {
        corpos[i].pos.x += corpos[i].vel.x * dt;
        corpos[i].pos.y += corpos[i].vel.y * dt;
    }

	// zera aceleração novamente
	for (int i = 0; i < n; i++) {
		acc[i] = (Vector2){0,0};
	}

	// recalcula aceleração
    for (int i = 0; i < n; i++) {
        double maxVel = 50.0;
        for (int j = 0; j < n; j++) {

            if (i == j) continue;

            float dx = corpos[j].pos.x - corpos[i].pos.x;
            float dy = corpos[j].pos.y - corpos[i].pos.y;

			float distSq = dx*dx + dy*dy + SOFTENING*SOFTENING;

			double invDist = 1.0 / sqrt(distSq);
			double invDist3 = invDist * invDist * invDist;

            acc[i].x += G * corpos[j].mass * dx * invDist3;
            acc[i].y += G * corpos[j].mass * dy * invDist3;
        }
    }

	// completa velocidade
    for (int i=0;i<n;i++) {
        corpos[i].vel.x += acc[i].x * dt * 0.5;
        corpos[i].vel.y += acc[i].y * dt * 0.5;
    }
}

int encontrar_dominante (Corpo corpos[], int n, int i) {

	int best = -1;
	double bestScore = 0;

	for (int j = 0; j < n; j++) {
		if (i == j) continue;
		
		double dx = corpos[j].pos.x - corpos[i].pos.x;
        double dy = corpos[j].pos.y - corpos[i].pos.y;

		double distSq = dx * dx + dy * dy;

		double score = corpos[j].mass / distSq;

		if (score > bestScore) {
			bestScore = score;
			best = j;
		}
	}
	return best;
}

void aplicar_orbita (Corpo corpos[], int i, int j) {
	double dx = corpos[i].pos.x - corpos[j].pos.x;
    double dy = corpos[i].pos.y - corpos[j].pos.y;

    double r = sqrt(dx*dx + dy*dy);

    if (r < 1) return;

    double v = sqrt(G * corpos[j].mass / r);

    double nx = -dy / r;
    double ny =  dx / r;

    corpos[i].vel.x = nx * v;
    corpos[i].vel.y = ny * v;
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


int gera_pos_aleatoria () {
	return rand() % 300 + 1; 
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
		{gera_pos_aleatoria(), gera_pos_aleatoria()},
		{0.1, 0},
		100
	};

	Corpo planeta2 = {
		{gera_pos_aleatoria(), gera_pos_aleatoria()},
		{0.1, 0},
		50
	};
	
	//trail
	Vector2 pos = corpos[2].pos;
    Vector2 prev = pos;

	//trail2
	Vector2 pos2 = corpos[1].pos;
    Vector2 prev2 = pos2;

	RenderTexture2D trail = LoadRenderTexture(WIDTH, HEIGHT);

	// Limpa a textura UMA vez
    BeginTextureMode(trail);
        ClearBackground(BLANK);
    EndTextureMode();

    adicionar_corpo_lista(&corpos, &count, &capacity, sol);
    adicionar_corpo_lista(&corpos, &count, &capacity, planeta);
    adicionar_corpo_lista(&corpos, &count, &capacity, planeta2);

	for (int i = 0; i <	count; i++) {
		int j = encontrar_dominante(corpos, count, i);
		if (j != -1) {
			aplicar_orbita(corpos, i, j);
		}
	}
	
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
