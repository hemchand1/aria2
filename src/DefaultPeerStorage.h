/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#ifndef D_DEFAULT_PEER_STORAGE_H
#define D_DEFAULT_PEER_STORAGE_H

#include "PeerStorage.h"

#include <string>
#include <map>
#include <set>

#include "TimerA2.h"
#include "a2functional.h"

namespace aria2 {

class BtRuntime;
class BtSeederStateChoke;
class BtLeecherStateChoke;
class PieceStorage;

typedef std::set<SharedHandle<Peer>, RefLess<Peer> > PeerSet;

class DefaultPeerStorage : public PeerStorage {
private:
  SharedHandle<BtRuntime> btRuntime_;
  SharedHandle<PieceStorage> pieceStorage_;
  size_t maxPeerListSize_;

  // This contains ip address and port pair and is used to ensure that
  // no duplicate peers are stored.
  std::set<std::pair<std::string, uint16_t> > uniqPeers_;
  // Unused (not connected) peers, sorted by last added.
  std::deque<SharedHandle<Peer> > unusedPeers_;
  // The set of used peers. Some of them are not connected yet. To
  // know it is connected or not, call Peer::isActive().
  PeerSet usedPeers_;

  std::deque<SharedHandle<Peer> > droppedPeers_;

  BtSeederStateChoke* seederStateChoke_;
  BtLeecherStateChoke* leecherStateChoke_;

  Timer lastTransferStatMapUpdated_;

  std::map<std::string, time_t> badPeers_;
  Timer lastBadPeerCleaned_;

  bool isPeerAlreadyAdded(const SharedHandle<Peer>& peer);
  void addUniqPeer(const SharedHandle<Peer>& peer);

  void addDroppedPeer(const SharedHandle<Peer>& peer);
public:
  DefaultPeerStorage();

  virtual ~DefaultPeerStorage();

  // TODO We need addAndCheckoutPeer for incoming peers
  virtual bool addPeer(const SharedHandle<Peer>& peer);

  virtual size_t countAllPeer() const;

  SharedHandle<Peer> getPeer(const std::string& ipaddr, uint16_t port) const;

  virtual void addPeer(const std::vector<SharedHandle<Peer> >& peers);

  const std::deque<SharedHandle<Peer> >& getUnusedPeers();

  const PeerSet& getUsedPeers();

  virtual const std::deque<SharedHandle<Peer> >& getDroppedPeers();

  virtual bool isPeerAvailable();

  virtual void getActivePeers(std::vector<SharedHandle<Peer> >& peers);

  virtual bool isBadPeer(const std::string& ipaddr);

  virtual void addBadPeer(const std::string& ipaddr);

  virtual SharedHandle<Peer> checkoutPeer(cuid_t cuid);

  virtual void returnPeer(const SharedHandle<Peer>& peer);

  virtual bool chokeRoundIntervalElapsed();

  virtual void executeChoke();

  void deleteUnusedPeer(size_t delSize);

  void onErasingPeer(const SharedHandle<Peer>& peer);

  void onReturningPeer(const SharedHandle<Peer>& peer);

  void setPieceStorage(const SharedHandle<PieceStorage>& pieceStorage);

  void setBtRuntime(const SharedHandle<BtRuntime>& btRuntime);

  void setMaxPeerListSize(size_t maxPeerListSize)
  {
    maxPeerListSize_ = maxPeerListSize;
  }
};

} // namespace aria2

#endif // D_DEFAULT_PEER_STORAGE_H
