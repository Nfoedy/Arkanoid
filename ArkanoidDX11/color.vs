// Input del Vertex Shader.
// Deve combaciare con i dati che manderemo dalla CPU.
// Per ora ogni vertice avrà:
// - position
// - color
struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


// Vertex Shader principale. Riceve un vertice in input e prepara i dati per la pipeline grafica
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

    output.position = float4(input.position, 1.0f);

    output.color = input.color;

    return output;
}