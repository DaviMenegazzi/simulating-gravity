#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"

// capacidade e contagem de corpos na simulação
int capacity = 100;
int count = 0;

int qnt_de_corpos = 5;

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
		Corpo corpo = {
			{gera_pos_aleatoria(), gera_pos_aleatoria()}, // posição inicial
			{0, 0}, 									  // velocidade inicial
			rand() % 10000 + 1, 						  // massa inicial aleatória,
			GetRandomColor() 							  // tipo inicial aleatória
		};
		adicionar_corpo_lista(&corpos, &count, &capacity, corpo);
	}
	return corpos;
}

Vector2 GetVirtualMouse(Vector2 mouse, int vw, int vh)
{
    Vector2 v;
    v.x = mouse.x * ((float)vw / GetScreenWidth());
    v.y = mouse.y * ((float)vh / GetScreenHeight());
    return v;
}

int main ()
{

	// Set the window flag for fullscreen mode
    SetConfigFlags(FLAG_FULLSCREEN_MODE); //

	const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    const int virtualScreenWidth = 640;
    const int virtualScreenHeight = 360;

	const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	int monitorWidth = GetMonitorWidth(GetCurrentMonitor()); //
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor()); //
	InitWindow(monitorWidth, monitorHeight, "Simulação");
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
	camera.rotation = 0.0f;
	camera.target = (Vector2){0, 0};
	camera.offset = (Vector2){0, 0};

	int zoomMode = 0;

	// pra renderizar todos os objetos
	RenderTexture2D target = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
	SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
	
	while (!WindowShouldClose())
	{
		// ATUALIZA ZOOM
		if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
		else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

		// se o botão de click continua pressionado
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			Vector2 delta = GetVirtualMouse(GetMouseDelta(), virtualScreenWidth, virtualScreenHeight);
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
		}

		// se o modo de zoom estiver ativo
		if (zoomMode == 0)
        {
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                Vector2 vMouse = GetVirtualMouse(GetMousePosition(), virtualScreenWidth, virtualScreenHeight);
				Vector2 mouseWorldPos = GetScreenToWorld2D(vMouse, camera);

				camera.offset = vMouse;
                camera.target = mouseWorldPos;
                float scale = 0.2f*wheel; 
				camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.25f, 32.0f);
				camera.zoom = roundf(camera.zoom * 8.0f) / 8.0f;
            }
        }
		else
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                Vector2 vMouse = GetVirtualMouse(GetMousePosition(), virtualScreenWidth, virtualScreenHeight);
				Vector2 mouseWorldPos = GetScreenToWorld2D(vMouse, camera);
				camera.offset = vMouse;
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

		BeginTextureMode(target);
			ClearBackground(BLACK);
			BeginMode2D(camera);
				for (int i = 0; i < qnt_de_corpos; i++) {
					DrawCircleV(corpos[i].pos,
								(corpos[i].mass * 0.005f) + 1.0f,
								corpos[i].cor);
				}	
			EndMode2D();
		EndTextureMode();

		BeginDrawing();
			ClearBackground(BLACK);
			DrawTexturePro(
				target.texture,
				(Rectangle){0, 0, virtualScreenWidth, -virtualScreenHeight},
				(Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
				(Vector2){0,0},
				0.0f,
				WHITE
			);
			DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
                Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, WHITE);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
