/*
 * UnionRDD.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_UNIONRDD_HPP_
#define INCLUDE_UNIONRDD_HPP_

#include "UnionRDD.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "SunwayMRContext.hpp"
#include "UnionPartition.hpp"
using namespace std;

/*
 * constructor
 */
template <class T>
UnionRDD<T>::UnionRDD(SunwayMRContext *context, vector< RDD<T>* > &rdds)
:RDD<T>::RDD(context), rdds(rdds) {
	long unionRDD_id = RDD<T>::rddID;

	// all the partitions are from previous rdds
	vector<Partition*> partitions;
	int partition_id = 0;
	for (unsigned int i=0; i<rdds.size(); i++) {
		vector<Partition*> vp = rdds[i]->getPartitions();
		for (unsigned int j=0; j<vp.size(); j++) {
			Partition *p = new UnionPartition<T>(unionRDD_id, partition_id, rdds[i], vp[j]);
			partitions.push_back(p);
			partition_id ++;
		}
	}
	RDD<T>::partitions = partitions;
}

/*
 * to get partitions of this RDD
 */
template <class T>
vector<Partition*> UnionRDD<T>::getPartitions() {
	return RDD<T>::partitions;
}

/*
 * to get preferred locations of a partition
 */
template <class T>
vector<string> UnionRDD<T>::preferredLocations(Partition *p) {
	UnionPartition<T> *up = dynamic_cast<UnionPartition<T> * >(p);
	return up->preferredLocations();
}

/*
 * to get data of a partition
 */
template <class T>
IteratorSeq<T> * UnionRDD<T>::iteratorSeq(Partition *p) {
	UnionPartition<T> *up = dynamic_cast<UnionPartition<T> * >(p);
	return up->iteratorSeq();
}

/*
 * to shuffle.
 * just do shuffle in each previous RDD.
 */
template <class T>
void UnionRDD<T>::shuffle() {
	for(unsigned int i=0; i<rdds.size(); i++) {
		rdds[i]->shuffle();
	}
}

/*
 * destructor, deleting previous RDDs if not sticky
 */
template <class T>
UnionRDD<T>::~UnionRDD() {
	for(unsigned int i=0; i<rdds.size(); i++) {
		if(!this->rdds[i]->isSticky()) {
			delete this->rdds[i];
		}
	}
	rdds.clear();
}

#endif /* INCLUDE_UNIONRDD_HPP_ */
