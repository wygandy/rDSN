/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Microsoft Corporation
 * 
 * -=- Robust Distributed System Nucleus (rDSN) -=- 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
# pragma once

# include <dsn/internal/task.h>
# include <dsn/internal/service_app.h>
# include <dsn/internal/zlocks.h>

namespace dsn { namespace service {

namespace tasking
{
    inline void enqueue(task_ptr& task, int delay_milliseconds = 0)
    { 
        if (delay_milliseconds > 0)
        {
            task->set_delay(delay_milliseconds);
        }           
        task->enqueue(); 
    }

    inline bool cancel(task_ptr& task, bool wait_until_finished)
    {
        return task->cancel(wait_until_finished);
    }

    inline bool wait(task_ptr& task, int timeout_milliseconds = TIME_MS_MAX)
    {
        return task->wait(timeout_milliseconds);
    }
}

namespace rpc
{
    extern const end_point& primary_address();

    extern bool register_rpc_handler(task_code code, const char* name, rpc_server_handler* handler);

    extern bool unregister_rpc_handler(task_code code);

    extern void reply(message_ptr& response);

    // when callback is empty, we assume callers will invoke return::wait() to perform a synchronous rpc call
    // to invoke a one way rpc call, use call_one_way below
    extern rpc_response_task_ptr call(const end_point& server, message_ptr& request, rpc_response_task_ptr callback = nullptr);

    extern void call_one_way(const end_point& server, message_ptr& request);
}

namespace file
{
    extern handle_t open(const char* file_name, int flag, int pmode);

    extern void read(handle_t hFile, char* buffer, int count, uint64_t offset, aio_task_ptr& callback);

    extern void write(handle_t hFile, const char* buffer, int count, uint64_t offset, aio_task_ptr& callback); 

    extern error_code close(handle_t hFile);

    extern void copy_remote_files(
        const end_point& remote,
        std::string& source_dir,
        std::vector<std::string>& files,  // empty for all
        std::string& dest_dir,
        bool overwrite,
        aio_task_ptr& callback
        );
}

namespace env
{
    // since Epoch (1970-01-01 00:00:00 +0000 (UTC))
    extern uint64_t now_ns();

    // generate random number [min, max]
    extern uint64_t random64(uint64_t min, uint64_t max);

    inline uint64_t now_us() { return now_ns() / 1000; }
    inline uint64_t now_ms() { return now_ns() / 1000000; }
    inline uint32_t random32(uint32_t min, uint32_t max) { return static_cast<uint32_t>(random64(min, max)); }
    inline double   probability() { return static_cast<double>(random32(0, 1000000000)) / 1000000000.0; }
}

namespace system
{
    extern bool run(const char* config, bool sleep_after_init);
    extern bool is_ready();
    
    namespace internal_use_only
    {
        extern bool register_service(const char* name, service_app_factory factory);
    }

    template<typename T> bool register_service(const char* name)
    {
        return internal_use_only::register_service(name, service_app::create<T>);
    }

    extern configuration_ptr config();
    extern service_app* get_current_app();
    extern const std::map<std::string, service_app*>& get_all_apps();
}

}} // end namespace dsn::service
