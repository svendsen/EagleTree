/* Copyright 2011 Matias Bjørling */

/* ssd_ftlparent.cpp  */

/* FlashSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version. */

/* FlashSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License
 * along with FlashSim.  If not, see <http://www.gnu.org/licenses/>. */

/****************************************************************************/

/*
 * Implements parent interface for all FTL implementations to use.
 */

#include "ssd.h"

using namespace ssd;


FtlParent::FtlParent(Controller &controller) : controller(controller)
{
	printf("Number of addressable blocks: %u\n", NUMBER_OF_ADDRESSABLE_BLOCKS);
}


ulong FtlParent::get_erases_remaining(const Address &address) const
{
	return controller.get_erases_remaining(address);
}

enum page_state FtlParent::get_state(const Address &address) const
{
	return controller.get_state(address);
}

enum block_state FtlParent::get_block_state(const Address &address) const
{
	return controller.get_block_state(address);
}

Block *FtlParent::get_block_pointer(const Address &address)
{
	return controller.get_block_pointer(address);
}

void FtlParent::print_ftl_statistics()
{
	return;
}

void FtlParent::register_write_completion(Event const& event, enum status result) {
	assert(false);
	return;
}

void FtlParent::register_read_completion(Event const& event, enum status result) {
	assert(false);
	return;
}

void FtlParent::register_trim_completion(Event & event) {
	assert(false);
	return;
}

long FtlParent::get_logical_address(uint physical_address) const {
	assert(false);
	return 0;
}

void FtlParent::set_replace_address(Event& event) const {
	assert(false);
	return;
}

void FtlParent::set_read_address(Event& event) {
	assert(false);
	return;
}
