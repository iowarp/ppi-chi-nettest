/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CHI_INCLUDE_CHI_CHI_TYPES_H_
#define CHI_INCLUDE_CHI_CHI_TYPES_H_

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/atomic.hpp>
#include "hermes_shm/memory/allocator/allocator_factory_.h"

#define HSHM_CUSTOM_SETTINGS
#include <hermes_shm/constants/macros.h>
#ifndef HSHM_DEFAULT_ALLOC_T
#define HSHM_DEFAULT_ALLOC_T hipc::ScalablePageAllocator
#endif
#define CHI_ALLOC_T HSHM_DEFAULT_ALLOC_T

#ifdef HSHM_IS_HOST
#ifndef HSHM_DEFAULT_THREAD_MODEL
#ifdef CHIMAERA_RUNTIME
#define HSHM_DEFAULT_THREAD_MODEL hshm::thread::Argobots
#else
#define HSHM_DEFAULT_THREAD_MODEL hshm::thread::Pthread
#endif
#endif
#else
#define HSHM_DEFAULT_THREAD_MODEL hshm::thread::Cuda
#endif

#include <hermes_shm/util/config_parse.h>
#include <hermes_shm/data_structures/all.h>
#include <hermes_shm/util/auto_trace.h>
#include <hermes_shm/thread/lock.h>
#include <hermes_shm/thread/thread_model_manager.h>
#include <hermes_shm/types/atomic.h>
#include "hermes_shm/util/singleton.h"
#include "hermes_shm/constants/macros.h"

#include <boost/context/fiber_fcontext.hpp>

namespace bctx = boost::context::detail;

typedef uint8_t u8;   /**< 8-bit unsigned integer */
typedef uint16_t u16; /**< 16-bit unsigned integer */
typedef uint32_t u32; /**< 32-bit unsigned integer */
typedef uint64_t u64; /**< 64-bit unsigned integer */
typedef int8_t i8;    /**< 8-bit signed integer */
typedef int16_t i16;  /**< 16-bit signed integer */
typedef int32_t i32;  /**< 32-bit signed integer */
typedef int64_t i64;  /**< 64-bit signed integer */
typedef float f32;    /**< 32-bit float */
typedef double f64;   /**< 64-bit float */

#define NANOSECONDS(X) ((size_t)(X))
#define MICROSECONDS(X) (NANOSECONDS(X) * 1000)
#define MILLISECONDS(X) (NANOSECONDS(X) * 1000000)
#define SECONDS(X) (NANOSECONDS(X) * 1000000000)

HSHM_DATA_STRUCTURES_TEMPLATE(chi, CHI_ALLOC_T);

namespace chi {

using hshm::RwLock;
using hshm::Mutex;
using hshm::bitfield;
using hshm::bitfield8_t;
using hshm::bitfield16_t;
using hshm::bitfield32_t;
typedef hshm::bitfield<uint64_t> bitfield64_t;
using hshm::ScopedRwReadLock;
using hshm::ScopedRwWriteLock;
using hshm::ScopedMutex;
using hipc::LPointer;

typedef u32 NodeId;  /**< The ID of a node */
typedef u32 LaneId;  /**< The ID of a lane */
typedef u32 LaneGroupId;  /**< The ID of a lane group */
typedef u32 ContainerId;  /**< The ID of a container */
typedef u32 MethodId;     /**< The ID of a container method */
typedef u32 MonitorModeId;  /**< The ID of a container monitor mode */

/** Represents unique ID for states + queues */
template<int TYPE>
struct UniqueId {
  union {
    NodeId node_id_;  /**< The node the content is on */
    u32 prio_;      /**< The priority of the lane */
  };
  u32 hash_;     /**< The hash of the content the ID represents */
  u64 unique_;   /**< A unique id for the blob */

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar & node_id_;
    ar & hash_;
    ar & unique_;
  }

  /** Default constructor */
  HSHM_ALWAYS_INLINE
  UniqueId() = default;

  /** Emplace constructor */
  HSHM_ALWAYS_INLINE explicit
  UniqueId(NodeId node_id, u64 unique)
  : node_id_(node_id), hash_(0), unique_(unique) {}

  /** Emplace constructor (+hash) */
  HSHM_ALWAYS_INLINE explicit
  UniqueId(NodeId node_id, u32 hash, u64 unique)
  : node_id_(node_id), hash_(hash), unique_(unique) {}

