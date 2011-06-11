/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership.  Green Enery
 * Corp licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef __VTO_ROUTER_MANAGER_H_
#define __VTO_ROUTER_MANAGER_H_

#include <vector>

#include <APL/Types.h>
#include <APL/Loggable.h>
#include <APL/IPhysMonitor.h>

namespace apl
{
class ITimerSource;
class IPhysicalLayerAsync;
class IPhysicalLayerSource;
}

namespace boost
{
namespace asio
{
class io_service;
}
}

namespace apl
{
namespace dnp
{

class VtoRouter;
struct VtoRouterSettings;
class IVtoWriter;
class IVtoDataHandler;

class VtoRouterManager : private Loggable
{
public:
	class RouterRecord
	{
	public:
		RouterRecord(const std::string& arPortName, VtoRouter* apRouter, IVtoWriter* apWriter, boost::uint8_t aVtoChannelId);

		std::string mPortName;
		VtoRouter* mpRouter;
		IVtoWriter* mpWriter;
		boost::uint8_t mVtoChannelId;
	};


	VtoRouterManager(Logger* apLogger, ITimerSource* apTimerSrc, IPhysicalLayerSource* apPhysSrc);

	~VtoRouterManager();

	VtoRouter* StartRouter(
	        const std::string& arPortName,
	        const VtoRouterSettings& arSettings,
	        IVtoWriter* apWriter);

	void StopRouter(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId);

	RouterRecord GetRouterOnWriter(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId);
	std::vector<RouterRecord> GetAllRoutersOnWriter(IVtoWriter* apWriter);
	std::vector<RouterRecord> GetAllRouters();

private:

	void StopRouter(VtoRouter* apRouter);

	static void ClenupAfterRouter(IPhysicalLayerAsync* apPhys, VtoRouter* apRouter);

	typedef std::vector<RouterRecord> RouterRecordVector;

	RouterRecordVector::iterator Find(IVtoWriter* apWriter, boost::uint8_t aVtoChannelId);

	RouterRecordVector::iterator Find(IVtoWriter* apWriter);

	Logger* GetSubLogger(const std::string& arId, boost::uint8_t aVtoChannelId);

	void StopRouter(const RouterRecordVector::iterator& arIter);

	RouterRecordVector mRecords;

	ITimerSource* mpTimerSrc;
	IPhysicalLayerSource* mpPhysSource;
};

}
}

#endif
