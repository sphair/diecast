CXXFLAGS = -std=c++0x -g -I . -I Include -I Sources/ServerGame -I Sources `pkg-config --cflags clanDisplay-3.0 clanCore-3.0 clanApp-3.0 clanSqlite-3.0 clanNetwork-3.0`
libs = `pkg-config --libs clanDisplay-3.0 clanCore-3.0 clanApp-3.0 clanSqlite-3.0 clanNetwork-3.0` -lpthread -ldl

Precomp = Sources/ServerGame/precomp.h

Server = \
	Sources/DatabaseSqlite/Sqlite/sqlite_database.o \
	Sources/DatabaseSqlite/Sqlite/sqlite_gameobjects.o \
	Sources/DatabaseSqlite/Sqlite/sqlite_users.o \
	Sources/ServerEngine/server.o \
	Sources/ServerEngine/server_user.o \
	Sources/ServerEngine/GameWorld/gameobject_manager.o \
	Sources/ServerEngine/GameWorld/player.o \
	Sources/ServerEngine/GameWorld/player_manager.o \
	Sources/ServerEngine/GameWorld/server_gameobject.o \
	Sources/ServerEngine/GameWorld/Components/component_serializer.o \
	Sources/ServerEngine/GameWorld/Components/property_serializer.o \
	Sources/ServerEngine/GameWorld/Components/replicated_container.o \
	Sources/ServerEngine/GameWorld/Components/replicated_properties.o \
	Sources/ServerEngine/GameWorld/Components/template_manager.o \
	Sources/ServerEngine/GameWorld/Zone/zone.o \
	Sources/ServerEngine/GameWorld/Zone/zone_manager.o \
	Sources/ServerEngine/GameWorld/Zone/zone_visibility_all.o \
	Sources/ServerEngine/Utilities/scope_timer.o \
	Sources/ServerEngine/Utilities/tick_timer.o \
	Sources/ServerGame/gameserver.o \
	Sources/ServerGame/main.o \
	Sources/ServerGame/precomp.o \
	Sources/ServerGame/service.o \
	Sources/ServerGame/GameWorld/adventure_manager.o \
	Sources/ServerGame/GameWorld/character_manager.o \
	Sources/ServerGame/GameWorld/city_manager.o \
	Sources/ServerGame/GameWorld/Components/aidrunk.o \
	Sources/ServerGame/GameWorld/Components/character_fov.o \
	Sources/ServerGame/GameWorld/Components/character_spawnpoint.o \
	Sources/ServerGame/GameWorld/Components/city.o \
	Sources/ServerGame/GameWorld/Components/game_component_serializer.o \
	Sources/ServerGame/GameWorld/Components/movement.o \
	Sources/ServerGame/GameWorld/Components/resources_container.o \
	Sources/ServerGame/GameWorld/Components/tile.o \
	Sources/ServerGame/GameWorld/Components/tile_map.o \
	Sources/ServerGame/GameWorld/Components/transform.o \
	Sources/ServerGame/GameWorld/Components/world.o \
	Sources/ServerGame/GameWorld/Components/world_city_tile.o \
	Sources/ServerGame/GameWorld/Components/world_tile.o \
	Sources/ServerGame/GameWorld/Components/Adventures/adventure.o \
	Sources/ServerGame/GameWorld/Components/Adventures/rescue_adventure.o \
	Sources/ServerGame/GameWorld/Components/Adventures/warrior_adventure.o \
	Sources/ServerGame/GameWorld/Components/Adventures/wizard_adventure.o \
	Sources/ServerGame/GameWorld/Components/Character/armor.o \
	Sources/ServerGame/GameWorld/Components/Character/character.o \
	Sources/ServerGame/GameWorld/Components/Character/character_info.o \
	Sources/ServerGame/GameWorld/Components/Character/enemy.o \
	Sources/ServerGame/GameWorld/Components/Character/equipment.o \
	Sources/ServerGame/GameWorld/Components/Character/homestash.o \
	Sources/ServerGame/GameWorld/Components/Character/inventory.o \
	Sources/ServerGame/GameWorld/Components/Character/necklace.o \
	Sources/ServerGame/GameWorld/Components/Character/orb.o \
	Sources/ServerGame/GameWorld/Components/Character/warrior.o \
	Sources/ServerGame/GameWorld/Components/Character/weapon.o \
	Sources/ServerGame/GameWorld/Components/Character/wizard.o \
	Sources/ServerGame/GameWorld/Components/TurnSystem/turnlistener.o \
	Sources/ServerGame/GameWorld/Components/TurnSystem/turnsystem.o \
	Sources/ServerGame/GameWorld/Components/TurnSystem/turnsystem_discrete.o \
	Sources/ServerGame/GameWorld/Components/TurnSystem/turnsystem_realtime.o \
	Sources/ServerGame/GameWorld/NetEvents/netevents_gameplay.o \
	Sources/ServerGame/GameWorld/NetEvents/netevents_negotiation.o \
	Sources/SquidLibCpp/Grid/FOV/Extensions/fov_translator.o \
	Sources/SquidLibCpp/Grid/FOV/Extensions/merge_fov.o \
	Sources/SquidLibCpp/Grid/FOV/Extensions/translucence_wrapper_fov.o \
	Sources/SquidLibCpp/Grid/FOV/FOVSolvers/elias_fov.o \
	Sources/SquidLibCpp/Grid/FOV/FOVSolvers/ray_casting_fov.o \
	Sources/SquidLibCpp/Grid/FOV/FOVSolvers/ripple_fov.o \
	Sources/SquidLibCpp/Grid/FOV/FOVSolvers/shadow_fov.o \
	Sources/SquidLibCpp/Grid/FOV/FOVSolvers/spread_fov.o \
	Sources/SquidLibCpp/Grid/FOV/LOSSolvers/bresenham_los.o \
	Sources/SquidLibCpp/Grid/FOV/LOSSolvers/elias_los.o \
	Sources/SquidLibCpp/Grid/FOV/LOSSolvers/ray_casting_los.o \
	Sources/SquidLibCpp/Grid/FOV/Strategies/basic_radius_strategy.o \
	Sources/SquidLibCpp/Grid/Utility/direction.o \
	Sources/SquidLibCpp/Grid/Utility/map.o \
	Sources/SquidLibCpp/Math/elias.o \
	Sources/SquidLibCpp/Math/bresenham.o \
	Sources/SquidLibCpp/Math/RNG.o \
	Sources/SquidLibCpp/Math/vector3.o \
	Sources/SquidLibCpp/Utility/pair.o \
	Sources/SquidLibCpp/Utility/s_collection.o

all: server

clean:
	rm server `find . -name *.o`

veryclean:
	rm server `find . -name *.o` `find . -name *.gch`

COMPILE.cxx = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
OUTPUT_OPTION = -o $@

$(Precomp).gch:
	$(CXX) $(CXXFLAGS) -x c++-header $(Precomp)

%.o: %.cxx
	$(COMPILE.cxx) -H $< $(OUTPUT_OPTION)

# server: $(Precomp).gch $(Server) 
server: $(Server) 
	g++ -o server $(Server) $(libs)
	echo All done
	
