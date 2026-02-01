#ifndef ENGINE_H
#define ENGINE_H

extern double G;
//extern double dt;
extern double SOFTENING;

typedef struct {
	Vector2 pos;
	Vector2 vel;
	double mass;
	Color cor;
} Corpo;

int atualiza_sistema_gravidade(Corpo* corpos, int n);
int encontrar_dominante (Corpo corpos[], int n, int i);
void aplicar_orbita (Corpo corpos[], int i, int j);
void adicionar_corpo_lista(Corpo** arr, int* count, int* capacity, Corpo b);

#endif