  /** Copy constructor */
  HSHM_ALWAYS_INLINE
  UniqueId(const UniqueId &other) {
    node_id_ = other.node_id_;
    hash_ = other.hash_;
    unique_ = other.unique_;
  }

  /** Copy constructor */
  template<int OTHER_TYPE=TYPE>
  HSHM_ALWAYS_INLINE
  UniqueId(const UniqueId<OTHER_TYPE> &other) {
    node_id_ = other.node_id_;
    hash_ = other.hash_;
    unique_ = other.unique_;
  }

  /** Copy assignment */
  HSHM_ALWAYS_INLINE
  UniqueId& operator=(const UniqueId &other) {
    if (this != &other) {
      node_id_ = other.node_id_;
      hash_ = other.hash_;
      unique_ = other.unique_;
    }
    return *this;
  }

  /** Move constructor */
  HSHM_ALWAYS_INLINE
  UniqueId(UniqueId &&other) noexcept {
    node_id_ = other.node_id_;
    hash_ = other.hash_;
    unique_ = other.unique_;
  }

  /** Move assignment */
  HSHM_ALWAYS_INLINE
  UniqueId& operator=(UniqueId &&other) noexcept {
    if (this != &other) {
      node_id_ = other.node_id_;
      hash_ = other.hash_;
      unique_ = other.unique_;
    }
    return *this;
  }

  /** Check if null */
  [[nodiscard]]
  HSHM_ALWAYS_INLINE bool IsNull() const {
    return node_id_ == 0;
  }

  /** Get null id */
  HSHM_ALWAYS_INLINE
  static UniqueId GetNull() {
    static const UniqueId id(0, 0);
    return id;
  }

  /** Set to null id */
  HSHM_ALWAYS_INLINE
  void SetNull() {
    node_id_ = 0;
    hash_ = 0;
    unique_ = 0;
  }

  /** Get id of node from this id */
  [[nodiscard]]
  HSHM_ALWAYS_INLINE
  u32 GetNodeId() const { return node_id_; }

  /** Compare two ids for equality */
  HSHM_ALWAYS_INLINE
  bool operator==(const UniqueId &other) const {
    return unique_ == other.unique_ && node_id_ == other.node_id_;
  }

  /** Compare two ids for inequality */
  HSHM_ALWAYS_INLINE
  bool operator!=(const UniqueId &other) const {
    return unique_ != other.unique_ || node_id_ != other.node_id_;
  }

  /** Print operator */
  friend std::ostream& operator<<(std::ostream &os, const UniqueId &id) {
    return os << (std::to_string(id.node_id_) + "."
        + std::to_string(id.unique_));
  }

  /** Hash function */
  size_t Hash() const {
    return std::hash<u64>{}(unique_) +
           std::hash<u32>{}(node_id_);
  }
};

/** Uniquely identify a pool */
using PoolId = UniqueId<1>;
/** Uniquely identify a queue */
using QueueId = UniqueId<2>;
/** Uniquely identify a task */
using TaskId = UniqueId<3>;

/** The types of I/O that can be performed (for IoCall RPC) */
enum class IoType {
  kRead,
  kWrite,
  kNone
};

/** Route mode */
enum class TaskRouteMode {
  kThisWorker,
  kLocalWorker,
  kRemoteWorker
};

/** Context used for creating objects */
struct CreateContext {
  PoolId id_ = PoolId::GetNull();
  u32 global_containers_ = 0;
  u32 local_containers_pn_ = 0;
  u32 lanes_per_container_ = 0;

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(id_, global_containers_, local_containers_pn_, lanes_per_container_);
  }
};

/** Major identifier of subdomain */
typedef u32 SubDomainGroup;

/** Minor identifier of subdomain */
typedef u32 SubDomainMinor;

/** An unscoped subdomain of nodes or lanes */
struct SubDomainId {
  SubDomainGroup major_;  /**< NodeSet, ContainerSet, ... */
  SubDomainMinor minor_;  /**< NodeId, ContainerId, ... */

  /** Subdomain major groups */
  CLS_CONST SubDomainGroup kPhysicalNode = 0;
  CLS_CONST SubDomainGroup kContainerSet = 1;
  CLS_CONST SubDomainGroup kGlobalContainers = 1;  // Alias for kContainerSet
  CLS_CONST SubDomainGroup kLocalContainers = 3;
  CLS_CONST SubDomainGroup kContainerCache = 4;
  CLS_CONST SubDomainGroup kLast = 5;

  /** Default constructor */
  SubDomainId() = default;

