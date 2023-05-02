#include <Ethane.h>
//-----EntryPoint-----
#include <Ethane/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Ethane {

	class EthaneEditor : public Application
	{
	public:
		EthaneEditor(ApplicationCommandLineArgs args)
			:Application("Ethane Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~EthaneEditor()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EthaneEditor(args);
	}

}