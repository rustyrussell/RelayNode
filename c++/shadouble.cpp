// Distributed under the MIT/X11 software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <assert.h>
#include "shadouble.h"

CSHA256Double& CSHA256Double::Write(const unsigned char *data, size_t len) {
	assert(!finalized);
	hstate.Write(data, len);
    return *this;
}

void CSHA256Double::Finalize(unsigned char hash[CSHA256::OUTPUT_SIZE]) {
	hstate.Finalize(hash);
	hstate.Reset();
	hstate.Write(hash, CSHA256::OUTPUT_SIZE);
	hstate.Finalize(hash);
	finalized = true;
}
