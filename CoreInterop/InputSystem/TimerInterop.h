#pragma once

namespace EduEngine
{
	public ref class EduTime
	{
	public:
		static property float DeltaTime
		{
			float get();
		}
		static property float TotalTime
		{
			float get();
		}
	};

	public ref class EditorTime
	{
	public:
		static property float DeltaTime
		{
			float get();
		}
		static property float TotalTime
		{
			float get();
		}
	};
}