// Distributed under the MIT/X11 software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

// Logging to allow us to determine what was missing/known when we get a block.
#ifndef RELAYNODE_LOGTX_H
#define RELAYNODE_LOGTX_H
#include <vector>
#include <stdint.h>

#include "flaggedarrayset.h"

class LogTx {
private:
	bool active;

public:
	LogTx(bool active_in = false);

	// The following txs are for this block header:
	void processing_block(const unsigned char *block) const;
	// We didn't know this tx, they had to give it to us.
	void unknown_tx(const unsigned char *tx, uint32_t size) const;
	// We knew about these txs, but they weren't in the block.
	void unused_txs(FlaggedArraySet &txs) const;
};
#endif // RELAYNODE_LOGTX_H
