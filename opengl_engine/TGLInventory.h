#ifndef _TGL_INVENTORY
#define _TGL_INVENTORY

#include <vector>
#include <stdio.h> 

#include "BlockGenerator.h"
#include "TGLChunkSpawn.h"

enum TGLItemId
{
	none,
	iid_dirt_with_grass_block,
	iid_dirt_block,
	iid_stone_block,
	iid_tree_block,
	iid_leaves_block,
	iid_water_block,
	iid_sand_block,
	pickaxe
};



e_block_type item_id_to_block_type(TGLItemId item_id);
TGLItemId block_type_to_item_id(e_block_type in_block_type);

class TGLInventoryCoords
{
public:
	TGLInventoryCoords(unsigned int in_x, unsigned int in_y) :
		x(in_x), y(in_y)
	{   }
	unsigned int x;
	unsigned int y;
};

class TGLInventoryItem
{
public:
	unsigned int quantity;
	TGLItemId type;

	TGLInventoryItem(TGLItemId in_type = none, unsigned int in_quantity = 0);

	bool operator== (const TGLInventoryItem& item_b);
	bool operator!= (const TGLInventoryItem& item_b);
	
	hit_properties get_hit_properties();
};


// TGLInventory is intended to provide an abstraction of an inventory grid
// in which items in different quantities can be moved around, added, and
// deleted. The underlying data structure is a vector of TGLInventoryItem
// pointers whose horizontal rows are x_size long and vertical columns
// are y_size long. It also provides the quick_use structure, intended
// to store easy access to 5 items in the TGLInventory's main vector.
class TGLInventory//: public TGLComponent
{
	

	// object data
	unsigned int x_size;
	unsigned int y_size;
	std::vector <TGLInventoryItem*> items;

public:
	// class constants
	static const unsigned int default_quick_use_size = 5;
	static TGLInventoryItem no_item;

	TGLInventory(int in_x_size, int in_y_size);

	// Item retrieval
	std::vector <TGLInventoryItem*>& get_items();
	TGLInventoryItem * get_item(unsigned int in_x, unsigned int in_y);
	TGLInventoryItem * get_quick_use(unsigned int index);

	// Item Manipulation
	void set_item(unsigned int in_x, unsigned int in_y, TGLItemId in_type, unsigned int in_quantity);
	bool move_item(unsigned int src_x, unsigned int src_y, unsigned int dest_x, unsigned int dest_y);
	bool delete_item(unsigned int in_x, unsigned int in_y);
	bool change_quantity(TGLItemId in_type, int in_quantity);
	void print_inventory();

	// Management
	void repair_garbage();
};

#endif