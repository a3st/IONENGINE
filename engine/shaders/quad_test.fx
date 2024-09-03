Quad {
    ShaderData {
        fullscreenTexture: {
            type: Texture2D,
            default: "assets/debug-empty"
        },

        linearSampler: {
            type: SamplerState
        },

        screenBuffer: {
            type: ConstantBuffer,
            buffer: {
                opacity: {
                    type: float
                },
                alpha: {
                    type: int
                },
                emptyTexture: {
                    type: Texture2D
                }
            }
        }
    }

    Include [
        "engine.fx"
    ]

    VertexShader {
        struct VS_INPUT {
            uint id: SV_VertexID;
        };

        struct VS_OUTPUT {
            float4 position: SV_Position;
            float2 uv: TEXCOORD0;
        };

        VS_OUTPUT vs_main(VS_INPUT input) {
            VS_OUTPUT output;
            output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
            output.uv = float2((input.id << 1) & 2, input.id & 2);
            return output;
        }
    }

    PixelShader {
        struct VS_OUTPUT {
            float4 position: SV_Position;
            float2 uv: TEXCOORD0;
        };

        struct PS_OUTPUT {
            float4 color: SV_Target0;
        };

        PS_OUTPUT vs_main(VS_OUTPUT input) {
            Texture2D fullscreenTexture = GetTexture(fullscreenTexture);
            SamplerState linearSampler = GetSamplerState(linearSampler);

            VS_OUTPUT output;
            output.color = fullscreenTexture.Sample(linearSampler, input.uv);
            return output;
        }
    }

    Technique {
        cullSide: "back",
        depthWrite: false,
        stencilWrite: false
    }
}