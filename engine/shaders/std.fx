
sampleTexture: <Texture: type> (texture: Texture, samplerState: SamplerState, location: type) -> type = {
    textureObject: Texture = texture;
    samplerStateObject: SamplerState = samplerState;

    hlsl {
        textureObject.Sample(samplerStateObject, location)
    }
}