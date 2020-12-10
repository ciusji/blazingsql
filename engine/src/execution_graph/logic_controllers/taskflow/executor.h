#pragma once

#include "kernel.h"
#include "execution_graph/logic_controllers/CacheMachine.h"
#include "ExceptionHandling/BlazingThread.h"

namespace ral {
namespace execution{

class priority {
public:

private:
	size_t priority_num_query; //can be used to prioritize one query over another
	size_t priority_num_kernel; //can be
};

class executor;

class task {
public:

	task(
	std::vector<std::unique_ptr<ral::cache::CacheData > > inputs,
	std::shared_ptr<ral::cache::CacheMachine> output,
	size_t task_id,
	ral::cache::kernel * kernel, size_t attempts_limit,
	std::string kernel_process_name, size_t attempts = 0);

	/**
	* Function which runs the kernel process on the inputs and puts results into output.
	* This function does not modify the inputs and can throw an exception. In the case it throws an exception it
	* gets placed back in the executor if it was a memory exception.
	*/
	void run(cudaStream_t stream, executor * executor);
	void complete();
	std::size_t task_memory_needed();

protected:
	std::vector<std::unique_ptr<ral::cache::CacheData > > inputs;
	std::shared_ptr<ral::cache::CacheMachine> output;
	size_t task_id;
	ral::cache::kernel * kernel;
	size_t attempts = 0;
	size_t attempts_limit;
	std::string kernel_process_name = "";
};


class executor{
public:
	static executor * get_instance(){
		if(_instance == nullptr){
			throw std::runtime_error("Executor not initialized.");
		}
		return _instance;
	}

	static void init_executor(int num_threads){
		if(!_instance){
			_instance = new executor(num_threads);
			_instance->task_id_counter = 0;
			_instance->active_tasks_counter = 0;
			auto thread = std::thread([_instance]{
				_instance->execute();
			});
			thread.detach();
		}
	}

	void execute();
	size_t add_task(std::vector<std::unique_ptr<ral::cache::CacheData > > inputs,
		std::shared_ptr<ral::cache::CacheMachine> output,
		ral::cache::kernel * kernel,std::string kernel_process_name = "");

	void add_task(std::vector<std::unique_ptr<ral::cache::CacheData > > inputs,
		std::shared_ptr<ral::cache::CacheMachine> output,
		ral::cache::kernel * kernel,
		size_t attempts,
		size_t task_id,std::string kernel_process_name);

private:
	executor(int num_threads);
	ctpl::thread_pool<BlazingThread> pool;
	std::vector<cudaStream_t> streams; //one stream per thread
	ral::cache::WaitingQueue< std::unique_ptr<task> > task_queue;
	int shutdown = 0;
	static executor * _instance;
	std::atomic<int> task_id_counter;
	size_t attempts_limit = 10;
	
	BlazingMemoryResource* resource;
	std::atomic<int> active_tasks_counter;
	std::mutex memory_safety_mutex;
	std::condition_variable memory_safety_cv;
};


} // namespace execution
} // namespace ral