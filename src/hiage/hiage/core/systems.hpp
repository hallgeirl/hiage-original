#pragma once

namespace hiage
{
	class System
	{
	public:
		virtual void invoke(double frameTime) = 0;
	};

	class ObjectRenderingSystem : public System
	{
		virtual void invoke(double frameTime) override;
	};
}