  /** Emplace constructor */
  SubDomainId(SubDomainGroup major, SubDomainMinor minor) {
    major_ = major;
    minor_ = minor;
  }

  /** Emplace constructor (2) */
  SubDomainId(SubDomainGroup major) {
    major_ = major;
    minor_ = 0;
  }

  /** Copy constructor */
  SubDomainId(const SubDomainId &other) {
    major_ = other.major_;
    minor_ = other.minor_;
  }

  /** Copy assignment */
  SubDomainId& operator=(const SubDomainId &other) {
    if (this != &other) {
      major_ = other.major_;
      minor_ = other.minor_;
    }
    return *this;
  }

  /** Move constructor */
  SubDomainId(SubDomainId &&other) noexcept {
    major_ = other.major_;
    minor_ = other.minor_;
  }

  /** Move assignment */
  SubDomainId& operator=(SubDomainId &&other) noexcept {
    if (this != &other) {
      major_ = other.major_;
      minor_ = other.minor_;
    }
    return *this;
  }

  /** ID represents a physical node ID */
  bool IsPhysical() const {
    return major_ == kPhysicalNode;
  }

  /** ID represents a major+minor ID */
  bool IsMajor() const {
    return minor_ == 0;
  }

  /** ID represents a major ID */
  bool IsMinor() const {
    return minor_ > 0;
  }

  /** Create a physical ID subdomain */
  static SubDomainId CreatePhysical(SubDomainMinor minor) {
    return {kPhysicalNode, minor};
  }

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(major_, minor_);
  }

  /** Equality operator */
  bool operator==(const SubDomainId &other) const {
    return major_ == other.major_ && minor_ == other.minor_;
  }

  /** Hash function */
  size_t Hash() const {
    return std::hash<u32>{}(major_) + std::hash<u32>{}(minor_);
  }

  /** Print operator */
  friend std::ostream& operator<<(std::ostream &os,
                                  const SubDomainId &subdom_id) {
    return os << subdom_id.major_ << "." << subdom_id.minor_;
  }
};

/** Represents a scoped domain */
struct DomainId {
  PoolId scope_;
  SubDomainId sub_id_;

  /** Default constructor */
  DomainId() = default;

  /** Emplace constructor */
  DomainId(const PoolId &scope,
           const SubDomainId &sub_id) {
    scope_ = scope;
    sub_id_ = sub_id;
  }

  /** Emplace constructor (2) */
  DomainId(const PoolId &scope,
           const SubDomainGroup &group) {
    scope_ = scope;
    sub_id_ = {group};
  }

  /** Emplace constructor (3) */
  DomainId(const PoolId &scope,
           const SubDomainGroup &group,
           const SubDomainMinor &minor) {
    scope_ = scope;
    sub_id_ = {group, minor};
  }

  /** Copy constructor */
  DomainId(const DomainId &other) {
    scope_ = other.scope_;
    sub_id_ = other.sub_id_;
  }

  /** Copy assignment */
  DomainId& operator=(const DomainId &other) {
    scope_ = other.scope_;
    sub_id_ = other.sub_id_;
    return *this;
  }

  /** Move constructor */
  DomainId(DomainId &&other) noexcept {
    scope_ = other.scope_;
    sub_id_ = other.sub_id_;
  }

  /** Move assignment */
  DomainId& operator=(DomainId &&other) noexcept {
    scope_ = other.scope_;
    sub_id_ = other.sub_id_;
    return *this;
  }

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(scope_, sub_id_);
  }

  /** Equality operator */
  bool operator==(const DomainId &other) const {
    return scope_ == other.scope_ && sub_id_ == other.sub_id_;
  }

  /** Hash function */
  size_t Hash() const {
    return scope_.Hash() + sub_id_.Hash();
  }

  /** Print operator */
  friend std::ostream& operator<<(std::ostream &os,
                                  const DomainId &subdom_id) {
    return os << subdom_id.scope_ << "." << subdom_id.sub_id_;
  }
};

/** Select a region of a domain */
union DomainSelection {
  u32 hash_;
  u32 id_;
  struct {
    u32 off_;
    u32 count_;
  } range_;
  u64 int_;

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(int_);
  }

  /** Equality operator */
  bool operator==(const DomainSelection &other) const {
    return int_ == other.int_;
  }
};

/** Operations that can be performed on a domain */
enum class UpdateDomainOp {
  kContract,
  kExpand
};

/** Subdomain ID range */
struct SubDomainIdRange {
  SubDomainGroup group_;
  u32 off_, count_;

