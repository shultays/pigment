float3 lightCoor;
float3 lightColor;
float4 ambientColor;

float3 color;

float dx;
float dy;

float lightRange, lightFallOff;

float pU, pV;


float4x4 WorldViewProj; 



texture ColorTex0, ColorTex1, ColorTex2, ColorTex3,
        ColorTex4, ColorTex5, ColorTex6, ColorTex7;

sampler ColorSampler0 = sampler_state{
  Texture = <ColorTex0>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};
sampler ColorSampler1 = sampler_state{
  Texture = <ColorTex1>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};
sampler ColorSampler2 = sampler_state{
  Texture = <ColorTex2>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};

sampler ColorSampler3 = sampler_state{
  Texture = <ColorTex3>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};

sampler ColorSampler4 = sampler_state{
  Texture = <ColorTex4>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};

sampler ColorSampler5 = sampler_state{
  Texture = <ColorTex5>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};

sampler ColorSampler6 = sampler_state{
  Texture = <ColorTex6>;  MinFilter = NONE;  MagFilter = NONE;  MipFilter = NONE;  AddressU = CLAMP;AddressV = CLAMP;
};

struct A2V { 
   float4 Position : POSITION;
   float2 TexCoord : TEXCOORD0;
};

struct V2P{ 
   float4 Position : POSITION;
   float2 TexCoord : TEXCOORD0;
};


void VS(in A2V IN, out V2P OUT)
{
   OUT.Position = IN.Position;
   OUT.TexCoord = IN.TexCoord;
}


float4 clearPS(in V2P IN) : COLOR0
{
  return float4(0.0, 0.0, 0.0, 1.0);
}


float4 NormalPS(in V2P IN) : COLOR0{
  return tex2D( ColorSampler0, IN.TexCoord );
}



float4 EdgePS(in V2P IN) : COLOR0
{

  IN.TexCoord.x += dx/2;
  IN.TexCoord.y += dy/2;
  float x = IN.TexCoord.x;
  float y = IN.TexCoord.y;  
  

  float dist = tex2D( ColorSampler1, float2(x,y) ).w;
  float dx2 = dx - dx*0.5*dist;
  float dy2 = dy - dy*0.5*dist;
  
  
  float4 G =   tex2D( ColorSampler1, float2(x,y-dy2) )
         -     tex2D( ColorSampler1, float2(x,y+dy2) );
         
         
  float4 H =   tex2D( ColorSampler1, float2(x-dx2,y) )
         -     tex2D( ColorSampler1, float2(x+dx2,y) );
      
  
 
  float Color = 1.0;
  x = length(G.xyz);
  y = length(H.xyz);
  
  x = x*x + y*y;
  
  float a = 0.001;
  const float b = 5;
  if(x > a){
    x = 1-saturate((x - a)*b);
  
    Color *= x;
  }
  
  
  
  x = G.w*G.w + H.w*H.w;
  
  a = 0.0001;
  if(x > a){
    x = 1-saturate((x - a)*b);
    Color *= x;
  } 
 
  return float4(Color, Color, Color, 1.0);   
}

float4 CombinePS(in V2P IN):COLOR0{

  IN.TexCoord.x += dx/2;
  IN.TexCoord.y += dy/2;
  
  float4 G =  tex2D( ColorSampler0, IN.TexCoord )*ambientColor
  
  + tex2D( ColorSampler2, IN.TexCoord );
  
  float4 H =  tex2D( ColorSampler1, IN.TexCoord );
 
 
  return G*H;
}

float4 DeathPS(in V2P IN):COLOR0{

  IN.TexCoord.x += dx/2;
  IN.TexCoord.y += dy/2;
  float4 G =  tex2D( ColorSampler0, IN.TexCoord );
  
  if(G.x > color.x || G.y > color.y || G.z > color.z){
	float z = (G.x+G.y+G.z)/3;
	G.x = z;
	G.y = z;
	G.z = z;
  
  }
 
  return G;
}




technique Normal
{

  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  NormalPS();
    ZEnable = false;
  }
}


