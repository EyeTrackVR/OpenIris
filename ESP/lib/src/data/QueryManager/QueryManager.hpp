#pragma once
#ifndef QUERY_MANAGER_HPP
#define QUERY_MANAGER_HPP
#include <string>
#include <unordered_map>
#include "data/utilities/helpers.hpp"

enum QueryAction {
  READY_TO_RECEIVE,
  PARSE_COMMANDS,
  CONNECT_TO_WIFI,
};

enum QueryStatus {
  NONE,
  SUCCESS,
  ERROR,
};

struct QueryPayload {
  QueryAction action;
  QueryStatus status;
  std::string additionalInfo;

  QueryPayload(QueryAction action,
               QueryStatus status,
               std::string additionalInfo) {
    this->action = action;
    this->status = status;
    this->additionalInfo = additionalInfo;
  }
};

class QueryManager {
 private:
  const std::unordered_map<QueryAction, std::string> queryActionMap = {
      {QueryAction::READY_TO_RECEIVE, "ready_to_receive"},
      {QueryAction::PARSE_COMMANDS, "parse_commands"},
      {QueryAction::CONNECT_TO_WIFI, "connect_to_wifi"},
  };

 public:
  std::string formatMessage(QueryPayload* messagePayload);
};

#endif