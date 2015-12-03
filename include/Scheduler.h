/*
 * Scheduler.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>
#include "Task.h"
#include "TaskResult.h"
using std::vector;

class Scheduler {
public:
	Scheduler(string hostFilePath, string master, string appName, bool localMode = false);
	virtual ~Scheduler();
	virtual void start();
	virtual int totalThreads();
	template <class T> vector<TaskResult<T>> runTasks(vector<Task<T>> &tasks);

private:
	template <class T> void taskFinished(TaskResult<T> &t);

	// other new functions below...
};


#endif /* SCHEDULER_H_ */
