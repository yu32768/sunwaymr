/*
 * SunwayMRContext.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_HPP_
#define SUNWAYMRCONTEXT_HPP_


#include "SunwayMRContext.h"

#include <stdlib.h>

#include "IteratorSeq.hpp"
#include "JobScheduler.hpp"
#include "Logging.hpp"
#include "ParallelArray.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"

SunwayMRContext::SunwayMRContext() {
	hostsFilePath = "";
	master = "";
	listenPort = 0;
	scheduler = JobScheduler();
}

SunwayMRContext::SunwayMRContext(string appName, int argc, char *argv[])
: appName(appName) {
	if (argc < 4) {
		// error
		logError("SunwayMRContext: 3 parameters at least: hostsFile, master, listenPort");
		exit(101);

	} else {
		hostsFilePath = string(argv[1]);
		master = string(argv[2]);
		listenPort = atoi(argv[3]);

		scheduler = JobScheduler(hostsFilePath, master, appName, listenPort);


		logInfo("SunwayMRContext: starting scheduler...");
		bool r = scheduler.start();
		if (!r) {
			logError("SunwayMRContext: failed to start scheduler, listen port may be in use.");
			exit(102);
		}
		logInfo("SunwayMRContext: starting scheduler succeeded");

	}
}

SunwayMRContext::SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort)
: hostsFilePath(hostsFilePath), master(master), appName(appName),
  listenPort(listenPort) {

	scheduler = JobScheduler(hostsFilePath, master, appName, listenPort);


	logInfo("SunwayMRContext: starting scheduler...");
	bool r = scheduler.start();
	if (!r) {
		logError("SunwayMRContext: failed to start scheduler, listen port may be in use.");
		exit(102);
	}
	logInfo("SunwayMRContext: starting scheduler succeeded");

}

void SunwayMRContext::init(string hostsFilePath, string master, string appName, int listenPort) {
	this->hostsFilePath = hostsFilePath;
	this->master = master;
	this->appName = appName;
	this->listenPort = listenPort;

	scheduler = JobScheduler(hostsFilePath, master, appName, listenPort);
	startScheduler();

}

void SunwayMRContext::startScheduler() {
	logInfo("SunwayMRContext: starting scheduler...");
	bool r = scheduler.start();
	if (!r) {
		logError("SunwayMRContext: failed to start scheduler, listen port may be in use.");
		exit(102);
	}
	logInfo("SunwayMRContext: starting scheduler succeeded");
}

ParallelArray<int> SunwayMRContext::parallelize(int start, int end) {
	int step = (start > end) ? -1 : 1;
	IteratorSeq<int> *iter = new IteratorSeq<int>(start, end, step);
	return parallelize<int>(*iter, scheduler.totalThreads());
}

ParallelArray<int> SunwayMRContext::parallelize(int start, int end, int numSlices) {
	int step = (start > end) ? -1 : 1;
	IteratorSeq<int> *iter = new IteratorSeq<int>(start, end, step);
	return parallelize<int>(*iter, numSlices);
}

ParallelArray<long> SunwayMRContext::parallelize(long start, long end) {
	long step = (start > end) ? -1l : 1l;
	IteratorSeq<long> *iter = new IteratorSeq<long>(start, end, step);
	return parallelize<long>(*iter, scheduler.totalThreads());
}

ParallelArray<long> SunwayMRContext::parallelize(long start, long end, int numSlices) {
	long step = (start > end) ? -1l : 1l;
	IteratorSeq<long> *iter = new IteratorSeq<long>(start, end, step);
	return parallelize<long>(*iter, numSlices);
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(vector<T> &v) {
	IteratorSeq<T> *iter = new IteratorSeq<T>(v);
	return parallelize<T>(*iter, scheduler.totalThreads());
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(vector<T> &v, int numSlices){
	IteratorSeq<T> *iter = new IteratorSeq<T>(v);
	return parallelize<T>(*iter, numSlices);
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(IteratorSeq<T> &iter) {
	return parallelize<T>(iter, scheduler.totalThreads());
}

// actual parallelizer
template <class T> ParallelArray<T> SunwayMRContext::parallelize(IteratorSeq<T> &iter, int numSlices) {
	if (numSlices < 1)
	{
		this->logError("ParallelArray: slice number should be positive integer!");
		exit(104);
	}
	return ParallelArray<T>(*this, iter, numSlices);
}

template <class T> vector< TaskResult<T>* > SunwayMRContext::runTasks(vector< Task<T>* > &tasks) {
	return scheduler.runTasks(tasks);
}



#endif /* SUNWAYMRCONTEXT_HPP_ */
