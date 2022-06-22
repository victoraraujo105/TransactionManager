#include "TransactionManager.h"

const map<ProtocolType, Protocol> TransactionManager::getProtocol{{ProtocolType::ABORT, abort}, {ProtocolType::WAIT_DIE, wait_die}, {ProtocolType::WOUND_WAIT, wound_wait}};