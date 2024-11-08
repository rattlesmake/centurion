def unit_move_condition(args):
    return True
    
def unit_move_body(args):
    this = args['this']
    target = args['target']
    target_obj = target.object
    if target_obj is not None and target_obj.IsBuilding():
        this.SetCommandWithTarget("enter", target) 
    else:
        this.GoTo(target.point)