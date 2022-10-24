#pragma once

#include <pthread.h>

#include "gb.h"

int ui_main(pthread_mutex_t *gb_mutex, GameBoy *gb);