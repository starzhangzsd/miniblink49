﻿
#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "ThreadCall.h"
#include "dictionary.h"
#include "options_switches.h"
#include "api_web_contents.h"
#include "NodeRegisterHelp.h"
#include "window_list.h"

using namespace v8;
using namespace node;

namespace atom {

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char helloNative[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };
#pragma warning(pop)

static NodeNative nativeHello{ "hello", helloNative, sizeof(helloNative) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class Window : public node::ObjectWrap {
public:
    explicit Window() {
        m_webContents = nullptr;
        m_hWnd = nullptr;
        m_memoryBMP = nullptr;
        m_memoryDC = nullptr;
        m_clientRect.left = 0;
        m_clientRect.top = 0;
        m_clientRect.right = 0;
        m_clientRect.bottom = 0;
    }

    ~Window() {
        DebugBreak();

        if (m_memoryBMP)
            ::DeleteObject(m_memoryBMP);
        if (m_memoryDC)
            ::DeleteDC(m_memoryDC);

        ThreadCall::callUiThreadSync([this] {
            //delete data->m_webContents;
            SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);
        });
        WindowList::GetInstance()->RemoveWindow(this);
    }

    static void init(Local<Object> target, Environment* env) {
        Isolate* isolate = env->isolate();

        // Function模板
        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, newFunction);
        // 类名
        tpl->SetClassName(String::NewFromUtf8(isolate, "BrowserWindow"));
        // InternalField
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        v8::Local<v8::Template> t = tpl->InstanceTemplate();
        // 设置Prototype函数
        NODE_SET_METHOD(t, "close", close);
        NODE_SET_METHOD(t, "focus", focus);
        NODE_SET_METHOD(t, "blur", blur);
        NODE_SET_METHOD(t, "isFocused", isFocused);
        NODE_SET_METHOD(t, "show", show);
        NODE_SET_METHOD(t, "showInactive", showInactive);
        NODE_SET_METHOD(t, "hide", hide);
        NODE_SET_METHOD(t, "isVisible", isVisible);
        NODE_SET_METHOD(t, "isEnabled", isEnabled);
        NODE_SET_METHOD(t, "maximize", maximize);
        NODE_SET_METHOD(t, "unmaximize", unmaximize);
        NODE_SET_METHOD(t, "isMaximized", isMaximized);
        NODE_SET_METHOD(t, "minimize", minimize);
        NODE_SET_METHOD(t, "restore", restore);
        NODE_SET_METHOD(t, "isMinimized",isMinimized);
        NODE_SET_METHOD(t, "setFullScreen", setFullScreen);
        NODE_SET_METHOD(t, "isFullScreen", isFullScreen);
        NODE_SET_METHOD(t, "setAspectRatio", nullFunction);
        NODE_SET_METHOD(t, "previewFile", nullFunction);
        NODE_SET_METHOD(t, "closeFilePreview", nullFunction);
        NODE_SET_METHOD(t, "setParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getParentWindow", nullFunction);
        NODE_SET_METHOD(t, "getChildWindows", nullFunction);
        NODE_SET_METHOD(t, "isModal", isModal);
        NODE_SET_METHOD(t, "getNativeWindowHandle", getNativeWindowHandle);
        NODE_SET_METHOD(t, "getBounds", getBounds);
        NODE_SET_METHOD(t, "setBounds", setBounds);
        NODE_SET_METHOD(t, "getSize", getSize);
        NODE_SET_METHOD(t, "setSize", setSize);
        NODE_SET_METHOD(t, "getContentBounds", getContentBounds);
        NODE_SET_METHOD(t, "setContentBounds", setContentBounds);
        NODE_SET_METHOD(t, "getContentSize", getContentSize);
        NODE_SET_METHOD(t, "setContentSize", setContentSize);
        NODE_SET_METHOD(t, "setMinimumSize", setMinimumSize);
        NODE_SET_METHOD(t, "getMinimumSize", getMinimumSize);
        NODE_SET_METHOD(t, "setMaximumSize", setMaximumSize);
        NODE_SET_METHOD(t, "getMaximumSize", getMaximumSize);
        NODE_SET_METHOD(t, "setSheetOffset", nullFunction);
        NODE_SET_METHOD(t, "setResizable", setResizable);
        NODE_SET_METHOD(t, "isResizable", isResizable);
        NODE_SET_METHOD(t, "setMovable", setMovable);
        NODE_SET_METHOD(t, "isMovable", isMovable);
        NODE_SET_METHOD(t, "setMinimizable", setMinimizable);
        NODE_SET_METHOD(t, "isMinimizable", isMinimizable);
        NODE_SET_METHOD(t, "isMaximizable", isMaximizable);
        NODE_SET_METHOD(t, "setFullScreenable", setFullScreenable);
        NODE_SET_METHOD(t, "isFullScreenable", isFullScreenable);
        NODE_SET_METHOD(t, "setClosable", setClosable);
        NODE_SET_METHOD(t, "isClosable", isClosable);
        NODE_SET_METHOD(t, "setAlwaysOnTop", setAlwaysOnTop);
        NODE_SET_METHOD(t, "isAlwaysOnTop", isAlwaysOnTop);
        NODE_SET_METHOD(t, "center", center);
        NODE_SET_METHOD(t, "setPosition", setPosition);
        NODE_SET_METHOD(t, "getPosition", getPosition);
        NODE_SET_METHOD(t, "setTitle", setTitle);
        NODE_SET_METHOD(t, "getTitle", getTitle);
        NODE_SET_METHOD(t, "flashFrame", flashFrame);
        NODE_SET_METHOD(t, "setSkipTaskbar", setSkipTaskbar);
        NODE_SET_METHOD(t, "setKiosk", nullFunction);
        NODE_SET_METHOD(t, "isKiosk", nullFunction);
        NODE_SET_METHOD(t, "setBackgroundColor", setBackgroundColor);
        NODE_SET_METHOD(t, "setHasShadow", nullFunction);
        NODE_SET_METHOD(t, "hasShadow", nullFunction);
        NODE_SET_METHOD(t, "setRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "getRepresentedFilename", nullFunction);
        NODE_SET_METHOD(t, "setDocumentEdited", setDocumentEdited);
        NODE_SET_METHOD(t, "isDocumentEdited", isDocumentEdited);
        NODE_SET_METHOD(t, "setIgnoreMouseEvents", setIgnoreMouseEvents);
        NODE_SET_METHOD(t, "setContentProtection", setContentProtection);
        NODE_SET_METHOD(t, "setFocusable", setFocusable);
        NODE_SET_METHOD(t, "focusOnWebView", focusOnWebView);
        NODE_SET_METHOD(t, "blurWebView", blur);
        NODE_SET_METHOD(t, "isWebViewFocused", isWebViewFocused);
        NODE_SET_METHOD(t, "setOverlayIcon", setOverlayIcon);
        NODE_SET_METHOD(t, "setThumbarButtons", setThumbarButtons);
        NODE_SET_METHOD(t, "setMenu", setMenu);
        NODE_SET_METHOD(t, "setAutoHideMenuBar", setAutoHideMenuBar);
        NODE_SET_METHOD(t, "isMenuBarAutoHide", isMenuBarAutoHide);
        NODE_SET_METHOD(t, "setMenuBarVisibility", setMenuBarVisibility);
        NODE_SET_METHOD(t, "isMenuBarVisible", isMenuBarVisible);
        NODE_SET_METHOD(t, "setVisibleOnAllWorkspaces", setVisibleOnAllWorkspaces);
        NODE_SET_METHOD(t, "isVisibleOnAllWorkspaces", isVisibleOnAllWorkspaces);
        NODE_SET_METHOD(t, "setVibrancy", nullFunction);
        NODE_SET_METHOD(t, "hookWindowMessage", hookWindowMessage);
        NODE_SET_METHOD(t, "isWindowMessageHooked", isWindowMessageHooked);
        NODE_SET_METHOD(t, "unhookWindowMessage", unhookWindowMessage);
        NODE_SET_METHOD(t, "unhookAllWindowMessages", unhookAllWindowMessages);
        NODE_SET_METHOD(t, "setThumbnailClip", setThumbnailClip);
        NODE_SET_METHOD(t, "setThumbnailToolTip", setThumbnailToolTip);
        NODE_SET_METHOD(t, "setAppDetails", setAppDetails);
        NODE_SET_METHOD(t, "setIcon", setIcon);
        NODE_SET_PROTOTYPE_METHOD(tpl, "id", nullFunction);
        NODE_SET_PROTOTYPE_METHOD(tpl, "webContents", getWebContents);

        // 设置constructor
        constructor.Reset(isolate, tpl->GetFunction());
        // export `BrowserWindow`
        target->Set(String::NewFromUtf8(isolate, "BrowserWindow"), tpl->GetFunction());
    }

    static void staticOnPaintUpdated(wkeWebView webView, Window* win, const HDC hdc, int x, int y, int cx, int cy) {
        HWND hWnd = win->m_hWnd;
        HDC hdcScreen = ::GetDC(hWnd);
        RECT rectDest;
        if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) {
            ::GetWindowRect(hWnd, &rectDest);

            SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
            POINT pointDest = { rectDest.left, rectDest.top };
            POINT pointSource = { 0, 0 };

            BLENDFUNCTION blend = { 0 };
            memset(&blend, 0, sizeof(blend));
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;
            ::UpdateLayeredWindow(hWnd, hdcScreen, &pointDest, &sizeDest, hdc, &pointSource, RGB(0, 0, 0), &blend, ULW_ALPHA);
        }
        else {
            win->onPaintUpdated(hdcScreen, hdc, x, y, cx, cy);
        }

        ::ReleaseDC(hWnd, hdcScreen);
    }

