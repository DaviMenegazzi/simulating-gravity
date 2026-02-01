#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "math.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

// Gravitational constant
double G = 0.9;

// Definindo corpos
//double dt = 0.003;

// Softening para deixar as órbitas mais suaves
double SOFTENING = 1;


int atualiza_sistema_gravidade(Corpo* corpos, int n) {

    double dt = GetFrameTime();
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
        
        double maxVel = 50.0;

        if (corpos[i].vel.x > maxVel) corpos[i].vel.x = maxVel;
        if (corpos[i].vel.x < -maxVel) corpos[i].vel.x = -maxVel;
        if (corpos[i].vel.y > maxVel) corpos[i].vel.y = maxVel;
        if (corpos[i].vel.y < -maxVel) corpos[i].vel.y = -maxVel;
    }
    return 1;
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