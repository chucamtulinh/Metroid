#include ".\model.h"

CModel::CModel(void)
{
}

CModel::~CModel(void)
{
	// Release the mesh being used
	if (mesh)
		mesh->Release();

	// Release the materials
	if (m_pMeshMaterials)
		delete[] m_pMeshMaterials;

	// Release the textures
	if (m_pMeshTextures)
		delete[] m_pMeshTextures;
}

bool CModel::loadModel(LPDIRECT3DDEVICE9 device, std::string filename)
{
	HRESULT hr;
	
	hr = D3DXLoadMeshFromX(filename.c_str(), 
					  D3DXMESH_SYSTEMMEM,
					  device,
					  NULL,
					  &matBuffer,
					  NULL,
					  &numMaterials,
					  &mesh);

	if FAILED(hr)
		return false;

	D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)matBuffer->GetBufferPointer();
    
    //Create two arrays. One to hold the materials and only to hold the textures
    m_pMeshMaterials = new D3DMATERIAL9[numMaterials];
	m_pMeshTextures  = new LPDIRECT3DTEXTURE9[numMaterials];
    
	// iterate through the materials
    for(DWORD i = 0; i < numMaterials; i++)
    {
        //Copy the material
        m_pMeshMaterials[i] = matMaterials[i].MatD3D;

        //Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		// Make sure the texture name is valid
		if (matMaterials[i].pTextureFilename != NULL)
		{
			// Load the texture
			if (FAILED(D3DXCreateTextureFromFile( device, matMaterials[i].pTextureFilename, &m_pMeshTextures[i] )))
				return false;
		}
		// If no texture name, then NULL out this spot in the list
		else
			m_pMeshTextures[i] = NULL;
    }

	// Release the material buffer
	if (matBuffer)
		matBuffer->Release();

	return true;
}

void CModel::render(LPDIRECT3DDEVICE9 pDevice)
{
	for( DWORD i=0; i<numMaterials; i++ )
    {
        // Set the material and texture for this subset
        pDevice->SetMaterial( &m_pMeshMaterials[i] );

		// Set the texture if a texture is used for this material
		if (m_pMeshTextures != NULL)
			pDevice->SetTexture( 0, m_pMeshTextures[i] );
        
        // Draw the mesh subset
		mesh->DrawSubset( i );
	}
}