    void onPaintUpdated(HDC hdcScreen, const HDC hdc, int x, int y, int cx, int cy) {
        HWND hWnd = m_hWnd;
        RECT rectDest;
        ::GetClientRect(hWnd, &rectDest);
        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        if (0 == sizeDest.cx * sizeDest.cy)
            return;
            
        if (!m_memoryDC)
            m_memoryDC = ::CreateCompatibleDC(nullptr);

        if (!m_memoryBMP || m_clientRect.top != rectDest.top || m_clientRect.bottom != rectDest.bottom ||
            m_clientRect.right != rectDest.right || m_clientRect.left != rectDest.left) {
            m_clientRect = rectDest;

            if (m_memoryBMP)
                ::DeleteObject((HGDIOBJ)m_memoryBMP);
            m_memoryBMP = ::CreateCompatibleBitmap(hdc, sizeDest.cx, sizeDest.cy);
        }

        HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
        ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, SRCCOPY);
        ::SelectObject(m_memoryDC, (HGDIOBJ)hbmpOld);

        ::BitBlt(hdcScreen, x, y, cx, cy, hdc, x, y, SRCCOPY);
    }

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        Window* win = (Window *)::GetPropW(hwnd, kPrppW);
        if (!win) {
            if (message == WM_CREATE) {
                LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
                Window *win = (Window *)cs->lpCreateParams;
                ThreadCall::callBlinkThreadSync([win, hwnd] {
                    wkeSetHandle(win->m_webContents->m_view, hwnd);
                });
                
                ::SetPropW(hwnd, kPrppW, (HANDLE)win);
                ::SetTimer(hwnd, (UINT_PTR)win, 70, NULL);
                return 0;
            }
        }
        if (!win)
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
        wkeWebView pthis = win->m_webContents->m_view;
        if (!pthis)
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
        switch (message) {
        case WM_CLOSE:
            ::ShowWindow(hwnd, SW_HIDE);
            ::DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            ::KillTimer(hwnd, (UINT_PTR)win);
            ::RemovePropW(hwnd, kPrppW);
            ThreadCall::callBlinkThreadSync([pthis] {
                wkeDestroyWebView(pthis);
            });
            //delete win->m_webContents;
            return 0;

        case WM_TIMER:
            //wkeRepaintIfNeeded(pthis);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps = { 0 };
            HDC hdc = ::BeginPaint(hwnd, &ps);

            RECT rcClip = ps.rcPaint;

            RECT rcClient;
            ::GetClientRect(hwnd, &rcClient);

            RECT rcInvalid = rcClient;
            if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
                ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

            int srcX = rcInvalid.left - rcClient.left;
            int srcY = rcInvalid.top - rcClient.top;
            int destX = rcInvalid.left;
            int destY = rcInvalid.top;
            int width = rcInvalid.right - rcInvalid.left;
            int height = rcInvalid.bottom - rcInvalid.top;

            if (0 != width && 0 != height && win->m_memoryBMP && win->m_memoryDC) {
                HBITMAP hbmpOld = (HBITMAP)::SelectObject(win->m_memoryDC, win->m_memoryBMP);
                BOOL b = ::BitBlt(hdc, destX, destY, width, height, win->m_memoryDC, srcX, srcY, SRCCOPY);
                b = b;
            }

            ::EndPaint(hwnd, &ps);
        }
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SIZE: {
            if (win->m_memoryDC)
                ::DeleteDC(win->m_memoryDC);
            win->m_memoryDC = nullptr;

            if (win->m_memoryBMP)
                ::DeleteObject((HGDIOBJ)win->m_memoryBMP);
            win->m_memoryBMP = nullptr;

            ::GetClientRect(hwnd, &win->m_clientRect);
            
            ThreadCall::callBlinkThreadSync([pthis, lParam] {
                wkeResize(pthis, LOWORD(lParam), HIWORD(lParam));
                wkeRepaintIfNeeded(pthis);
            });
            return 0;
        }
        case WM_KEYDOWN: {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, virtualKeyCode, flags, &retVal] {
                retVal = wkeFireKeyDownEvent(pthis, virtualKeyCode, flags, false);
            });
            if (retVal)
                return 0;
            break;
        }
        case WM_KEYUP: {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, virtualKeyCode, flags, &retVal] {
                retVal = wkeFireKeyUpEvent(pthis, virtualKeyCode, flags, false);
            });
            if (retVal)
                return 0;
            break;
        }
        case WM_CHAR: {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            bool retVal = false;
            ThreadCall::callBlinkThreadSync([pthis, charCode, flags, &retVal] {
                retVal = wkeFireKeyPressEvent(pthis, charCode, flags, false);
            });
            if (retVal)
                return 0;
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE: {
            if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
                ::SetFocus(hwnd);
                ::SetCapture(hwnd);
            }
            else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
                ::ReleaseCapture();
            }

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            ThreadCall::callBlinkThreadAsync([pthis, message, x, y, flags] {
                wkeFireMouseEvent(pthis, message, x, y, flags);
            });
            break;
        }
        case WM_CONTEXTMENU: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            if (pt.x != -1 && pt.y != -1)
                ::ScreenToClient(hwnd, &pt);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            ThreadCall::callBlinkThreadAsync([pthis, pt, flags] {
                wkeFireContextMenuEvent(pthis, pt.x, pt.y, flags);
            });
            break;
        }
        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ::ScreenToClient(hwnd, &pt);

            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            ThreadCall::callBlinkThreadAsync([pthis, pt, delta, flags] {
                wkeFireMouseWheelEvent(pthis, pt.x, pt.y, delta, flags);
            });
            break;
        }
        case WM_SETFOCUS:
            ThreadCall::callBlinkThreadAsync([pthis]{
                wkeSetFocus(pthis);
            });
            return 0;

        case WM_KILLFOCUS:
            ThreadCall::callBlinkThreadAsync([pthis] {
                wkeKillFocus(pthis);
            });
            return 0;

        case WM_SETCURSOR: {
            bool retVal = false;
            ThreadCall::callBlinkThreadAsync([pthis, hwnd, &retVal] {
                retVal = wkeFireWindowsMessage(pthis, hwnd, WM_SETCURSOR, 0, 0, nullptr);
            });
            if (retVal)
                return 0;
        }
            break;

        case WM_IME_STARTCOMPOSITION: {
            wkeRect caret;
            ThreadCall::callBlinkThreadSync([pthis, &caret] {
                caret = wkeGetCaretRect(pthis);
            });

            COMPOSITIONFORM compositionForm;
            compositionForm.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            compositionForm.ptCurrentPos.x = caret.x;
            compositionForm.ptCurrentPos.y = caret.y;

            HIMC hIMC = ::ImmGetContext(hwnd);
            ::ImmSetCompositionWindow(hIMC, &compositionForm);
            ::ImmReleaseContext(hwnd, hIMC);
        }
            return 0;
        }

        return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }

    struct CreateWindowParam {
        int x;
        int y;
        int width;
        int height;
        unsigned styles;
        unsigned styleEx;
        bool transparent;
        std::wstring title;
    };

    static void UTF8ToUTF16(const std::string& utf8, std::wstring* utf16) {
        size_t n = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);

        std::vector<wchar_t> wbuf(n);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
        utf16->resize(n);
        utf16->assign(&wbuf[0], n);
    }

    static Window* newWindow(gin::Dictionary* options) {
        Window* win = new Window();
        CreateWindowParam createWindowParam;
        createWindowParam.styles = 0;
        createWindowParam.styleEx = 0;
        createWindowParam.transparent = false;

        WebContents* webContents;
        Handle<Object> webContentsV8;
        // If no WebContents was passed to the constructor, create it from options.
        if (!options->Get("webContents", &webContentsV8)) {
            // Use options.webPreferences to create WebContents.
            gin::Dictionary webPreferences = gin::Dictionary::CreateEmpty(options->isolate());
            options->Get(options::kWebPreferences, &webPreferences);

            // Copy the backgroundColor to webContents.
            v8::Local<v8::Value> value;
            if (options->Get(options::kBackgroundColor, &value))
                webPreferences.Set(options::kBackgroundColor, value);

            v8::Local<v8::Value> transparent;
            if (options->Get("transparent", &transparent))
                webPreferences.Set("transparent", transparent);

            // Offscreen windows are always created frameless.
            bool offscreen;
            if (webPreferences.Get("offscreen", &offscreen) && offscreen) {
                options->Set(options::kFrame, false);
            }
            webContents = WebContents::create(options->isolate(), webPreferences);
        } else
            webContents = WebContents::ObjectWrap::Unwrap<WebContents>(webContentsV8);

        win->m_webContents = webContents;

        v8::Local<v8::Value> transparent;
        options->Get("transparent", &transparent);
        v8::Local<v8::Value> height;
        options->Get("height", &height);
        v8::Local<v8::Value> width;
        options->Get("width", &width);
        v8::Local<v8::Value> x;
        options->Get("x", &x);
        v8::Local<v8::Value> y;
        options->Get("y", &y);
        v8::Local<v8::Value> title;
        options->Get("title", &title);
        if (title->IsString()) {
            v8::String::Utf8Value str(title);
            UTF8ToUTF16(*str, &createWindowParam.title);
        } else 
            createWindowParam.title = L"Electron";
        
        createWindowParam.x = x->Int32Value();
        createWindowParam.y = y->Int32Value();
        createWindowParam.width = width->Int32Value();
        createWindowParam.height = height->Int32Value();

        if (transparent->IsBoolean() && transparent->ToBoolean()->BooleanValue()) {
            createWindowParam.transparent = true;
            createWindowParam.styles = WS_POPUP;
            createWindowParam.styleEx = WS_EX_LAYERED;
        } else {
            createWindowParam.styles = WS_OVERLAPPEDWINDOW;
            createWindowParam.styleEx = 0;
        }

        ThreadCall::callUiThreadSync([win, &createWindowParam] {
            win->newWindowTask(&createWindowParam);
        });
        return win;
    }

    void newWindowTask(const CreateWindowParam* createWindowParam) {
        //HandleScope scope(options->isolate());
        m_hWnd = ::CreateWindowEx(
            createWindowParam->styleEx,        // window ex-style
            L"mb_electron_window",    // window class name
            createWindowParam->title.c_str(), // window caption
            createWindowParam->styles,         // window style
            createWindowParam->x,              // initial x position
            createWindowParam->y,              // initial y position
            createWindowParam->width,          // initial x size
            createWindowParam->height,         // initial y size
            NULL,         // parent window handle
            NULL,           // window menu handle
            ::GetModuleHandleW(NULL),           // program instance handle
            this);         // creation parameters

        if (!::IsWindow(m_hWnd))
            return;

        m_clientRect.right = createWindowParam->width;
        m_clientRect.bottom = createWindowParam->height;

        Window* win = this;
        ThreadCall::callBlinkThreadSync([win, createWindowParam] {
            if (createWindowParam->transparent)
                wkeSetTransparent(win->m_webContents->m_view, true);
            wkeResize(win->m_webContents->m_view, createWindowParam->width, createWindowParam->height);
            wkeOnPaintUpdated(win->m_webContents->m_view, (wkePaintUpdatedCallback)staticOnPaintUpdated, win);
        });

        ::ShowWindow(m_hWnd, TRUE);
    }

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        if (args.IsConstructCall()) {
            if (args.Length() > 1)
                return;
            
            gin::Dictionary options(args.GetIsolate(), args[0]->ToObject()); // // 使用new调用 `new Point(...)`
            Window* win = newWindow(&options);
            
            WindowList::GetInstance()->AddWindow(win);
            
			win->Wrap(args.This(), isolate); // 包装this指针
            args.GetReturnValue().Set(args.This());
        } else {
            // 使用`Point(...)`
            const int argc = 2;
            Local<Value> argv[argc] = { args[0], args[1] };
            // 使用constructor构建Function
            Local<Function> cons = Local<Function>::New(isolate, constructor);
            args.GetReturnValue().Set(cons->NewInstance(argc, argv));
        }
    }

    //close方法
    static void close(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SendMessage(win->m_hWnd, WM_CLOSE, 0, 0);
    }

    //focus方法
    static void focus(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(win->m_hWnd);
    }

    //blur方法
    static void blur(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::SetFocus(NULL);
    }

    //isFocused方法
    static void isFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, GetFocus() == win->m_hWnd);
        args.GetReturnValue().Set(ret);
    }

    //show方法
    static void show(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
        ::SetFocus(win->m_hWnd);
    }

    //showInactive方法
    static void showInactive(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, TRUE);
    }

    //hide
    static void hide(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, FALSE);
    }

    //isVisible
    static void isVisible(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowVisible(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //isEnabled
    static void isEnabled(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsWindowEnabled(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //maximize
    static void maximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MAXIMIZE);
    }

    //unmaximize
    static void unmaximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMaximized
    static void isMaximized(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsZoomed(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //minimize
    static void minimize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_MINIMIZE);
    }

    //restore
    static void restore(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        ::ShowWindow(win->m_hWnd, SW_RESTORE);
    }

    //isMinimized
    static void isMinimized(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, !!IsIconic(win->m_hWnd));
        args.GetReturnValue().Set(ret);
    }

    //setFullScreen
    static void setFullScreen(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            RECT rc;
            HWND hDesk = ::GetDesktopWindow();
            ::GetWindowRect(hDesk, &rc);
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) | WS_BORDER);
            ::SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
        }
        else {
            ::SetWindowLong(win->m_hWnd, GWL_STYLE, GetWindowLong(win->m_hWnd, GWL_STYLE) ^ WS_BORDER);
        }
    }

    //
    static void isFullScreen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setParentWindow(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getParentWindow(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getChildWindows(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isModal(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getNativeWindowHandle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Integer> x = Integer::New(isolate, win->m_clientRect.left);
        Local<Integer> y = Integer::New(isolate, win->m_clientRect.top);
        Local<Integer> width = Integer::New(isolate, win->m_clientRect.right - win->m_clientRect.left);
        Local<Integer> height = Integer::New(isolate, win->m_clientRect.bottom - win->m_clientRect.top);
        Local<Object> bounds = Object::New(isolate);
        bounds->Set(String::NewFromUtf8(isolate, "x"), x);
        bounds->Set(String::NewFromUtf8(isolate, "y"), y);
        bounds->Set(String::NewFromUtf8(isolate, "width"), width);
        bounds->Set(String::NewFromUtf8(isolate, "height"), height);
        args.GetReturnValue().Set(bounds);
    }

    static void setBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsObject()) {
            Local<Object> bounds = args[0]->ToObject();
            LONG x = (LONG)bounds->Get(String::NewFromUtf8(isolate, "x"))->NumberValue();
            LONG y = (LONG)bounds->Get(String::NewFromUtf8(isolate, "y"))->NumberValue();
            LONG width = (LONG)bounds->Get(String::NewFromUtf8(isolate, "width"))->NumberValue();
            LONG height = (LONG)bounds->Get(String::NewFromUtf8(isolate, "height"))->NumberValue();
            ::MoveWindow(win->m_hWnd, x, y, width, height, TRUE);
        }
    }

    static void getSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Integer> width = Integer::New(isolate, win->m_clientRect.right - win->m_clientRect.left);
        Local<Integer> height = Integer::New(isolate, win->m_clientRect.bottom - win->m_clientRect.top);
        Local<Array> size = Array::New(isolate, 2);
        size->Set(0, width);
        size->Set(1, height);
        args.GetReturnValue().Set(size);
    }

    static void setSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsNumber() && args[1]->IsNumber()) {
            LONG width = (LONG)(args[0]->ToNumber()->NumberValue());
            LONG height = (LONG)(args[1]->ToNumber()->NumberValue());
            ::SetWindowPos(win->m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
        }
    }

    static void getContentBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setContentBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getContentSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            Local<Integer> width = Integer::New(isolate, wkeGetContentWidth(win->m_webContents->m_view));
            Local<Integer> height = Integer::New(isolate, wkeGetContentHeight(win->m_webContents->m_view));
            Local<Array> size = Array::New(isolate, 2);
            size->Set(0, width);
            size->Set(1, height);
            args.GetReturnValue().Set(size);
        });
    }

    static void setContentSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            if (args[0]->IsNumber() && args[1]->IsNumber()) {
                int width = (int)(args[0]->ToNumber()->NumberValue());
                int height = (int)(args[1]->ToNumber()->NumberValue());
                wkeResize(win->m_webContents->m_view, width, height);
                wkeRepaintIfNeeded(win->m_webContents->m_view);
            }
        });
    }

    static void setMinimumSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getMinimumSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMaximumSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getMaximumSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setResizable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isResizable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMovable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMovable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMinimizable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMinimizable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMaximizable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setFullScreenable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isFullScreenable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setClosable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isClosable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAlwaysOnTop(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            ::SetWindowPos(win->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else {
            ::SetWindowPos(win->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
    }

    static void isAlwaysOnTop(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        Local<Boolean> ret = Boolean::New(isolate, GetWindowLong(win->m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
        args.GetReturnValue().Set(ret);
    }

    static void center(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        int screenX, screenY;
        screenX = ::GetSystemMetrics(SM_CXSCREEN);  //取得屏幕的宽度
        screenY = ::GetSystemMetrics(SM_CYSCREEN);  //取得屏幕的高度

        RECT rect;
        GetWindowRect(win->m_hWnd, &rect);
        rect.left = (screenX - rect.right) / 2;
        rect.top = (screenY - rect.bottom) / 2;

        //设置窗体位置
        ::SetWindowPos(win->m_hWnd, NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_NOSIZE);
    }

    static void setPosition(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (args[0]->IsInt32() && args[1]->IsInt32())
        {
            int x = args[0]->ToInt32()->Int32Value();
            int y = args[1]->ToInt32()->Int32Value();
            ::SetWindowPos(win->m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
        }
    }

    static void getPosition(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        RECT rect;
        GetWindowRect(win->m_hWnd, &rect);
        Local<Integer> x = Integer::New(isolate, rect.left);
        Local<Integer> y = Integer::New(isolate, rect.top);
        Local<Array> pos = Array::New(isolate, 2);
        pos->Set(0, x);
        pos->Set(1, y);
        args.GetReturnValue().Set(pos);
    }

    static void setTitle(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());

        if (args[0]->IsString()) {
            String::Utf8Value str(args[0]);
            std::wstring title;
            UTF8ToUTF16(*str, &title);
            ::SetWindowText(win->m_hWnd, title.c_str());
        }
    }

    static void getTitle(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        wchar_t text[MAX_PATH] = { 0 };
        ::GetWindowText(win->m_hWnd, text, MAX_PATH);
        Local<String> title = String::NewFromTwoByte(isolate, (const uint16_t*)text);
        args.GetReturnValue().Set(title);
    }

    static void flashFrame(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setSkipTaskbar(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        DWORD style = ::GetWindowLong(win->m_hWnd, GWL_STYLE);
        if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
            style |= WS_EX_TOOLWINDOW;
            style &= ~WS_EX_APPWINDOW;
        }
        else {  //todo 如果窗口原来的style没有WS_EX_APPWINDOW，就可能有问题
            style &= ~WS_EX_TOOLWINDOW;
            style |= WS_EX_APPWINDOW;
        }
        ::SetWindowLong(win->m_hWnd, GWL_EXSTYLE, style);
    }

    static void setBackgroundColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setDocumentEdited(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            if (args[0]->IsBoolean() && args[0]->ToBoolean()->BooleanValue()) {
                wkeSetEditable(win->m_webContents->m_view, true);
            }
            else {
                wkeSetEditable(win->m_webContents->m_view, false);
            }
        });
    }

    static void isDocumentEdited(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setIgnoreMouseEvents(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setContentProtection(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setFocusable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void focusOnWebView(const v8::FunctionCallbackInfo<v8::Value>& args) {
        ThreadCall::callBlinkThreadSync([args] {
            Isolate* isolate = args.GetIsolate();
            HandleScope scope(isolate);

            Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
            wkeSetFocus(win->m_webContents->m_view);
        });
    }

    static void isWebViewFocused(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setOverlayIcon(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbarButtons(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMenu(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAutoHideMenuBar(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMenuBarAutoHide(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setMenuBarVisibility(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isMenuBarVisible(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setVisibleOnAllWorkspaces(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isVisibleOnAllWorkspaces(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void hookWindowMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void isWindowMessageHooked(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void unhookWindowMessage(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void unhookAllWindowMessages(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbnailClip(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setThumbnailToolTip(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setAppDetails(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void setIcon(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static void getWebContents(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Isolate* isolate = args.GetIsolate();
        HandleScope scope(isolate);

        Window* win = ObjectWrap::Unwrap<Window>(args.Holder());
        if (!win->m_webContents)
            args.GetReturnValue().SetNull();
        else
            args.GetReturnValue().Set(v8::Local<v8::Value>::New(isolate, win->m_webContents->handle()));
    }

    // 空实现
    static void nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    static v8::Persistent<v8::Function> constructor;

private:
    static const WCHAR* kPrppW;
    WebContents* m_webContents;
    HWND m_hWnd;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
};

const WCHAR* Window::kPrppW = L"mele";
Persistent<Function> Window::constructor;

static void initializeWindowApi(Local<Object> target, Local<Value> unused, Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    Window::init(target, env);
    WNDCLASS wndClass = { 0 };
    if (!GetClassInfoW(NULL, L"mb_electron_window", &wndClass)) {
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = &Window::windowProc;
        wndClass.cbClsExtra = 200;
        wndClass.cbWndExtra = 200;
        wndClass.hInstance = GetModuleHandleW(NULL);
        //wndClass.hIcon = LoadIcon(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON1));
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = L"mb_electron_window";
        RegisterClass(&wndClass);
    }
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_window, initializeWindowApi, &nativeHello)

} // atom