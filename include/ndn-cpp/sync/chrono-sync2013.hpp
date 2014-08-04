/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from ChronoChat-js by Qiuhan Ding and Wentao Shang.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU General Public License is in the file COPYING.
 */

#ifndef NDN_CHRONO_SYNC_HPP
#define NDN_CHRONO_SYNC_HPP

#include <vector>
#include "../face.hpp"
#include "../security/key-chain.hpp"
#include "../util/memory-content-cache.hpp"

namespace google { namespace protobuf { template <typename Element> class RepeatedPtrField; } }
namespace Sync { class SyncStateMsg; }
namespace Sync { class SyncState; }

namespace ndn {

class DigestTree;

/**
 * ChronoSync2013 implements the NDN ChronoSync protocol as described in the
 * 2013 paper "Let’s ChronoSync: Decentralized Dataset State Synchronization in 
 * Named Data Networking". http://named-data.net/publications/chronosync .
 * @note The support for ChronoSync is experimental and the API is not finalized.
 * See the API docs for more detail at
 * http://named-data.net/doc/ndn-ccl-api/chrono-sync2013.html .
 */
class ChronoSync2013 {
public:
  class SyncState;
  typedef func_lib::function<void
    (const std::vector<ChronoSync2013::SyncState>& syncStates, bool isRecovery)>
      OnReceivedSyncState;

  typedef func_lib::function<void()> OnInitialized;

  /**
   * Create a new ChronoSync2013 to communicate using the given face. Initialize
   * the digest log with a digest of "00" and and empty content. Register the
   * applicationBroadcastPrefix to receive interests for sync state messages and
   * express an interest for the initial root digest "00".
   * @param onReceivedSyncState When ChronoSync receives a sync state message,
   * this calls onReceivedSyncState(syncStates, isRecovery) where syncStates is the
   * list of SyncState messages and isRecovery is true if this is the initial
   * list of SyncState messages or from a recovery interest. (For example, if
   * isRecovery is true, a chat application would not want to re-display all
   * the associated chat messages.) The callback should send interests to fetch
   * the application data for the sequence numbers in the sync state.
   * @param onInitialized This calls onInitialized() when the first sync data
   * is received (or the interest times out because there are no other
   * publishers yet).
   * @param applicationDataPrefix The prefix used by this application instance
   * for application data. For example, "/my/local/prefix/ndnchat4/0K4wChff2v".
   * This is used when sending a sync message for a new sequence number.
   * In the sync message, this uses applicationDataPrefix.toUri().
   * @param applicationBroadcastPrefix The broadcast name prefix including the
   * application name. For example, "/ndn/broadcast/ChronoChat-0.3/ndnchat1".
   * This makes a copy of the name.
   * @param sessionNo The session number used with the applicationDataPrefix in
   * sync state messages.
   * @param face The Face for calling registerPrefix and expressInterest. The
   * Face object must remain valid for the life of this ChronoSync2013 object.
   * @param keyChain To sign a data packet containing a sync state message, this
   * calls keyChain.sign(data, certificateName).
   * @param certificateName The certificate name of the key to use for signing a
   * data packet containing a sync state message.
   * @param syncLifetime The interest lifetime in milliseconds for sending
   * sync interests.
   * @param onRegisterFailed If failed to register the prefix to receive
   * interests for the applicationBroadcastPrefix, this calls
   * onRegisterFailed(applicationBroadcastPrefix).
   */
  ChronoSync2013
    (const OnReceivedSyncState& onReceivedSyncState,
     const OnInitialized& onInitialized, const Name& applicationDataPrefix, 
     const Name& applicationBroadcastPrefix, int sessionNo,
     Face& face, KeyChain& keyChain, const Name& certificateName,
     Milliseconds syncLifetime, const OnRegisterFailed& onRegisterFailed);

