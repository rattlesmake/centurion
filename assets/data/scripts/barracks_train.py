def barracks_train_condition(args):	
    this = args['this']
    cmd = args['cmd']
    if cmd.IsValid() == True:
        settlement = this.GetSettlement()
        if settlement.gold < cmd.goldCost:
            cmd.SetRollover("UI_NotEnoughGold")
            return False
        if settlement.food < cmd.foodCost:
            cmd.SetRollover("UI_NotEnoughFood")
            return False
        if settlement.population < (cmd.populationCost + 1):
            cmd.SetRollover("UI_NotEnoughPopulation")
            return False
        return True
    return False
    
def barracks_train_body(args): 
    this = args['this']
    match = args['match']
    className = args['className']
    settlement = this.GetSettlement()
    newUnit = match.PlaceObject(className, this.exitPoint.x, this.exitPoint.y, this.player)
    newUnit.SetCommandWithTarget("enter", settlement.GetFirstBuilding().AsTarget())
    