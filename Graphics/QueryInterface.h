#pragma once
#include "pch.h"

namespace EduEngine
{
	class GRAPHICS_API QueryInterface
	{
	public:
		static QueryInterface& GetInstance();

		void Initialize(ID3D12Device* device);
		void PrintInfoMessages();

	private:
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> mInfoQueue;
	};
}
