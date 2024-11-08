def unit_attach_condition(args):
    this = args['this']
    target = args['target']
    target_obj = target.object
    # Check that the target is valid and is an hero
    if target_obj is None or target_obj.IsHero() == False:
        return False
    # TODO - Only allies players
    if this.player != target_obj.player:
        return False
    # Check that the hero can contains other units
    if target_obj.GetArmy().IsFull() == True:
        return False
    return True

def unit_attach_body(args):
    this = args['this']
    target = args['target']
    target_obj = target.object # target is an hero
    this.GoToApproach(target_obj, 300)    