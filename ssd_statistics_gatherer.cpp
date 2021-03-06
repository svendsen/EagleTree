/*
 * ssd_statistics_gatherer.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: niv
 */

#include "ssd.h"
using namespace ssd;
#include <stdio.h>

StatisticsGatherer *StatisticsGatherer::inst = NULL;

StatisticsGatherer::StatisticsGatherer(Ssd& ssd)
	: ssd(ssd),
	  sum_bus_wait_time_for_reads_per_LUN(SSD_SIZE, vector<double>(PACKAGE_SIZE, 0)),
	  num_reads_per_LUN(SSD_SIZE, vector<uint>(PACKAGE_SIZE, 0)),
	  sum_bus_wait_time_for_writes_per_LUN(SSD_SIZE, vector<double>(PACKAGE_SIZE, 0)),
	  num_writes_per_LUN(SSD_SIZE, vector<uint>(PACKAGE_SIZE, 0)),
	  num_gc_reads_per_LUN(SSD_SIZE, vector<uint>(PACKAGE_SIZE, 0)),
	  num_gc_writes_per_LUN(SSD_SIZE, vector<uint>(PACKAGE_SIZE, 0)),
	  num_erases_per_LUN(SSD_SIZE, vector<uint>(PACKAGE_SIZE, 0))
{}

StatisticsGatherer::~StatisticsGatherer() {}

void StatisticsGatherer::init(Ssd * ssd)
{
	StatisticsGatherer::inst = new StatisticsGatherer(*ssd);
}

StatisticsGatherer *StatisticsGatherer::get_instance()
{
	return StatisticsGatherer::inst;
}

void StatisticsGatherer::register_completed_event(Event const& event) {
	Address a = event.get_address();
	if (event.get_event_type() == WRITE) {
		sum_bus_wait_time_for_writes_per_LUN[a.package][a.die] += event.get_bus_wait_time();
		if (event.is_original_application_io()) {
			num_writes_per_LUN[a.package][a.die]++;
		} else if (event.is_garbage_collection_op()) {
			num_gc_writes_per_LUN[a.package][a.die]++;
		}
	} else if (event.get_event_type() == READ_COMMAND || event.get_event_type() == READ_TRANSFER) {
		sum_bus_wait_time_for_reads_per_LUN[a.package][a.die] += event.get_bus_wait_time();
		if (event.get_event_type() == READ_TRANSFER) {
			if (event.is_original_application_io()) {
				num_reads_per_LUN[a.package][a.die]++;
			} else if (event.is_garbage_collection_op()) {
				num_gc_reads_per_LUN[a.package][a.die]++;
			}
		}
	} else if (event.get_event_type() == ERASE) {
		num_erases_per_LUN[a.package][a.die]++;
	}
}

void StatisticsGatherer::print() {
	printf("\n\t");
	printf("num writes\t");
	printf("num reads\t");
	printf("GC writes\t");
	printf("GC reads\t");
	printf("erases\t\t");
	printf("avg write wait\t");
	printf("avg read wait\t");
	//printf("age\t");

	printf("\n");


	uint total_writes = 0;
	uint total_reads = 0;
	uint total_gc_writes = 0;
	uint total_gc_reads = 0;
	uint total_erases = 0;
	double avg_overall_write_wait_time = 0;
	double avg_overall_read_wait_time = 0;

	for (uint i = 0; i < SSD_SIZE; i++) {
		for (uint j = 0; j < PACKAGE_SIZE; j++) {
			total_writes += num_writes_per_LUN[i][j];
			total_reads += num_reads_per_LUN[i][j];
			total_gc_writes += num_gc_writes_per_LUN[i][j];
			total_gc_reads += num_gc_reads_per_LUN[i][j];
			total_erases += num_erases_per_LUN[i][j];

			double average_write_wait_time = sum_bus_wait_time_for_writes_per_LUN[i][j] / num_writes_per_LUN[i][j];

			avg_overall_write_wait_time += average_write_wait_time;

			double average_read_wait_time = sum_bus_wait_time_for_reads_per_LUN[i][j] / num_reads_per_LUN[i][j];

			avg_overall_read_wait_time += average_read_wait_time;

			if (num_writes_per_LUN[i][j] == 0) {
				average_write_wait_time = 0;
			}
			if (num_reads_per_LUN[i][j] == 0) {
				average_read_wait_time = 0;
			}
			double avg_age = compute_average_age(i, j);

			printf("C%d D%d\t", i, j);

			printf("%d\t\t", num_writes_per_LUN[i][j]);
			printf("%d\t\t", num_reads_per_LUN[i][j]);

			printf("%d\t\t", num_gc_writes_per_LUN[i][j]);
			printf("%d\t\t", num_gc_reads_per_LUN[i][j]);
			printf("%d\t\t", num_erases_per_LUN[i][j]);

			printf("%f\t", average_write_wait_time);
			printf("%f\t", average_read_wait_time);

			//printf("%f\t", avg_age);
			printf("\n");
		}
	}
	avg_overall_write_wait_time /= SSD_SIZE * PACKAGE_SIZE;
	avg_overall_read_wait_time /= SSD_SIZE * PACKAGE_SIZE;
	printf("\nTotals:\t");
	printf("%d\t\t", total_writes);
	printf("%d\t\t", total_reads);
	printf("%d\t\t", total_gc_writes);
	printf("%d\t\t", total_gc_reads);
	printf("%d\t\t", total_erases);
	printf("%f\t\t", avg_overall_write_wait_time);
	printf("%f\t\t", avg_overall_read_wait_time);
	printf("\n");
}

void StatisticsGatherer::print_csv() {
	printf("\n");
	printf("Channel,");
	printf("Die,");
	printf("num writes,");
	printf("num reads,");
	printf("GC writes,");
	printf("GC reads,");
	printf("erases,");
	printf("write wait,");
	printf("read wait,");
	//printf("age\t");

	printf("\n");
	for (uint i = 0; i < SSD_SIZE; i++) {
		for (uint j = 0; j < PACKAGE_SIZE; j++) {
			double average_write_wait_time = sum_bus_wait_time_for_writes_per_LUN[i][j] / num_writes_per_LUN[i][j];
			double average_read_wait_time = sum_bus_wait_time_for_reads_per_LUN[i][j] / num_reads_per_LUN[i][j];
			if (num_writes_per_LUN[i][j] == 0) {
				average_write_wait_time = 0;
			}
			if (num_reads_per_LUN[i][j] == 0) {
				average_read_wait_time = 0;
			}
			double avg_age = compute_average_age(i, j);

			printf("%d,%d,", i, j);

			printf("%d,", num_writes_per_LUN[i][j]);
			printf("%d,", num_reads_per_LUN[i][j]);

			printf("%d,", num_gc_writes_per_LUN[i][j]);
			printf("%d,", num_gc_reads_per_LUN[i][j]);
			printf("%d,", num_erases_per_LUN[i][j]);

			printf("%f,", average_write_wait_time);
			printf("%f,", average_read_wait_time);

			//printf("%f\t", avg_age);
			printf("\n");
		}
	}
}

double StatisticsGatherer::compute_average_age(uint package_id, uint die_id) {
	double sum_age = 0;
	for (uint i = 0; i < DIE_SIZE; i++) {
		for (uint j = 0; j < PLANE_SIZE; j++) {
			 Block& b = ssd.getPackages()[package_id].getDies()[die_id].getPlanes()[i].getBlocks()[j];
			 uint age = BLOCK_ERASES - b.get_erases_remaining();
			 sum_age += age;
		}
	}
	return sum_age / (DIE_SIZE * PLANE_SIZE);
}





