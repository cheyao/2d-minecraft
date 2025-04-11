#include "registers.hpp"

#include "components.hpp"
#include "components/crafting.hpp"
#include "components/furnace.hpp"
#include "items.hpp"
#include "screens/screen.hpp"
#include "third_party/Eigen/Core"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>

namespace registers {

using namespace Components;

enum class MiningSystem { PICKAXE, AXE, SHOVEL, HOE, SHEARS, SWORD };

std::unordered_map<Components::Item, std::string> TEXTURES = {
	{Item::CAMPFIRE, "blocks/campfire.png"},
	{Item::CRAFTING_TABLE, "blocks/crafting-table.png"},
	{Item::COBBLESTONE, "blocks/cobblestone.png"},
	{Item::COAL_ORE, "blocks/coal-ore.png"},
	{Item::COAL_BLOCK, "blocks/coal-block.png"},
	{Item::DIRT, "blocks/dirt.png"},
	{Item::DIAMOND_BLOCK, "blocks/diamond-block.png"},
	{Item::DIAMOND_ORE, "blocks/diamond-ore.png"},
	{Item::FURNACE, "blocks/furnace.png"},
	{Item::GRASS_BLOCK, "blocks/grass-block.png"},
	{Item::IRON_ORE, "blocks/iron-ore.png"},
	{Item::OAK_LOG, "blocks/oak-log.png"},
	{Item::OAK_LEAVES, "blocks/oak-leaves.png"},
	{Item::OAK_PLANKS, "blocks/oak-planks.png"},
	{Item::STONE, "blocks/stone.png"},
	{Item::TORCH, "blocks/torch.png"},

	{Item::APPLE, "items/apple.png"},
	{Item::COAL, "items/coal.png"},
	{Item::CHARCOAL, "items/charcoal.png"},
	{Item::DIAMOND, "items/diamond.png"},
	{Item::DIAMOND_SHOVEL, "items/diamond-shovel.png"},
	{Item::DIAMOND_AXE, "items/diamond-axe.png"},
	{Item::DIAMOND_HOE, "items/diamond-hoe.png"},
	{Item::DIAMOND_PICKAXE, "items/diamond-pickaxe.png"},
	{Item::DIAMOND_SWORD, "items/diamond-sword.png"},
	{Item::IRON_INGOT, "items/iron-ingot.png"},
	{Item::IRON_SHOVEL, "items/iron-shovel.png"},
	{Item::IRON_AXE, "items/iron-axe.png"},
	{Item::IRON_HOE, "items/iron-hoe.png"},
	{Item::IRON_PICKAXE, "items/iron-pickaxe.png"},
	{Item::IRON_SWORD, "items/iron-sword.png"},
	{Item::STONE_SHOVEL, "items/stone-shovel.png"},
	{Item::STONE_AXE, "items/stone-axe.png"},
	{Item::STONE_HOE, "items/stone-hoe.png"},
	{Item::STONE_PICKAXE, "items/stone-pickaxe.png"},
	{Item::STONE_SWORD, "items/stone-sword.png"},
	{Item::STICK, "items/stick.png"},
	{Item::WOODEN_SHOVEL, "items/wooden-shovel.png"},
	{Item::WOODEN_AXE, "items/wooden-axe.png"},
	{Item::WOODEN_HOE, "items/wooden-hoe.png"},
	{Item::WOODEN_PICKAXE, "items/wooden-pickaxe.png"},
	{Item::WOODEN_SWORD, "items/wooden-sword.png"},
};

const std::unordered_map<Components::Item, std::pair<int, std::uint64_t>> BREAK_TIMES = {
	{Item::AIR, {0, 0}},	      {Item::GRASS_BLOCK, {0, 30}},    {Item::DIRT, {0, 30}},
	{Item::STONE, {1, 80}},	      {Item::OAK_LOG, {0, 50}},	       {Item::OAK_LEAVES, {0, 20}},
	{Item::OAK_PLANKS, {0, 60}},  {Item::CRAFTING_TABLE, {0, 50}}, {Item::COBBLESTONE, {1, 80}},
	{Item::FURNACE, {1, 80}},     {Item::CAMPFIRE, {0, 50}},       {Item::TORCH, {0, 2}},
	{Item::IRON_ORE, {3, 120}},   {Item::COAL_ORE, {1, 120}},      {Item::COAL_BLOCK, {1, 80}},
	{Item::IRON_BLOCK, {3, 180}}, {Item::DIAMOND_ORE, {5, 280}},   {Item::DIAMOND_BLOCK, {5, 200}}};

// Will add one in the real calculation
// WOOD 1 STONE 3 IRON 5 diamond 7 neth 8 gold 11
extern const std::unordered_map<Components::Item, int> MINING_LEVEL = {
	{Item::WOODEN_PICKAXE, 1},  {Item::WOODEN_AXE, 1},   {Item::WOODEN_SHOVEL, 1},
	{Item::WOODEN_HOE, 1},	    {Item::WOODEN_SWORD, 1},

	{Item::STONE_PICKAXE, 3},   {Item::STONE_AXE, 3},    {Item::STONE_SHOVEL, 3},
	{Item::STONE_HOE, 3},	    {Item::STONE_SWORD, 3},

	{Item::IRON_PICKAXE, 5},    {Item::IRON_AXE, 5},     {Item::IRON_SHOVEL, 5},
	{Item::IRON_HOE, 5},	    {Item::IRON_SWORD, 5},

	{Item::DIAMOND_SHOVEL, 7},  {Item::DIAMOND_HOE, 7},  {Item::DIAMOND_SWORD, 7},
	{Item::DIAMOND_PICKAXE, 7}, {Item::DIAMOND_AXE, 7},
};

extern const std::unordered_map<Components::Item, registers::MiningSystem> MINING_SYSTEM = {
	{Item::GRASS_BLOCK, MiningSystem::SHOVEL},
	{Item::DIRT, MiningSystem::SHOVEL},
	{Item::STONE, MiningSystem::PICKAXE},
	{Item::OAK_LOG, MiningSystem::AXE},
	{Item::OAK_LEAVES, MiningSystem::HOE},
	{Item::OAK_PLANKS, MiningSystem::AXE},
	{Item::CRAFTING_TABLE, MiningSystem::AXE},
	{Item::WOODEN_SHOVEL, MiningSystem::SHOVEL},
	{Item::WOODEN_AXE, MiningSystem::AXE},
	{Item::WOODEN_HOE, MiningSystem::HOE},
	{Item::WOODEN_PICKAXE, MiningSystem::PICKAXE},
	{Item::STONE_SHOVEL, MiningSystem::SHOVEL},
	{Item::STONE_AXE, MiningSystem::AXE},
	{Item::STONE_HOE, MiningSystem::HOE},
	{Item::STONE_PICKAXE, MiningSystem::PICKAXE},
	{Item::COBBLESTONE, MiningSystem::PICKAXE},
	{Item::FURNACE, MiningSystem::PICKAXE},
	{Item::CAMPFIRE, MiningSystem::AXE},
	{Item::IRON_ORE, MiningSystem::PICKAXE},
	{Item::COAL_ORE, MiningSystem::PICKAXE},
	{Item::COAL_BLOCK, MiningSystem::PICKAXE},
	{Item::IRON_BLOCK, MiningSystem::PICKAXE},
	{Item::IRON_SHOVEL, MiningSystem::SHOVEL},
	{Item::IRON_AXE, MiningSystem::AXE},
	{Item::IRON_HOE, MiningSystem::HOE},
	{Item::IRON_PICKAXE, MiningSystem::PICKAXE},
	{Item::IRON_SWORD, MiningSystem::SWORD},
	{Item::STONE_SWORD, MiningSystem::SWORD},
	{Item::WOODEN_SWORD, MiningSystem::SWORD},
	{Item::DIAMOND_SWORD, MiningSystem::SWORD},
	{Item::DIAMOND_AXE, MiningSystem::AXE},
	{Item::DIAMOND_HOE, MiningSystem::HOE},
	{Item::DIAMOND_SHOVEL, MiningSystem::SHOVEL},
	{Item::DIAMOND_ORE, MiningSystem::PICKAXE},
	{Item::DIAMOND_PICKAXE, MiningSystem::PICKAXE},
	{Item::DIAMOND_BLOCK, MiningSystem::PICKAXE},
};

const std::vector<std::pair<float, std::vector<std::pair<Components::Item, Eigen::Vector2i>>>> SURFACE_STRUCTURES = {
	{0.08,
	 {
		 // Main Bark
		 {Item::OAK_LOG, {0, 0}},
		 {Item::OAK_LOG, {0, 1}},
		 {Item::OAK_LOG, {0, 2}},
		 {Item::OAK_LOG, {0, 3}},
		 // Leafs
		 {Item::OAK_LEAVES, {0, 5}},
		 {Item::OAK_LEAVES, {1, 5}},
		 {Item::OAK_LEAVES, {-1, 5}},

		 {Item::OAK_LEAVES, {-2, 4}},
		 {Item::OAK_LEAVES, {-1, 4}},
		 {Item::OAK_LEAVES, {0, 4}},
		 {Item::OAK_LEAVES, {1, 4}},
		 {Item::OAK_LEAVES, {2, 4}},

		 {Item::OAK_LEAVES, {-2, 3}},
		 {Item::OAK_LEAVES, {-1, 3}},
		 {Item::OAK_LEAVES, {1, 3}},
		 {Item::OAK_LEAVES, {2, 3}},
	 }},
};

const std::unordered_map<Components::Item, std::vector<std::pair<float, Components::Item>>> LOOT_TABLES = {
	{Item::OAK_LEAVES,
	 {
		 {0.2f, Item::STICK},
		 {0.1f, Item::APPLE},
	 }},
	{Item::GRASS_BLOCK,
	 {
		 {1.1f, Item::DIRT},
	 }},
	{Item::STONE,
	 {
		 {1.1f, Item::COBBLESTONE},
	 }},
	{Item::CAMPFIRE,
	 {
		 {1.1f, Item::CHARCOAL},
		 {1.1f, Item::CHARCOAL},
		 {0.5f, Item::CHARCOAL},
	 }},
	{Item::COAL_ORE,
	 {
		 {1.1f, Item::COAL},
		 {0.5f, Item::COAL},
		 {0.5f, Item::COAL},
	 }},
	{Item::DIAMOND_ORE,
	 {
		 {1.1f, Item::DIAMOND},
		 {0.25f, Item::DIAMOND},
		 {0.25f, Item::DIAMOND},
	 }},
};

// NOLINTBEGIN
const std::vector<std::tuple<std::pair<std::uint64_t, std::uint64_t>, std::vector<Components::Item>,
			     std::pair<std::uint64_t, Components::Item>>>
	CRAFTING_RECIPIES = {
		{/* KEEP THIS! 0th recipie represents no recipie*/},
		{{0, 0}, {Item::OAK_LOG}, {4, Item::OAK_PLANKS}},
		{{1, 2}, {Item::OAK_PLANKS, Item::OAK_PLANKS}, {4, Item::STICK}},
		{{2, 2},
		 {Item::OAK_PLANKS, Item::OAK_PLANKS, Item::OAK_PLANKS, Item::OAK_PLANKS},
		 {1, Item::CRAFTING_TABLE}},
		{{3, 3},
		 {
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
		 },
		 {1, Item::WOODEN_PICKAXE}},
		{{2, 3},
		 {
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::WOODEN_AXE}},
		{{1, 3},
		 {
			 Item::OAK_PLANKS,
			 Item::STICK,
			 Item::STICK,
		 },
		 {1, Item::WOODEN_SHOVEL}},
		{{2, 3},
		 {
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::WOODEN_HOE}},
		{{1, 3},
		 {
			 Item::OAK_PLANKS,
			 Item::OAK_PLANKS,
			 Item::STICK,
		 },
		 {1, Item::WOODEN_SWORD}},

		{{3, 3},
		 {
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
		 },
		 {1, Item::STONE_PICKAXE}},
		{{2, 3},
		 {
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::STONE_AXE}},
		{{1, 3},
		 {
			 Item::COBBLESTONE,
			 Item::STICK,
			 Item::STICK,
		 },
		 {1, Item::STONE_SHOVEL}},
		{{2, 3},
		 {
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::STONE_HOE}},
		{{1, 3},
		 {
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::STICK,
		 },
		 {1, Item::STONE_SWORD}},

		{{3, 3},
		 {
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
		 },
		 {1, Item::IRON_PICKAXE}},
		{{2, 3},
		 {
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::IRON_AXE}},
		{{1, 3},
		 {
			 Item::IRON_INGOT,
			 Item::STICK,
			 Item::STICK,
		 },
		 {1, Item::IRON_SHOVEL}},
		{{2, 3},
		 {
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::IRON_HOE}},
		{{1, 3},
		 {
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::STICK,
		 },
		 {1, Item::IRON_SWORD}},

		{{3, 3},
		 {
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
		 },
		 {1, Item::DIAMOND_PICKAXE}},
		{{2, 3},
		 {
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::DIAMOND_AXE}},
		{{1, 3},
		 {
			 Item::DIAMOND,
			 Item::STICK,
			 Item::STICK,
		 },
		 {1, Item::DIAMOND_SHOVEL}},
		{{2, 3},
		 {
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
		 },
		 {1, Item::DIAMOND_HOE}},
		{{1, 3},
		 {
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::STICK,
		 },
		 {1, Item::DIAMOND_SWORD}},

		{{3, 3},
		 {
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::AIR,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
			 Item::COBBLESTONE,
		 },
		 {1, Item::FURNACE}},
		{{3, 3},
		 {
			 Item::AIR,
			 Item::STICK,
			 Item::AIR,
			 Item::STICK,
			 Item::CHARCOAL,
			 Item::STICK,
			 Item::OAK_LOG,
			 Item::OAK_LOG,
			 Item::OAK_LOG,
		 },
		 {1, Item::CAMPFIRE}},
		{{1, 2},
		 {
			 Item::CHARCOAL,
			 Item::STICK,
		 },
		 {4, Item::TORCH}},
		{{1, 2},
		 {
			 Item::COAL,
			 Item::STICK,
		 },
		 {4, Item::TORCH}},

		// Ingots to blocks
		{{3, 3},
		 {
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
			 Item::COAL,
		 },
		 {1, Item::COAL_BLOCK}},
		{{3, 3},
		 {
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
			 Item::IRON_INGOT,
		 },
		 {1, Item::IRON_BLOCK}},
		{{3, 3},
		 {
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
			 Item::DIAMOND,
		 },
		 {1, Item::DIAMOND_BLOCK}},

		// Blocks to ingots
		{{0, 0},
		 {
			 Item::IRON_BLOCK,
		 },
		 {9, Item::IRON_INGOT}},
		{{0, 0},
		 {
			 Item::COAL_BLOCK,
		 },
		 {9, Item::COAL}},
		{{0, 0},
		 {
			 Item::DIAMOND_BLOCK,
		 },
		 {9, Item::DIAMOND}},
};
// NOLINTEND

// Time in seconds
const std::unordered_map<Components::Item, double> BURNING_TIME = {
	{Item::OAK_LOG, 16},	   {Item::OAK_PLANKS, 4},      {Item::STICK, 2},	 {Item::WOODEN_AXE, 8},
	{Item::WOODEN_PICKAXE, 8}, {Item::WOODEN_HOE, 8},      {Item::WOODEN_SHOVEL, 8}, {Item::CHARCOAL, 32},
	{Item::COAL, 32},	   {Item::COAL_BLOCK, 9 * 32},
};

// Time in seconds
const std::unordered_map<Components::Item, std::pair<double, Components::Item>> SMELTING_RECIPIE = {
	{Item::COBBLESTONE, {8, Item::STONE}},	 {Item::OAK_LOG, {8, Item::CHARCOAL}},
	{Item::IRON_ORE, {8, Item::IRON_INGOT}}, {Item::COAL_ORE, {8, Item::COAL}},
	{Item::DIAMOND_ORE, {8, Item::DIAMOND}},
};

template <typename T, typename... Args> T* staticHelper(Args&&... args) {
	static T t(std::forward<Args>(args)...);
	return &t;
};

const std::unordered_map<Components::Item, Screen* (*)(void)> CLICKABLES = {
	{Item::CRAFTING_TABLE,
	 [] { return static_cast<Screen*>(staticHelper<CraftingInventory>(crafting_table_t())); }},
	{Item::FURNACE, [] { return static_cast<Screen*>(staticHelper<FurnaceInventory>(furnace_t())); }},
};

const std::vector<std::string> BACKGROUND_SOUNDS = {
	"sweden.wav",
	"subwoofer_lullaby.wav",
};

constexpr const static inline auto BLOCK_SIZE = 16 * 7;
// A collision box of 0x0 is no collision box
const std::unordered_map<Components::Item, std::pair<Eigen::Vector2f, Eigen::Vector2f>> COLLISION_BOXES = {
	{Item::CAMPFIRE, {Eigen::Vector2f(0, 0), Eigen::Vector2f(BLOCK_SIZE, BLOCK_SIZE / 2)}},
	{Item::TORCH, {Eigen::Vector2f(0, 0), Eigen::Vector2f(0, 0)}},
};

const std::vector<std::tuple<float, std::uint64_t, Components::Item, std::uint64_t>> VEINS = {
	{0.02, 32, Item::COAL_ORE, 8},
	{0.01, 14, Item::IRON_ORE, 3},
	{0.005, 10, Item::DIAMOND_ORE, 2},
};

} // namespace registers
