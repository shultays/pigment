

float3   g_vLight;     

float4x4 g_mWorld;
float4x4 g_mViewProjection;  
    


void VertShadowVolume( float4 vPos : POSITION,
                       float3 vNormal : NORMAL,
                       out float4 oPos : POSITION ){
    vPos = mul( vPos, g_mWorld );
    vNormal = mul( float4(vNormal.xyz, 0.0), g_mWorld );
    
    float3 LightVecView = normalize(g_vLight - vPos.xyz);

    vPos.xyz -= LightVecView*4;
    if( dot( normalize(vNormal), LightVecView ) > 0.0f ){
        vPos.xyz -= LightVecView*20000;
    }
    
    oPos = mul( vPos, g_mViewProjection );
}


float4 PixShadowVolume() : COLOR0{
    return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}


technique RenderShadowVolume2Sided
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertShadowVolume();
        PixelShader  = compile ps_2_0 PixShadowVolume();
        CullMode = None;
        // Disable writing to the frame buffer
       
        AlphaBlendEnable = true;
        SrcBlend = Zero;
        DestBlend = One;
        
        
        // Disable writing to depth buffer
        ZWriteEnable = false;
        ZFunc = Less;
        // Setup stencil states
        TwoSidedStencilMode = true;
        StencilEnable = true;
        StencilRef = 1;
        StencilMask = 0x0F;
        StencilWriteMask = 0x0F;
        Ccw_StencilFunc = Always;
        Ccw_StencilZFail = Incr;
        Ccw_StencilPass = Keep;
        StencilFunc = Always;
        StencilZFail = Decr;
        StencilPass = Keep;
    }
}
