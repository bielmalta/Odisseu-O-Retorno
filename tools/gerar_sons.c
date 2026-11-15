/* Gera os efeitos sonoros do combate por sintese (sem amostras de terceiros).
 *
 * Os arquivos de assets/audio/ foram produzidos por este programa, entao nao
 * ha direitos de autor externos sobre eles. Para recria-los:
 *
 *   gcc -std=c11 tools/gerar_sons.c -o gerar_sons -lm
 *   ./gerar_sons assets/audio
 *
 * Saida: golpe, impacto, bloqueio, defesa e queda (WAV, mono, 16 bits,
 * 22050 Hz). O ruido usa uma semente fixa: rodar de novo gera os mesmos
 * arquivos. */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TAXA 22050
#define PI 3.14159265358979323846

static uint32_t semente = 20241115u;

/* Ruido branco em [-1, 1), deterministico (xorshift). */
static double ruido(void)
{
    semente ^= semente << 13;
    semente ^= semente >> 17;
    semente ^= semente << 5;

    return (double)(semente & 0xFFFFFFu) / (double)0x800000u - 1.0;
}

/* ---- Escrita do WAV ---- */

static void escrever32(FILE *f, uint32_t v)
{
    fputc((int)(v & 0xFF), f);
    fputc((int)((v >> 8) & 0xFF), f);
    fputc((int)((v >> 16) & 0xFF), f);
    fputc((int)((v >> 24) & 0xFF), f);
}

static void escrever16(FILE *f, uint16_t v)
{
    fputc((int)(v & 0xFF), f);
    fputc((int)((v >> 8) & 0xFF), f);
}

/* Normaliza para `pico`, aplica entrada e saida suaves (sem estalos) e grava. */
static int gravar(const char *pasta, const char *nome, double *amostras, int n, double pico)
{
    char caminho[512];
    double maior = 0.0;
    FILE *f;

    for (int i = 0; i < n; i++) {
        const double a = fabs(amostras[i]);

        if (a > maior) {
            maior = a;
        }
    }
    if (maior < 1e-9) {
        return 0;
    }

    for (int i = 0; i < n; i++) {
        const double entrada = fmin(1.0, (double)i / (0.003 * TAXA));
        const double saida = fmin(1.0, (double)(n - 1 - i) / (0.020 * TAXA));

        amostras[i] *= pico / maior * entrada * saida;
    }

    snprintf(caminho, sizeof caminho, "%s/%s.wav", pasta, nome);
    f = fopen(caminho, "wb");
    if (f == NULL) {
        fprintf(stderr, "Nao foi possivel criar %s\n", caminho);
        return 0;
    }

    fwrite("RIFF", 1, 4, f);
    escrever32(f, (uint32_t)(36 + n * 2));
    fwrite("WAVEfmt ", 1, 8, f);
    escrever32(f, 16);
    escrever16(f, 1);               /* PCM */
    escrever16(f, 1);               /* mono */
    escrever32(f, TAXA);
    escrever32(f, TAXA * 2);
    escrever16(f, 2);
    escrever16(f, 16);
    fwrite("data", 1, 4, f);
    escrever32(f, (uint32_t)(n * 2));

    for (int i = 0; i < n; i++) {
        const int v = (int)lrint(amostras[i] * 32767.0);

        escrever16(f, (uint16_t)(int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v));
    }

    fclose(f);
    printf("%s (%d amostras, %.2f s)\n", caminho, n, (double)n / TAXA);
    return 1;
}

/* ---- Os sons ---- */

/* Golpe: um sopro de ar, que abre e fecha. */
static int golpe(const char *pasta)
{
    const double duracao = 0.30;
    const int n = (int)(duracao * TAXA);
    double *s = calloc((size_t)n, sizeof *s);
    double passa = 0.0, lento = 0.0;
    int ok;

    for (int i = 0; i < n; i++) {
        const double t = (double)i / TAXA;
        const double fase = sin(PI * t / duracao);
        const double a = 0.04 + 0.55 * fase;

        passa += (ruido() - passa) * a;
        lento += (passa - lento) * 0.05;
        s[i] = (passa - lento) * fase * fase;
    }

    ok = gravar(pasta, "golpe", s, n, 0.55);
    free(s);
    return ok;
}

