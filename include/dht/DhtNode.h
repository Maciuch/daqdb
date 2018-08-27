/**
 * Copyright 2017-2018 Intel Corporation.
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you (Intel OBL Internal Use License).
 * Unless the License provides otherwise, you may not use, modify, copy,
 * publish, distribute, disclose or transmit this software or the related
 * documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no
 * express or implied warranties, other than those that are expressly
 * stated in the License.
 */

#pragma once

#include <asio/io_service.hpp>
#include <dht/PureNode.h>

namespace FogKV
{

/*!
 * Class that defines interface for DHT
 */
class DhtNode : public PureNode {
public:
	DhtNode(asio::io_service &io_service, unsigned short port, unsigned short dragonPort);
	virtual ~DhtNode();

	/*!
	 * Prints DHT status.
	 * @return
	 */
	virtual std::string printStatus() = 0;

	/*!
	 * Fill peerNodes vector with peer node list from DHT.
	 * This is a subset of full list of nodes in system.
	 *
	 * @param peerNodes vector to insert peer nodes
	 * @return number of peer nodes
	 */
	virtual unsigned int
		getPeerList(std::vector<PureNode*>& peerNodes) = 0;

	/*!
	 * Triggers dragon aggregation table update.
	 */
	virtual void triggerAggregationUpdate() = 0;
};

}
