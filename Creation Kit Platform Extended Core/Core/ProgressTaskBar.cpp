﻿// Copyright © 2024 aka perchik71. All rights reserved.
// Contacts: <email:timencevaleksej@gmail.com>
// License: https://www.gnu.org/licenses/gpl-3.0.html

#include "Engine.h"
#include "ProgressTaskBar.h"

namespace CreationKitPlatformExtended
{
	namespace Core
	{
		ProgressTaskBar::ProgressTaskBar(HWND LinkWnd, DWORD Total) : _linkWnd(LinkWnd), _marquee(false), _beg(false),
			_progressCompleted(0), _progressTotal(Total)
#if 0
			, _procAffinity(0), _sysAffinity(0), _hasAffinity(false)
#endif
		{
			if (!IsWindow(LinkWnd))
				_MESSAGE("IProgressTaskBar could not be created, because LinkWnd isn't window.");
			else
			{
				if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(_context.GetAddressOf()))))
				{
					if (FAILED(_context->HrInit()))
					{
						_context = nullptr;
						_MESSAGE("ITaskbarList::HrInit() has failed.");
					}
					else
					{
						SetProgressState(TBPF_NOPROGRESS);
#if 0
#error "It has lost its meaning, since corrections have already been made to Sleep()"
						if (GlobalEnginePtr->HasHyperThreads() && (GlobalEnginePtr->GetTotalLogicalCores() >= 4))
						{
							HANDLE hProcess = GetCurrentProcess();
							if (!GetProcessAffinityMask(hProcess, &_procAffinity, &_sysAffinity))
								_MESSAGE("IProgressTaskBar cannot get the mask of the active processors of the process.");
							else
								_hasAffinity = true;
						}
#endif
					}
				}
				else
					_MESSAGE("ITaskbarList could not be created.");
			}
		}

		ProgressTaskBar::~ProgressTaskBar()
		{
			End();
		}

		void ProgressTaskBar::Begin()
		{
			_beg = SetProgressState(_marquee ? TBPF_INDETERMINATE : TBPF_NORMAL);

#if 0
#error "It has lost its meaning, since corrections have already been made to Sleep()"
			if (_hasAffinity)
			{
				// I noticed that on a single core, it loads faster, most likely due to incorrect multithreaded management of the load process.
				// For more acceleration, it is necessary to find a more or less free core. 
				// Most often, Windows apps uses the first half of the cores, ignoring hyper technology.
				auto logical_core_id = (GlobalEnginePtr->GetTotalLogicalCores() >> 1);

				bool success = false;
				DWORD_PTR mask = (DWORD_PTR)1 << logical_core_id;
				for (size_t bit = logical_core_id; bit < 64; bit++)
				{
					if (mask & _sysAffinity)	// check enable core this processor
					{
						success = true;
						break;
					}
					
					logical_core_id++;
					mask <<= 1;
				}

				if (success)
				{
					// If the first time did not work out, then it will never work out.
					if (_hasAffinity = SetProcessAffinityMask(GetCurrentProcess(), mask))
						_CONSOLE("Enabling single-threaded mode on the core: #%u aff. mask %llu", logical_core_id, mask);
				}
			}
#endif
		}

		void ProgressTaskBar::End()
		{
			_beg = !SetProgressState(TBPF_NOPROGRESS);

#if 0
#error "It has lost its meaning, since corrections have already been made to Sleep()"
			if (_hasAffinity)
			{
				if (SetProcessAffinityMask(GetCurrentProcess(), _procAffinity))
					_CONSOLE("Restoring multithreaded mode: aff. mask %llu", _procAffinity);
				else
				{
					_hasAffinity = false;
					_CONSOLE("System failure to restore multithreaded mode");	
				}
			}
#endif
		}

		void ProgressTaskBar::SetMarquee(bool Value)
		{
			if (!_beg || (Value == _marquee))
				return;

			if (Value)
				_marquee = SetProgressState(TBPF_INDETERMINATE);
			else
			{
				_marquee = !SetProgressState(TBPF_NORMAL);
				SetProgressValue(_progressCompleted, _progressTotal);
			}
		}

		void ProgressTaskBar::SetPosition(DWORD Completed)
		{
			if (Completed < _progressTotal && SetProgressValue(Completed, _progressTotal))
				_progressCompleted = Completed;
		}

		void ProgressTaskBar::Step()
		{
			SetPosition(_progressCompleted + 1);
		}

		void ProgressTaskBar::SetTotal(DWORD Total)
		{
			_progressTotal = Total;
			if (_progressCompleted > _progressTotal)
				_progressCompleted = _progressTotal;
			SetProgressValue(_progressCompleted, _progressTotal);
		}

		void ProgressTaskBar::Reset()
		{
			SetPosition(0);
		}

		bool ProgressTaskBar::SetProgressState(DWORD State)
		{
			if (_context != nullptr)
				return SUCCEEDED(_context->SetProgressState(_linkWnd, (TBPFLAG)State));
			return false;
		}

		bool ProgressTaskBar::SetProgressValue(DWORD Completed, DWORD Total)
		{
			if ((_context != nullptr) && _beg && !_marquee)
				return SUCCEEDED(_context->SetProgressValue(_linkWnd, Completed, Total));
			return false;
		}
	}
}