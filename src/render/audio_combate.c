#include "audio_combate.h"

static Sound carregarSom(const char *arquivo)
{
    const char *caminho = TextFormat("%sassets/audio/%s", GetApplicationDirectory(), arquivo);

    if (!FileExists(caminho)) {
        TraceLog(LOG_WARNING, "Som nao encontrado: %s", caminho);
        return (Sound){0};
    }

    return LoadSound(caminho);
}

bool iniciarAudioCombate(EfeitosSonoros *efeitos)
{
    *efeitos = (EfeitosSonoros){0};

    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "Sem dispositivo de audio: o jogo seguira em silencio.");
        return false;
    }

    efeitos->dispositivoPronto = true;
    efeitos->golpe = carregarSom("golpe.wav");
    efeitos->impacto = carregarSom("impacto.wav");
    efeitos->bloqueio = carregarSom("bloqueio.wav");
    efeitos->defesa = carregarSom("defesa.wav");
    efeitos->queda = carregarSom("queda.wav");
    return true;
}

static void tocar(Sound som)
{
    if (IsSoundValid(som)) {
        PlaySound(som);
    }
}

void tocarCue(const EfeitosSonoros *efeitos, const CueAnimacao *cue)
{
    if (!efeitos->dispositivoPronto) {
        return;
    }

    switch (cue->tipo) {
    case CUE_INICIO_GOLPE:
        tocar(efeitos->golpe);
        break;
    case CUE_IMPACTO:
        tocar(cue->evento.alvoDefendia ? efeitos->bloqueio : efeitos->impacto);
        break;
    case CUE_DEFESA_ATIVADA:
        tocar(efeitos->defesa);
        break;
    case CUE_QUEDA:
        tocar(efeitos->queda);
        break;
    }
}

static void descarregar(Sound som)
{
    if (IsSoundValid(som)) {
        UnloadSound(som);
    }
}

void encerrarAudioCombate(EfeitosSonoros *efeitos)
{
    if (!efeitos->dispositivoPronto) {
        return;
    }

    descarregar(efeitos->golpe);
    descarregar(efeitos->impacto);
    descarregar(efeitos->bloqueio);
    descarregar(efeitos->defesa);
    descarregar(efeitos->queda);
    CloseAudioDevice();
    *efeitos = (EfeitosSonoros){0};
}
