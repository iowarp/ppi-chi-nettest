#include "chimaera_types.h"

namespace chi {

/**
 * Sender writes to data_
 * Receiver reads from data_
 * */
#define DT_EXPOSE BIT_OPT(u32, 0)
#define DT_WRITE (BIT_OPT(u32, 1) | DT_EXPOSE)

/** Free data_ when the data transfer is complete */
#define DT_FREE_DATA BIT_OPT(u32, 2)

/** Indicate how data should be transferred over network */
template <bool NO_XFER>
struct DataTransferBase {
  hshm::bitfield32_t flags_; /**< Indicates how data will be accessed */
  void *data_;               /**< The virtual address of data on the node */
  size_t data_size_;         /**< The amount of data to transfer */

  /** Serialize a data transfer object */
  template <typename Ar>
  void serialize(Ar &ar) {
    ar(flags_, (size_t)data_, data_size_);
  }

  /** Default constructor */
  DataTransferBase() = default;

  /** Emplace constructor */
  DataTransferBase(u32 flags, void *data, size_t data_size)
      : flags_(flags), data_(data), data_size_(data_size) {}

  /** Copy constructor */
  DataTransferBase(const DataTransferBase &xfer)
      : flags_(xfer.flags_), data_(xfer.data_), data_size_(xfer.data_size_) {}

  /** Copy assignment */
  DataTransferBase &operator=(const DataTransferBase &xfer) {
    flags_ = xfer.flags_;
    data_ = xfer.data_;
    data_size_ = xfer.data_size_;
    return *this;
  }

  /** Move constructor */
  DataTransferBase(DataTransferBase &&xfer) noexcept
      : flags_(xfer.flags_), data_(xfer.data_), data_size_(xfer.data_size_) {}

  /** Equality operator */
  bool operator==(const DataTransferBase &other) const {
    return flags_.bits_ == other.flags_.bits_ && data_ == other.data_ &&
           data_size_ == other.data_size_;
  }
};

using DataTransfer = DataTransferBase<true>;
using PassDataTransfer = DataTransferBase<false>;

class SegmentedTransfer {
 public:
  NodeId ret_node_;                /**< The node to return to */
  std::vector<DataTransfer> bulk_; /**< Data payloads */

  void AllocateBulksServer() {
    for (DataTransfer &xfer : bulk_) { 
      xfer.data_ = malloc(xfer.data_size_);
    }
  }

  size_t size() const {
    size_t size = 0;
    for (const DataTransfer &xfer : bulk_) {
      size += xfer.data_size_;
    }
    return size;
  }

  template <typename Ar>
  void serialize(Ar &ar) {
    ar(ret_node_, bulk_);
  }
};

}  // namespace chi