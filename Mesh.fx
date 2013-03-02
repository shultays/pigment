float4x4 World;                 
float4x4 WorldViewProjection;  
texture  Texture0; 

float4 ConstantColor;
float3 EyePos;
sampler textureSampler = sampler_state{
    Texture = <Texture0>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

texture Paint0;

sampler paintSampler = sampler_state{
    Texture = <Paint0>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU = CLAMP;AddressV = CLAMP;
};


texture PaintTexture0;

sampler paintTextureSampler = sampler_state{
    Texture = <PaintTexture0>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

float2 maxUV;
float2 minUV;
float3 PaintColor;


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
};


void VS(in A2V IN, out V2P OUT){
    OUT.Pos = mul( IN.Pos , WorldViewProjection);
    OUT.TexCoord0 = IN.Tex0;    
    
    OUT.Norm = mul(  float4(IN.Normal,0.0) , World);  
    OUT.Norm.w = saturate(distance(IN.Pos.xyz, EyePos.xyz)/10000); 
     
    OUT.Coor = mul(IN.Pos, World);   
}


struct PS_OUT{
  float4 Color : COLOR0;
  float4 Normal : COLOR1;
  float4 Coor : COLOR2;
};

PS_OUT PS_TOON( in V2P IN){
    PS_OUT out2;
    
    float2 pUV = IN.TexCoord0-minUV;
    pUV.x /= maxUV.x;
    pUV.y /= maxUV.y;
    
    float4 f = tex2D(paintSampler, pUV);
    
    if(ConstantColor.a > 0){
      out2.Color = float4(
      ConstantColor.xyz*(1-f.a)+ 
      f.xyz*tex2D(paintTextureSampler, pUV)*f.a, 1.0);
    
    }else{
      float4 c = tex2D(textureSampler, IN.TexCoord0);
      if(c.y < 0.1) clip(-1);
      
      out2.Color = float4(
      c.xyz*(1-f.a)+ 
      f.xyz*tex2D(paintTextureSampler, pUV)*f.a, 1.0);
      
      
    }
    
    /*
    if(out2.Color.g > 0 || out2.Color.b > 0){
      out2.Color.x = (out2.Color.r + out2.Color.g + out2.Color.b)/3;
      out2.Color.y = out2.Color.x;
      out2.Color.z = out2.Color.x;
    }*/
      
    out2.Normal = float4(normalize(IN.Norm.xyz)/2+0.5, IN.Norm.w);
    out2.Coor = float4(IN.Coor.xyz, 1.0);
    
    return out2;
}



technique Mesh{
    
    pass P0{
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_TOON();
        ALPHABLENDENABLE = False;
        
    }
 
}