/* Impacto: pancada grave com um estalo curto. */
static int impacto(const char *pasta)
{
    const int n = (int)(0.32 * TAXA);
    double *s = calloc((size_t)n, sizeof *s);
    double fase = 0.0, suave = 0.0;
    int ok;

    for (int i = 0; i < n; i++) {
        const double t = (double)i / TAXA;
        const double freq = 45.0 + 115.0 * exp(-t * 18.0);

        fase += 2.0 * PI * freq / TAXA;
        suave += (ruido() - suave) * 0.30;
        s[i] = sin(fase) * exp(-t * 10.0) + suave * 0.55 * exp(-t * 80.0);
    }

    ok = gravar(pasta, "impacto", s, n, 0.90);
    free(s);
    return ok;
}

/* Bloqueio: metal contra metal. */
static int bloqueio(const char *pasta)
{
    const double freq[] = { 410.0, 673.0, 1177.0, 1621.0, 2310.0 };
    const double peso[] = { 1.0, 0.8, 0.6, 0.4, 0.25 };
    const int n = (int)(0.40 * TAXA);
    double *s = calloc((size_t)n, sizeof *s);
    int ok;

    for (int i = 0; i < n; i++) {
        const double t = (double)i / TAXA;
        double v = 0.0;

        for (int k = 0; k < 5; k++) {
            v += peso[k] * sin(2.0 * PI * freq[k] * t) * exp(-t * (10.0 + 6.0 * k));
        }
        s[i] = v + 0.45 * ruido() * exp(-t * 220.0);
    }

    ok = gravar(pasta, "bloqueio", s, n, 0.80);
    free(s);
    return ok;
}

/* Defesa: o escudo sobe, com um toque grave no comeco. */
static int defesa(const char *pasta)
{
    const double duracao = 0.28;
    const int n = (int)(duracao * TAXA);
    double *s = calloc((size_t)n, sizeof *s);
    double fase = 0.0, fase2 = 0.0, suave = 0.0;
    int ok;

    for (int i = 0; i < n; i++) {
        const double t = (double)i / TAXA;
        const double janela = sin(PI * t / duracao);
        const double freq = 220.0 + 300.0 * (t / duracao);

        fase += 2.0 * PI * freq / TAXA;
        fase2 += 2.0 * PI * 80.0 / TAXA;
        suave += (ruido() - suave) * 0.15;
        s[i] = 0.45 * sin(fase) * janela * janela +
               0.35 * suave * janela +
               0.7 * sin(fase2) * exp(-t * 30.0);
    }

    ok = gravar(pasta, "defesa", s, n, 0.60);
    free(s);
    return ok;
}

/* Queda: um corpo pesado tomba e quica uma vez. */
static int queda(const char *pasta)
{
    const int n = (int)(1.00 * TAXA);
    double *s = calloc((size_t)n, sizeof *s);
    double fase = 0.0, fase2 = 0.0, grave = 0.0;
    int ok;

    for (int i = 0; i < n; i++) {
        const double t = (double)i / TAXA;
        const double t2 = t > 0.32 ? t - 0.32 : 0.0;
        double v;

        fase += 2.0 * PI * (35.0 + 40.0 * exp(-t * 6.0)) / TAXA;
        fase2 += 2.0 * PI * (38.0 + 30.0 * exp(-t2 * 8.0)) / TAXA;
        grave += (ruido() - grave) * 0.02;

        v = sin(fase) * exp(-t * 7.0);
        if (t > 0.32) {
            v += 0.6 * sin(fase2) * exp(-t2 * 9.0);
        }
        s[i] = v + 6.0 * grave * exp(-t * 3.0);
    }

    ok = gravar(pasta, "queda", s, n, 0.90);
    free(s);
    return ok;
}

int main(int argc, char **argv)
{
    const char *pasta = argc > 1 ? argv[1] : ".";

    return (golpe(pasta) && impacto(pasta) && bloqueio(pasta) &&
            defesa(pasta) && queda(pasta)) ? 0 : 1;
}
