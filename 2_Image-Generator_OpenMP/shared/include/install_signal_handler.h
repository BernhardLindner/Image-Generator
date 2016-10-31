/*
 * FILE = HEADER: /include/install_signal_handler.h

 * Copyright (c) 2016 Bernhard Lindner
 *
 * This file is licensed under the terms of the MIT License.
 * See /LICENSE for details.
 */

#ifndef _install_signal_handler_
#define _install_signal_handler_

int init_signal_handler(int signum, void (*handlerfunc)(int));

#endif
