#pragma once
#include "framework.h"

namespace EduEngine
{
	enum GRAPHICS_API EDU_SHADER_TYPE
	{
		EDU_SHADER_TYPE_VERTEX,
		EDU_SHADER_TYPE_GEOMETRY,
		EDU_SHADER_TYPE_PIXEL,
		EDU_SHADER_TYPE_COMPUTE,
	};

	class GRAPHICS_API ShaderD3D12
	{
	public:
		ShaderD3D12(std::wstring			fileName,
					EDU_SHADER_TYPE			type,
					const D3D_SHADER_MACRO* defines,
					std::string				entryPoint,
					std::string				target);

		D3D12_SHADER_BYTECODE GetShaderBytecode() const;

		EDU_SHADER_TYPE GetShaderType() const;

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_ShaderBlob;
		EDU_SHADER_TYPE m_Type;
	};
}

