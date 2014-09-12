#include "blocks.h"

#include <set>
#include <string.h>
#include <algorithm>

#include "crypto/sha2.h"
#include "utils.h"

static std::set<std::vector<unsigned char> > hashesSeen;

static inline void doubleDoubleHash(std::vector<unsigned char>& first, std::vector<unsigned char>& second) {
	assert(first.size() == 32);
	CSHA256 hash; // Probably not BE-safe
	hash.Write(&first[0], first.size()).Write(&second[0], second.size()).Finalize(&first[0]);
	hash.Reset().Write(&first[0], 32).Finalize(&first[0]);
}

bool is_block_sane(const std::vector<unsigned char>& hash, std::vector<unsigned char>::const_iterator readit, std::vector<unsigned char>::const_iterator end) {
	try {
		if (!hashesSeen.insert(hash).second || hash[31] != 0 || hash[30] != 0 || hash[29] != 0 || hash[28] != 0 || hash[27] != 0)
			return false;

		move_forward(readit, 4 + 32, end);
		auto merkle_hash_it = readit;
		move_forward(readit, 80 - (4 + 32), end);
		uint64_t txcount = read_varint(readit, end);
		if (txcount < 1 || txcount > 100000)
			return false;

		std::vector<std::vector<unsigned char> > hashlist;
		hashlist.reserve(txcount);

		for (uint32_t i = 0; i < txcount; i++) {
			std::vector<unsigned char>::const_iterator txstart = readit;

			move_forward(readit, 4, end);

			uint32_t txins = read_varint(readit, end);
			for (uint32_t j = 0; j < txins; j++) {
				move_forward(readit, 36, end);
				uint32_t scriptlen = read_varint(readit, end);
				move_forward(readit, scriptlen + 4, end);
			}

			uint32_t txouts = read_varint(readit, end);
			for (uint32_t j = 0; j < txouts; j++) {
				move_forward(readit, 8, end);
				uint32_t scriptlen = read_varint(readit, end);
				move_forward(readit, scriptlen, end);
			}

			move_forward(readit, 4, end);

			hashlist.emplace_back(32);
			CSHA256 hash; // Probably not BE-safe
			hash.Write(&(*txstart), readit - txstart).Finalize(&hashlist.back()[0]);
			hash.Reset().Write(&hashlist.back()[0], 32).Finalize(&hashlist.back()[0]);
		}

		// "Two" blocks may have the same hash if a transaction is duplicated on the end (sometimes)
		// Otherwise, you'd have to create a transaction with the same representation as two
		// transactions concatenated, which cant happen as transactions encode their length within
		// their bodies
		if (hashlist.back() == hashlist[hashlist.size() - 2])
			return false;

		uint32_t stepCount = 1, lastMax = hashlist.size() - 1;
		for (uint32_t rowSize = hashlist.size(); rowSize > 1; rowSize = (rowSize + 1) / 2) {
			for (uint32_t i = 0; i < rowSize; i += 2) {
				assert(i*stepCount < hashlist.size() && lastMax < hashlist.size());
				doubleDoubleHash(hashlist[i*stepCount], hashlist[std::min((i + 1)*stepCount, lastMax)]);
			}
			lastMax = ((rowSize - 1) / 2)*2 * stepCount;
			stepCount *= 2;
		}

		if (memcmp(&(*merkle_hash_it), &hashlist[0][0], 32))
			return false;

		return true;
	} catch (read_exception) {
		return false;
	}
}

void recv_headers_msg_from_trusted(const std::vector<unsigned char> headers) {
	try {
		auto it = headers.begin();
		uint64_t count = read_varint(it, headers.end());

		for (uint64_t i = 0; i < count; i++) {
			move_forward(it, 81, headers.end());

			if (*(it - 1) != 0)
				return;

			std::vector<unsigned char> fullhash(it - 81, it - 1);
			hashesSeen.insert(fullhash);

			for (unsigned int i = 0; i < sizeof(fullhash); i++)
				printf("%02x", fullhash[sizeof(fullhash) - i - 1]);
			printf(" added to via trusted header\n");
		}
	} catch (read_exception) { }
}