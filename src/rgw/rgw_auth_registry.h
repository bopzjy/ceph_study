// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab


#ifndef CEPH_RGW_AUTH_REGISTRY_H
#define CEPH_RGW_AUTH_REGISTRY_H

#include <functional>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>

#include "rgw_auth.h"
#include "rgw_auth_s3.h"
#include "rgw_swift_auth.h"

namespace rgw {
namespace auth {

/* A class aggregating the knowledge about all Strategies in RadosGW. It is
 * responsible for handling the dynamic reconfiguration on e.g. realm update. */
class StrategyRegistry {
  template <class ExtractorT>
  using s3_strategy_t = rgw::auth::s3::AWSv2AuthStrategy<ExtractorT>;

  using s3_main_strategy_t = \
    s3_strategy_t<rgw::auth::s3::RGWS3V2Extractor>;
  using s3_post_strategy_t = \
    s3_strategy_t<rgw::auth::s3::RGWGetPolicyV2Extractor>;

  s3_main_strategy_t s3_main_strategy;
  s3_post_strategy_t s3_post_strategy;

  rgw::auth::swift::DefaultStrategy swift_strategy;

public:
  StrategyRegistry(CephContext* const cct,
                   RGWRados* const store)
    : s3_main_strategy(cct, store),
      s3_post_strategy(cct, store),
      swift_strategy(cct, store) {
  }

public:
  const s3_main_strategy_t& get_s3_main() const {
    return s3_main_strategy;
  }

  const s3_post_strategy_t& get_s3_post() const {
    return s3_post_strategy;
  }

  const rgw::auth::swift::DefaultStrategy& get_swift() const {
    return swift_strategy;
  }

  static std::shared_ptr<StrategyRegistry>
  create(CephContext* const cct,
         RGWRados* const store) {
    return std::make_shared<StrategyRegistry>(cct, store);
  }
};

} /* namespace auth */
} /* namespace rgw */

using rgw_auth_registry_t = rgw::auth::StrategyRegistry;
using rgw_auth_registry_ptr_t = std::shared_ptr<rgw_auth_registry_t>;

#endif /* CEPH_RGW_AUTH_REGISTRY_H */
