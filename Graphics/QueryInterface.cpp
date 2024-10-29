#include "pch.h"
#include "QueryInterface.h"
#include <vector>

namespace EduEngine
{
	QueryInterface& QueryInterface::GetInstance()
	{
		static QueryInterface instance;
		return instance;
	}

	void QueryInterface::Initialize(ID3D12Device* device)
	{
		device->QueryInterface(IID_PPV_ARGS(&mInfoQueue));
		mInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		//mInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	}

	void QueryInterface::PrintInfoMessages()
	{
		UINT64 messageCount = mInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
		for (UINT64 i = 0; i < messageCount; ++i) {
			SIZE_T messageLength = 0;
			mInfoQueue->GetMessage(i, nullptr, &messageLength);

			std::vector<char> messageData(messageLength);
			D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
			mInfoQueue->GetMessage(i, message, &messageLength);

			printf("D3D12 Message: %s\n", message->pDescription);
		}
		mInfoQueue->ClearStoredMessages();
	}
}