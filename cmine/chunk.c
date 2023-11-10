#include <stdlib.h>
#include "safety.h"

struct Uvs {
	float x_min;
	float y_min;
	float x_max;
	float y_max;
};
typedef struct Uvs Uvs;

struct Block {
	Uvs px, py, pz,
		nx, ny, nz;
};
typedef struct Block Block;

//struct BlockDataBase {
//	Block* blocks;
//	size_t cap;
//	size_t len;
//};
//typedef struct BlockDataBase BlockDataBase;
//
//BlockDataBase block_db_new(void) {
//	return (BlockDataBase) {
//		.blocks = NULL,
//		.cap = 0,
//		.len = 0
//	};
//}
//
//size_t block_db_store(BlockDataBase* db, Block block) {
//	if (db->len == db->cap) {
//		db->cap = db->cap ? db->cap * 2 : 1;
//		db->blocks = srealloc(db->blocks, db->cap);
//	}
//	db->blocks[db->len] = block;
//	return db->len++;
//}
//
//Block block_db_load(size_t index) {
//
//}
