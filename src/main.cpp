#include "std_include.hpp"

#include "window.hpp"
#include "camera.hpp"

#include "model.hpp"
#include "background.hpp"
#include "stereogram.hpp"

#include "obj_loader.hpp"


int main(int argc, char* argv[])
{
	if (argc < 2) return 1;

	try
	{
		window window(800, 600, "stereogram-model-viewer");
		camera camera(&window);

		auto list = window.get_painter_list();

		obj_loader loader(argv[1]);
		model model = loader.get_model();

		stereogram stereogram;
		background background(0.0, 0.0, 0.0);

		list->add(&camera);
		list->add(&background);
		list->add(&model);
		//list->add(&stereogram);

		window.show();
	}
	catch (std::exception& e)
	{
#ifdef _WIN32
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
#endif
		return 1;
	}

	return 0;
}
