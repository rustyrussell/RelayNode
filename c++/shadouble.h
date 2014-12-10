// Distributed under the MIT/X11 software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef RELAYNODE_DOUBLESHA_H
#define RELAYNODE_DOUBLESHA_H

#include "crypto/sha2.h"

// Helper to perform SHA256(SHA256())
class CSHA256Double {
private:
	CSHA256 hstate;
	bool finalized = false;

public:
    CSHA256Double& Write(const unsigned char *data, size_t len);
	// Don't re-use after Finalize!
    void Finalize(unsigned char hash[CSHA256::OUTPUT_SIZE]);
};

#endif // RELAYNODE_DOUBLESHA_H
