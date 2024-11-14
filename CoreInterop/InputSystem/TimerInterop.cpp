#include "TimerInterop.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	float EduTime::DeltaTime::get()
	{
		return CoreSystems::GetInstance()->GetTimer()->GetDeltaTime();
	}

	float EduTime::TotalTime::get()
	{
		return CoreSystems::GetInstance()->GetTimer()->GetTotalTime();
	}
}