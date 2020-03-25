#pragma once

// #include <blazingdb/manager/NodeDataMessage.h>
#include <blazingdb/transport/Message.h>
#include <blazingdb/transport/Status.h>
#include <memory>

namespace ral {
namespace communication {
namespace network {
namespace experimental{
using Node = blazingdb::transport::experimental::Node;
using GPUMessage = blazingdb::transport::experimental::GPUMessage;
using Message = blazingdb::transport::experimental::Message;
using Status = blazingdb::transport::experimental::Status;

class Client {
public:
	static Status send(const Node & node, GPUMessage & message);

	static bool setNumberOfBatches(const Node & node, Message::MetaData &message_metadata, size_t n_batches);

	static bool notifyLastMessageEvent(const Node & node, const Message::MetaData &message_metadata);

	static Status sendNodeData(std::string ip, int16_t port, Message & message);

	static void closeConnections();
};
}  // namespace experimental
}  // namespace network
}  // namespace communication
}  // namespace ral