technique Edge
{
  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  EdgePS();
    ZENABLE = FALSE;
    ALPHABLENDENABLE = FALSE;
  }
}



technique Combine
{
  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  CombinePS();
    ZENABLE = FALSE;
    STENCILENABLE = FALSE;
    
  }
}

struct V2P2{ 
   float4 Position : POSITION;
   float4 TexCoord : TEXCOORD0;
};

void VSLightVolume2(in A2V IN, out V2P2 OUT)
{
   OUT.Position = mul( IN.Position , WorldViewProj);

   OUT.TexCoord = OUT.Position;
   
}

float4 LightPS(in V2P2 IN) : COLOR0{

  
  IN.TexCoord.xy = IN.TexCoord.xy/(2*IN.TexCoord.w) + 0.5;
  IN.TexCoord.y = 1-IN.TexCoord.y;
  
  IN.TexCoord.x += dx/2;
  IN.TexCoord.y += dy/2;
  
  float4 Pos =  tex2D( ColorSampler2, IN.TexCoord );
  float d = min((lightRange-distance(lightCoor, Pos.xyz))/lightFallOff, 1.0);
   
  if(d <= 0) return float4(0.0, 0.0, 0.0, 1.0);
  float4 Normal =  tex2D( ColorSampler1, IN.TexCoord );
  Normal = (Normal - 0.5)*2;
  
  d *= dot( Normal.xyz, normalize(lightCoor-Pos.xyz));
  if(d <= 0) return float4(0.0, 0.0, 0.0, 1.0);
  
  float4 Color =  tex2D( ColorSampler0, IN.TexCoord );
  //Color =  float4(1.0, 1.0, 1.0, 1.0);
  
  Color.x *= lightColor.x*d;
  Color.y *= lightColor.y*d;
  Color.z *= lightColor.z*d;
 
  return Color;

}

technique Light
{
  pass p0
  {
    VertexShader = compile vs_1_1 VSLightVolume2();
    PixelShader = compile ps_2_0  LightPS();
    ZENABLE = TRUE;
    ZWRITEENABLE = FALSE;
    ZFUNC = GREATER;
    
    STENCILENABLE = TRUE;
    STENCILFUNC = EQUAL;
    STENCILREF =  0x00;
    STENCILPASS = KEEP ;
    
    STENCILZFAIL =  KEEP;
    STENCILFAIL =  KEEP;
    STENCILMASK =  0xffffffff;
    STENCILWRITEMASK = 0x00;
    CCW_STENCILZFAIL = KEEP ;  
    CCW_STENCILFAIL=  KEEP ;
    
    cullmode = CW;
    
    AlphaBlendEnable = true;
    SrcBlend = One;
    DestBlend = One;
        
  }
}


technique LightWithoutShadow
{
  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  LightPS();
    ZENABLE = FALSE;
    
  }
}
technique Death{
  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  DeathPS();
    ZENABLE = FALSE;
  }
}

technique Clear{


  pass p0
  {
    VertexShader = compile vs_1_1 VS();
    PixelShader = compile ps_2_0  clearPS();
    ZENABLE = FALSE;
  }
}



void VSLightVolume(in A2V IN,  out V2P OUT)
{
   OUT.Position = mul( IN.Position , WorldViewProj);
}


float4 PSLightVolume(in V2P IN) : COLOR0
{
  return float4(0.0, 0.0, 0.0, 1.0);
}


technique LightVolume{
pass p0
  {
    VertexShader = compile vs_1_1 VSLightVolume();
    PixelShader = compile ps_2_0  PSLightVolume();
    ZWRITEENABLE = FALSE;
    ZENABLE = true;
    
    
    AlphaBlendEnable = true;
    SrcBlend = Zero;
    DestBlend = One;
 
        
    cullmode = CCW;
    TwoSidedStencilMode = false;
    StencilEnable = true;
    StencilRef = 0x10;
    StencilMask = 0x10;
    StencilWriteMask = 0x10;
    
    StencilFunc = Always;
    StencilZFail = REPLACE;
    StencilPass = Keep;
  }
}
