#include <zkutil/ZooKeeperHolder.h>

using namespace zkutil;

ZooKeeperHolder::UnstorableZookeeperHandler ZooKeeperHolder::getZooKeeper()
{
	std::unique_lock<std::mutex> lock(mutex);
	return UnstorableZookeeperHandler(ptr);
}

bool ZooKeeperHolder::replaceZooKeeperSessionToNewOne()
{
	std::unique_lock<std::mutex> lock(mutex);

	if (ptr.unique())
	{
		ptr = ptr->startNewSession();
		return true;
	}
	else
	{
		LOG_ERROR(log, "replaceZooKeeperSessionToNewOne(): Fail to replace zookeeper session to new one because handlers for old zookeeper session still exists.");
		return false;
	}
}

bool ZooKeeperHolder::isSessionExpired() const
{
	return ptr ? ptr->expired() : false;
}


std::string ZooKeeperHolder::nullptr_exception_message =
	"UnstorableZookeeperHandler::zk_ptr is nullptr. "
	"ZooKeeperHolder should be initialized before sending any request to ZooKeeper";

ZooKeeperHolder::UnstorableZookeeperHandler::UnstorableZookeeperHandler(ZooKeeper::Ptr zk_ptr_)
: zk_ptr(zk_ptr_)
{
}

ZooKeeper * ZooKeeperHolder::UnstorableZookeeperHandler::operator->()
{
	if (zk_ptr == nullptr)
		throw DB::Exception(nullptr_exception_message);

	return zk_ptr.get();
}

const ZooKeeper * ZooKeeperHolder::UnstorableZookeeperHandler::operator->() const
{
	if (zk_ptr == nullptr)
		throw DB::Exception(nullptr_exception_message);
	return zk_ptr.get();
}

ZooKeeper & ZooKeeperHolder::UnstorableZookeeperHandler::operator*()
{
	if (zk_ptr == nullptr)
		throw DB::Exception(nullptr_exception_message);
	return *zk_ptr;
}

const ZooKeeper & ZooKeeperHolder::UnstorableZookeeperHandler::operator*() const
{
	if (zk_ptr == nullptr)
		throw DB::Exception(nullptr_exception_message);
	return *zk_ptr;
}