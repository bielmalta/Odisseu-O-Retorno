#ifndef AUDIO_COMBATE_H
#define AUDIO_COMBATE_H

#include <stdbool.h>

#include "raylib.h"

#include "animacao_combate.h"

/* Efeitos sonoros do combate (Raylib).
 * Cada som toca no marco da animacao a que pertence (CueAnimacao), entao o
 * som e a imagem acontecem juntos. Sem placa de som ou sem os arquivos o
 * jogo segue normalmente, apenas em silencio. */

typedef struct {
    bool dispositivoPronto;
    Sound golpe;     /* o golpe comeca */
    Sound impacto;   /* o golpe acerta */
    Sound bloqueio;  /* o golpe acerta uma guarda */
    Sound defesa;    /* alguem levanta a guarda */
    Sound queda;     /* alguem tomba */
} EfeitosSonoros;

/* Abre o dispositivo de audio e carrega os arquivos .wav de assets/audio
 * (LoadSound), a partir da pasta do executavel. Chamar depois de
 * InitWindow(). Devolve false se nao houver dispositivo de audio. */
bool iniciarAudioCombate(EfeitosSonoros *efeitos);

/* Toca o som do marco (PlaySound). */
void tocarCue(const EfeitosSonoros *efeitos, const CueAnimacao *cue);

/* Descarrega os sons e fecha o dispositivo. */
void encerrarAudioCombate(EfeitosSonoros *efeitos);

#endif
