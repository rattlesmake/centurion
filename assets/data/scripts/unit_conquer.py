def unit_conquer_condition(args):
    return True
    
def unit_conquer_body(args):
    this = args['this']
    target = args['target']
    target_obj = target.object
    if (target.object.IsCentralBuilding() == False):
        this.GoToConquer(target_obj.GetSettlement().GetFirstBuilding())
    else:
       this.GoToConquer(target_obj) 