  /** Default constructor */
  SubDomainIdRange() = default;

  /** Emplace constructor */
  SubDomainIdRange(SubDomainGroup group, u32 off, u32 count) {
    group_ = group;
    off_ = off;
    count_ = count;
  }

  /** Copy constructor */
  SubDomainIdRange(const SubDomainIdRange &other) {
    group_ = other.group_;
    off_ = other.off_;
    count_ = other.count_;
  }

  /** Copy assignment */
  SubDomainIdRange& operator=(const SubDomainIdRange &other) {
    if (this != &other) {
      group_ = other.group_;
      off_ = other.off_;
      count_ = other.count_;
    }
    return *this;
  }

  /** Move constructor */
  SubDomainIdRange(SubDomainIdRange &&other) noexcept {
    group_ = other.group_;
    off_ = other.off_;
    count_ = other.count_;
  }

  /** Move assignment */
  SubDomainIdRange& operator=(SubDomainIdRange &&other) noexcept {
    if (this != &other) {
      group_ = other.group_;
      off_ = other.off_;
      count_ = other.count_;
    }
    return *this;
  }

  /** Serialization */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(group_, off_, count_);
  }
};

/** Info required for updating a domain */
struct UpdateDomainInfo {
  DomainId domain_id_;
  UpdateDomainOp op_;
  SubDomainIdRange range_;

  template<typename Ar>
  void serialize(Ar &ar) {
    ar(domain_id_, op_, range_);
  }
};

/** Flags for domain query */
typedef u32 DomainFlag;

/**
 * Represents the scheduling domain of a task.
 * */
struct DomainQuery {
  bitfield<DomainFlag> flags_;  /**< Flags */
  SubDomainGroup sub_id_;       /**< The subdomain to query */
  DomainSelection sel_;  /**< The subset of the subdomain to query */

  /** Range flags */
  CLS_CONST DomainFlag kLocal =
      BIT_OPT(DomainFlag, 1);
  CLS_CONST DomainFlag kDirect =
      BIT_OPT(DomainFlag, 2);
  CLS_CONST DomainFlag kGlobal =
      BIT_OPT(DomainFlag, 3);

  /** Selection flags */
  CLS_CONST DomainFlag kId =
      BIT_OPT(DomainFlag, 4);
  CLS_CONST DomainFlag kHash =
      BIT_OPT(DomainFlag, 5);
  CLS_CONST DomainFlag kRange =
      BIT_OPT(DomainFlag, 6);

  /** Iteration algos */
  CLS_CONST DomainFlag kBroadcast =
      BIT_OPT(DomainFlag, 15);
  CLS_CONST DomainFlag kRepUntilSuccess =
      BIT_OPT(DomainFlag, 18);
  CLS_CONST DomainFlag kChooseOne =
      BIT_OPT(DomainFlag, 19);
  CLS_CONST DomainFlag kForwardToLeader =
      BIT_OPT(DomainFlag, 17);

