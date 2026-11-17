#include "iluminacao.h"

#include "rlgl.h"

static const char *VERTICE =
    "#version 330\n"
    "in vec3 vertexPosition;\n"
    "in vec4 vertexColor;\n"
    "uniform mat4 mvp;\n"
    "out vec3 pos;\n"
    "out vec4 cor;\n"
    "void main() {\n"
    "    pos = vertexPosition;\n" /* o rlgl ja entrega os vertices no mundo */
    "    cor = vertexColor;\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

/* A normal sai das derivadas da posicao: nao precisa de normais na malha. */
static const char *FRAGMENTO =
    "#version 330\n"
    "in vec3 pos;\n"
    "in vec4 cor;\n"
    "out vec4 saida;\n"
    "uniform vec3 olho;\n"
    "uniform vec3 nevoa;\n"
    "uniform float blocos;\n"
    "float sorteio(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }\n"
    "void main() {\n"
    "    vec3 rgb = cor.rgb;\n"
    "    float alfa = 1.0;\n"
    "    if (cor.a > 0.998) {\n"
    "        vec3 d = cross(dFdx(pos), dFdy(pos));\n"
    "        vec3 n = length(d) > 1e-9 ? normalize(d) : vec3(0.0, 1.0, 0.0);\n"
    "        if (dot(n, olho - pos) < 0.0) n = -n;\n"
    "        float luz = 0.40 + 0.60 * max(dot(n, normalize(vec3(-0.45, 0.8, 0.4))), 0.0) + 0.10 * n.y;\n"
    "        if (blocos > 0.5) {\n"
    "            vec3 a = abs(n);\n"
    "            vec2 g = (a.y > 0.5 ? pos.xz : (a.x > 0.5 ? pos.zy : pos.xy)) / 1.5;\n"
    "            g.x += 0.5 * mod(floor(g.y), 2.0);\n"
    "            vec2 f = fract(g);\n"
    "            float borda = min(min(f.x, 1.0 - f.x), min(f.y, 1.0 - f.y));\n"
    "            float junta = (1.0 - smoothstep(0.0, 0.04, borda)) * (1.0 - clamp((fwidth(g.x) + fwidth(g.y)) * 3.0, 0.0, 1.0));\n"
    "            luz *= (0.90 + 0.20 * sorteio(floor(g))) * (1.0 - 0.30 * junta);\n"
    "        }\n"
    "        rgb *= luz;\n"
    "    } else if (cor.a > 0.9) {\n"
    "        rgb *= 1.15;\n"
    "    } else {\n"
    "        alfa = cor.a;\n"
    "    }\n"
    "    float f = clamp((length(pos - olho) - 22.0) / 58.0, 0.0, 1.0);\n"
    "    saida = vec4(mix(rgb, nevoa, f), alfa);\n"
    "}\n";

static Shader shader;
static int posOlho = -1, posNevoa = -1, posBlocos = -1;

void iniciarIluminacao(void)
{
    shader = LoadShaderFromMemory(VERTICE, FRAGMENTO);
    posOlho = GetShaderLocation(shader, "olho");
    posNevoa = GetShaderLocation(shader, "nevoa");
    posBlocos = GetShaderLocation(shader, "blocos");
}

void encerrarIluminacao(void)
{
    UnloadShader(shader);
}

void comecarLuz(Camera3D camera, Color nevoa)
{
    const float cor[3] = { nevoa.r / 255.0f, nevoa.g / 255.0f, nevoa.b / 255.0f };

    BeginShaderMode(shader);
    SetShaderValue(shader, posOlho, &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, posNevoa, cor, SHADER_UNIFORM_VEC3);
    definirBlocosLuz(false);
}

void terminarLuz(void)
{
    EndShaderMode();
}

void definirBlocosLuz(bool ativo)
{
    const float valor = ativo ? 1.0f : 0.0f;

    /* O que ja foi pedido usa o valor antigo: descarrega antes de trocar. */
    rlDrawRenderBatchActive();
    SetShaderValue(shader, posBlocos, &valor, SHADER_UNIFORM_FLOAT);
}
