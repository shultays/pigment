#include "ShadowVolumeCPU.h"


ID3DXEffect* ShadowVolumeCPU::shadowEffect;

  D3DXVECTOR3 m_pVertices[1000000];
  int m_dwNumVertices;

  WORD* pEdges;


struct MESHVERTEX { D3DXVECTOR3 p, n; FLOAT tu, tv; };


ShadowVolumeCPU::ShadowVolumeCPU(BasicX *b){
  
  base = b;

}


inline void AddEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 ){
  for( DWORD i=0; i < dwNumEdges; i++ ){
    if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) ||
            ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) ){
      
      if( dwNumEdges > 1 ){
        pEdges[2*i+0] = pEdges[2*(dwNumEdges-1)+0];
        pEdges[2*i+1] = pEdges[2*(dwNumEdges-1)+1];
      }

      dwNumEdges--;
      return;
    }
  }

  pEdges[2*dwNumEdges+0] = v0;
  pEdges[2*dwNumEdges+1] = v1;
  dwNumEdges++;
}



void ShadowVolumeCPU::calculateShadow(D3DXVECTOR3 light){

  struct MESHVERTEX { D3DXVECTOR3 p, n; FLOAT tu, tv; };

  m_dwNumVertices= 0;
  MESHVERTEX* pVertices;
  WORD* pIndices;


  base->mesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
  base->mesh->LockIndexBuffer( 0L, (LPVOID*)&pIndices );
  DWORD dwNumFaces    = base->mesh->GetNumFaces();

  
  WORD* pEdges = new WORD[dwNumFaces*6];

  DWORD dwNumEdges = 0;

  light -= base->coors;
  light /= base->size;

  for( DWORD i=0; i<dwNumFaces; i++ ){
    WORD wFace0 = pIndices[ 3 * i + 0 ];
    WORD wFace1 = pIndices[ 3 * i + 1 ];
    WORD wFace2 = pIndices[ 3 * i + 2 ];

    D3DXVECTOR3 v0 = pVertices[ wFace0 ].p;
    D3DXVECTOR3 v1 = pVertices[ wFace1 ].p;
    D3DXVECTOR3 v2 = pVertices[ wFace2 ].p;

    D3DXVECTOR3 center = (v0 + v1 + v2)/3;

    D3DXVECTOR3 vCross1(v2-v1);
    D3DXVECTOR3 vCross2(v1-v0);
    D3DXVECTOR3 vNormal;
    D3DXVec3Cross( &vNormal, &vCross1, &vCross2 );

    if( D3DXVec3Dot( &vNormal, &(center-light) ) > 0.0f ){


      AddEdge( pEdges, dwNumEdges, wFace0, wFace1 );
      AddEdge( pEdges, dwNumEdges, wFace1, wFace2 );
      AddEdge( pEdges, dwNumEdges, wFace2, wFace0 );
    }
  }

  for(DWORD i=0; i<dwNumEdges; i++ ){		
    D3DXVECTOR3 v1 = pVertices[pEdges[2*i+0]].p;
    D3DXVECTOR3 v2 = pVertices[pEdges[2*i+1]].p;
    D3DXVECTOR3 v3 = v1 - Tools::Normalize(light-v1)*500000000;
    D3DXVECTOR3 v4 = v2 - Tools::Normalize(light-v2)*500000000;

    m_pVertices[m_dwNumVertices++] = v1;
    m_pVertices[m_dwNumVertices++] = v2;
    m_pVertices[m_dwNumVertices++] = v3;

    m_pVertices[m_dwNumVertices++] = v4;
    m_pVertices[m_dwNumVertices++] = v3;
    m_pVertices[m_dwNumVertices++] = v2;
  }

  delete[] pEdges;

  base->mesh->UnlockVertexBuffer();
  base->mesh->UnlockIndexBuffer();
}

void ShadowVolumeCPU::drawShadowVolume(D3DXVECTOR3 light){

  
  calculateShadow(light);

  Tools::D3DDevice->SetFVF( D3DFVF_XYZ );

  D3DXMATRIX W1, W2;
  D3DXMatrixScaling(&W1, base->size, base->size, base->size);
  D3DXMatrixTranslation(&W2, base->coors.x, base->coors.y, base->coors.z);
  W1 = W1*W2;

  shadowEffect->SetMatrix( "World", &(W1));
	shadowEffect->SetMatrix( "WorldViewProjection", &(W1 * (*base->View) * (*base->Proj)) );

	shadowEffect->Begin(0, 0);
	shadowEffect->BeginPass(0);


  Tools::D3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumVertices/3,
                                        m_pVertices, sizeof(D3DXVECTOR3) );
	shadowEffect->EndPass();
	shadowEffect->End();
}