  /** Serialize domain id */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(flags_, sub_id_, sel_);
  }

  /** Get iteration flags */
  DomainFlag GetIterFlags() const {
    return flags_.bits_ & (kBroadcast |
                           kRepUntilSuccess | kChooseOne |
                           kForwardToLeader);
  }

  /** Default constructor. */
  HSHM_ALWAYS_INLINE
  DomainQuery() {}

  /** Copy constructor */
  HSHM_ALWAYS_INLINE
  DomainQuery(const DomainQuery &other) {
    flags_ = other.flags_;
    sub_id_ = other.sub_id_;
    sel_ = other.sel_;
  }

  /** Copy operator */
  HSHM_ALWAYS_INLINE
  DomainQuery& operator=(const DomainQuery &other) {
    if (this != &other) {
      flags_ = other.flags_;
      sub_id_ = other.sub_id_;
      sel_ = other.sel_;
    }
    return *this;
  }

  /** Move constructor */
  HSHM_ALWAYS_INLINE
  DomainQuery(DomainQuery &&other) noexcept {
    flags_ = other.flags_;
    sub_id_ = other.sub_id_;
    sel_ = other.sel_;
  }

  /** Move operator */
  HSHM_ALWAYS_INLINE
  DomainQuery& operator=(DomainQuery &&other) noexcept {
    if (this != &other) {
      flags_ = other.flags_;
      sub_id_ = other.sub_id_;
      sel_ = other.sel_;
    }
    return *this;
  }

  /** Equality operator */
  HSHM_ALWAYS_INLINE
  bool operator==(const DomainQuery &other) const {
    return flags_.bits_ == other.flags_.bits_ &&
        sub_id_ == other.sub_id_ &&
        sel_ == other.sel_;
  }

  /** Get the local node domain */
  static DomainQuery GetLocalId(const SubDomainGroup &sub_id, u32 id) {
    DomainQuery query;
    query.flags_.SetBits(kLocal | kId);
    query.sub_id_ = sub_id;
    query.sel_.id_ = id;
    return query;
  }

  /**
   * The scope of the query is the entire subdomain
   * @param sub_id The subdomain to query
   * @param iter_flags The iteration flags to set (e.g., kBroadcast)
   * */
  static DomainQuery GetGlobal(const SubDomainGroup &sub_id, u32 iter_flags) {
    DomainQuery query;
    query.flags_.SetBits(kGlobal | iter_flags);
    query.sub_id_ = sub_id;
    return query;
  }

  /**
   * The scope of the query is a subset of the domain
   * @param sub_id The subdomain to query
   * @param iter_flags The iteration flags to set (e.g., kBroadcast)
   * */
  static DomainQuery GetRange(const SubDomainGroup &sub_id,
                              u32 off, u32 count,
                              u32 iter_flags) {
    DomainQuery query;
    query.flags_.SetBits(kGlobal | kRange | iter_flags);
    query.sub_id_ = sub_id;
    query.sel_.range_.off_ = off;
    query.sel_.range_.count_ = count;
    return query;
  }

  /**
   * The scope of the query is the entire subdomain
   * @param sub_id The subdomain to query
   * @param iter_flags The iteration flags to set (e.g., kBroadcast)
   * */
  static DomainQuery GetGlobalBcast() {
    DomainQuery query;
    query.flags_.SetBits(kGlobal | kBroadcast);
    query.sub_id_ = SubDomainId::kGlobalContainers;
    return query;
  }

  /**
   * Id the query to an offset in the subdomain vector
   * @param sub_id The subdomain to query
   * @param id The ID to resolve in the subdomain
   * @param flags The iteration flags to set (e.g., kBroadcast)
   * */
  static DomainQuery GetDirectId(const SubDomainGroup &sub_id, u32 id,
                                   u32 iter_flags = kChooseOne) {
    DomainQuery query;
    query.flags_.SetBits(kDirect | kId | iter_flags);
    query.sub_id_ = sub_id;
    query.sel_.id_ = id;
    return query;
  }

  /**
   * Hash the query to an offset in the subdomain vector
   * @param sub_id The subdomain to query
   * @param hash The offset hash to resolve in the subdomain
   * @param flags The iteration flags to set (e.g., kBroadcast)
   * */
  static DomainQuery GetDirectHash(const SubDomainGroup &sub_id, u32 hash,
                                   u32 iter_flags = kBroadcast) {
    DomainQuery query;
    query.flags_.SetBits(kDirect | kHash | iter_flags);
    query.sub_id_ = sub_id;
    query.sel_.hash_ = hash;
    return query;
  }

  /** Hash function */
  size_t Hash() const {
      return
        std::hash<DomainFlag>{}(flags_.bits_) +
        std::hash<SubDomainGroup>{}(sub_id_) +
        std::hash<u64>{}(sel_.int_);
  }

  /** Print operator */
  friend std::ostream& operator<<(std::ostream &os,
      const DomainQuery &dom_query) {
    // Get scope string
    std::string scope;
    if (dom_query.flags_.Any(kLocal)) {
      scope = "local";
    } else if (dom_query.flags_.Any(kGlobal)) {
      scope = "global";
    } else if (dom_query.flags_.Any(kDirect)) {
      scope = "direct";
    }

    // Get iteration string
    std::string iter;
    if (dom_query.flags_.Any(kBroadcast)) {
      iter = "broadcast";
    } else if (dom_query.flags_.Any(kRepUntilSuccess)) {
      iter = "rep_until_success";
    } else if (dom_query.flags_.Any(kChooseOne)) {
      iter = "choose_one";
    } else if (dom_query.flags_.Any(kForwardToLeader)) {
      iter = "forward_to_leader";
    }

    // Get selection string
    std::string sel;
    if (dom_query.flags_.Any(kId)) {
      sel = "id";
    } else if (dom_query.flags_.Any(kHash)) {
      sel = "hash";
    } else if (dom_query.flags_.Any(kRange)) {
      sel = "range";
    }

    // Get range string
    std::string range;
    if (dom_query.flags_.Any(kRange)) {
      range = std::to_string(dom_query.sel_.range_.off_) + ":" +
              std::to_string(dom_query.sel_.range_.count_);
    } else if (dom_query.flags_.Any(kId)) {
      range = std::to_string(dom_query.sel_.id_);
    } else if (dom_query.flags_.Any(kHash)) {
      range = std::to_string(dom_query.sel_.hash_);
    }

    std::string format = hshm::Formatter::format(
        "DomainQuery(iter_algo={}, scope={}, sel={}, range={}, subdom={})",
        iter, scope, sel, range, dom_query.sub_id_);
    return os << format;
  }
};

