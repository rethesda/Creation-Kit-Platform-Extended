﻿// Copyright © 2023-2024 aka perchik71. All rights reserved.
// Contacts: <email:timencevaleksej@gmail.com>
// License: https://www.gnu.org/licenses/gpl-3.0.html

#include "Core/Engine.h"
#include "Core/RegistratorWindow.h"
#include "UITheme/VarCommon.h"
#include "Editor API/EditorUI.h"
#include "ObjectWindowSF.h"

#define UI_OBJECT_WINDOW_CHECKBOX			6329
#define UI_OBJECT_WINDOW_ADD_ITEM			2579
#define UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW	(WM_USER + 34400)

namespace CreationKitPlatformExtended
{
	namespace Patches
	{
		namespace Starfield
		{
			OBJWNDS ObjectWindows;
			ObjectWindow* GlobalObjectWindowBasePtr = nullptr;
			uintptr_t pointer_ObjectWindow_sub = 0;

			void ResizeObjectWndChildControls(LPOBJWND lpObjWnd)
			{
				// The perfectionist in me is dying....

				lpObjWnd->Controls.TreeList.LockUpdate();
				lpObjWnd->Controls.ItemList.LockUpdate();
				lpObjWnd->Controls.EditFilter.LockUpdate();
				lpObjWnd->Controls.ToggleDecompose.LockUpdate();
				lpObjWnd->Controls.BtnObjLayout.LockUpdate();

				auto WndRect = lpObjWnd->ObjectWindow.ClientRect();

				// Fix https://www.nexusmods.com/fallout4/images/274405
				lpObjWnd->Controls.TreeList.Left = 1;

				LONG w_btns = lpObjWnd->Controls.BtnObjLayout.Width;
				LONG w_tree = lpObjWnd->Controls.TreeList.Width;
				LONG w_left = w_tree - w_btns + 1;
				lpObjWnd->Controls.BtnObjLayout.Left = w_left;
				lpObjWnd->Controls.ToggleDecompose.Left = w_left;
				lpObjWnd->Controls.BtnDataView.Left = w_left;
				lpObjWnd->Controls.ActiveOnly.Width = w_tree;

				w_left = w_tree - lpObjWnd->Controls.EditFilter.Left - w_btns - 3;
				lpObjWnd->Controls.EditFilter.Width = w_left;
				lpObjWnd->Controls.ComboLayout.Width = w_left;
				lpObjWnd->Controls.DataView.Width = w_left;

				auto TopT = lpObjWnd->Controls.TreeList.Top;

				lpObjWnd->Controls.ItemList.Left = w_tree + 5;
				lpObjWnd->Controls.ItemList.Width = WndRect.Width - (w_tree + 5);
				lpObjWnd->Controls.ItemList.Height = WndRect.Height - 3;
				lpObjWnd->Controls.TreeList.Height = WndRect.Height - TopT;
				lpObjWnd->Controls.Spliter.Height = WndRect.Height - TopT;
				
				// fix bad pic
				auto handle = lpObjWnd->ObjectWindow.Handle;
				RECT r = { 0, 0, lpObjWnd->Controls.ItemList.Left, TopT };
				InvalidateRect(handle, &r, TRUE);
				UpdateWindow(handle);

				lpObjWnd->Controls.BtnObjLayout.UnlockUpdate();
				lpObjWnd->Controls.ToggleDecompose.UnlockUpdate();
				lpObjWnd->Controls.EditFilter.UnlockUpdate();
				lpObjWnd->Controls.ItemList.UnlockUpdate();
				lpObjWnd->Controls.TreeList.UnlockUpdate();
				lpObjWnd->Controls.EditFilter.Repaint();
				lpObjWnd->Controls.BtnObjLayout.Repaint();
				lpObjWnd->Controls.ToggleDecompose.Repaint();
			}

