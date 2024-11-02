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
		device->QueryInterface(IID_PPV_ARGS(&m_InfoQueue));
		m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		//m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	}

	void QueryInterface::PrintInfoMessages()
	{
#if defined(DEGUG) || defined(_DEBUG)
		UINT64 messageCount = m_InfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
		for (UINT64 i = 0; i < messageCount; ++i) {
			SIZE_T messageLength = 0;
			m_InfoQueue->GetMessage(i, nullptr, &messageLength);

			std::vector<char> messageData(messageLength);
			D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
			m_InfoQueue->GetMessage(i, message, &messageLength);

			printf("D3D12 Message: %s\n", message->pDescription);
		}
		m_InfoQueue->ClearStoredMessages();
#endif
	}
}