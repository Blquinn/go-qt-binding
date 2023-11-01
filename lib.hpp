#pragma once

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
	extern void appExited(int code);

	/* Application */
	typedef struct AppStruct
	{
		void* app_thread;
		void* qt_application;
		// void* return_lock; // ReturnLock
	} AppStruct;

	AppStruct* Application_new(int argc, char** argv);
	void Application_exec(void* app_ptr);
	// int Application_await_exit(AppStruct* app);
	/* End Application */

	/* Window */
	void* Window_new(void* app_ptr);
	void Window_set_title(void* win_ptr, char* title);
	void Window_resize(void* win_ptr, int width, int height);
	void Window_set_minimum_size(void* win_ptr, int width, int height);
	/* End Window */

	/* Window Layout */
	void* WindowLayout_new(void* win_ptr);
	/* End Window Layout */

	/* WebEngineView */
	void* WebEngineView_new(void* win_ptr, void* win_layout_ptr);
	void WebEngineView_load_url(void* web_engine_ptr, char* url);
	/* End WebEngineView */


	// int run_app(int argc, char **argv);

#ifdef __cplusplus
}
#endif