  /**
   * A SyncState holds the values of a sync state message which is passed to the
   * onReceivedSyncState callback which was given to the ChronoSyn2013
   * constructor. Note: this has the same info as the Protobuf class
   * Sync::SyncState, but we make a separate class so that we don't need the
   * Protobuf definition in the ChronoSync API.
   */
  class SyncState {
  public:
    SyncState(const std::string& dataPrefixUri, int sessionNo, int sequenceNo)
    : dataPrefixUri_(dataPrefixUri), sessionNo_(sessionNo), sequenceNo_(sequenceNo)
    {
    }

    /**
     * Get the application data prefix for this sync state message.
     * @return The application data prefix as a Name URI string.
     */
    const std::string&
    getDataPrefix() const { return dataPrefixUri_; }

    /**
     * Get the session number associated with the application data prefix for
     * this sync state message.
     * @return The session number.
     */
    int
    getSessionNo() const { return sessionNo_; }

    /**
     * Get the sequence number for this sync state message.
     * @return The sequence number.
     */
    int
    getSequenceNo() const { return sequenceNo_; }

  private:
    std::string dataPrefixUri_;
    int sessionNo_;
    int sequenceNo_;
  };

  /**
   * Get the current sequence number in the digest tree for the given
   * producer dataPrefix and sessionNo.
   * @param dataPrefix The producer data prefix as a Name URI string.
   * @param sessionNo The producer session number.
   * @return The current producer sequence number, or -1 if the producer
   * namePrefix and sessionNo are not in the digest tree.
   */
  int
  getProducerSequenceNo(const std::string& dataPrefix, int sessionNo) const;

  /**
   * Increment the sequence number, create a sync message with the new
   * sequence number and publish a data packet where the name is
   * the applicationBroadcastPrefix + the root digest of the current digest
   * tree. Then add the sync message to the digest tree and digest log which
   * creates a new root digest. Finally, express an interest for the next sync
   * update with the name applicationBroadcastPrefix + the new root digest.
   * After this, your application should publish the content for the new
   * sequence number. You can get the new sequence number with getSequenceNo().
   */
  void
  publishNextSequenceNo();

  /**
   * Get the sequence number of the latest data published by this application
   * instance.
   * @return The sequence number.
   */
  int
  getSequenceNo() const { return usrseq_; }

private:
  class DigestLogEntry {
  public:
    DigestLogEntry
      (const std::string& digest,
       const google::protobuf::RepeatedPtrField<Sync::SyncState>& data);

    const std::string&
    getDigest() const { return digest_; }

    const google::protobuf::RepeatedPtrField<Sync::SyncState>&
    getData() const { return *data_; }

  private:
    std::string digest_;
    ptr_lib::shared_ptr<google::protobuf::RepeatedPtrField<Sync::SyncState> > data_;
  };

  /**
   * A PendingInterest holds an interest which onInterest received but could
   * not satisfy. When we add a new data packet to the contentCache_, we will
   * also check if it satisfies a pending interest.
   */
  class PendingInterest {
  public:
    /**
     * Create a new PendingInterest and set the timeoutTime_ based on the current time and the interest lifetime.
     * @param interest A shared_ptr for the interest.
     * @param transport The transport from the onInterest callback. If the
     * interest is satisfied later by a new data packet, we will send the data
     * packet to the transport.
     */
    PendingInterest
      (const ptr_lib::shared_ptr<const Interest>& interest,
       Transport& transport);

    /**
     * Return the interest given to the constructor.
     */
    const ptr_lib::shared_ptr<const Interest>&
    getInterest() { return interest_; }

    /**
     * Return the transport given to the constructor.
     */
    Transport&
    getTransport() { return transport_; }

    /**
     * Check if this interest is timed out.
     * @param nowMilliseconds The current time in milliseconds from ndn_getNowMilliseconds.
     * @return true if this interest timed out, otherwise false.
     */
    bool
    isTimedOut(MillisecondsSince1970 nowMilliseconds)
    {
      return timeoutTimeMilliseconds_ >= 0.0 && nowMilliseconds >= timeoutTimeMilliseconds_;
    }

  private:
    ptr_lib::shared_ptr<const Interest> interest_;
    Transport& transport_;
    MillisecondsSince1970 timeoutTimeMilliseconds_; /**< The time when the
      * interest times out in milliseconds according to ndn_getNowMilliseconds,
      * or -1 for no timeout. */
  };

