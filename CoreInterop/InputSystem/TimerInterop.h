
namespace EduEngine
{
	private ref class EduTime
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