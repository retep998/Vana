/*
Copyright (C) 2008-2016 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include "common/types.hpp"
#include <atomic>
#include <thread>

namespace vana {
	namespace util {
		class thread_pool {
		public:
			static auto lease(function<void()> work, function<void()> pre_wait_hook) -> ref_ptr<std::thread> {
				return s_pool.lease(work, pre_wait_hook);
			}

			static auto lease(function<void(owned_lock<recursive_mutex> &)> work, function<void()> pre_wait_hook, recursive_mutex &mutex) -> ref_ptr<std::thread> {
				return s_pool.lease(work, pre_wait_hook, mutex);
			}

			static auto wait() -> void {
				s_pool.wait();
			}
		private:
			struct thread_pair {
				ref_ptr<std::thread> thread;
				function<void()> pre_wait_hook;

				thread_pair(ref_ptr<std::thread> thread, function<void()> pre_wait_hook) :
					thread{thread},
					pre_wait_hook{pre_wait_hook}
				{
				}
			};

			class _impl {
			public:
				_impl() {
					m_runhread = true;
				}

				auto lease(function<void()> work, function<void()> pre_wait_hook) -> ref_ptr<std::thread> {
					auto thread = make_ref_ptr<std::thread>([this, work]() -> void {
						while (m_runhread.load(std::memory_order_relaxed)) {
							work();
						}
					});
					auto pair = make_owned_ptr<thread_pair>(thread, pre_wait_hook);
					mhreads.emplace_back(std::move(pair));
					return thread;
				}

				auto lease(function<void(owned_lock<recursive_mutex> &)> work, function<void()> pre_wait_hook, recursive_mutex &mutex) -> ref_ptr<std::thread> {
					auto thread = make_ref_ptr<std::thread>([this, work, &mutex]() -> void {
						owned_lock<recursive_mutex> l{mutex};
						while (m_runhread.load(std::memory_order_relaxed)) {
							work(l);
						}
					});
					auto pair = make_owned_ptr<thread_pair>(thread, pre_wait_hook);
					mhreads.emplace_back(std::move(pair));
					return thread;
				}

				auto wait() -> void {
					m_runhread.store(false, std::memory_order_relaxed);
					for (auto &pair : mhreads) {
						pair->pre_wait_hook();
						pair->thread->join();
					}
				}
			private:
				std::atomic_bool m_runhread;
				vector<owned_ptr<thread_pair>> mhreads;
			};

			static _impl s_pool;
		};
	}
}