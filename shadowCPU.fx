float4x4 World;   
float4x4 WorldViewProjection;

struct A2V{ 
    float4 Pos : POSITION;
};

struct V2P{
    float4 Pos : POSITION;    
};

void VS_AMBIENT(in A2V IN, out V2P OUT){
    OUT.Pos = mul(  IN.Pos , WorldViewProjection);  
}

float4 PS( in V2P IN) : COLOR0{
    return float4(1.0, 1.0, 1.0, 0.2);
}

technique PointLight{    
    pass P2{
        VertexShader = compile vs_1_1 VS_AMBIENT();
        PixelShader  = compile ps_2_0 PS();
        
        ZWRITEENABLE = FALSE;

		ZFUNC = LESS;
		
        STENCILENABLE = TRUE;
        STENCILFUNC= ALWAYS;

#if 1
        STENCILPASS = INCR;
        CCW_STENCILPASS = DECR;
        STENCILZFAIL = KEEP;
        CCW_STENCILZFAIL = KEEP;
#else
        STENCILPASS = KEEP;
        CCW_STENCILPASS = KEEP;
        STENCILZFAIL = INCR;
        CCW_STENCILZFAIL = DECR;
#endif
        
        
        AlphaBlendEnable = true;
        SrcBlend = Zero;
        DestBlend = One;
        
        CULLMODE = NONE;
        TWOSIDEDSTENCILMODE = TRUE;
        
        STENCILFAIL = KEEP;
        STENCILREF = 0x1;
        STENCILMASK = 0xff;
        STENCILWRITEMASK = 0xff;

        CCW_STENCILFUNC = ALWAYS;
        CCW_STENCILFAIL = KEEP;
    }
}