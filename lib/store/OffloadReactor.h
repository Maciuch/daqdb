/**
 * Copyright 2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "spdk/bdev.h"
#include "spdk/env.h"
#include "spdk/event.h"
#include "spdk/io_channel.h"
#include "spdk/queue.h"

#include <functional>

namespace FogKV {

using OffloadReactorShutdownCallback = std::function<void()>;

/** Intarface has been created to enable mocking in unit tests */
class OffloadReactorInterface {
    virtual void StartThread() = 0;
};

class OffloadReactor : public OffloadReactorInterface {
  public:
    OffloadReactor(const size_t cpuCore = 0, std::string spdkConfigFile = "",
                   spdk_app_opts *spdkAppOpts = nullptr,
                   OffloadReactorShutdownCallback clb = nullptr);
    virtual ~OffloadReactor();

    void StartThread();

    std::atomic<int> isRunning;

  private:
    void _ThreadMain(void);

    /** declared as static to allow using by SPDK */
    static void Shutdown(void);
    /** declared as static to allow using by SPDK */
    static void Start(void *arg1, void *arg2);

    std::string _spdkConfigFile;
    std::unique_ptr<spdk_app_opts> _spdkAppOpts;

    OffloadReactorShutdownCallback _shutdownClb;
    std::thread *_thread;
    size_t _cpuCore = 0;
};
}
