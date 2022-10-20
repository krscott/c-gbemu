#pragma once

#include "common.h"

typedef struct Bus Bus;

u8 bus_read(const Bus *bus, u16 address);
void bus_write(Bus *bus, u16 address, u8 value);
