def hero_enter_condition(args):
    this = args['this']
    target = args['target']
    target_obj = target.object
    # Check that the target is valid and is a building
    if target_obj is None or target_obj.IsBuilding() == False:
        return False
    # Check that the target building has a garrison (i.e. that can host units)
    if target_obj.HasGarrison() == False:
        return False
    # Check that the target building doesn't already have this unit.
    if target_obj.GetGarrison().Contains(this) == True:
        return False;
    return True   

def hero_enter_body(args):
    this = args['this']
    target = args['target']
    this.KeepFormationMoving()
    this.GoToEnter(target.object)