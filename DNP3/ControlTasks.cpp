//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Green Enery Corp licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

#include "ControlTasks.h"
#include "APDU.h"

#include <boost/bind.hpp>
#include <APL/Logger.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

namespace apl
{
namespace dnp
{

ControlTaskBase::ControlTaskBase(Logger* apLogger) :
	MasterTaskBase(apLogger),
	mState(INVALID)
{}

bool ControlTaskBase::GetSelectBit()
{
	switch(mState) {
	case(SELECT): return true;
	case(OPERATE): return false;
	default:
		throw InvalidStateException(LOCATION, "INVALID");
	}
}

void ControlTaskBase::Respond(CommandStatus aStatus)
{
	mData.mpRspAcceptor->AcceptResponse(CommandResponse(aStatus), mData.mSequence);
}

void ControlTaskBase::OnFailure()
{
	this->Respond(CS_HARDWARE_ERROR);
}

TaskResult ControlTaskBase::_OnPartialResponse(const APDU& arAPDU)
{
	LOG_BLOCK(LEV_ERROR, "Non fin responses not allowed for control tasks");
	ofstream myfile;
	myfile.open ("example.txt", ios::app);
	myfile << "Got non-final response\n";
	myfile.close();
	
	return TR_CONTINUE;
}

TaskResult ControlTaskBase::_OnFinalResponse(const APDU& arAPDU)
{
	ofstream myfile;
	myfile.open ("example.txt", ios::app);
	myfile << "Got final response\n with state " << mState << " success is " << CS_SUCCESS;
	myfile.close();
	CommandStatus cs = mValidator(arAPDU);

	if(mState == SELECT && cs == CS_SUCCESS) {
		mState = OPERATE;
		return TR_CONTINUE;
	}
	else {
		this->Respond(cs);
		return TR_SUCCESS;
	}

}

/* -------- BinaryOutputTask -------- */

BinaryOutputTask::BinaryOutputTask(Logger* apLogger) :
	ControlTask<BinaryOutput>(apLogger)
{}

CommandObject<BinaryOutput>* BinaryOutputTask::GetObject(const BinaryOutput&)
{
	return Group12Var1::Inst();
}

/* -------- SetpointTask -------- */

SetpointTask::SetpointTask(Logger* apLogger) :
	ControlTask<Setpoint>(apLogger)
{}

CommandObject<Setpoint>* SetpointTask::GetOptimalEncoder(SetpointEncodingType aType)
{
	switch(aType) {
	case SPET_INT16: return Group41Var2::Inst();
	case SPET_INT32: return Group41Var1::Inst();
	case SPET_FLOAT: return Group41Var3::Inst();
	case SPET_DOUBLE: return Group41Var4::Inst();
	default:
		throw ArgumentException(LOCATION, "Enum not handled");
	}
}

CommandObject<Setpoint>* SetpointTask::GetObject(const Setpoint& arSetpoint)
{
	return GetOptimalEncoder(arSetpoint.GetOptimalEncodingType());
}

/* -------- AnalogReadTask -------- */

AnalogReadTask::AnalogReadTask(Logger* apLogger) :
	ControlTask<AnalogRead>(apLogger)
{}

CommandObject<AnalogRead>* AnalogReadTask::GetObject(const AnalogRead& analogRead)
{
	mStartVal = analogRead.GetStartValue();
	//return Group30Var2::Inst();
	return Group12Var4::Inst();
}
//CommandObject<BinaryOutput>* BinaryOutputTask::GetObject(const BinaryOutput&)
//	return Group12Var1::Inst();

void AnalogReadTask::ConfigureRequest(APDU& arAPDU)
{
	LOG_BLOCK(LEV_ERROR, "Non fin respXXXXXXXXXXXXXXXXXX");
	  ofstream myfile;
	    myfile.open ("example.txt", ios::app);
	      myfile << "Writing this to a file.\n";
	myfile << "got val\n";
	myfile << "got val of " << mStartVal << endl;
	        myfile.close();
	if(mStartVal == 3) {
	  arAPDU.DoPlaceholderWrite(Group60Var1::Inst());
	} else {
	  arAPDU.DoPlaceholderWrite(Group60Var4::Inst());
	}
	//arAPDU.DoPlaceholderWrite(Group60Var3::Inst());
}



}
} //ens ns
