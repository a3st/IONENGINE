{
    "name": "Standard",

    "parameters": {
        "color": "vec3(0.1, 0.3, 0.3)"
    }

    "passes": 
    {
        "name": "GBuffer",
        
        "state_parameters": {
            "cull_mode": "Back",
            "fill_mode": "Solid"
        },

        "vertex_shader": "link_file('gbuffer_vertex.hlsl')",
        "pixel_shader": "link_file('gbuffer_pixel.hlsl')"
    },
    {
        "name": "PresentPass",
        
        "state_parameters": {
            "cull_mode": "Back",
            "fill_mode": "Solid"
        },

        "vertex_shader": "link_file('offscreen_vertex.hlsl')",
        "pixel_shader": "link_file('offscreen_pixel.hlsl')"
    }
}
