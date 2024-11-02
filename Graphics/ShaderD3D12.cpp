#include "pch.h"
#include "ShaderD3D12.h"
#include "QueryInterface.h"

namespace EduEngine
{
	ShaderD3D12::ShaderD3D12(std::wstring			 fileName,
							 EDU_SHADER_TYPE		 type,
							 const D3D_SHADER_MACRO* defines,
							 std::string			 entryPoint,
							 std::string			 target) :
		m_Type(type)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(fileName.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());
		
		THROW_IF_FAILED(hr, L"Failed to compile shader");

		m_ShaderBlob = byteCode;
	}

	D3D12_SHADER_BYTECODE ShaderD3D12::GetShaderBytecode() const
	{
		return D3D12_SHADER_BYTECODE 
		{ 
			reinterpret_cast<BYTE*>(m_ShaderBlob->GetBufferPointer()),
			m_ShaderBlob->GetBufferSize()
		};
	}

	EDU_SHADER_TYPE ShaderD3D12::GetShaderType() const
	{
		return m_Type;
	}
}
