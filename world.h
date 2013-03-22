#include <windows.h>
class CWorld
{
	private:

	BOOL Init(void);
	BOOL Shutdown(void);

	public:

	float left, right, top, bottom, front, back;

	CWorld()
	{
		Init();
	}
	~CWorld()
	{
		Shutdown();
	}
};