			void SplitterResizeObjectWndChildControls(LPOBJWND lpObjWnd)
			{
				lpObjWnd->Controls.TreeList.LockUpdate();
				lpObjWnd->Controls.ItemList.LockUpdate();
				lpObjWnd->Controls.EditFilter.LockUpdate();
				lpObjWnd->Controls.ToggleDecompose.LockUpdate();
				lpObjWnd->Controls.BtnObjLayout.LockUpdate();

				auto WndRect = lpObjWnd->ObjectWindow.ClientRect();

				// Fix https://www.nexusmods.com/fallout4/images/274405
				lpObjWnd->Controls.TreeList.Left = 1;

				LONG w_btns = lpObjWnd->Controls.BtnObjLayout.Width;
				LONG w_tree = lpObjWnd->Controls.TreeList.Width;
				LONG w_left = w_tree - w_btns + 1;
				lpObjWnd->Controls.BtnObjLayout.Left = w_left;
				lpObjWnd->Controls.BtnDataView.Left = w_left;
				lpObjWnd->Controls.ToggleDecompose.Left = w_left;
				lpObjWnd->Controls.ActiveOnly.Width = w_tree;

				w_left = w_tree - lpObjWnd->Controls.EditFilter.Left - w_btns - 3;
				lpObjWnd->Controls.EditFilter.Width = w_left;
				lpObjWnd->Controls.ComboLayout.Width = w_left;
				lpObjWnd->Controls.DataView.Width = w_left;

				auto TopT = lpObjWnd->Controls.TreeList.Top;

				lpObjWnd->Controls.ItemList.Left = w_tree + 5;
				lpObjWnd->Controls.ItemList.Width = WndRect.Width - (w_tree + 5);
				lpObjWnd->Controls.ItemList.Height = WndRect.Height - 3;
				lpObjWnd->Controls.TreeList.Height = WndRect.Height - TopT;
				lpObjWnd->Controls.Spliter.Height = WndRect.Height - TopT;

				// fix bad pic
				auto handle = lpObjWnd->ObjectWindow.Handle;
				RECT r = { 0, 0, lpObjWnd->Controls.ItemList.Left, TopT };
				InvalidateRect(handle, &r, TRUE);
				UpdateWindow(handle);

				lpObjWnd->Controls.BtnObjLayout.UnlockUpdate();
				lpObjWnd->Controls.ToggleDecompose.UnlockUpdate();
				lpObjWnd->Controls.EditFilter.UnlockUpdate();
				lpObjWnd->Controls.ItemList.UnlockUpdate();
				lpObjWnd->Controls.TreeList.UnlockUpdate();

				lpObjWnd->Controls.EditFilter.Repaint();
				lpObjWnd->Controls.BtnObjLayout.Repaint();
				lpObjWnd->Controls.ToggleDecompose.Repaint();
			}

			bool ObjectWindow::HasOption() const
			{
				return false;
			}

			bool ObjectWindow::HasCanRuntimeDisabled() const
			{
				return false;
			}

			const char* ObjectWindow::GetOptionName() const
			{
				return nullptr;
			}

			const char* ObjectWindow::GetName() const
			{
				return "Object Window";
			}

			bool ObjectWindow::HasDependencies() const
			{
				return false;
			}

			Array<String> ObjectWindow::GetDependencies() const
			{
				return {};
			}

			bool ObjectWindow::QueryFromPlatform(EDITOR_EXECUTABLE_TYPE eEditorCurrentVersion,
				const char* lpcstrPlatformRuntimeVersion) const
			{
				return (eEditorCurrentVersion >= EDITOR_EXECUTABLE_TYPE::EDITOR_STARFIELD_1_14_70_0) &&
					(eEditorCurrentVersion <= EDITOR_EXECUTABLE_TYPE::EDITOR_STARFIELD_LAST);
			}

			bool ObjectWindow::Activate(const Relocator* lpRelocator,
				const RelocationDatabaseItem* lpRelocationDatabaseItem)
			{
				auto verPatch = lpRelocationDatabaseItem->Version();
				if (verPatch == 1)
				{
					*(uintptr_t*)&_oldWndProc = voltek::detours_function_class_jump(_RELDATA_ADDR(0), (uintptr_t)&HKWndProc);

					// Fix resize ObjectWindowProc
					auto rva = _RELDATA_RAV(1);
					lpRelocator->PatchNop(rva, 0x4B);
					lpRelocator->DetourCall(rva, (uintptr_t)&HKMoveWindow);

					return true;
				}

				return false;
			}

			bool ObjectWindow::Shutdown(const Relocator* lpRelocator,
				const RelocationDatabaseItem* lpRelocationDatabaseItem)
			{
				return false;
			}

			BOOL WINAPI ObjectWindow::HKMoveWindow(HWND hWindow, INT32 X, INT32 Y, INT32 nWidth, INT32 nHeight, BOOL bRepaint)
			{
				BOOL bResult = MoveWindow(hWindow, X, Y, nWidth, nHeight, bRepaint);

				if (auto iterator = ObjectWindows.find(GetParent(hWindow)); iterator != ObjectWindows.end())
				{
					LPOBJWND lpObjWnd = (*iterator).second;
					if (lpObjWnd) lpObjWnd->ObjectWindow.Perform(WM_COMMAND, UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW, 0);
				}

				return bResult;
			}

			ObjectWindow::ObjectWindow() : BaseWindow(), Classes::CUIBaseWindow()
			{
				GlobalObjectWindowBasePtr = this;
			}

