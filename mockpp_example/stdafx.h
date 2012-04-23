// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include <mockpp/mockpp.h> // always first

#include <mockpp/MockObject.h>
#include <mockpp/ExpectationList.h>
#include <mockpp/ReturnObjectList.h>
#include <mockpp/ExpectationCounter.h>

#include <mockpp/constraint/ConstraintList.h>
#include <mockpp/chaining/ChainingMockObjectSupport.h>
