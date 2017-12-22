#include "Shader.h"
#include <iostream>
#include "../GameComponents/GameGlobal.h"
#include "../GameComponents/GameLog.h"

Shader::Shader(std::string sourceFile)
{
	HRESULT hr;
	ID3DXBuffer* error = NULL;

	//load file Shader
	hr = D3DXCreateEffectFromFileA(GameGlobal::GetCurrentDevice(), sourceFile.c_str(), 0, 0, 0, 0, &pShader ,&error);
	
	if(FAILED(hr))
	{
		if(error)
		{
			GAMELOG("Shader: %s", (char*)error ->GetBufferPointer());
		}
		else
		{
			GAMELOG("cant find shader");
		}
	}
}


Shader::~Shader(void)
{
	if(pShader)
	{
		pShader ->Release();
		pShader = NULL;
	}
}
