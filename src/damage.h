#ifndef DAMAGE_H
#define DAMAGE_H

#include "enums.h"
#include <string>
#include <vector>
#include <set>
#include <memory>

class item;
class monster;
class JsonObject;

enum body_part : int;

    enum damage_type : int {
    DT_NULL = 0, // null damage, doesn't exist
    DT_TRUE, // typeless damage, should always go through
    DT_BIOLOGICAL, // internal damage, like from smoke or poison
    DT_BASH, // bash damage
    DT_CUT, // cut damage
    DT_ACID, // corrosive damage, e.g. acid
    DT_STAB, // stabbing/piercing damage
    DT_HEAT, // e.g. fire, plasma
    DT_COLD, // e.g. heatdrain, cryogrenades
    DT_ELECTRIC, // e.g. electrical discharge
    NUM_DT
};

struct damage_unit {
    damage_type type;
    float amount;
    int res_pen;
    float res_mult;
    float damage_multiplier;

    damage_unit( damage_type dt, float a, int rp = 0, float rm = 1.0f, float mul = 1.0f ) :
        type( dt ), amount( a ), res_pen( rp ), res_mult( rm ), damage_multiplier( mul ) { }
};


// a single atomic unit of damage from an attack. Can include multiple types
// of damage at different armor mitigation/penetration values
struct damage_instance {
    std::vector<damage_unit> damage_units;
    damage_instance();
    static damage_instance physical( float bash, float cut, float stab, int arpen = 0 );
    void add_damage( damage_type dt, float a, int rp = 0, float rm = 1.0f, float mul = 1.0f );
    damage_instance( damage_type dt, float a, int rp = 0, float rm = 1.0f, float mul = 1.0f );
    void mult_damage( double multiplier );
    float type_damage( damage_type dt ) const;
    float total_damage() const;
    void clear();
};

struct dealt_damage_instance {
    std::array<int, NUM_DT> dealt_dams;
    body_part bp_hit;

    dealt_damage_instance();
    void set_damage( damage_type dt, int amount );
    int type_damage( damage_type dt ) const;
    int total_damage() const;
};

struct resistances {
    std::array<float, NUM_DT> resist_vals;

    resistances();

    // If to_self is true, we want armor's own resistance, not one it provides to wearer
    resistances( const item &armor, bool to_self = false );
    resistances( monster &monster );
    void set_resist( damage_type dt, float amount );
    float type_resist( damage_type dt ) const;

    float get_effective_resist( const damage_unit &du ) const;

    resistances &operator+=( const resistances &other );
};

damage_type dt_by_name( const std::string &name );
const std::string &name_by_dt( const damage_type &dt );

damage_instance load_damage_instance( JsonObject &jo );
damage_instance load_damage_instance( JsonArray &jarr );

resistances load_resistances_instance( JsonObject &jo );

// Returns damage or resistance data
// Handles some shorthands
std::array<float, NUM_DT> load_damage_array( JsonObject &jo );

#endif
