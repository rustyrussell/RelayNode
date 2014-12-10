// Distributed under the MIT/X11 software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <iostream>
#include <assert.h>

#include "logtx.h"
#include "shadouble.h"

static char hexchar[] = "0123456789abcdef";

/* bitcoind spits out hashes in little-endian, but noone else does. */
static void to_be_hex(char hex[CSHA256::OUTPUT_SIZE * 2 + 1],
					  const unsigned char hash[CSHA256::OUTPUT_SIZE])
{
	char *dest = hex;

	for (int off = CSHA256::OUTPUT_SIZE - 1; off >= 0; off--) {
		*(dest++) = hexchar[hash[off] >> 4];
		*(dest++) = hexchar[hash[off] & 0xF];
	}
	*dest = '\0';
}

LogTx::LogTx(bool active_in) : active(active_in)
{
}

void LogTx::processing_block(const unsigned char *block) const
{
	if (!active)
		return;

	CSHA256Double hash;
	unsigned char blockid[CSHA256::OUTPUT_SIZE];
	char hex[sizeof(blockid) * 2 + 1];

	hash.Write(block, 80).Finalize(blockid);
	to_be_hex(hex, blockid);
	std::cout << "block-txids: Block " << hex << std::endl;
}

void LogTx::unknown_tx(const unsigned char *tx, uint32_t size) const
{
	if (!active)
		return;

	CSHA256Double hash;
	unsigned char txid[CSHA256::OUTPUT_SIZE];
	char hex[sizeof(txid) * 2 + 1];

	hash.Write(tx, size).Finalize(txid);
	to_be_hex(hex, txid);
	std::cout << "block-txids: unknown tx " << hex << std::endl;
}

static void unused_tx(const std::vector<unsigned char> &tx)
{
	CSHA256Double hash;
	unsigned char txid[CSHA256::OUTPUT_SIZE];
	char hex[sizeof(txid) * 2 + 1];

	hash.Write(tx.data(), tx.size()).Finalize(txid);
	to_be_hex(hex, txid);
	std::cout << "block-txids: unused tx " << hex << std::endl;
}

void LogTx::unused_txs(FlaggedArraySet &txs) const
{
	if (!active)
		return;

	txs.for_all_txn([&] (std::shared_ptr<std::vector<unsigned char> > tx) {
			unused_tx(*tx);
		});
}
