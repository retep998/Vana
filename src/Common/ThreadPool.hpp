/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Types.hpp"
#include <atomic>
#include <thread>

class ThreadPool {
public:
	static auto lease(function_t<void()> work, function_t<void()> preWaitHook) -> ref_ptr_t<thread_t> {
		return s_pool.lease(work, preWaitHook);
	}

	static auto lease(function_t<void(owned_lock_t<recursive_mutex_t> &)> work, function_t<void()> preWaitHook, recursive_mutex_t &mutex) -> ref_ptr_t<thread_t> {
		return s_pool.lease(work, preWaitHook, mutex);
	}

	static auto wait() -> void {
		s_pool.wait();
	}
private:
	struct ThreadPair {
		ref_ptr_t<thread_t> thread;
		function_t<void()> preWaitHook;

		ThreadPair(ref_ptr_t<thread_t> thread, function_t<void()> preWaitHook) :
			thread{thread},
			preWaitHook{preWaitHook}
		{
		}
	};

	class _impl {
	public:
		_impl() {
			m_runThread = true;
		}

		auto lease(function_t<void()> work, function_t<void()> preWaitHook) -> ref_ptr_t<thread_t> {
			auto thread = make_ref_ptr<thread_t>([this, work]() -> void {
				while (m_runThread.load(std::memory_order_relaxed)) {
					work();
				}
			});
			auto pair = make_owned_ptr<ThreadPair>(thread, preWaitHook);
			m_threads.emplace_back(std::move(pair));
			return thread;
		}

		auto lease(function_t<void(owned_lock_t<recursive_mutex_t> &)> work, function_t<void()> preWaitHook, recursive_mutex_t &mutex) -> ref_ptr_t<thread_t> {
			auto thread = make_ref_ptr<thread_t>([this, work, &mutex]() -> void {
				owned_lock_t<recursive_mutex_t> l{mutex};
				while (m_runThread.load(std::memory_order_relaxed)) {
					work(l);
				}
			});
			auto pair = make_owned_ptr<ThreadPair>(thread, preWaitHook);
			m_threads.emplace_back(std::move(pair));
			return thread;
		}

		auto wait() -> void {
			m_runThread.store(false, std::memory_order_relaxed);
			for (auto &pair : m_threads) {
				pair->preWaitHook();
				pair->thread->join();
			}
		}
	private:
		std::atomic_bool m_runThread;
		vector_t<owned_ptr_t<ThreadPair>> m_threads;
	};

	static _impl s_pool;
};