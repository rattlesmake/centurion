def hero_move_condition(args):
    return True
    
def hero_move_body(args):
    this = args['this']
    target = args['target']
    target_obj = target.object
    if target_obj is not None and target_obj.IsBuilding():
        this.SetCommandWithTarget("enter", target) 
    else:
        this.KeepFormationMoving()
        this.GoTo(target.point)