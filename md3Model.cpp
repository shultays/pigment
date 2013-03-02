#include "md3Model.h"

#include <math.h>


void md3Model::load(char *file, char *skin, int s){
     
  fstream fp;

  int i,j;

  fp.open(file, fstream::binary | fstream::in);

  fp.read((char*)&h, sizeof(header));
  
  //fread (&h, sizeof (header), 1, fp);
  
  frames = new frame[h.framenum];
  fp.seekp(h.frameoff);
  //fseek(fp, h.frameoff, SEEK_SET);
  fp.read((char*)frames, sizeof(frame)*h.framenum);
  //fread (frames, sizeof (frame),  h.framenum, fp);
  
  tags = new tag[h.framenum*h.tagnum];
  fp.seekp(h.tagoff);
  //fseek(fp, h.tagoff, SEEK_SET);
  fp.read((char*)tags, sizeof(tag)*h.framenum*h.tagnum);
  //fread (tags, sizeof (tag),  h.framenum*h.tagnum, fp);  

  surfaces = new surface[h.surfacenum];
  sel = new surfaceelements[h.surfacenum];
  v_buffer = new LPDIRECT3DVERTEXBUFFER9[h.surfacenum];
  height_buffer = new LPDIRECT3DVERTEXBUFFER9[h.surfacenum];
  i_buffer = new LPDIRECT3DINDEXBUFFER9[h.surfacenum];
  allver = new customvertex*[h.surfacenum];

  indices = new short*[h.surfacenum];
  vertnum = new int[h.surfacenum];
  trianglenum = new int[h.surfacenum];

  fp.seekp(h.surfaceoff);
  //fseek(fp, h.surfaceoff, SEEK_SET);

  min = 10000000000; 
  max = -10000000000;

  for(i=0; i<h.surfacenum; i++){
       
    fp.read((char*)(surfaces+i), sizeof(surface));
    //fread(surfaces+i, sizeof(surface), 1, fp);
    
    sel[i].shaders = new shader[surfaces[i].numshaders];
    
    fp.read((char*)(sel[i].shaders), sizeof(shader)*surfaces[i].numshaders);
    //fread(sel[i].shaders, sizeof(shader), surfaces[i].numshaders, fp);
    
    sel[i].triangles = new triangle[surfaces[i].numtris];
    fp.read((char*)(sel[i].triangles), sizeof(triangle)*surfaces[i].numtris);
    //fread(sel[i].triangles, sizeof(triangle), surfaces[i].numtris, fp);
 

    sel[i].texcoords = new texcoord[surfaces[i].numverts];
    fp.read((char*)(sel[i].texcoords), sizeof(texcoord)*surfaces[i].numverts);
    //fread(sel[i].texcoords, sizeof(texcoord), surfaces[i].numverts, fp);

    sel[i].vertices = new vertex[surfaces[i].numverts*surfaces[i].numframes];
    fp.read((char*)(sel[i].vertices), sizeof(vertex)*surfaces[i].numverts*surfaces[i].numframes);
    //fread(sel[i].vertices, sizeof(vertex), surfaces[i].numverts*surfaces[i].numframes, fp);
    
    //for(j=0; j<surfaces[i].numverts*surfaces[i].numframes; j++)
    //printf("t %d %d %d %d\n", sel[i].vertices[j].x,
    //                      sel[i].vertices[j].y,sel[i].vertices[j].z, sel[i].vertices[j].normal );
    
    

    trianglenum[i] = surfaces[i].numtris;
    vertnum[i] = surfaces[i].numverts;

    allver[i] = new customvertex[surfaces[i].numverts*surfaces[i].numframes];
    //tempver[i] = new customvertex[surfaces[i].numverts];

    int total = surfaces[i].numverts*surfaces[i].numframes;//*surfaces[select].numframes;
    float carp = 1.0/64;
    for(j=0; j<total; j++){
      allver[i][j].coors[0] = sel[i].vertices[j].x*carp;
      allver[i][j].coors[2] = sel[i].vertices[j].y*carp;
      allver[i][j].coors[1] = sel[i].vertices[j].z*carp;

      float lat = ((sel[i].vertices[j].normal>>8)&255)*2*PI/255;
      float lng = ((sel[i].vertices[j].normal)&255)*2*PI/255;


      allver[i][j].normal[0] = cos ( lat ) * sin ( lng );
      allver[i][j].normal[2] = sin ( lat ) * sin ( lng );
      allver[i][j].normal[1] = cos ( lng );

     
      allver[i][j].UV[0] = sel[i].texcoords[j%surfaces[i].numverts].U;
      allver[i][j].UV[1] = sel[i].texcoords[j%surfaces[i].numverts].V;

     // PFILE3("<<%f %f %f\n",  allver[i].UV[0],  allver[i].UV[1],  allver[i].normal[2]);
    }

    int a = Tools::D3DDevice->CreateVertexBuffer(total*sizeof(customvertex),
                               0,
                               CUSTOMFVF,
                               D3DPOOL_MANAGED,
                               &v_buffer[i],
                               NULL);


    VOID *pVoid;
    
    // lock v_buffer and load the vertices into it
    while(v_buffer[i]->Lock(0, 0, (void**)&pVoid, 0));
    memcpy(pVoid, allver[i], total*sizeof(customvertex));
    v_buffer[i]->Unlock();


    indices[i] = new short[surfaces[i].numtris*3];
    for(j=0; j<surfaces[i].numtris; j++){
      indices[i][j*3] = sel[i].triangles[j].t0;
      indices[i][j*3+1] = sel[i].triangles[j].t1;
      indices[i][j*3+2] = sel[i].triangles[j].t2;

      //PFILE3("%d %d %d\n", indices[i*3], indices[i*3+1], indices[i*3+2]);
    }

    a = Tools::D3DDevice->CreateIndexBuffer(surfaces[i].numtris*3*sizeof(short),
                              0,
                              D3DFMT_INDEX16,
                              D3DPOOL_MANAGED,
                              &i_buffer[i],
                              NULL);

    if(a != D3D_OK) ALERT;

    // lock i_buffer and load the indices into it
    while(i_buffer[i]->Lock(0, 0, (void**)&pVoid, 0));
    memcpy(pVoid, indices[i], sizeof(short)*surfaces[i].numtris*3);
    i_buffer[i]->Unlock();


    float *heights = new float[surfaces[i].numverts];
    for(j=0; j<surfaces[i].numverts; j++){
      heights[j] = allver[i][j+surfaces[i].numverts*s].coors[1];
      if(heights[j] < min) min = heights[j];
      else if(heights[j] > max) max = heights[j];
    }

    //if(s != 0) ALERT2(min, max);
    a = Tools::D3DDevice->CreateVertexBuffer(surfaces[i].numverts*sizeof(float),
                               0,
                               0,
                               D3DPOOL_MANAGED,
                               &height_buffer[i],
                               NULL);

    
    if(a != D3D_OK) ALERT;

    while(height_buffer[i]->Lock(0, 0, (void**)&pVoid, 0));
    memcpy(pVoid, heights, sizeof(float)*surfaces[i].numverts);
    height_buffer[i]->Unlock();


    delete[] heights;
    
  }
  
  //ALERT1(surfaces[select].numtris);

  texture = Tools::LoadTexture(skin);

  
  fp.close();

     
      D3DVERTEXELEMENT9 decl[] =
{
{ 0, sizeof( float ) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
{ 0, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
{ 0, sizeof( float ) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
{ 1, sizeof( float ) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
{ 1, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
{ 1, sizeof( float ) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
{ 2, sizeof( float ) * 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
D3DDECL_END()
};
      
  Tools::D3DDevice->CreateVertexDeclaration(decl, &vertexDecl);
  Tools::D3DDevice->SetVertexDeclaration( vertexDecl);
}

void md3Model::draw(int f1, int f2, float inter){
  int i=1;
  if(1){
    for(i=0; i<h.surfacenum; i++){
      Tools::D3DDevice->SetVertexDeclaration( vertexDecl);
      Tools::D3DDevice->SetStreamSource(0, v_buffer[i], f1*sizeof(customvertex)*vertnum[i], sizeof(customvertex));
      Tools::D3DDevice->SetStreamSource(1, v_buffer[i], f2*sizeof(customvertex)*vertnum[i], sizeof(customvertex));
      Tools::D3DDevice->SetStreamSource(2, height_buffer[i], 0, sizeof(float));


      Tools::D3DDevice->SetIndices(i_buffer[i]);

      Tools::D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertnum[i], 0, trianglenum[i]); 

    }
  }

}

void md3Model::clear(){
  delete[] frames;
  delete[] tags;
  
  //delete[] surfaces;
  
  for(int i=0; i<h.surfacenum; i++){
    delete[] sel[i].shaders;
    delete[] sel[i].triangles;
    delete[] sel[i].texcoords;
    delete[] sel[i].vertices;
  }
 
  for(int i=0; i<h.surfacenum; i++){
    delete[] indices[i];
    delete[] allver[i];

  }
  delete[] indices;
  delete[] allver;
  delete[] surfaces;

  delete[] sel;
  
}

void md3Model::release(){
  for(int i=0; i<h.surfacenum; i++){
    v_buffer[i]->Release();
    i_buffer[i]->Release();
  }

  delete[] trianglenum;
  delete[] vertnum;

  //texture->Release();
}

void md3Model::draw(int m){
  draw(0, 0, 0.0);
}