/**
 * Represents a resolved domain query
 * */
struct ResolvedDomainQuery {
  NodeId node_;
  DomainQuery dom_;

  /** Default constructor */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery() {}

  /** Emplace constructor */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery(NodeId node, const DomainQuery &dom)
  : node_(node), dom_(dom) {}

  /** Emplace constructor (2) */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery(NodeId node)
  : node_(node) {}

  /** Copy constructor */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery(const ResolvedDomainQuery &other) {
    node_ = other.node_;
    dom_ = other.dom_;
  }

  /** Copy assignment */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery& operator=(const ResolvedDomainQuery &other) {
    if (this != &other) {
      node_ = other.node_;
      dom_ = other.dom_;
    }
    return *this;
  }

  /** Move constructor */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery(ResolvedDomainQuery &&other) noexcept {
    node_ = other.node_;
    dom_ = other.dom_;
  }

  /** Move assignment */
  HSHM_ALWAYS_INLINE
  ResolvedDomainQuery& operator=(ResolvedDomainQuery &&other) noexcept {
    if (this != &other) {
      node_ = other.node_;
      dom_ = other.dom_;
    }
    return *this;
  }

  /** Print operator */
  friend std::ostream& operator<<(
      std::ostream &os, const ResolvedDomainQuery &id) {
    return os << std::to_string(id.node_) << " / " << id.dom_;
  }

  /** Serialize */
  template<typename Ar>
  void serialize(Ar &ar) {
    ar(node_, dom_);
  }
};

class CacheTimer {
 public:
  hshm::Timer timer_;
  size_t cur_ns_ = 0;
  size_t start_ns_ = 0;
  size_t net_ns_ = 0;

 public:
  CacheTimer() {
    timer_.Now();
  }

  void Wrap(CacheTimer &other) {
    cur_ns_ = other.cur_ns_;
  }

  void Refresh() {
    cur_ns_ = timer_.GetNsecFromStart();
  }

  void Tick(size_t nanosec) {
    cur_ns_ += nanosec;
  }

  size_t GetNsecFromStart() {
    return cur_ns_ - start_ns_;
  }

  size_t GetNsecFromStart(size_t start) {
    if (start < cur_ns_) {
      return cur_ns_ - start;
    }
    return 0;
  }

  size_t GetNsec() {
    return net_ns_;
  }

  void Resume() {
    start_ns_ = cur_ns_;
  }

  void Pause() {
    net_ns_ += cur_ns_ - start_ns_;
    start_ns_ = cur_ns_;
  }

  void Reset() {
    start_ns_ = cur_ns_;
    net_ns_ = 0;
  }
};

}  // namespace chi

namespace std {

/** Hash function for UniqueId */
template <int TYPE>
struct hash<chi::UniqueId<TYPE>> {
  HSHM_ALWAYS_INLINE
  std::size_t operator()(const chi::UniqueId<TYPE> &key) const {
    return key.Hash();
  }
};

/** Hash function for SubDomainId */
template<>
struct hash<chi::SubDomainId> {
  HSHM_ALWAYS_INLINE
  std::size_t operator()(const chi::SubDomainId &key) const {
    return key.Hash();
  }
};

/** Hash function for DomainId */
template<>
struct hash<chi::DomainId> {
  HSHM_ALWAYS_INLINE
  std::size_t operator()(const chi::DomainId &key) const {
    return key.Hash();
  }
};

/** Hash function for DomainQuery */
template<>
struct hash<chi::DomainQuery> {
  HSHM_ALWAYS_INLINE
  std::size_t operator()(const chi::DomainQuery &key) const {
    return key.Hash();
  }
};

}  // namespace std

#endif  // CHI_INCLUDE_CHI_CHI_TYPES_H_
