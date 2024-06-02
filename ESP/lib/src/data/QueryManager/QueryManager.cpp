#include "QueryManager.hpp"

std::string QueryManager::formatMessage(QueryPayload* messagePayload) {
  return Helpers::format_string(
      "{\"action\": \"%s\", \"status\": %d, \"additional_info\": \"%s\"}",
      queryActionMap.at(messagePayload->action).c_str(), messagePayload->status,
      messagePayload->additionalInfo.c_str());
}