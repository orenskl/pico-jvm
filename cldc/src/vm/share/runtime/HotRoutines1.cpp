/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/*
 * This file includes HotRoutines.incl.hpp and selects the routines that
 * ARE compiled in fast memory. See HotRoutines.incl.hpp for more info.
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "GlobalDefinitions_c.hpp"
#include "GlobalDefinitions_gcc.hpp"
#include "Globals.hpp"

#include "Verifier.hpp"
#include "ObjectHeap.hpp"
#include "Bytecodes.hpp"
#include "Method.hpp"
#include "ClassFileParser.hpp"
#include "Throw.hpp"
#include "Inflate.hpp"
#include "VerifierFrame.hpp"
#include "Field.hpp"
#include "ExecutionStackDesc.hpp"
#include "ObjArrayDesc.hpp"
#include "CompiledMethodDesc.hpp"
#include "EntryActivationDesc.hpp"
#include "StackmapListDesc.hpp"
#include "TypeArrayClass.hpp"
#include "OopDesc.inline.hpp"

#define USE_HOT_ROUTINES 1
#include "HotRoutines.incl.hpp"