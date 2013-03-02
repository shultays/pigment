float4x4 World;                 
float4x4 WorldViewProjection;  


float4 ConstantColor;
float3 EyePos;

float Height;

struct A2V
{ 
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex0 : TEXCOORD0;
};

struct V2P{
    float4 Pos : POSITION;
    float2 TexCoord0 : TEXCOORD0;
    float4 Norm: TEXCOORD1;
    float4 Coor: TEXCOORD2;
    float y: TEXCOORD3;
};

void VS(in A2V IN, out V2P OUT){
    OUT.Pos = mul( IN.Pos , WorldViewProjection);
    OUT.TexCoord0 = IN.Tex0;    
    
    OUT.Norm = mul(  float4(IN.Normal,0.0) , World);  
    OUT.Norm.w = saturate(distance(IN.Pos.xyz, EyePos.xyz)/10000); 
     
    OUT.Coor = mul(IN.Pos, World); 
    
    OUT.y = IN.Pos.y;  
}


struct PS_OUT{
  float4 Color : COLOR0;
  float4 Normal : COLOR1;
  float4 Coor : COLOR2;
};


PS_OUT PS_FLAG( in V2P IN){
    PS_OUT out2;
    

	if(Height > IN.y){
		out2.Color = ConstantColor;
	}else{
		
		out2.Color = float4(1.0, 1.0, 1.0, 1.0);
	}
    
      
    out2.Normal = float4(normalize(IN.Norm.xyz)/2+0.5, IN.Norm.w);
    out2.Coor = float4(IN.Coor.xyz, 1.0);
    
    return out2;
}

technique Flag{
   pass P0{
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_FLAG();
        ALPHABLENDENABLE = FALSE;
        CULLMODE = NONE;
    }
}