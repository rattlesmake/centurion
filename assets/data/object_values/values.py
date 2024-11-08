# ---------------------------
# CENTURION
# [2019] - [2022] Rattlesmake
# All Rights Reserved.
# ---------------------------

# Script for values.xml
# N.B: All scripts MUST always return a string (we are using f-strings)

def value_level(args):
    this = args['this']
    return f"{this.level}"
	
def value_health(args):
    this = args['this']
    return f"{this.health}/{this.maxHealth}"

def value_stamina(args):
    this = args['this']
    return f"{this.stamina}/{this.maxStamina}"
	
def value_speed(args):
    this = args['this']
    return f"{this.speed}"
	
def value_food(args):
    this = args['this']
    return f"{this.food}/{this.maxFood}"
	
def value_attack(args):
    this = args['this']
    return f"{this.minAttack} - {this.maxAttack}"
	
def value_defence(args):
    this = args['this']
    return f"{this.armorPierce} - {this.armorSlash}"
	
def value_range(args):
    this = args['this']
    if (this.IsRanged() == True):
        return f"{this.range}"
    return ""
    
def value_sight(args):
    this = args['this']
    return f"{this.sight}"
    
def value_gold_building(args):
    this = args['this']
    settlement = this.GetSettlement()
    return f"{settlement.gold}"
	
def value_food_building(args):
    this = args['this']
    settlement = this.GetSettlement()
    return f"{settlement.food}"

def value_population(args):
    this = args['this']
    settlement = this.GetSettlement()
    return f"{settlement.population}/{settlement.maxPopulation}"
    
def value_loyalty(args):
    this = args['this']
    settlement = this.GetSettlement()
    return f"{settlement.loyalty}"
	