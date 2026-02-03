#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "gui/frame.h"

// tamanho da tela
const int WIDTH = 1280;
const int HEIGHT = 800;

// capacidade e contagem de corpos na simulação
int capacity = 100;
int count = 0;

int qnt_de_corpos = 5;

int id_selecionado = -1;

int gera_pos_aleatoria () {
	return rand() % 300 + 1; 
}

Color GetRandomColor() {
    unsigned char r = (unsigned char)GetRandomValue(0, 255);
    unsigned char g = (unsigned char)GetRandomValue(0, 255);
    unsigned char b = (unsigned char)GetRandomValue(0, 255);
    unsigned char a = 255;

    Color randomColor = { r, g, b, a };
    return randomColor;
}

Corpo* gerar_corpos (int n_de_corpos) {
	Corpo* corpos = malloc(capacity * sizeof(Corpo)); // realoca memória para capacidade disponível
	
	for (int i = 0; i < n_de_corpos; i++) {
		double massa = rand() % 10000 + 1;
		double r_tam_corpo = (massa * 0.005f) + 1.0f;
		Vector2 vel_inicial = {0, 0};
		Vector2 pos_inicial = {gera_pos_aleatoria(), gera_pos_aleatoria()};
		Color cor = GetRandomColor();
		Corpo corpo = {
			pos_inicial,  			// posição inicial
			vel_inicial, 			// velocidade inicial
			massa, 					// massa inicial aleatória,
			r_tam_corpo,			// tamanho do corpo
			cor 					// tipo inicial aleatória
		};
		adicionar_corpo_lista(&corpos, &count, &capacity, corpo);
	}
	return corpos;
}

int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(WIDTH, HEIGHT, "Simulação");
	SearchAndSetResourceDir("resources");

	Corpo* corpos = gerar_corpos(qnt_de_corpos);

	for (int i = 0; i <	count; i++) {
		int j = encontrar_dominante(corpos, count, i);
		if (j != -1) {
			aplicar_orbita(corpos, i, j);
		}
	}

	Camera2D camera = { 0 };
    camera.zoom = 1.0f;

	int zoomMode = 0;
	
	while (!WindowShouldClose())
	{
		// ATUALIZA ZOOM
		if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
		else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

		// SE CLICADO
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 posicaoClick = GetScreenToWorld2D(GetMousePosition(), camera);
			for(int i = 0; i < qnt_de_corpos; i++) {
				
				double dx = posicaoClick.x-corpos[i].pos.x;
				double dy = posicaoClick.y-corpos[i].pos.y;
				double distSq = dx*dx + dy*dy;
				
				if (sqrt(distSq) < corpos[i].tam) {
					// salva o index
					id_selecionado = i;
				}
			}
		}

		// se o botão de click continua pressionado
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
		}

		// se o modo de zoom estiver ativo
		if (zoomMode == 0)
        {
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;
                float scale = 0.2f*wheel;
                camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            }
        }
		else
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
                camera.offset = GetMousePosition();
                camera.target = mouseWorldPos;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                float deltaX = GetMouseDelta().x;
                float scale = 0.005f*deltaX;
                camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            }
        }
		
		atualiza_sistema_gravidade(corpos, count);

		BeginDrawing();
			ClearBackground(BLACK);
			BeginMode2D(camera);
				for (int i = 0; i < qnt_de_corpos; i++) {
					DrawCircleV(corpos[i].pos, corpos[i].tam, corpos[i].cor);
					if (id_selecionado != -1) {
						DrawCircleV(corpos[id_selecionado].pos, corpos[id_selecionado].tam + 2, WHITE);
					}
				}
			EndMode2D();
			if (id_selecionado != -1) {
				Vector2 pos_panel = GetWorldToScreen2D(corpos[id_selecionado].pos, camera);
				corpo_pop_up(
					corpos[id_selecionado],
					(Rectangle) {
						pos_panel.x, pos_panel.y,
						100, 100
					},
					NULL
				);
			}
			DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
                Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, WHITE);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
