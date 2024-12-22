#include "TimerInterop.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	float EduTime::DeltaTime::get()
	{
		return CoreSystems::GetInstance()->GetRuntimeTimer()->GetDeltaTime();
	}

	float EduTime::TotalTime::get()
	{
		return CoreSystems::GetInstance()->GetRuntimeTimer()->GetTotalTime();
	}

	float EditorTime::DeltaTime::get()
	{
		return CoreSystems::GetInstance()->GetEditorTimer()->GetDeltaTime();
	}

	float EditorTime::TotalTime::get()
	{
		return CoreSystems::GetInstance()->GetEditorTimer()->GetTotalTime();
	}
}