#pragma once
#include <Windows.h>
#include <PxPhysicsAPI.h>
#include <sstream>

namespace EduEngine
{
	class PhysXErrorCallback : public physx::PxErrorCallback
	{
	public:
		PhysXErrorCallback()
		{
		}

		virtual	~PhysXErrorCallback()
		{
		}

		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			const wchar_t* errorType = L"";
			switch (code)
			{
			case physx::PxErrorCode::eDEBUG_INFO:
				errorType = L"Debug Info";
				break;
			case physx::PxErrorCode::eDEBUG_WARNING:
				errorType = L"Debug Warning";
				break;
			case physx::PxErrorCode::eINVALID_PARAMETER:
				errorType = L"Invalid Parameter";
				break;
			case physx::PxErrorCode::eINVALID_OPERATION:
				errorType = L"Invalid Operation";
				break;
			case physx::PxErrorCode::eOUT_OF_MEMORY:
				errorType = L"Out of Memory";
				break;
			case physx::PxErrorCode::eINTERNAL_ERROR:
				errorType = L"Internal Error";
				break;
			case physx::PxErrorCode::eABORT:
				errorType = L"Abort";
				break;
			case physx::PxErrorCode::ePERF_WARNING:
				errorType = L"Performance Warning";
				break;
			default:
				errorType = L"Unknown Error";
				break;
			}

			std::wstringstream ss;
			ss << L"PhysX Error [" << errorType << L"] in file " << file << L" at line " << line << L":\n";
			ss << L"Message: " << message << L"\n";

			OutputDebugStringW(ss.str().c_str());
		}
	};
}