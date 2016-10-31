/*
 * FILE = HEADER: /include/interrupt_handler.h
 *
 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#ifndef _interrupt_handler_
#define _interrupt_handler_

#include "global_ids.h"
#include <stdlib.h>

void interrupt_handler(int signum);

#endif
