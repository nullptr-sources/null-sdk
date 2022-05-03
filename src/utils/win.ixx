module;

#include <Windows.h>

export module null.sdk:utils.win;
import std.core;

import :data_types.vec2;
import :data_types.callbacks;

export namespace utils {
	enum class e_window_callbacks {
		wnd_proc //std::function<bool(HWND, UINT, WPARAM, LPARAM)>. If callback returns true then DefWindowProc call will be skipped and wndproc will return true
	};

	namespace win {
		class c_window {
		public:
			struct time_data_t {
				std::uint64_t ticks_per_second{ }, time{ };
				float delta_time{ };
				
				void initialize() {
					if(!QueryPerformanceFrequency((LARGE_INTEGER*)&ticks_per_second)) throw std::runtime_error("QueryPerformanceFrequency(&ticks_per_second) exception");
					if(!QueryPerformanceCounter((LARGE_INTEGER*)&time)) throw std::runtime_error("QueryPerformanceCounter(&time) exception");
				}

				void begin_frame() {
					std::uint64_t current_time{ };
					if(!QueryPerformanceCounter((LARGE_INTEGER*)&current_time)) throw std::runtime_error("QueryPerformanceCounter(&current_time) exception");

					delta_time = (float)(current_time - time) / ticks_per_second;
					time = current_time;
				}
			} time_data{ };

			single_callbacks_t<e_window_callbacks> callbacks{ };

			HWND wnd_handle{ nullptr };
			HINSTANCE instance{ };
			WNDCLASSA wnd_class{
				CS_CLASSDC,
				&c_window::wnd_proc,
				0,
				sizeof(c_window*),
				instance,
				LoadIcon(instance, MAKEINTRESOURCE(107)),
				LoadCursor(nullptr, IDC_ARROW),
				(HBRUSH)(COLOR_WINDOW),
				MAKEINTRESOURCEA(109),
				"null"
			};

			std::string name{ "null" };
			vec2_t pos{ 100, 100 }, size{ 800, 600 };

			std::uint32_t styles{ WS_OVERLAPPEDWINDOW };

		public:
			c_window() = default;
			c_window(HINSTANCE _instance) : instance(_instance) { }

			bool create() {
				if(!RegisterClassA(&wnd_class)) throw std::runtime_error("register class error");

				wnd_handle = CreateWindowA(wnd_class.lpszClassName, name.c_str(), styles, pos.x, pos.y, size.x, size.y, NULL, NULL, instance, NULL);
				if(wnd_handle) {
					SetWindowLongPtrA(wnd_handle, 0, (LONG_PTR)this);
					time_data.initialize();
				}

				return wnd_handle != nullptr;
			}

			void destroy() {
				if(!wnd_handle) throw std::runtime_error("window handle is nullptr");
				if(!instance) throw std::runtime_error("instance is nullptr");

				DestroyWindow(wnd_handle);
				UnregisterClassA(name.c_str(), instance);
			}

			void main_loop(std::function<void()> function) {
				if(!wnd_handle) throw std::runtime_error("window handle is nullptr");

				ShowWindow(wnd_handle, SW_SHOWDEFAULT);
				UpdateWindow(wnd_handle);

				MSG msg{ };
				while(msg.message != WM_QUIT) {
					if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessageA(&msg);
						continue;
					}

					time_data.begin_frame();

					function();
				}
			}

		private:
			static LRESULT WINAPI wnd_proc(HWND _wnd_handle, UINT msg, WPARAM w_param, LPARAM l_param) {
				c_window* window = (c_window*)GetWindowLongPtrA(_wnd_handle, 0);
				if(window && window->callbacks.have_callback(e_window_callbacks::wnd_proc)) {
					if(std::any_cast<bool>(window->callbacks.call<bool(HWND, UINT, WPARAM, LPARAM)>(e_window_callbacks::wnd_proc, _wnd_handle, msg, w_param, l_param)))
						return true;
				}

				return DefWindowProcA(_wnd_handle, msg, w_param, l_param);
			}
		};
	}
}