			INT_PTR CALLBACK ObjectWindow::HKWndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
			{
				if (Message == WM_INITDIALOG)
				{
					LPOBJWND lpObjWnd = new OBJWND;

					GlobalRegistratorWindowPtr->RegisterMajor(Hwnd, "ObjectWindow");
					lpObjWnd->ObjectWindow = Hwnd;

					lpObjWnd->Controls.TreeList = lpObjWnd->ObjectWindow.GetControl(2093);
					lpObjWnd->Controls.ItemList = lpObjWnd->ObjectWindow.GetControl(1041);
					lpObjWnd->Controls.ToggleDecompose = lpObjWnd->ObjectWindow.GetControl(6027);
					lpObjWnd->Controls.BtnObjLayout = lpObjWnd->ObjectWindow.GetControl(6025);
					lpObjWnd->Controls.ComboLayout = lpObjWnd->ObjectWindow.GetControl(6024);
					lpObjWnd->Controls.BtnDataView = lpObjWnd->ObjectWindow.GetControl(6028);
					lpObjWnd->Controls.DataView = lpObjWnd->ObjectWindow.GetControl(6026);
					lpObjWnd->Controls.EditFilter = lpObjWnd->ObjectWindow.GetControl(2581);
					lpObjWnd->Controls.Spliter = lpObjWnd->ObjectWindow.GetControl(2157);
					lpObjWnd->Controls.ActiveOnly = GetDlgItem(Hwnd, 5904);

					auto StyleEx = ListView_GetExtendedListViewStyle(lpObjWnd->Controls.ItemList.Handle);
					if ((StyleEx & LVS_EX_DOUBLEBUFFER) != LVS_EX_DOUBLEBUFFER)
					{
						// Eliminate the flicker when changing categories
						StyleEx |= LVS_EX_DOUBLEBUFFER;
						ListView_SetExtendedListViewStyleEx(lpObjWnd->Controls.ItemList.Handle, StyleEx, StyleEx);
					}

					StyleEx = TreeView_GetExtendedStyle(lpObjWnd->Controls.TreeList.Handle);
					if ((StyleEx & TVS_EX_DOUBLEBUFFER) != TVS_EX_DOUBLEBUFFER)
					{
						// Eliminate the flicker when changing size trees
						StyleEx |= TVS_EX_DOUBLEBUFFER;
						TreeView_SetExtendedStyle(lpObjWnd->Controls.TreeList.Handle, StyleEx, StyleEx);
					}

					ObjectWindows.emplace(Hwnd, lpObjWnd);
				}
				// Don't let us reduce the window too much
				else if (Message == WM_GETMINMAXINFO)
				{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = 350;
					lpMMI->ptMinTrackSize.y = 200;

					return S_OK;
				}
				else if (Message == WM_ERASEBKGND) {
					if (auto iterator = ObjectWindows.find(Hwnd);  iterator != ObjectWindows.end())
					{
						LPOBJWND lpObjWnd = (*iterator).second;
						if (lpObjWnd) {
							HDC dc = (HDC)wParam;
							auto Rect = lpObjWnd->Controls.ItemList.BoundsRect;
							ExcludeClipRect(dc, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
							Rect = lpObjWnd->Controls.TreeList.BoundsRect;
							ExcludeClipRect(dc, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
							Rect = lpObjWnd->Controls.ActiveOnly.BoundsRect;
							ExcludeClipRect(dc, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
							Rect = lpObjWnd->Controls.EditFilter.BoundsRect;
							ExcludeClipRect(dc, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);

							HBRUSH brush;

							if (UITheme::IsDarkTheme())
								brush = UITheme::Comctl32GetSysColorBrush(COLOR_BTNFACE);
							else
								brush = GetSysColorBrush(COLOR_BTNFACE);

							RECT rc;
							GetClipBox(dc, &rc);
							FillRect(dc, &rc, brush);

							return 1;
						}
					}
				}
				else if (Message == WM_SIZE)
				{
					if (auto iterator = ObjectWindows.find(Hwnd);  iterator != ObjectWindows.end())
					{
						LPOBJWND lpObjWnd = (*iterator).second;
						if (lpObjWnd) {
							ResizeObjectWndChildControls(lpObjWnd);
							return 0;
						}
					}
				}
				else if (Message == WM_COMMAND)
				{
					const uint32_t param = LOWORD(wParam);
					if (param == UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW)
					{
						if (auto iterator = ObjectWindows.find(Hwnd); iterator != ObjectWindows.end())
						{
							LPOBJWND lpObjWnd = (*iterator).second;
							if (lpObjWnd) SplitterResizeObjectWndChildControls(lpObjWnd);
						}
						return S_OK;
					}
				}
				else if (Message == WM_SHOWWINDOW)
				{
					if (auto iterator = ObjectWindows.find(Hwnd); iterator != ObjectWindows.end())
					{
						LPOBJWND lpObjWnd = (*iterator).second;
						if (lpObjWnd) ResizeObjectWndChildControls(lpObjWnd);
					}
				}
				else if (Message == WM_DESTROY)
				{
					LPOBJWND lpObjWnd = ObjectWindows.at(Hwnd);
					if (lpObjWnd)
					{
						GlobalRegistratorWindowPtr->Unregister(Hwnd, true);
						ObjectWindows.erase(Hwnd);

						delete lpObjWnd;
						lpObjWnd = NULL;
					}
				}

				return CallWindowProc(GlobalObjectWindowBasePtr->GetOldWndProc(), Hwnd, Message, wParam, lParam);
			}
		}
	}
}