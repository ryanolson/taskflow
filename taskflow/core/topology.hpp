#pragma once

#define ENABLE_FIBERS

#ifdef ENABLE_FIBERS
#include <boost/fiber/all.hpp>
#else
#include <future>
#include <mutex>
#include <condition_variable>
#endif

namespace tf {

#ifdef ENABLE_FIBERS
// boost fibers sync

template<typename T>
using BaseFuture = boost::fibers::future<T>;

template<typename T>
using Promise = boost::fibers::promise<T>;

using Mutex = boost::fibers::mutex;
using CondV = boost::fibers::condition_variable;

#else
// standard library sync

template<typename T>
using BaseFuture = std::future<T>;

template<typename T>
using Promise = tf::Promise<T>;

using Mutex = std::mutex;
using CondV = std::condition_variable;

#endif


// ----------------------------------------------------------------------------

// class: TopologyBase
class TopologyBase {
  
  friend class Executor;
  friend class Node;
  
  template <typename T>
  friend class Future;

  protected:

  bool _is_cancelled { false };
};

// ----------------------------------------------------------------------------

// class: AsyncTopology
class AsyncTopology : public TopologyBase {
};

// ----------------------------------------------------------------------------
  
// class: Topology
class Topology : public TopologyBase {
  
  friend class Executor;

  public:

    template <typename P, typename C>
    Topology(Taskflow&, P&&, C&&);

  private:

    Taskflow& _taskflow;

    tf::Promise<void> _promise;

    std::vector<Node*> _sources;

    std::function<bool()> _pred;
    std::function<void()> _call;

    std::atomic<size_t> _join_counter {0};
};

// Constructor
template <typename P, typename C>
Topology::Topology(Taskflow& tf, P&& p, C&& c): 
  _taskflow(tf),
  _pred {std::forward<P>(p)},
  _call {std::forward<C>(c)} {
}

}  // end of namespace tf. ----------------------------------------------------
