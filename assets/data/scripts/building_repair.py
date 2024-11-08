def building_repair_condition(args):	
    this = args['this']
    cmd = args['cmd']
    if (cmd.IsValid() == True):
        settlement = this.GetSettlement()
        if (settlement.gold < cmd.goldCost):
            cmd.SetRollover("UI_NotEnoughGold")
            return False
        if (settlement.food < cmd.foodCost):
            cmd.SetRollover("UI_NotEnoughFood")
            return False
        if (settlement.population < cmd.populationCost + 1):
            cmd.SetRollover("UI_NotEnoughPopulation")
            return False
        if (this.IsBroken() == False):
            return False
        return True
    return False
    
def building_repair_body(args): 
    this = args['this']
    print("building_repair_body")
    this.Repair();
    