float inter;

float4x4 World;                 
float4x4 WorldViewProjection;  
texture  Texture0; 


float3 EyePos;

float lifeBar;

float3 Color;

float minY;

float isDead;

sampler textureSampler = sampler_state{
    Texture = <Texture0>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};


struct A2V{ 
    float3 Pos : POSITION0;
    float3 Normal : NORMAL0;
    float2 Tex0 : TEXCOORD0;
    
    float3 Pos2 : POSITION1;
    float3 Normal2 : NORMAL1;
    float2 Tex02 : TEXCOORD1;
    
    float Height : TEXCOORD2;
    
};

struct V2P{
    float4 Pos : POSITION;
    float2 TexCoord0 : TEXCOORD2;
    float4 Norm: TEXCOORD1;
    float4 Coor: TEXCOORD4;
    float Height: TexCOORD5;
};

struct PS_OUT{
  float4 Color : COLOR0;
  float4 Normal : COLOR1;
  float4 Coor : COLOR2;
};

void VS(in A2V IN, out V2P OUT){

    IN.Pos = lerp(IN.Pos,IN.Pos2, inter);
    IN.Normal = lerp(IN.Normal,IN.Normal2, inter);
   
    OUT.Pos = mul(  float4(IN.Pos,1.0) , WorldViewProjection);
    OUT.TexCoord0 = IN.Tex0;    
    
    
    OUT.Norm = mul(  float4(IN.Normal.xyz,0.0) , World);  
    OUT.Norm.w = saturate(distance(IN.Pos.xyz, EyePos.xyz)/10000); 
    
    OUT.Coor =  mul(float4(IN.Pos, 1.0), World);
    
    OUT.Height = IN.Height;

}


PS_OUT PS_TOON( in V2P IN){

	if(IN.Coor.y < minY) clip(-1);
    PS_OUT out2;
    
    if(IN.Height > lifeBar)
      out2.Color = float4(Color, 1.0);
    else{
	  float z = (IN.Height+5-lifeBar)*1.8+1;
	  if(z<1)z=1;
      out2.Color = float4(Color*z/10, 1.0);
    }
      
    
    out2.Normal = float4(normalize(IN.Norm.xyz)/2+0.5, IN.Norm.w);
    
    out2.Coor = IN.Coor ;
    
    return out2;
}


PS_OUT PS_TOON2( in V2P IN){

	
    PS_OUT out2;
    
    
    if(IN.Height > lifeBar){
      out2.Color = float4(Color, 1.0);
    }else{
      out2.Color = float4(1.0, 1.0, 1.0, 0.2);
    }
    
      
      
    out2.Normal = float4(normalize(IN.Norm.xyz)/2+0.5, 1.0);
    
    out2.Coor = IN.Coor ;
    
    
    return out2;
}


technique PointLight{
    pass P0{
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_TOON();
        
        ALPHABLENDENABLE = FALSE;
    }
}


technique PointLight2{
    pass P0{
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_TOON2();
        
        ALPHABLENDENABLE = TRUE;
    }
}


