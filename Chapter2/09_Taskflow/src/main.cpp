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

	auto task = taskflow.for_each(
		items.begin(), items.end(), [](int item)
		{
			std::cout << item;
		}
	);

	taskflow.emplace([]() { std::cout << "\nS - Start\n"; }).name("S").precede(task);
	taskflow.emplace([]() { std::cout << "\nT - End\n"; }).name("T").succeed(task);

	{
		std::ofstream os("taskflow.dot");
		taskflow.dump(os);
	}

	tf::Executor executor;
	executor.run(taskflow).wait();

	return 0;
}