  /**
   * Make a data packet with the syncMessage and with name 
   * applicationBroadcastPrefix_ + digest. Sign and send.
   * @param digest The root digest as a hex string for the data packet name.
   * @param syncMessage The SyncStateMsg which updates the digest tree state
   * with the given digest.
   */
  void
  broadcastSyncState
    (const std::string& digest, const Sync::SyncStateMsg& syncMessage);

  /**
   * Update the digest tree with the messages in content. If the digest tree
   * root is not in the digest log, also add a log entry with the content.
   * @param content The sync state messages.
   * @return True if added a digest log entry (because the updated digest
   * tree root was not in the log), false if didn't add a log entry.
   */
  bool
  update(const google::protobuf::RepeatedPtrField<Sync::SyncState >& content);

  // Search the digest log by digest.
  int
  logfind(const std::string& digest) const;

  /**
   * Process the sync interest from the applicationBroadcastPrefix. If we can't
   * satisfy the interest, add it to the pendingInterestTable_ so that a
   * future call to contentCacheAdd may satisfy it.
   */
  void
  onInterest
    (const ptr_lib::shared_ptr<const Name>& prefix,
     const ptr_lib::shared_ptr<const Interest>& inst, Transport& transport,
     uint64_t registerPrefixId);

  // Process Sync Data.
  void
  onData
    (const ptr_lib::shared_ptr<const Interest>& inst,
     const ptr_lib::shared_ptr<Data>& co);

  // Initial sync interest timeout, which means there are no other publishers yet.
  void
  initialTimeOut(const ptr_lib::shared_ptr<const Interest>& interest);

  void
  processRecoveryInst
    (const Interest& inst, const std::string& syncdigest, Transport& transport);

  /**
   * Common interest processing, using digest log to find the difference after
   * syncdigest_t. Return true if sent a data packet to satisfy the interest,
   * otherwise false.
   */
  bool
  processSyncInst(int index, const std::string& syncdigest_t, Transport& transport);

  // Send Recovery Interest.
  void
  sendRecovery(const std::string& syncdigest_t);

  /**
   * This is called by onInterest after a timeout to check if a recovery is needed.
   * This method has an "interest" argument because we use it as the onTimeout
   * for Face.expressInterest.
   */
  void
  judgeRecovery
    (const ptr_lib::shared_ptr<const Interest> &interest,
     const std::string& syncdigest_t, Transport* transport);

  // Sync interest time out, if the interest is the static one send again.
  void
  syncTimeout(const ptr_lib::shared_ptr<const Interest>& interest);

  // Process initial data which usually includes all other publisher's info, and send back the new comer's own info.
  void
  initialOndata(const google::protobuf::RepeatedPtrField<Sync::SyncState >& content);

  /**
   * Add the data packet to the contentCache_. Remove timed-out entries
   * from pendingInterestTable_. If the data packet satisfies any pending
   * interest, then send the data packet to the pending interest's transport
   * and remove from the pendingInterestTable_.
   * @param data
   */
  void
  contentCacheAdd(const Data& data);

  /**
   * This is a do-nothing onData for using expressInterest for timeouts.
   * This should never be called.
   */
  static void
  dummyOnData
    (const ptr_lib::shared_ptr<const Interest>& interest,
     const ptr_lib::shared_ptr<Data>& data);

  Face& face_;
  KeyChain& keyChain_;
  Name certificateName_;
  Milliseconds sync_lifetime_;
  OnReceivedSyncState onReceivedSyncState_;
  OnInitialized onInitialized_;
  std::vector<ptr_lib::shared_ptr<DigestLogEntry> > digest_log_;
  ptr_lib::shared_ptr<DigestTree> digest_tree_;
  std::string applicationDataPrefixUri_;
  const Name applicationBroadcastPrefix_;
  int session_;
  int usrseq_;
  MemoryContentCache contentCache_;
  std::vector<ptr_lib::shared_ptr<PendingInterest> > pendingInterestTable_;
};

}

#endif