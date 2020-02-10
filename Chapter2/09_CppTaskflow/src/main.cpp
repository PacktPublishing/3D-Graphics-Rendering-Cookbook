#include <stdio.h>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

#include <taskflow/taskflow.hpp>

int main()
{
	tf::Taskflow taskflow;

	std::vector<int> items{ 1, 2, 3, 4, 5, 6, 7, 8 };

	auto [s, t] = taskflow.parallel_for(
		items.begin(), items.end(), []( int item )
		{
			std::cout << std::this_thread::get_id() << " runs " << item << std::endl;
		}
	);

	s.work( []() { std::cout << "S - Start\n"; } ).name( "S" );
	t.work( []() { std::cout << "T - End\n"; } ).name( "T" );

	{
		std::ofstream os( "taskflow.dot" );
		taskflow.dump( os );
	}

	tf::Executor executor;
	executor.run( taskflow ).wait();

	return 0;
}
